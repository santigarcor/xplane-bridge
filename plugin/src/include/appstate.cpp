#include "appstate.h"

#include "bridge.h"
#include "config.h"
#include "logger.h"
#include "plugins-menu.h"
#include "preferences.h"
#include "protocol.h"
#include "serial-link.h"
#include "win-utils.h"

#include <filesystem>
#include <XPLMPlugin.h>

using Clock = std::chrono::steady_clock;

static constexpr std::chrono::milliseconds kMenuRefreshInterval{250};
// A frame budget the flight loop should stay far below
static constexpr double kSlowTickMs = 1.0;
static constexpr std::chrono::seconds kSlowTickReportInterval{10};

AppState::AppState() {
    try {
        initialize();
    } catch (...) {
        // The destructor doesn't run for a half-built object, and X-Plane must not keep
        // callbacks into it
        shutdown();
        throw;
    }
}

AppState::~AppState() {
    shutdown();
}

void AppState::initialize() {
    preferences = std::make_unique<Preferences>(getXPlaneDirectory() + "/Output/preferences/" PRODUCT_NAME ".ini");

    bool debugLogging = preferences->getBool("debug_logging", false);
    Logger::getInstance().setLogLevel(debugLogging ? LogLevel::Debug : LogLevel::Info);

    SerialConfig config{preferences->getInt("baud", DEFAULT_BAUD_RATE), preferences->get("port")};
    Logger::getInstance().info("Serial settings: %d baud, port %s (preferences: %s)", config.baudRate,
        config.forcedPort.empty() ? "autodetect" : config.forcedPort.c_str(), preferences->path().c_str());

    link = std::make_unique<SerialLink>(config);
    bridge = std::make_unique<Bridge>(*link);

    menu = std::make_unique<PluginsMenu>(FRIENDLY_NAME);
    statusItem = menu->addInfoItem("Status: starting...");
    aircraftItem = menu->addInfoItem("Aircraft: none");
    menu->addSeparator();
    menu->addItem("Restart serial connection", [this]() {
        restartSerial();
    });
    menu->addItem("Reload aircraft mappings", [this]() {
        reloadMappings();
    });
    debugItem = menu->addItem("Debug logging", [this]() {
        toggleDebugLogging();
    });
    menu->setChecked(debugItem, debugLogging);

    addCommand(PRODUCT_NAME "/restart_serial", "Restart the Arduino serial connection", [this]() {
        restartSerial();
    });
    addCommand(PRODUCT_NAME "/reload_mappings", "Reload the aircraft mappings and resend them to the Arduino", [this]() {
        reloadMappings();
    });

    if (!link->start()) {
        Logger::getInstance().error("The serial connection could not be started");
    }

    XPLMCreateFlightLoop_t params{};
    params.structSize = sizeof(params);
    params.phase = xplm_FlightLoop_Phase_AfterFlightModel;
    params.callbackFunc = flightLoopCallback;
    params.refcon = this;
    flightLoop = XPLMCreateFlightLoop(&params);
    XPLMScheduleFlightLoop(flightLoop, -1.0f, 1);

    bridge->detectAircraft(false);
    refreshMenu();
}

void AppState::shutdown() {
    // Reverse order of creation. The flight loop goes first so nothing calls into the objects
    // being destroyed. Safe to call on a partly initialized object and more than once.
    if (flightLoop != nullptr) {
        XPLMDestroyFlightLoop(flightLoop);
        flightLoop = nullptr;
    }

    for (auto &command : commands) {
        XPLMUnregisterCommandHandler(command->ref, commandCallback, 1, command.get());
    }
    commands.clear();

    menu.reset();
    // X-Plane is quitting (or the plugin is disabled): blank the displays and LEDs until the next
    // session connects and sends the state again
    if (link != nullptr && link->isConnected()) {
        Logger::getInstance().info("Switching the panel off");
        link->setShutdownLine(protocol::buildOutgoing("power", 0.0));
    }
    // Releases any held commands, then the link waits (at most 3 s) for the serial thread
    bridge.reset();
    link.reset();
    preferences.reset();
}

void AppState::addCommand(const char *name, const char *description, std::function<void()> action) {
    auto binding = std::make_unique<CommandBinding>();
    binding->ref = XPLMCreateCommand(name, description);
    binding->action = std::move(action);
    XPLMRegisterCommandHandler(binding->ref, commandCallback, 1, binding.get());
    commands.push_back(std::move(binding));
}

int AppState::commandCallback(XPLMCommandRef, XPLMCommandPhase phase, void *refcon) {
    if (phase != xplm_CommandBegin) {
        return 0;
    }

    try {
        static_cast<CommandBinding *>(refcon)->action();
    } catch (const std::exception &e) {
        Logger::getInstance().error("Command failed: %s", e.what());
    } catch (...) {
        Logger::getInstance().error("Command failed");
    }
    return 0;
}

float AppState::flightLoopCallback(float, float, int, void *refcon) {
    // Exceptions must never cross back into X-Plane
    try {
        static_cast<AppState *>(refcon)->tick();
    } catch (const std::exception &e) {
        Logger::getInstance().error("Flight loop error: %s", e.what());
    } catch (...) {
        Logger::getInstance().error("Flight loop error");
    }
    return -1.0f;
}

void AppState::tick() {
    auto start = Clock::now();

    for (const auto &event : link->drainEvents()) {
        bridge->handleSerialEvent(event);
    }
    bridge->update();

    if (start >= nextMenuRefresh) {
        refreshMenu();
        nextMenuRefresh = start + kMenuRefreshInterval;
    }

    double elapsedMs = std::chrono::duration<double, std::milli>(Clock::now() - start).count();
    if (elapsedMs > slowestTickMs) {
        slowestTickMs = elapsedMs;
    }
    if (start >= nextSlowTickReport) {
        if (slowestTickMs > kSlowTickMs) {
            Logger::getInstance().debug("Slowest flight loop in the last %llds: %.2f ms", static_cast<long long>(kSlowTickReportInterval.count()), slowestTickMs);
        }
        slowestTickMs = 0;
        nextSlowTickReport = start + kSlowTickReportInterval;
    }
}

void AppState::refreshMenu() {
    LinkStatus status = link->getStatus();
    std::string mappingError = bridge->mappingError();

    std::string text;
    if (!status.error.empty()) {
        text = "Status: ERROR - " + status.error;
    } else if (!mappingError.empty()) {
        text = "Status: ERROR - " + mappingError;
    } else {
        switch (status.state) {
            case LinkState::Connected:
                text = "Status: Connected (" + status.port + ")";
                break;
            case LinkState::Connecting:
                text = "Status: Connecting (" + status.port + ", waiting for boot)...";
                break;
            case LinkState::Searching:
                text = "Status: Searching for Arduino...";
                break;
            case LinkState::Stopped:
                text = "Status: ERROR - serial connection not running";
                break;
        }
    }

    if (text != statusText) {
        statusText = text;
        menu->setName(statusItem, statusText);
    }

    std::string aircraft;
    std::string label = bridge->aircraftLabel();
    if (!label.empty()) {
        aircraft = "Aircraft: " + label + " (" + std::to_string(bridge->resolvedDataRefCount()) + "/" + std::to_string(bridge->mappedDataRefCount()) + " datarefs)";
    } else if (!bridge->unsupportedAircraft().empty()) {
        aircraft = "Aircraft: " + bridge->unsupportedAircraft() + " (not supported)";
    } else {
        aircraft = "Aircraft: none";
    }

    if (aircraft != aircraftText) {
        aircraftText = aircraft;
        menu->setName(aircraftItem, aircraftText);
    }
}

void AppState::onPlaneLoaded() {
    bridge->detectAircraft(false);
    refreshMenu();
}

void AppState::onPlaneUnloaded() {
    bridge->unloadAircraft();
    refreshMenu();
}

void AppState::onDataRefsAdded() {
    bridge->resolvePendingDataRefs();
}

void AppState::restartSerial() {
    Logger::getInstance().info("Serial restart requested");
    // Only sets a flag, the serial thread does the work
    link->requestRestart();
    refreshMenu();
}

void AppState::reloadMappings() {
    Logger::getInstance().info("Reloading aircraft mappings");
    // Clears the previous values, so every mapped value is sent again on the next frame
    bridge->detectAircraft(true);
    refreshMenu();
}

void AppState::toggleDebugLogging() {
    bool enabled = Logger::getInstance().getLogLevel() != LogLevel::Debug;
    Logger::getInstance().setLogLevel(enabled ? LogLevel::Debug : LogLevel::Info);
    menu->setChecked(debugItem, enabled);
    Logger::getInstance().info("Debug logging %s", enabled ? "enabled" : "disabled");

    if (!preferences->set("debug_logging", enabled ? "1" : "0")) {
        Logger::getInstance().warn("Could not save %s", preferences->path().c_str());
    }
}

std::string AppState::getXPlaneDirectory() {
    char systemPath[2048] = {};
    XPLMGetSystemPath(systemPath);
    std::string root = systemPath;
    while (root.ends_with("/") || root.ends_with("\\")) {
        root.pop_back();
    }
    return root;
}

std::string AppState::getPluginDirectory() {
    // <X-Plane>/Resources/plugins/xplane_bridge/win_x64/xplane_bridge.xpl -> .../xplane_bridge
    char pluginPath[2048] = {};
    XPLMGetPluginInfo(XPLMGetMyID(), nullptr, pluginPath, nullptr, nullptr);
    if (pluginPath[0] == '\0') {
        return getXPlaneDirectory() + "/Resources/plugins/" PRODUCT_NAME;
    }
    return pathToUtf8(utf8ToPath(pluginPath).parent_path().parent_path());
}
