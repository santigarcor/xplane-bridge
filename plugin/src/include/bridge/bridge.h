#ifndef BRIDGE_H
#define BRIDGE_H

#include "serial-link.h"
#include "value.h"

#include <chrono>
#include <initializer_list>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>

struct AircraftProfile;

// What to send to the Arduino for an X-Plane dataref
struct DataRefMapping {
        // Arduino command, e.g. "set_speed" or "n1_led"
        std::string cmd;
        // Numeric changes smaller than this are not sent (e.g. 100 ft for the altitude display)
        double threshold = 0;
        // Converts the raw value before it's compared and sent. Empty sends it unchanged.
        Parser parser;
};

// One name or several, like `string | string[]` in the TypeScript mappings:
//   "sim/lights/nav_lights_on"   or   {"anim/43/button", "anim/43/button/anim"}
struct Names {
        std::vector<std::string> items;

        Names(const char *name) : items{name} {}
        Names(std::string name) : items{std::move(name)} {}
        Names(std::initializer_list<const char *> names) : items(names.begin(), names.end()) {}
};

// For addMomentarySwitchInputDataRef: flip the dataref between 0 and 1 (TOGGLE_DATAREF in TypeScript)
struct ToggleValue {};
inline constexpr ToggleValue kToggle{};

// Port of src/bridge/XPlaneBridge.ts, talking to X-Plane through the SDK instead of the web API.
//
// Main thread only: it calls XPLM functions everywhere. The flight loop calls update() every frame,
// which reads the mapped datarefs and sends whatever changed. Serial events (inputs, connects,
// resets) are passed in through handleSerialEvent().
class Bridge {
    public:
        explicit Bridge(SerialLink &link);
        ~Bridge();

        Bridge(const Bridge &) = delete;
        Bridge &operator=(const Bridge &) = delete;

        // Mapping API, same names and behaviour as the TypeScript bridge

        void addDataRef(const std::string &dataRefName, DataRefMapping mapping);
        // Any value is sent as 1 when > 0, else 0
        void addBooleanDataRef(const std::string &dataRefName, const std::string &arduinoCmd);
        void addValueMapDataRef(const std::string &dataRefName, const std::string &arduinoCmd, std::map<double, double> valueMap);

        // <switch>_on / <switch>_off set the datarefs to 1 / 0 (0 / 1 when inverse)
        void addToggleSwitchInputDataRef(const std::string &switchName, Names dataRefNames, bool inverse = false);
        // <switch>_on / <switch>_off run the commands. Without offCommands, onCommands run for both.
        void addToggleSwitchInputCommands(const std::string &switchName, Names onCommands, std::optional<Names> offCommands = std::nullopt);
        // <switch> runs the commands, held for `duration` seconds (0 = press and release)
        void addMomentarySwitchInputCommand(const std::string &switchName, Names commands, double duration = 0);
        // <switch> sets the datarefs to `value`
        void addMomentarySwitchInputDataRef(const std::string &switchName, Names dataRefNames, double value);
        // <switch> flips the datarefs between 0 and 1
        void addMomentarySwitchInputDataRef(const std::string &switchName, Names dataRefNames, ToggleValue);
        // <encoder>_increment / <encoder>_decrement run the commands
        void addRotaryEncoderCommands(const std::string &encoderName, Names incrementCommands, Names decrementCommands);

        // Lifecycle

        // Loads the mappings for the user's aircraft. With force, reloads them even if the aircraft
        // didn't change.
        void detectAircraft(bool force);
        void unloadAircraft();
        // Retries datarefs that weren't found yet (X-Plane announces new ones with XPLM_MSG_DATAREFS_ADDED)
        void resolvePendingDataRefs();

        void handleSerialEvent(const SerialEvent &event);
        void update();

        // Status, for the menu

        // "FF 757", or empty when no mappings are loaded
        std::string aircraftLabel() const;
        // The .acf file name when the loaded aircraft has no mappings
        const std::string &unsupportedAircraft() const {
            return unsupportedAcf;
        }
        size_t mappedDataRefCount() const {
            return datarefs.size();
        }
        size_t resolvedDataRefCount() const;
        // Problem with the current mappings (e.g. datarefs still missing after a grace period), empty if none
        std::string mappingError() const;

    private:
        using Clock = std::chrono::steady_clock;

        struct DataRefEntry {
                std::string name;
                DataRefMapping mapping;
                XPLMDataRef ref = nullptr;
                XPLMDataTypeID types = 0;
                // Reused every frame, so reading numbers and arrays doesn't allocate
                Value raw = 0.0;
                std::vector<float> floatBuffer;
                std::vector<int> intBuffer;
                bool errorLogged = false;
        };

        struct InputMapping {
                enum class Type { Command, DataRef };

                Type type;
                std::vector<std::string> actions;
                double value = 0;
                bool toggle = false;
                double duration = 0;
        };

        struct PreviousValue {
                Value parsed;
                std::string dataRefName;
        };

        struct HeldCommand {
                XPLMCommandRef ref;
                Clock::time_point releaseAt;
        };

        void clearMappings();
        void resolveDataRefs();
        bool readValue(DataRefEntry &entry);
        bool shouldSendUpdate(const std::string &cmd, const Value &value, double threshold) const;
        void sendToArduino(const std::string &cmd, const Value &value);
        void resendState();

        void handleInput(const std::string &name);
        void executeCommands(const std::vector<std::string> &commands, double duration);
        void setDataRefs(const std::vector<std::string> &dataRefNames, std::optional<double> value);
        XPLMCommandRef findCommand(const std::string &name);
        XPLMDataRef findWritableDataRef(const std::string &name);
        void releaseHeldCommands(bool all);

        SerialLink &link;

        std::vector<DataRefEntry> datarefs;
        std::unordered_map<std::string, InputMapping> inputs;
        // Last value sent (or that would have been sent) per Arduino command
        std::unordered_map<std::string, PreviousValue> previousValues;
        std::unordered_map<std::string, XPLMCommandRef> commandCache;
        std::unordered_map<std::string, XPLMDataRef> dataRefCache;
        std::vector<HeldCommand> heldCommands;

        const AircraftProfile *currentAircraft = nullptr;
        std::string unsupportedAcf;
        Clock::time_point detectedAt;
        Clock::time_point nextResolveAttempt;
        bool missingDataRefsLogged = false;
};

#endif
