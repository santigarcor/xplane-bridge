#include "logger.h"

#include "config.h"
#include "win-utils.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <system_error>
#include <XPLMUtilities.h>

// Session log files kept in <plugin>/logs, the current one included
static constexpr size_t kKeptLogFiles = 10;

static const char *levelName(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "Debug";
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warn:
            return "Warning";
        case LogLevel::Error:
            return "Error";
    }
    return "";
}

static std::tm localTimeNow(long long &milliseconds) {
    auto now = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(now);
    milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    std::tm localTime{};
    localtime_s(&localTime, &nowTimeT);
    return localTime;
}

Logger &Logger::getInstance() {
    // Function-local static: no heap allocation, nothing to free
    static Logger instance;
    return instance;
}

Logger::Logger() : mainThreadId(std::this_thread::get_id()) {
}

void Logger::initialize(const std::string &logDirectory) {
    if (initialized.load()) {
        return;
    }

    mainThreadId = std::this_thread::get_id();
    openLogFile(logDirectory);

    XPLMCreateFlightLoop_t params{};
    params.structSize = sizeof(params);
    params.phase = xplm_FlightLoop_Phase_AfterFlightModel;
    params.callbackFunc = flushCallback;
    params.refcon = this;
    flushLoop = XPLMCreateFlightLoop(&params);
    XPLMScheduleFlightLoop(flushLoop, -1.0f, 1);

    initialized.store(true, std::memory_order_release);
}

void Logger::destroy() {
    if (!initialized.load()) {
        return;
    }

    initialized.store(false, std::memory_order_release);

    if (flushLoop != nullptr) {
        XPLMDestroyFlightLoop(flushLoop);
        flushLoop = nullptr;
    }

    // We are on the main thread here, so drain whatever is still queued
    flush();

    if (logFile != nullptr) {
        fclose(logFile);
        logFile = nullptr;
    }
}

void Logger::openLogFile(const std::string &logDirectory) {
    if (logDirectory.empty()) {
        return;
    }

    const std::filesystem::path directory = utf8ToPath(logDirectory);
    std::error_code ec;
    std::filesystem::create_directories(directory, ec);

    // Keep the newest kKeptLogFiles - 1 old sessions, leaving room for this one.
    // The names sort by start time (bridge-YYYYMMDD-HHMMSS.log).
    std::vector<std::filesystem::path> oldLogs;
    for (const auto &entry : std::filesystem::directory_iterator(directory, ec)) {
        const std::wstring name = entry.path().filename().native();
        if (name.starts_with(L"bridge-") && name.ends_with(L".log")) {
            oldLogs.push_back(entry.path());
        }
    }
    std::sort(oldLogs.begin(), oldLogs.end());
    while (oldLogs.size() > kKeptLogFiles - 1) {
        std::filesystem::remove(oldLogs.front(), ec);
        oldLogs.erase(oldLogs.begin());
    }

    long long milliseconds = 0;
    std::tm now = localTimeNow(milliseconds);
    char name[64];
    strftime(name, sizeof(name), "bridge-%Y%m%d-%H%M%S.log", &now);

    const std::filesystem::path filePath = directory / name;
    logFile = _wfopen(filePath.c_str(), L"a");
    logFilePath = logFile != nullptr ? pathToUtf8(filePath) : "";
}

void Logger::debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::Debug, format, args);
    va_end(args);
}

void Logger::info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::Info, format, args);
    va_end(args);
}

void Logger::warn(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::Warn, format, args);
    va_end(args);
}

void Logger::error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::Error, format, args);
    va_end(args);
}

void Logger::log(LogLevel level, const char *format, va_list args) {
    if (level < currentLogLevel.load(std::memory_order_relaxed)) {
        return;
    }

    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);

    Entry entry{level, "", buffer};
    while (!entry.message.empty() && (entry.message.back() == '\n' || entry.message.back() == '\r')) {
        entry.message.pop_back();
    }

    // Stamped here, not when flushed, so queued lines from the worker keep their real time
    long long milliseconds = 0;
    std::tm now = localTimeNow(milliseconds);
    char timestamp[32];
    size_t length = strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &now);
    snprintf(timestamp + length, sizeof(timestamp) - length, ".%03lld", milliseconds);
    entry.timestamp = timestamp;

    if (std::this_thread::get_id() == mainThreadId) {
        emit(entry);
        return;
    }

    if (initialized.load(std::memory_order_acquire)) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (pending.size() >= kMaxPending) {
            droppedEntries++;
            return;
        }
        pending.push_back(std::move(entry));
        return;
    }

    // Off the main thread with no flush loop to hand the line to (before initialize() or after
    // destroy()). XPLMDebugString here would abort X-Plane, so only the debugger output is left.
    std::string line = "[" PRODUCT_NAME "] " + std::string(levelName(level)) + ": " + entry.message + "\n";
    OutputDebugStringA(line.c_str());
}

void Logger::emit(const Entry &entry) {
    std::string line = "[" PRODUCT_NAME "] " + std::string(levelName(entry.level)) + ": " + entry.message + "\n";
    XPLMDebugString(line.c_str());

    if (logFile != nullptr) {
        fprintf(logFile, "%s [%s] %s\n", entry.timestamp.c_str(), levelName(entry.level), entry.message.c_str());
        fflush(logFile);
    }
}

void Logger::flush() {
    std::vector<Entry> drained;
    size_t dropped = 0;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        std::swap(drained, pending);
        std::swap(dropped, droppedEntries);
    }

    for (const auto &entry : drained) {
        emit(entry);
    }

    if (dropped > 0) {
        Entry entry{LogLevel::Warn, drained.empty() ? "" : drained.back().timestamp, std::to_string(dropped) + " log lines were dropped (queue full)"};
        emit(entry);
    }
}

float Logger::flushCallback(float, float, int, void *refcon) {
    static_cast<Logger *>(refcon)->flush();
    return -1.0f;
}
