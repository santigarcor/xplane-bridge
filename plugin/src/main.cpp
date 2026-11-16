#ifndef XPLM420
#error This is made to be compiled against the XPLM420 SDK for X-Plane 12.3.0 and newer
#endif

#include "appstate.h"
#include "config.h"
#include "logger.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <windows.h>
#include <XPLMPlugin.h>

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) {
    return TRUE;
}

// Exists only between XPluginEnable and XPluginDisable
static std::unique_ptr<AppState> appState;

PLUGIN_API int XPluginStart(char *name, char *signature, char *description) {
    strcpy(name, FRIENDLY_NAME);
    strcpy(signature, BUNDLE_ID);
    strcpy(description, "Connects an Arduino cockpit panel to X-Plane");

    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
    // Enables XPLM_MSG_DATAREFS_ADDED, used to pick up add-on datarefs as soon as they exist
    XPLMEnableFeature("XPLM_WANTS_DATAREF_NOTIFICATIONS", 1);

    Logger::getInstance().initialize(AppState::getPluginDirectory() + "/logs");
    Logger::getInstance().info("Plugin started (version %s), session log: %s", VERSION, Logger::getInstance().getLogFilePath().c_str());
    return 1;
}

PLUGIN_API void XPluginStop(void) {
    Logger::getInstance().info("Plugin stopped");
    Logger::getInstance().destroy();
}

PLUGIN_API int XPluginEnable(void) {
    try {
        appState = std::make_unique<AppState>();
    } catch (const std::exception &e) {
        Logger::getInstance().error("Could not enable the plugin: %s", e.what());
        appState.reset();
        return 0;
    }

    Logger::getInstance().info("Plugin enabled");
    return 1;
}

PLUGIN_API void XPluginDisable(void) {
    Logger::getInstance().info("Disabling plugin...");
    appState.reset();
    Logger::getInstance().info("Plugin disabled");
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, int message, void *param) {
    if (!appState) {
        return;
    }

    try {
        switch (message) {
            case XPLM_MSG_PLANE_LOADED:
                // Index 0 is the user's aircraft, others are AI planes
                if (reinterpret_cast<intptr_t>(param) == 0) {
                    appState->onPlaneLoaded();
                }
                break;

            case XPLM_MSG_PLANE_UNLOADED:
                if (reinterpret_cast<intptr_t>(param) == 0) {
                    appState->onPlaneUnloaded();
                }
                break;

            case XPLM_MSG_DATAREFS_ADDED:
                appState->onDataRefsAdded();
                break;

            default:
                break;
        }
    } catch (const std::exception &e) {
        Logger::getInstance().error("Error handling message %d: %s", message, e.what());
    } catch (...) {
        Logger::getInstance().error("Error handling message %d", message);
    }
}
