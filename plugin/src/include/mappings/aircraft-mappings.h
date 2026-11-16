#ifndef AIRCRAFT_MAPPINGS_H
#define AIRCRAFT_MAPPINGS_H

#include <string>

class Bridge;

// A supported aircraft. Port of src/mappings/index.ts and detector.ts.
struct AircraftProfile {
        // Same ids as the TypeScript SupportedAircraft type
        const char *id;
        const char *label;
        // Case-insensitive regex matched against the aircraft path relative to the X-Plane folder,
        // e.g. "Aircraft/Laminar Research/Cessna 172 SP/Cessna_172SP_G1000.acf"
        const char *pattern;
        void (*initialize)(Bridge &bridge);
};

// First profile whose pattern matches, in table order (the G1000 C172 must come before the plain one)
const AircraftProfile *findAircraftProfile(const std::string &relativePath);

// The .acf path relative to the X-Plane folder with forward slashes, so a digit in the install
// folder's name can't match an aircraft pattern
std::string relativeAircraftPath(const std::string &fullPath);

// One per file in mappings/profiles/
void initZibo737Mappings(Bridge &bridge);
void initFF757Mappings(Bridge &bridge);
void initFF777Mappings(Bridge &bridge);
void initC172Mappings(Bridge &bridge);
void initC172G1000Mappings(Bridge &bridge);

#endif
