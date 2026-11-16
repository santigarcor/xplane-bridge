#include "xplm-stub.h"

#include <cstdio>
#include <cstring>
#include <map>
#include <memory>
#include <XPLMPlanes.h>
#include <XPLMPlugin.h>
#include <XPLMProcessing.h>
#include <XPLMUtilities.h>

namespace {

    struct FakeDataRef {
            std::string name;
            XPLMDataTypeID types = 0;
            double value = 0;
            std::vector<float> array;
    };

    struct FakeFlightLoop {
            XPLMFlightLoop_f callback;
            void *refcon;
    };

    std::map<std::string, std::unique_ptr<FakeDataRef>> dataRefs;
    std::map<std::string, std::unique_ptr<std::string>> commands;
    std::vector<std::string> commandEvents;
    std::vector<std::string> debugOutput;
    std::vector<std::unique_ptr<FakeFlightLoop>> flightLoops;
    std::string aircraftFile = "Cessna_172SP.acf";
    std::string aircraftPath = "C:\\X-Plane 12\\Aircraft\\Laminar Research\\Cessna 172 SP\\Cessna_172SP.acf";

    FakeDataRef *fake(XPLMDataRef ref) {
        return static_cast<FakeDataRef *>(ref);
    }

    const std::string &commandName(XPLMCommandRef ref) {
        return *static_cast<std::string *>(ref);
    }

}

namespace stub {

    void defineDataRef(const std::string &name, XPLMDataTypeID types, double value) {
        auto ref = std::make_unique<FakeDataRef>();
        ref->name = name;
        ref->types = types;
        ref->value = value;
        dataRefs[name] = std::move(ref);
    }

    void defineArrayDataRef(const std::string &name, std::vector<float> values) {
        auto ref = std::make_unique<FakeDataRef>();
        ref->name = name;
        ref->types = xplmType_FloatArray;
        ref->array = std::move(values);
        dataRefs[name] = std::move(ref);
    }

    void setValue(const std::string &name, double value) {
        dataRefs.at(name)->value = value;
    }

    double getValue(const std::string &name) {
        return dataRefs.at(name)->value;
    }

    void removeAllDataRefs() {
        dataRefs.clear();
    }

    std::vector<std::string> &commandLog() {
        return commandEvents;
    }

    void setAircraft(const std::string &fileName, const std::string &fullPath) {
        aircraftFile = fileName;
        aircraftPath = fullPath;
    }

    std::vector<std::string> &debugLines() {
        return debugOutput;
    }

    void runFlightLoops() {
        for (size_t i = 0; i < flightLoops.size(); i++) {
            if (flightLoops[i]) {
                flightLoops[i]->callback(0.016f, 0.016f, 0, flightLoops[i]->refcon);
            }
        }
    }

    size_t flightLoopCount() {
        size_t count = 0;
        for (const auto &loop : flightLoops) {
            count += loop ? 1 : 0;
        }
        return count;
    }

}

// ---- XPLMUtilities ----

XPLM_API void XPLMDebugString(const char *inString) {
    debugOutput.emplace_back(inString);
    fputs(inString, stdout);
    fflush(stdout);
}

XPLM_API void XPLMGetSystemPath(char *outSystemPath) {
    strcpy(outSystemPath, "C:\\X-Plane 12\\");
}

XPLM_API XPLMCommandRef XPLMFindCommand(const char *inName) {
    auto &slot = commands[inName];
    if (!slot) {
        slot = std::make_unique<std::string>(inName);
    }
    return slot.get();
}

XPLM_API void XPLMCommandOnce(XPLMCommandRef inCommand) {
    commandEvents.push_back("once:" + commandName(inCommand));
}

XPLM_API void XPLMCommandBegin(XPLMCommandRef inCommand) {
    commandEvents.push_back("begin:" + commandName(inCommand));
}

XPLM_API void XPLMCommandEnd(XPLMCommandRef inCommand) {
    commandEvents.push_back("end:" + commandName(inCommand));
}

// ---- XPLMDataAccess ----

XPLM_API XPLMDataRef XPLMFindDataRef(const char *inDataRefName) {
    auto it = dataRefs.find(inDataRefName);
    return it != dataRefs.end() ? it->second.get() : nullptr;
}

XPLM_API int XPLMCanWriteDataRef(XPLMDataRef) {
    return 1;
}

XPLM_API int XPLMIsDataRefGood(XPLMDataRef inDataRef) {
    for (const auto &[name, ref] : dataRefs) {
        if (ref.get() == inDataRef) {
            return 1;
        }
    }
    return 0;
}

XPLM_API XPLMDataTypeID XPLMGetDataRefTypes(XPLMDataRef inDataRef) {
    return fake(inDataRef)->types;
}

XPLM_API int XPLMGetDatai(XPLMDataRef inDataRef) {
    return static_cast<int>(fake(inDataRef)->value);
}

XPLM_API void XPLMSetDatai(XPLMDataRef inDataRef, int inValue) {
    fake(inDataRef)->value = inValue;
}

XPLM_API float XPLMGetDataf(XPLMDataRef inDataRef) {
    return static_cast<float>(fake(inDataRef)->value);
}

XPLM_API void XPLMSetDataf(XPLMDataRef inDataRef, float inValue) {
    fake(inDataRef)->value = inValue;
}

XPLM_API double XPLMGetDatad(XPLMDataRef inDataRef) {
    return fake(inDataRef)->value;
}

XPLM_API void XPLMSetDatad(XPLMDataRef inDataRef, double inValue) {
    fake(inDataRef)->value = inValue;
}

XPLM_API int XPLMGetDatavf(XPLMDataRef inDataRef, float *outValues, int inOffset, int inMax) {
    auto &array = fake(inDataRef)->array;
    if (outValues == nullptr) {
        return static_cast<int>(array.size());
    }
    int count = 0;
    for (int i = inOffset; i < static_cast<int>(array.size()) && count < inMax; i++, count++) {
        outValues[count] = array[static_cast<size_t>(i)];
    }
    return count;
}

XPLM_API void XPLMSetDatavf(XPLMDataRef inDataRef, float *inValues, int inOffset, int inCount) {
    auto &array = fake(inDataRef)->array;
    for (int i = 0; i < inCount && inOffset + i < static_cast<int>(array.size()); i++) {
        array[static_cast<size_t>(inOffset + i)] = inValues[i];
    }
}

XPLM_API int XPLMGetDatavi(XPLMDataRef, int *, int, int) {
    return 0;
}

XPLM_API void XPLMSetDatavi(XPLMDataRef, int *, int, int) {
}

XPLM_API int XPLMGetDatab(XPLMDataRef, void *, int, int) {
    return 0;
}

// ---- XPLMPlanes / XPLMPlugin ----

XPLM_API void XPLMGetNthAircraftModel(int, char *outFileName, char *outPath) {
    strcpy(outFileName, aircraftFile.c_str());
    strcpy(outPath, aircraftPath.c_str());
}

XPLM_API XPLMPluginID XPLMGetMyID(void) {
    return 1;
}

XPLM_API void XPLMGetPluginInfo(XPLMPluginID, char *outName, char *outFilePath, char *outSignature, char *outDescription) {
    if (outName) {
        outName[0] = '\0';
    }
    if (outFilePath) {
        outFilePath[0] = '\0';
    }
    if (outSignature) {
        outSignature[0] = '\0';
    }
    if (outDescription) {
        outDescription[0] = '\0';
    }
}

// ---- XPLMProcessing ----

XPLM_API XPLMFlightLoopID XPLMCreateFlightLoop(XPLMCreateFlightLoop_t *inParams) {
    flightLoops.push_back(std::make_unique<FakeFlightLoop>(FakeFlightLoop{inParams->callbackFunc, inParams->refcon}));
    return flightLoops.back().get();
}

XPLM_API void XPLMDestroyFlightLoop(XPLMFlightLoopID inFlightLoopID) {
    for (auto &loop : flightLoops) {
        if (loop.get() == inFlightLoopID) {
            loop.reset();
        }
    }
}

XPLM_API void XPLMScheduleFlightLoop(XPLMFlightLoopID, float, int) {
}
