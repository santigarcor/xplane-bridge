// Host test: runs the plugin's bridge and serial code outside X-Plane, against the stub XPLM in
// xplm-stub.cpp. Unit checks always run; the hardware checks run when an Arduino is plugged in
// (and nothing else has its port open).
//
//   build\win-tests\host-test.exe
//
// Not part of the plugin. Built with -DXPLANE_BRIDGE_HOST_TESTS=ON (build.ps1 -Test).

#include "bridge.h"
#include "logger.h"
#include "parsers.h"
#include "protocol.h"
#include "serial-link.h"
#include "serial-ports.h"
#include "xplm-stub.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <functional>
#include <string>
#include <thread>
#include <tlhelp32.h>
#include <windows.h>

using Clock = std::chrono::steady_clock;
using std::chrono::milliseconds;

static int passed = 0;
static int failed = 0;
// Set when the Arduino connected in the hardware test, i.e. the sketch is running and talking
static bool boardResponds = false;

#define CHECK(condition)                                                          \
    do {                                                                          \
        if (condition) {                                                          \
            passed++;                                                             \
        } else {                                                                  \
            failed++;                                                             \
            printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);         \
            fflush(stdout);                                                       \
        }                                                                         \
    } while (0)

static void section(const char *name) {
    printf("\n=== %s ===\n", name);
    fflush(stdout);
}

// ---------------------------------------------------------------------------------------------

static void testProtocol() {
    section("protocol");

    CHECK(protocol::formatNumber(35000) == "35000");
    CHECK(protocol::formatNumber(-1500) == "-1500");
    CHECK(protocol::formatNumber(1.5) == "1.5");
    CHECK(protocol::formatNumber(0.1) == "0.1");
    CHECK(protocol::formatNumber(-0.0) == "0");
    CHECK(protocol::formatNumber(std::nan("")) == "null");
    CHECK(protocol::buildOutgoing("set_speed", 250.0) == "{\"cmd\":\"set_speed\",\"value\":250}");
    CHECK(protocol::buildOutgoing("x", std::vector<double>{1, 2.5}) == "{\"cmd\":\"x\",\"value\":[1,2.5]}");
    CHECK(protocol::buildOutgoing("x", std::string("a\"b")) == "{\"cmd\":\"x\",\"value\":\"a\\\"b\"}");

    protocol::IncomingMessage message;
    CHECK(protocol::parseIncoming("{\"user_input\":\"n1\"}", message) && message.userInput == "n1" && !message.heartbeat);
    CHECK(protocol::parseIncoming("{\"hb\":123}", message) && message.heartbeat == 123.0 && !message.userInput);
    CHECK(protocol::parseIncoming(" { \"hb\" : 7 , \"x\" : true } ", message) && message.heartbeat == 7.0);
    CHECK(!protocol::parseIncoming("Ready for input...", message));
    CHECK(!protocol::parseIncoming("{\"user_input\":", message));
    CHECK(!protocol::parseIncoming("\xff\xfe{garbage", message));
    CHECK(!protocol::parseIncoming("", message));
}

static void testParsers() {
    section("parsers");

    CHECK(num(parsers::round(2.5)) == 3);
    CHECK(num(parsers::round(-2.5)) == -2); // Math.round
    CHECK(num(parsers::round(249.6)) == 250);
    CHECK(num(parsers::toBoolean(0.2)) == 1);
    CHECK(num(parsers::toBoolean(0.0)) == 0);
    CHECK(num(parsers::toBoolean(-1.0)) == 0);
    CHECK(num(parsers::toBoolean(std::vector<double>{1})) == 0);
    CHECK(num(parsers::toDegrees(3.14159265358979)) == 180);

    auto power = parsers::pipe({parsers::atIndex(0), [](const Value &v) -> Value {
                                    return num(v) > 55 ? 1.0 : 0.0;
                                }});
    CHECK(num(power(std::vector<double>{56, 0})) == 1);
    CHECK(num(power(std::vector<double>{55, 99})) == 0);
    CHECK(num(power(std::vector<double>{})) == 0);

    auto mapped = parsers::valueMap({{1, 10}, {2, 20}});
    CHECK(num(mapped(2.0)) == 20);
    CHECK(num(mapped(3.0)) == 3);
}

// ---------------------------------------------------------------------------------------------

// Every dataref the FF 757 mapping reads or writes, as floats
static void defineFF757DataRefs() {
    stub::removeAllDataRefs();
    for (const char *name : {"sim/cockpit/electrical/battery_on", "1-sim/AP/spdSetting", "1-sim/AP/hdgSetting", "1-sim/AP/altSetting",
             "1-sim/AP/vviSetting", "757Avionics/mcp/vs/label", "757Avionics/mcp/speed/label", "757Avionics/ap/leftFdState",
             "757Avionics/ap/rghtFdState", "1-sim/AP/lamp/1", "1-sim/AP/lamp/2", "1-sim/AP/lamp/3", "1-sim/AP/lamp/4", "1-sim/AP/lamp/5",
             "1-sim/AP/hdgConfButton", "1-sim/AP/lamp/7", "1-sim/AP/lamp/8", "1-sim/AP/lamp/10", "1-sim/AP/lamp/11", "1-sim/AP/lamp/12",
             "1-sim/AP/lamp/14", "1-sim/AP/lamp/13", "anim/43/button", "anim/43/button/anim", "anim/44/button", "anim/44/button/anim",
             "anim/45/button", "anim/45/button/anim", "anim/46/button", "anim/46/button/anim", "anim/52/button", "anim/52/button/anim"}) {
        stub::defineDataRef(name, xplmType_Float | xplmType_Double, 0);
    }
    stub::setAircraft("757-200_xp12.acf", "C:\\X-Plane 12\\Aircraft\\FlightFactor 757v2\\757-200_xp12.acf");
}

static bool hasCommand(const std::string &entry) {
    auto &log = stub::commandLog();
    return std::find(log.begin(), log.end(), entry) != log.end();
}

// One sim frame: what AppState::tick() does, plus the logger flush loop. Returns the time spent
// in the bridge part, which is what runs on X-Plane's main thread every frame.
static double frame(SerialLink &link, Bridge &bridge) {
    auto start = Clock::now();
    for (const auto &event : link.drainEvents()) {
        bridge.handleSerialEvent(event);
    }
    bridge.update();
    double elapsed = std::chrono::duration<double, std::milli>(Clock::now() - start).count();
    stub::runFlightLoops();
    return elapsed;
}

static void testBridgeWithoutArduino() {
    section("bridge (no serial connection)");

    defineFF757DataRefs();
    SerialLink link({57600, "COM_TEST_NOT_STARTED"});
    Bridge bridge(link);

    bridge.detectAircraft(false);
    CHECK(bridge.aircraftLabel() == "FF 757");
    CHECK(bridge.mappedDataRefCount() == 22);
    CHECK(bridge.resolvedDataRefCount() == 22);
    CHECK(bridge.mappingError().empty());

    stub::commandLog().clear();

    // Encoder: press and release at once
    bridge.handleSerialEvent({SerialEvent::Type::Input, "speed_encoder_increment"});
    CHECK(hasCommand("once:1-sim/comm/AP/spdUP"));

    // Momentary with a 0.1 s hold, released by a later frame. "c_o" is what the sketch sends.
    bridge.handleSerialEvent({SerialEvent::Type::Input, "c_o"});
    CHECK(hasCommand("begin:1-sim/command/AP/iasmach_button"));
    CHECK(!hasCommand("end:1-sim/command/AP/iasmach_button"));
    bridge.update();
    CHECK(!hasCommand("end:1-sim/command/AP/iasmach_button"));
    std::this_thread::sleep_for(milliseconds(120));
    bridge.update();
    CHECK(hasCommand("end:1-sim/command/AP/iasmach_button"));

    // Toggle switch writing datarefs
    bridge.handleSerialEvent({SerialEvent::Type::Input, "position_steady_on"});
    CHECK(stub::getValue("anim/43/button") == 1 && stub::getValue("anim/43/button/anim") == 1);
    bridge.handleSerialEvent({SerialEvent::Type::Input, "position_steady_off"});
    CHECK(stub::getValue("anim/43/button") == 0 && stub::getValue("anim/43/button/anim") == 0);

    // Toggle switch running the same command both ways
    bridge.handleSerialEvent({SerialEvent::Type::Input, "landing_l_on"});
    bridge.handleSerialEvent({SerialEvent::Type::Input, "landing_l_off"});
    CHECK(std::count(stub::commandLog().begin(), stub::commandLog().end(), "once:1-sim/command/lights/landingL/switch_trigger") == 2);

    // A held command is never left down: destroying the bridge releases it
    stub::commandLog().clear();
    {
        Bridge other(link);
        other.detectAircraft(false);
        other.handleSerialEvent({SerialEvent::Type::Input, "speed"});
    }
    CHECK(hasCommand("begin:1-sim/command/AP/spdButton_button"));
    CHECK(hasCommand("end:1-sim/command/AP/spdButton_button"));

    // Plane change: unsupported aircraft clears the mappings and reports it
    stub::setAircraft("B200.acf", "C:\\X-Plane 12\\Aircraft\\Laminar Research\\King Air C90B\\B200.acf");
    bridge.detectAircraft(false);
    CHECK(bridge.aircraftLabel().empty());
    CHECK(bridge.unsupportedAircraft() == "B200.acf");
    CHECK(bridge.mappingError() == "aircraft not supported: B200.acf");

    // The G1000 C172 must win over the plain one, and a digit in the install folder must not match
    stub::setAircraft("Cessna_172SP_G1000.acf", "C:\\X-Plane 12\\Aircraft\\Laminar Research\\Cessna 172 SP\\Cessna_172SP_G1000.acf");
    bridge.detectAircraft(false);
    CHECK(bridge.aircraftLabel() == "Cessna 172 G1000");
    // Only battery_on exists in the stub, the rest stay unresolved (reported after the grace period)
    CHECK(bridge.resolvedDataRefCount() == 1);
    CHECK(bridge.mappingError().empty());
}

// ---------------------------------------------------------------------------------------------

static int countSent(size_t fromLine, const std::string &needle = "") {
    int count = 0;
    auto &lines = stub::debugLines();
    for (size_t i = fromLine; i < lines.size(); i++) {
        if (lines[i].find("[=> Arduino]") != std::string::npos && lines[i].find(needle) != std::string::npos) {
            count++;
        }
    }
    return count;
}

static bool runFramesUntil(SerialLink &link, Bridge &bridge, milliseconds timeout, const std::function<bool()> &done, double *slowestMs = nullptr) {
    auto deadline = Clock::now() + timeout;
    while (Clock::now() < deadline) {
        double elapsed = frame(link, bridge);
        if (slowestMs != nullptr) {
            *slowestMs = std::max(*slowestMs, elapsed);
        }
        if (done()) {
            return true;
        }
        std::this_thread::sleep_for(milliseconds(16));
    }
    return false;
}

static void runFrames(SerialLink &link, Bridge &bridge, milliseconds duration, double *slowestMs = nullptr) {
    runFramesUntil(link, bridge, duration, [] { return false; }, slowestMs);
}

static size_t threadCount() {
    size_t count = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 entry{};
    entry.dwSize = sizeof(entry);
    if (Thread32First(snapshot, &entry)) {
        do {
            count += entry.th32OwnerProcessID == GetCurrentProcessId() ? 1 : 0;
        } while (Thread32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return count;
}

static size_t handleCount() {
    DWORD count = 0;
    GetProcessHandleCount(GetCurrentProcess(), &count);
    return count;
}

static void testWithArduino() {
    auto port = findArduinoPort(listSerialPorts());
    if (!port) {
        section("hardware (skipped: no Arduino found)");
        return;
    }
    section(("hardware: " + port->port + " (" + port->friendlyName + ")").c_str());

    defineFF757DataRefs();
    stub::setValue("1-sim/AP/spdSetting", 250);
    stub::setValue("1-sim/AP/altSetting", 10000);
    stub::setValue("sim/cockpit/electrical/battery_on", 1);

    double slowestFrameMs = 0;
    SerialLink link({57600, ""});
    Bridge bridge(link);
    bridge.detectAircraft(false);
    CHECK(link.start());

    // Connect: opening the port resets the board, which takes ~2 s to boot. Nothing can be sent
    // before that, so counting from here catches exactly the state sent on connect.
    size_t mark = stub::debugLines().size();
    auto connectStart = Clock::now();
    bool connected = runFramesUntil(link, bridge, milliseconds(12000), [&] { return link.isConnected(); }, &slowestFrameMs);
    CHECK(connected);
    printf("  connected in %lld ms\n", static_cast<long long>(std::chrono::duration_cast<milliseconds>(Clock::now() - connectStart).count()));
    if (!connected) {
        printf("  status: %s\n", link.getStatus().error.c_str());
        printf("  The port opens but the board sends nothing (no boot banner, no heartbeat). Is the sketch uploaded?\n");

        auto stopStart = Clock::now();
        link.stop();
        double stopMs = std::chrono::duration<double, std::milli>(Clock::now() - stopStart).count();
        printf("  stop took %.1f ms, slowest main-thread frame %.3f ms\n", stopMs, slowestFrameMs);
        CHECK(stopMs < 500);
        CHECK(slowestFrameMs < 2.0);
        return;
    }
    boardResponds = true;
    CHECK(link.getStatus().state == LinkState::Connected);
    CHECK(link.getStatus().error.empty());

    // On connect the whole state is sent once (one line per mapped Arduino command)
    runFrames(link, bridge, milliseconds(500), &slowestFrameMs);
    int initial = countSent(mark);
    printf("  initial state: %d lines\n", initial);
    CHECK(initial == 22);
    CHECK(countSent(mark, "{\"cmd\":\"set_speed\",\"value\":250}") == 1);

    // Nothing changes -> nothing is sent, even for threshold 0 mappings
    mark = stub::debugLines().size();
    runFrames(link, bridge, milliseconds(1000), &slowestFrameMs);
    CHECK(countSent(mark) == 0);

    // Thresholds
    mark = stub::debugLines().size();
    stub::setValue("1-sim/AP/spdSetting", 251);
    runFrames(link, bridge, milliseconds(200), &slowestFrameMs);
    CHECK(countSent(mark, "set_speed\",\"value\":251}") == 1);
    stub::setValue("1-sim/AP/spdSetting", 251.4); // rounds to the same value
    stub::setValue("1-sim/AP/altSetting", 10050); // below the 100 ft threshold
    mark = stub::debugLines().size();
    runFrames(link, bridge, milliseconds(200), &slowestFrameMs);
    CHECK(countSent(mark) == 0);
    stub::setValue("1-sim/AP/altSetting", 10100);
    runFrames(link, bridge, milliseconds(200), &slowestFrameMs);
    CHECK(countSent(mark, "set_altitude\",\"value\":10100}") == 1);

    // 757 FD master LED: the first FD on is master, the other takes over when it goes off
    mark = stub::debugLines().size();
    stub::setValue("757Avionics/ap/leftFdState", 1);
    runFrames(link, bridge, milliseconds(200), &slowestFrameMs);
    CHECK(countSent(mark, "flight_director_1_led\",\"value\":1}") == 1);
    stub::setValue("757Avionics/ap/rghtFdState", 1);
    mark = stub::debugLines().size();
    runFrames(link, bridge, milliseconds(200), &slowestFrameMs);
    CHECK(countSent(mark, "flight_director_2_led") == 0);
    stub::setValue("757Avionics/ap/leftFdState", 0);
    runFrames(link, bridge, milliseconds(200), &slowestFrameMs);
    CHECK(countSent(mark, "flight_director_1_led\",\"value\":0}") == 1);
    CHECK(countSent(mark, "flight_director_2_led\",\"value\":1}") == 1);

    // Heartbeats keep the link alive past the 6 s liveness timeout
    runFrames(link, bridge, milliseconds(7000), &slowestFrameMs);
    CHECK(link.isConnected());

    // Manual restart: the port is closed and reopened (board resets), then the state is resent
    mark = stub::debugLines().size();
    auto restartStart = Clock::now();
    link.requestRestart();
    bool dropped = runFramesUntil(link, bridge, milliseconds(2000), [&] { return !link.isConnected(); }, &slowestFrameMs);
    CHECK(dropped);
    bool reconnected = runFramesUntil(link, bridge, milliseconds(12000), [&] { return link.isConnected(); }, &slowestFrameMs);
    CHECK(reconnected);
    runFrames(link, bridge, milliseconds(500), &slowestFrameMs);
    printf("  restart: reconnected in %lld ms, resent %d lines\n",
        static_cast<long long>(std::chrono::duration_cast<milliseconds>(Clock::now() - restartStart).count()), countSent(mark));
    CHECK(countSent(mark) == 22);

    // Stop is quick when the driver behaves
    auto stopStart = Clock::now();
    link.stop();
    double stopMs = std::chrono::duration<double, std::milli>(Clock::now() - stopStart).count();
    printf("  stop took %.1f ms\n", stopMs);
    CHECK(stopMs < 500);
    CHECK(link.getStatus().state == LinkState::Stopped);

    printf("  slowest main-thread frame: %.3f ms\n", slowestFrameMs);
    CHECK(slowestFrameMs < 2.0);
}

static void testNoLeaks() {
    auto port = findArduinoPort(listSerialPorts());
    if (!port) {
        return;
    }
    section("leaks: create, connect and destroy the serial link and bridge 5 times");

    defineFF757DataRefs();
    auto cycle = [] {
        SerialLink link({57600, ""});
        Bridge bridge(link);
        bridge.detectAircraft(false);
        link.start();
        bool connected = runFramesUntil(link, bridge, milliseconds(12000), [&] { return link.isConnected(); });
        runFrames(link, bridge, milliseconds(200));
        return connected;
    };

    // Warm-up cycle, so one-time allocations (CRT, SetupAPI, thread pools) are in the baseline
    cycle();
    stub::runFlightLoops();
    size_t threadsBefore = threadCount();
    size_t handlesBefore = handleCount();

    int connectedCycles = 0;
    for (int i = 0; i < 5; i++) {
        connectedCycles += cycle() ? 1 : 0;
    }
    stub::runFlightLoops();

    size_t threadsAfter = threadCount();
    size_t handlesAfter = handleCount();
    printf("  connected %d/5, threads %zu -> %zu, handles %zu -> %zu\n", connectedCycles, threadsBefore, threadsAfter, handlesBefore, handlesAfter);
    if (boardResponds) {
        CHECK(connectedCycles == 5);
    }
    CHECK(threadsAfter <= threadsBefore);
    CHECK(handlesAfter <= handlesBefore + 2);
}

int main() {
    std::filesystem::path logDirectory = std::filesystem::temp_directory_path() / "xplane_bridge_host_test";
    Logger::getInstance().initialize(logDirectory.string());
    Logger::getInstance().setLogLevel(LogLevel::Debug);

    testProtocol();
    testParsers();
    testBridgeWithoutArduino();
    testWithArduino();
    testNoLeaks();

    Logger::getInstance().destroy();
    CHECK(stub::flightLoopCount() == 0);

    printf("\n%d passed, %d failed\n", passed, failed);
    return failed == 0 ? 0 : 1;
}
