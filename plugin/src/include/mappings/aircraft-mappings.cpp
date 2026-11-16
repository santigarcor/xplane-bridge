#include "aircraft-mappings.h"

#include "logger.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <XPLMUtilities.h>

static const AircraftProfile kProfiles[] = {
    {"zibo_737", "Zibo 737", "737", initZibo737Mappings},
    {"ff_757", "FF 757", "757", initFF757Mappings},
    {"ff_777", "FF 777", "777", initFF777Mappings},
    {"c172_g1000", "Cessna 172 G1000", "Cessna_172SP_G1000", initC172G1000Mappings},
    {"c172", "Cessna 172", "Cessna_172SP.acf", initC172Mappings},
};

const AircraftProfile *findAircraftProfile(const std::string &relativePath) {
    for (const auto &profile : kProfiles) {
        try {
            if (std::regex_search(relativePath, std::regex(profile.pattern, std::regex::icase))) {
                return &profile;
            }
        } catch (const std::regex_error &e) {
            Logger::getInstance().error("Invalid aircraft pattern \"%s\": %s", profile.pattern, e.what());
        }
    }
    return nullptr;
}

std::string relativeAircraftPath(const std::string &fullPath) {
    char systemPath[2048] = {};
    XPLMGetSystemPath(systemPath);

    auto normalize = [](std::string path) {
        std::replace(path.begin(), path.end(), '\\', '/');
        return path;
    };

    std::string path = normalize(fullPath);
    std::string root = normalize(systemPath);

    auto lower = [](std::string text) {
        std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return text;
    };

    if (!root.empty() && lower(path).starts_with(lower(root))) {
        path = path.substr(root.size());
    }
    while (path.starts_with("/")) {
        path.erase(0, 1);
    }
    return path;
}
