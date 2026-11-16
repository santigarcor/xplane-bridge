#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <map>
#include <string>

// Flat key=value settings in <X-Plane>/Output/preferences/xplane_bridge.ini.
// Lines starting with # or ; and [section] headers are ignored. Keys used today:
//   baud=57600          serial speed (must match the sketch)
//   port=COM5           skip autodiscovery and always open this port
//   debug_logging=1     log every message to and from the Arduino
class Preferences {
    public:
        explicit Preferences(std::string filePath);

        std::string get(const std::string &key, const std::string &defaultValue = "") const;
        int getInt(const std::string &key, int defaultValue) const;
        bool getBool(const std::string &key, bool defaultValue) const;

        // Updates the value and rewrites the file. Returns false if the file can't be written.
        bool set(const std::string &key, const std::string &value);

        const std::string &path() const {
            return filePath;
        }

    private:
        void load();
        bool save() const;

        std::string filePath;
        std::map<std::string, std::string> values;
};

#endif
