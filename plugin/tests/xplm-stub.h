#ifndef XPLM_STUB_H
#define XPLM_STUB_H

// A fake X-Plane for the host test: the XPLM functions the bridge uses, backed by an in-memory
// dataref table and a command log. Not part of the plugin.

#include <string>
#include <vector>
#include <XPLMDataAccess.h>

namespace stub {

    void defineDataRef(const std::string &name, XPLMDataTypeID types, double value = 0);
    void defineArrayDataRef(const std::string &name, std::vector<float> values);
    void setValue(const std::string &name, double value);
    double getValue(const std::string &name);
    void removeAllDataRefs();

    // Every XPLMCommandOnce/Begin/End as "once:<name>", "begin:<name>", "end:<name>"
    std::vector<std::string> &commandLog();

    // What XPLMGetNthAircraftModel(0) returns. The system path is C:\X-Plane 12\.
    void setAircraft(const std::string &fileName, const std::string &fullPath);

    // Lines passed to XPLMDebugString
    std::vector<std::string> &debugLines();

    // Runs the registered flight loops once, like a sim frame does
    void runFlightLoops();
    size_t flightLoopCount();

}

#endif
