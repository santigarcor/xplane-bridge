#ifndef APPSTATE_H
#define APPSTATE_H

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <XPLMProcessing.h>
#include <XPLMUtilities.h>

class Bridge;
class PluginsMenu;
class Preferences;
class SerialLink;

// Everything the enabled plugin owns: preferences, the serial link, the bridge, the menu, the
// commands and the flight loop. Created in XPluginEnable and destroyed in XPluginDisable, so a
// disable/enable cycle in Plugin Admin always starts from scratch and leaves nothing behind.
//
// Main thread only. The flight loop runs every frame and does only non-blocking work: draining the
// serial events, reading datarefs and queueing updates.
class AppState {
    public:
        AppState();
        ~AppState();

        AppState(const AppState &) = delete;
        AppState &operator=(const AppState &) = delete;

        void onPlaneLoaded();
        void onPlaneUnloaded();
        void onDataRefsAdded();

        static std::string getXPlaneDirectory();
        static std::string getPluginDirectory();

    private:
        struct CommandBinding {
                XPLMCommandRef ref = nullptr;
                std::function<void()> action;
        };

        static float flightLoopCallback(float elapsedSinceLastCall, float elapsedSinceLastFlightLoop, int counter, void *refcon);
        static int commandCallback(XPLMCommandRef command, XPLMCommandPhase phase, void *refcon);

        void initialize();
        void shutdown();
        void tick();
        void refreshMenu();
        void addCommand(const char *name, const char *description, std::function<void()> action);

        void restartSerial();
        void reloadMappings();
        void toggleDebugLogging();

        std::unique_ptr<Preferences> preferences;
        std::unique_ptr<SerialLink> link;
        std::unique_ptr<Bridge> bridge;
        std::unique_ptr<PluginsMenu> menu;
        // unique_ptr so each binding's address, passed to X-Plane as refcon, never moves
        std::vector<std::unique_ptr<CommandBinding>> commands;
        XPLMFlightLoopID flightLoop = nullptr;

        int statusItem = -1;
        int aircraftItem = -1;
        int debugItem = -1;
        std::string statusText;
        std::string aircraftText;
        std::chrono::steady_clock::time_point nextMenuRefresh;

        // Flight loop timing, reported at debug level
        std::chrono::steady_clock::time_point nextSlowTickReport;
        double slowestTickMs = 0;
};

#endif
