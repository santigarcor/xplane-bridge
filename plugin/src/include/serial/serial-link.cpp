#include "serial-link.h"

#include "logger.h"
#include "protocol.h"
#include "serial-ports.h"
#include "win-utils.h"

#include <atomic>
#include <chrono>
#include <deque>
#include <mutex>
#include <optional>

using Clock = std::chrono::steady_clock;
using std::chrono::milliseconds;

// Same values as ArduinoSerialCommunicator.ts
/** Printed by the sketch at the end of setup() */
static constexpr const char *kBootBanner = "Ready for input";
/** The sketch sends a heartbeat every 2 s, so this much silence means the link is dead */
static constexpr milliseconds kLivenessTimeout{6000};
/** Silences longer than this are logged once data resumes */
static constexpr milliseconds kSilenceLogThreshold{3000};
/** How long CloseHandle may take before the USB link is reported as not responding */
static constexpr milliseconds kCloseTimeout{3000};
static constexpr milliseconds kReconnectDelay{5000};

/** After a manual restart, give Windows a moment to release the port before reopening it */
static constexpr milliseconds kRestartDelay{500};
/** ReadFile returns after this long without data, so the worker loop keeps turning */
static constexpr DWORD kReadTimeoutMs = 10;
/** A write that can't complete in this time means the Arduino stopped reading */
static constexpr DWORD kWriteTimeoutMs = 500;
/** Longest stop() waits for the worker before leaving it to finish on its own */
static constexpr milliseconds kStopTimeout{3000};

/** Lines longer than this are garbage (e.g. after a reset) and are dropped */
static constexpr size_t kMaxLineLength = 1024;
static constexpr size_t kMaxQueuedLines = 500;
static constexpr size_t kMaxQueuedEvents = 1000;

static long long nowMs() {
    return std::chrono::duration_cast<milliseconds>(Clock::now().time_since_epoch()).count();
}

// Everything the worker thread uses. Shared between SerialLink and the thread, so it outlives
// SerialLink if the thread has to be left running (see SerialLink::stop()).
class SerialWorker {
    public:
        explicit SerialWorker(SerialConfig config) : config(std::move(config)), wakeEvent(CreateEventW(nullptr, FALSE, FALSE, nullptr)) {}

        void run();

        void requestStop() {
            stopRequested.store(true);
            SetEvent(wakeEvent.get());
        }

        void requestRestart() {
            restartRequested.store(true);
            SetEvent(wakeEvent.get());
        }

        void send(const std::string &line) {
            if (!connected.load()) {
                return;
            }

            std::lock_guard<std::mutex> lock(queueMutex);
            if (outgoing.size() >= kMaxQueuedLines) {
                // The writer can't keep up, so the oldest updates are stale anyway
                outgoing.pop_front();
                droppedLines++;
            }
            outgoing.push_back(line);
        }

        std::vector<SerialEvent> drainEvents() {
            std::lock_guard<std::mutex> lock(queueMutex);
            std::vector<SerialEvent> drained;
            drained.swap(events);
            return drained;
        }

        LinkStatus getStatus() {
            LinkStatus copy;
            {
                std::lock_guard<std::mutex> lock(statusMutex);
                copy = status;
            }

            // The worker can't report this itself: it's the one stuck in CloseHandle
            long long closing = closingSince.load();
            if (closing != 0 && nowMs() - closing > kCloseTimeout.count()) {
                copy.error = copy.port + " not responding, replug the USB cable";
                if (!reportedCloseHang.exchange(true)) {
                    Logger::getInstance().error("USB link not responding: %s did not close. Replug the USB cable if the panel does not recover", copy.port.c_str());
                }
            }
            return copy;
        }

        bool isConnected() const {
            return connected.load();
        }

    private:
        bool openPort();
        void closePort();
        void failPort(const std::string &logMessage, const std::string &statusError);
        bool writePending();
        bool readAvailable();
        void handleLine(const std::string &line);
        void handleHeartbeat(double uptime);
        void handleReset(const std::string &reason);
        void markConnected();
        void checkLiveness();

        void pushEvent(SerialEvent::Type type, std::string text = "");
        void setStatus(LinkState state, const std::string &port);
        void setError(const std::string &error);
        void waitFor(milliseconds duration);

        const SerialConfig config;
        UniqueHandle wakeEvent;

        std::atomic<bool> stopRequested{false};
        std::atomic<bool> restartRequested{false};
        std::atomic<bool> connected{false};
        // nowMs() when CloseHandle started, 0 when not closing. Read by getStatus() on the main thread.
        std::atomic<long long> closingSince{0};
        std::atomic<bool> reportedCloseHang{false};

        std::mutex queueMutex;
        std::deque<std::string> outgoing;
        std::vector<SerialEvent> events;
        size_t droppedLines = 0;

        std::mutex statusMutex;
        LinkStatus status;

        // Only touched by the worker thread
        UniqueHandle port;
        std::string portName;
        std::string rxBuffer;
        bool discardingLine = false;
        Clock::time_point lastRx;
        Clock::time_point nextAttempt;
        /** Uptime reported by the last heartbeat, empty until the first one after a (re)boot */
        std::optional<double> lastHeartbeat;
        /** Avoids logging the same problem every 5 s while it lasts */
        std::string lastLoggedProblem;
};

void SerialWorker::run() {
    setStatus(LinkState::Searching, "");
    nextAttempt = Clock::now();

    while (!stopRequested.load()) {
        if (!port.valid()) {
            if (restartRequested.exchange(false)) {
                setError("");
                lastLoggedProblem.clear();
                nextAttempt = Clock::now();
            }

            auto now = Clock::now();
            if (now < nextAttempt) {
                waitFor(std::min(std::chrono::duration_cast<milliseconds>(nextAttempt - now), milliseconds(250)));
                continue;
            }

            if (!openPort()) {
                nextAttempt = Clock::now() + kReconnectDelay;
            }
            continue;
        }

        if (restartRequested.exchange(false)) {
            Logger::getInstance().info("Restarting the serial connection");
            closePort();
            setError("");
            lastLoggedProblem.clear();
            setStatus(LinkState::Searching, "");
            nextAttempt = Clock::now() + kRestartDelay;
            continue;
        }

        if (!writePending() || !readAvailable()) {
            continue;
        }

        checkLiveness();
    }

    closePort();
    setStatus(LinkState::Stopped, "");
}

bool SerialWorker::openPort() {
    std::string name = config.forcedPort;

    if (name.empty()) {
        auto found = findArduinoPort(listSerialPorts());
        if (!found) {
            setStatus(LinkState::Searching, "");
            if (lastLoggedProblem != "not found") {
                Logger::getInstance().warn("Arduino not found. Retrying every 5 seconds...");
                lastLoggedProblem = "not found";
            }
            return false;
        }
        name = found->port;
        Logger::getInstance().debug("Found %s (%s, %s)", name.c_str(), found->friendlyName.c_str(), found->instanceId.c_str());
    }

    setStatus(LinkState::Searching, name);

    std::wstring path = L"\\\\.\\" + utf8ToWide(name);
    UniqueHandle handle(CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr));
    if (!handle.valid()) {
        DWORD error = GetLastError();
        std::string statusError;
        if (error == ERROR_ACCESS_DENIED) {
            statusError = name + " in use (is the Node bridge running?)";
        } else if (error == ERROR_FILE_NOT_FOUND) {
            statusError = name + " not found";
        } else {
            statusError = "opening " + name + " failed (code " + std::to_string(error) + ")";
        }

        setError(statusError);
        if (lastLoggedProblem != statusError) {
            Logger::getInstance().error("Error opening %s: %s. Retrying every 5 seconds...", name.c_str(), win32ErrorMessage(error).c_str());
            lastLoggedProblem = statusError;
        }
        return false;
    }
    portName = name;

    DCB dcb{};
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(handle.get(), &dcb)) {
        failPort("Error reading the settings of " + name + ": " + win32ErrorMessage(GetLastError()), "configuring " + name + " failed");
        return false;
    }

    dcb.BaudRate = static_cast<DWORD>(config.baudRate);
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary = TRUE;
    dcb.fParity = FALSE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    // DTR on, like the Node bridge: opening the port resets the board, and the sketch announces
    // itself with the boot banner when setup() finishes
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fNull = FALSE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    dcb.fAbortOnError = FALSE;

    if (!SetCommState(handle.get(), &dcb)) {
        failPort("Error configuring " + name + ": " + win32ErrorMessage(GetLastError()), "configuring " + name + " failed");
        return false;
    }

    // ReadFile returns right away when bytes are waiting, otherwise after kReadTimeoutMs.
    // WriteFile gives up after kWriteTimeoutMs. Neither can block the worker loop for long.
    COMMTIMEOUTS timeouts{};
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = kReadTimeoutMs;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = kWriteTimeoutMs;
    if (!SetCommTimeouts(handle.get(), &timeouts)) {
        failPort("Error setting timeouts on " + name + ": " + win32ErrorMessage(GetLastError()), "configuring " + name + " failed");
        return false;
    }

    PurgeComm(handle.get(), PURGE_RXCLEAR | PURGE_TXCLEAR);

    port = std::move(handle);
    rxBuffer.clear();
    discardingLine = false;
    lastHeartbeat.reset();
    lastRx = Clock::now();
    reportedCloseHang.store(false);
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        outgoing.clear();
    }

    // The liveness check also covers the boot, in case the board never starts sending
    setStatus(LinkState::Connecting, portName);
    Logger::getInstance().info("Port %s opened, waiting for Arduino to boot...", portName.c_str());
    return true;
}

void SerialWorker::closePort() {
    bool wasConnected = connected.exchange(false);
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        outgoing.clear();
    }

    if (port.valid()) {
        // If the USB driver stopped responding, CloseHandle may take a long time or never return.
        // That only stalls this thread; getStatus() reports it from the main thread meanwhile.
        closingSince.store(nowMs());
        port.reset();
        closingSince.store(0);
    }

    rxBuffer.clear();
    discardingLine = false;

    if (wasConnected) {
        pushEvent(SerialEvent::Type::Disconnected);
    }
}

void SerialWorker::failPort(const std::string &logMessage, const std::string &statusError) {
    Logger::getInstance().error("%s", logMessage.c_str());
    lastLoggedProblem = statusError;
    std::string name = portName;
    closePort();
    setError(statusError);
    setStatus(LinkState::Searching, name);
    nextAttempt = Clock::now() + kReconnectDelay;
}

bool SerialWorker::writePending() {
    std::deque<std::string> lines;
    size_t dropped = 0;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        lines.swap(outgoing);
        std::swap(dropped, droppedLines);
    }

    if (dropped > 0) {
        Logger::getInstance().warn("Write queue full, %zu old updates were dropped", dropped);
    }

    for (const auto &line : lines) {
        std::string data = line + "\n";
        DWORD written = 0;
        if (!WriteFile(port.get(), data.data(), static_cast<DWORD>(data.size()), &written, nullptr)) {
            failPort("Error writing to " + portName + ": " + win32ErrorMessage(GetLastError()), "writing to " + portName + " failed, reconnecting");
            return false;
        }
        if (written < data.size()) {
            failPort("Writing to " + portName + " timed out, the Arduino is not reading. Reconnecting...", portName + " not accepting data, reconnecting");
            return false;
        }
        Logger::getInstance().debug("[=> Arduino] %s", line.c_str());
    }

    return true;
}

bool SerialWorker::readAvailable() {
    char buffer[512];
    DWORD bytesRead = 0;
    if (!ReadFile(port.get(), buffer, sizeof(buffer), &bytesRead, nullptr)) {
        DWORD error = GetLastError();
        if (stopRequested.load()) {
            // stop() cancelled the read
            return false;
        }
        failPort("Serial connection lost on " + portName + " (" + win32ErrorMessage(error) + "). Reconnecting...", "connection lost on " + portName + ", reconnecting");
        return false;
    }

    for (DWORD i = 0; i < bytesRead; i++) {
        char c = buffer[i];
        if (c == '\n') {
            if (!discardingLine) {
                std::string line;
                line.swap(rxBuffer);
                handleLine(line);
            }
            rxBuffer.clear();
            discardingLine = false;
            continue;
        }

        if (discardingLine) {
            continue;
        }
        if (rxBuffer.size() >= kMaxLineLength) {
            Logger::getInstance().warn("Dropping an over-long line from the Arduino");
            rxBuffer.clear();
            discardingLine = true;
            continue;
        }
        rxBuffer.push_back(c);
    }

    return true;
}

static std::string trim(const std::string &text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

void SerialWorker::handleLine(const std::string &rawLine) {
    auto now = Clock::now();
    auto silence = std::chrono::duration_cast<milliseconds>(now - lastRx);
    if (connected.load() && silence > kSilenceLogThreshold) {
        Logger::getInstance().warn("Link silent for %.1fs", silence.count() / 1000.0);
    }
    lastRx = now;

    std::string line = trim(rawLine);

    // Lines right after a reset can carry garbage from the bootloader, so don't match exactly
    if (line.find(kBootBanner) != std::string::npos) {
        if (connected.load()) {
            handleReset("boot banner");
        } else {
            markConnected();
        }
        return;
    }

    protocol::IncomingMessage message;
    if (!protocol::parseIncoming(line, message)) {
        if (!line.empty()) {
            Logger::getInstance().debug("[Arduino] text: %s", line.c_str());
        }
        return;
    }

    if (message.heartbeat) {
        // A heartbeat also proves the sketch is running, in case the banner got lost
        if (!connected.load()) {
            markConnected();
        }
        handleHeartbeat(*message.heartbeat);
        return;
    }

    if (message.userInput) {
        Logger::getInstance().debug("[Arduino =>] %s", line.c_str());
        pushEvent(SerialEvent::Type::Input, *message.userInput);
        return;
    }

    Logger::getInstance().warn("Unknown message from Arduino: %s", line.c_str());
}

void SerialWorker::handleHeartbeat(double uptime) {
    if (connected.load() && lastHeartbeat && uptime < *lastHeartbeat) {
        handleReset("uptime went from " + protocol::formatNumber(*lastHeartbeat) + "s to " + protocol::formatNumber(uptime) + "s");
    }
    lastHeartbeat = uptime;
}

void SerialWorker::handleReset(const std::string &reason) {
    Logger::getInstance().warn("Arduino reset detected (%s). Resending state...", reason.c_str());
    // So the banner and a later backwards uptime don't both fire for the same reset
    lastHeartbeat.reset();
    pushEvent(SerialEvent::Type::Reset);
}

void SerialWorker::markConnected() {
    Logger::getInstance().info("Connected to Arduino on %s", portName.c_str());
    lastLoggedProblem.clear();
    setError("");
    setStatus(LinkState::Connected, portName);
    connected.store(true);
    pushEvent(SerialEvent::Type::Connected);
}

void SerialWorker::checkLiveness() {
    auto silence = std::chrono::duration_cast<milliseconds>(Clock::now() - lastRx);
    if (silence <= kLivenessTimeout) {
        return;
    }

    char message[160];
    snprintf(message, sizeof(message), "No data from %s for %.1fs, closing the port. Reopening resets the Arduino.", portName.c_str(), silence.count() / 1000.0);
    failPort(message, "no data from " + portName + " for 6s, reconnecting");
}

void SerialWorker::pushEvent(SerialEvent::Type type, std::string text) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (events.size() >= kMaxQueuedEvents) {
        return;
    }
    events.push_back({type, std::move(text)});
}

void SerialWorker::setStatus(LinkState state, const std::string &portName) {
    std::lock_guard<std::mutex> lock(statusMutex);
    status.state = state;
    status.port = portName;
}

void SerialWorker::setError(const std::string &error) {
    std::lock_guard<std::mutex> lock(statusMutex);
    status.error = error;
}

void SerialWorker::waitFor(milliseconds duration) {
    WaitForSingleObject(wakeEvent.get(), static_cast<DWORD>(duration.count()));
}

namespace {

    struct ThreadStart {
            std::shared_ptr<SerialWorker> worker;
            HMODULE module;
    };

    DWORD WINAPI serialThreadMain(void *parameter) {
        HMODULE module = nullptr;
        {
            std::unique_ptr<ThreadStart> start(static_cast<ThreadStart *>(parameter));
            module = start->module;
            std::shared_ptr<SerialWorker> worker = std::move(start->worker);
            start.reset();

            try {
                worker->run();
            } catch (const std::exception &e) {
                Logger::getInstance().error("Serial thread stopped: %s", e.what());
            } catch (...) {
                Logger::getInstance().error("Serial thread stopped by an unknown error");
            }
            // The worker state is released here, before the DLL reference below
        }

        // Releases the reference taken in start() and exits without returning into code that may
        // have just been unloaded
        FreeLibraryAndExitThread(module, 0);
        return 0;
    }

}

SerialLink::SerialLink(SerialConfig config) : worker(std::make_shared<SerialWorker>(std::move(config))) {
}

SerialLink::~SerialLink() {
    stop();
}

bool SerialLink::start() {
    if (thread != nullptr) {
        return true;
    }

    // Keeps this DLL loaded for as long as the thread runs, released by FreeLibraryAndExitThread
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(&serialThreadMain), &module)) {
        Logger::getInstance().error("Could not start the serial thread: %s", win32ErrorMessage(GetLastError()).c_str());
        return false;
    }

    auto start = std::make_unique<ThreadStart>(ThreadStart{worker, module});
    thread = CreateThread(nullptr, 0, serialThreadMain, start.get(), 0, nullptr);
    if (thread == nullptr) {
        Logger::getInstance().error("Could not start the serial thread: %s", win32ErrorMessage(GetLastError()).c_str());
        FreeLibrary(module);
        return false;
    }

    // Owned by the thread from here on
    start.release();
    return true;
}

void SerialLink::stop() {
    if (thread == nullptr) {
        return;
    }

    worker->requestStop();

    auto deadline = Clock::now() + kStopTimeout;
    while (WaitForSingleObject(thread, 50) == WAIT_TIMEOUT) {
        // Interrupts a ReadFile/WriteFile the driver is sitting on
        CancelSynchronousIo(thread);

        if (Clock::now() > deadline) {
            Logger::getInstance().error("The serial thread did not stop within 3 s (USB driver not responding). Leaving it to finish on its own.");
            break;
        }
    }

    CloseHandle(thread);
    thread = nullptr;
}

void SerialLink::requestRestart() {
    worker->requestRestart();
}

void SerialLink::send(const std::string &line) {
    worker->send(line);
}

std::vector<SerialEvent> SerialLink::drainEvents() {
    return worker->drainEvents();
}

LinkStatus SerialLink::getStatus() const {
    return worker->getStatus();
}

bool SerialLink::isConnected() const {
    return worker->isConnected();
}
