#include "bridge.h"

#include "aircraft-mappings.h"
#include "logger.h"
#include "parsers.h"
#include "protocol.h"

#include <algorithm>
#include <cmath>
#include <XPLMPlanes.h>

// Add-on datarefs can appear a while after the aircraft loads, so missing ones are only reported
// after this long
static constexpr std::chrono::seconds kMissingDataRefGrace{10};
static constexpr std::chrono::seconds kResolveRetryInterval{5};
// Array and byte datarefs are capped at this many elements, which is plenty for panel values
static constexpr int kMaxArrayElements = 64;
static constexpr int kMaxDataBytes = 1024;

static std::string joinNames(const std::vector<std::string> &names) {
    std::string joined;
    for (const auto &name : names) {
        joined += (joined.empty() ? "" : ", ") + name;
    }
    return joined;
}

Bridge::Bridge(SerialLink &link) : link(link) {
}

Bridge::~Bridge() {
    // Never leave a command held down in the sim
    releaseHeldCommands(true);
}

// ---------------------------------------------------------------------------------------------
// Mapping API
// ---------------------------------------------------------------------------------------------

void Bridge::addDataRef(const std::string &dataRefName, DataRefMapping mapping) {
    Logger::getInstance().debug("Mapping added: %s -> %s (threshold %g)", dataRefName.c_str(), mapping.cmd.c_str(), mapping.threshold);

    // Same dataref added twice: the last mapping wins, like the object keys in TypeScript
    auto existing = std::find_if(datarefs.begin(), datarefs.end(), [&](const DataRefEntry &entry) {
        return entry.name == dataRefName;
    });
    if (existing != datarefs.end()) {
        existing->mapping = std::move(mapping);
        return;
    }

    DataRefEntry entry;
    entry.name = dataRefName;
    entry.mapping = std::move(mapping);
    datarefs.push_back(std::move(entry));
}

void Bridge::addBooleanDataRef(const std::string &dataRefName, const std::string &arduinoCmd) {
    addDataRef(dataRefName, {.cmd = arduinoCmd, .threshold = 0, .parser = parsers::toBoolean});
}

void Bridge::addValueMapDataRef(const std::string &dataRefName, const std::string &arduinoCmd, std::map<double, double> valueMap) {
    addDataRef(dataRefName, {.cmd = arduinoCmd, .threshold = 0, .parser = parsers::valueMap(std::move(valueMap))});
}

void Bridge::addToggleSwitchInputDataRef(const std::string &switchName, Names dataRefNames, bool inverse) {
    inputs[switchName + "_on"] = {.type = InputMapping::Type::DataRef, .actions = dataRefNames.items, .value = inverse ? 0.0 : 1.0};
    inputs[switchName + "_off"] = {.type = InputMapping::Type::DataRef, .actions = dataRefNames.items, .value = inverse ? 1.0 : 0.0};
    Logger::getInstance().debug("Toggle switch mapping added: %s -> %s", switchName.c_str(), joinNames(dataRefNames.items).c_str());
}

void Bridge::addToggleSwitchInputCommands(const std::string &switchName, Names onCommands, std::optional<Names> offCommands) {
    std::vector<std::string> off = offCommands ? offCommands->items : onCommands.items;
    inputs[switchName + "_on"] = {.type = InputMapping::Type::Command, .actions = onCommands.items};
    inputs[switchName + "_off"] = {.type = InputMapping::Type::Command, .actions = off};
    Logger::getInstance().debug("Toggle switch mapping added: %s -> %s / %s", switchName.c_str(), joinNames(onCommands.items).c_str(), joinNames(off).c_str());
}

void Bridge::addMomentarySwitchInputCommand(const std::string &switchName, Names commands, double duration) {
    inputs[switchName] = {.type = InputMapping::Type::Command, .actions = commands.items, .duration = duration};
    Logger::getInstance().debug("Momentary switch mapping added: %s -> %s", switchName.c_str(), joinNames(commands.items).c_str());
}

void Bridge::addMomentarySwitchInputDataRef(const std::string &switchName, Names dataRefNames, double value) {
    inputs[switchName] = {.type = InputMapping::Type::DataRef, .actions = dataRefNames.items, .value = value};
    Logger::getInstance().debug("Momentary switch mapping added: %s -> %s", switchName.c_str(), joinNames(dataRefNames.items).c_str());
}

void Bridge::addMomentarySwitchInputDataRef(const std::string &switchName, Names dataRefNames, ToggleValue) {
    inputs[switchName] = {.type = InputMapping::Type::DataRef, .actions = dataRefNames.items, .toggle = true};
    Logger::getInstance().debug("Momentary switch mapping added: %s -> %s (toggle)", switchName.c_str(), joinNames(dataRefNames.items).c_str());
}

void Bridge::addRotaryEncoderCommands(const std::string &encoderName, Names incrementCommands, Names decrementCommands) {
    inputs[encoderName + "_increment"] = {.type = InputMapping::Type::Command, .actions = incrementCommands.items};
    inputs[encoderName + "_decrement"] = {.type = InputMapping::Type::Command, .actions = decrementCommands.items};
    Logger::getInstance().debug("Rotary encoder mapping added: %s -> %s / %s", encoderName.c_str(), joinNames(incrementCommands.items).c_str(), joinNames(decrementCommands.items).c_str());
}

// ---------------------------------------------------------------------------------------------
// Aircraft
// ---------------------------------------------------------------------------------------------

void Bridge::detectAircraft(bool force) {
    char fileName[512] = {};
    char path[2048] = {};
    XPLMGetNthAircraftModel(0, fileName, path);

    const std::string relativePath = relativeAircraftPath(path);
    const AircraftProfile *profile = findAircraftProfile(relativePath);

    if (profile == nullptr) {
        if (currentAircraft != nullptr || unsupportedAcf != fileName) {
            Logger::getInstance().warn("No mappings for this aircraft: %s", relativePath.c_str());
        }
        unloadAircraft();
        unsupportedAcf = fileName;
        return;
    }

    if (!force && profile == currentAircraft) {
        return;
    }

    Logger::getInstance().info("Detected aircraft: %s (%s)", profile->id, relativePath.c_str());

    releaseHeldCommands(true);
    clearMappings();
    unsupportedAcf.clear();
    currentAircraft = profile;
    profile->initialize(*this);

    detectedAt = Clock::now();
    missingDataRefsLogged = false;
    resolveDataRefs();
    nextResolveAttempt = Clock::now() + kResolveRetryInterval;

    Logger::getInstance().info("Loaded %zu dataref and %zu input mappings, %zu datarefs found so far", datarefs.size(), inputs.size(), resolvedDataRefCount());
}

void Bridge::unloadAircraft() {
    releaseHeldCommands(true);
    clearMappings();
    currentAircraft = nullptr;
    unsupportedAcf.clear();
}

void Bridge::clearMappings() {
    datarefs.clear();
    inputs.clear();
    previousValues.clear();
}

void Bridge::resolvePendingDataRefs() {
    if (resolvedDataRefCount() < datarefs.size()) {
        resolveDataRefs();
    }
}

void Bridge::resolveDataRefs() {
    for (auto &entry : datarefs) {
        if (entry.ref != nullptr && XPLMIsDataRefGood(entry.ref)) {
            continue;
        }

        entry.ref = nullptr;
        XPLMDataRef ref = XPLMFindDataRef(entry.name.c_str());
        if (ref == nullptr || !XPLMIsDataRefGood(ref)) {
            continue;
        }

        XPLMDataTypeID types = XPLMGetDataRefTypes(ref);
        if (types == xplmType_Unknown) {
            continue;
        }

        entry.ref = ref;
        entry.types = types;
        entry.errorLogged = false;
        Logger::getInstance().debug("Found dataref %s (types 0x%x)", entry.name.c_str(), static_cast<unsigned>(types));
    }
}

size_t Bridge::resolvedDataRefCount() const {
    return static_cast<size_t>(std::count_if(datarefs.begin(), datarefs.end(), [](const DataRefEntry &entry) {
        return entry.ref != nullptr;
    }));
}

std::string Bridge::aircraftLabel() const {
    return currentAircraft != nullptr ? currentAircraft->label : "";
}

std::string Bridge::mappingError() const {
    if (currentAircraft == nullptr) {
        return unsupportedAcf.empty() ? "" : "aircraft not supported: " + unsupportedAcf;
    }

    size_t missing = datarefs.size() - resolvedDataRefCount();
    if (missing == 0 || Clock::now() - detectedAt < kMissingDataRefGrace) {
        return "";
    }
    return std::to_string(missing) + (missing == 1 ? " dataref not found" : " datarefs not found") + " (see log)";
}

// ---------------------------------------------------------------------------------------------
// X-Plane -> Arduino
// ---------------------------------------------------------------------------------------------

void Bridge::update() {
    releaseHeldCommands(false);

    auto now = Clock::now();
    if (resolvedDataRefCount() < datarefs.size()) {
        if (now >= nextResolveAttempt) {
            resolveDataRefs();
            nextResolveAttempt = now + kResolveRetryInterval;
        }

        if (!missingDataRefsLogged && now - detectedAt >= kMissingDataRefGrace) {
            missingDataRefsLogged = true;
            for (const auto &entry : datarefs) {
                if (entry.ref == nullptr) {
                    Logger::getInstance().error("Dataref not found: %s (for %s). Still retrying.", entry.name.c_str(), entry.mapping.cmd.c_str());
                }
            }
        }
    }

    for (auto &entry : datarefs) {
        if (entry.ref == nullptr || !readValue(entry)) {
            continue;
        }

        const std::string &cmd = entry.mapping.cmd;
        if (cmd.empty()) {
            if (!entry.errorLogged) {
                Logger::getInstance().warn("Command not set for dataref \"%s\"", entry.name.c_str());
                entry.errorLogged = true;
            }
            continue;
        }

        // Parsers run every frame, not only when the raw value changes, so stateful ones that look
        // at several datarefs (the 757 FD master LED) always see the latest state
        Value parsed;
        try {
            parsed = entry.mapping.parser ? entry.mapping.parser(entry.raw) : entry.raw;
        } catch (const std::exception &e) {
            if (!entry.errorLogged) {
                Logger::getInstance().error("Error parsing the value of \"%s\": %s", entry.name.c_str(), e.what());
                entry.errorLogged = true;
            }
            continue;
        }

        if (!shouldSendUpdate(cmd, parsed, entry.mapping.threshold)) {
            continue;
        }

        sendToArduino(cmd, parsed);
        previousValues[cmd] = {std::move(parsed), entry.name};
    }
}

bool Bridge::readValue(DataRefEntry &entry) {
    const XPLMDataTypeID types = entry.types;

    if (types & (xplmType_Double | xplmType_Float | xplmType_Int)) {
        double value;
        if (types & xplmType_Double) {
            value = XPLMGetDatad(entry.ref);
        } else if (types & xplmType_Float) {
            value = XPLMGetDataf(entry.ref);
        } else {
            value = XPLMGetDatai(entry.ref);
        }

        if (double *current = std::get_if<double>(&entry.raw)) {
            *current = value;
        } else {
            entry.raw = value;
        }
        return true;
    }

    if (types & (xplmType_FloatArray | xplmType_IntArray)) {
        if (!std::holds_alternative<std::vector<double>>(entry.raw)) {
            entry.raw = std::vector<double>();
        }
        auto &values = std::get<std::vector<double>>(entry.raw);

        if (types & xplmType_FloatArray) {
            int count = std::min(XPLMGetDatavf(entry.ref, nullptr, 0, 0), kMaxArrayElements);
            entry.floatBuffer.resize(static_cast<size_t>(std::max(count, 0)));
            count = XPLMGetDatavf(entry.ref, entry.floatBuffer.data(), 0, count);
            values.assign(entry.floatBuffer.begin(), entry.floatBuffer.begin() + std::max(count, 0));
        } else {
            int count = std::min(XPLMGetDatavi(entry.ref, nullptr, 0, 0), kMaxArrayElements);
            entry.intBuffer.resize(static_cast<size_t>(std::max(count, 0)));
            count = XPLMGetDatavi(entry.ref, entry.intBuffer.data(), 0, count);
            values.assign(entry.intBuffer.begin(), entry.intBuffer.begin() + std::max(count, 0));
        }
        return true;
    }

    if (types & xplmType_Data) {
        // Byte datarefs hold text (the web API sent them base64 encoded, here they come raw)
        char buffer[kMaxDataBytes];
        int count = XPLMGetDatab(entry.ref, buffer, 0, kMaxDataBytes);
        std::string text(buffer, static_cast<size_t>(std::clamp(count, 0, kMaxDataBytes)));
        text.resize(std::min(text.size(), text.find('\0')));
        entry.raw = std::move(text);
        return true;
    }

    return false;
}

bool Bridge::shouldSendUpdate(const std::string &cmd, const Value &value, double threshold) const {
    auto it = previousValues.find(cmd);
    if (it == previousValues.end()) {
        return true;
    }

    const Value &old = it->second.parsed;
    if (value == old) {
        // The Node bridge only got updates when X-Plane saw a change. Polling every frame, an
        // unchanged value must never be resent, even with threshold 0.
        return false;
    }

    const double *newNumber = std::get_if<double>(&value);
    const double *oldNumber = std::get_if<double>(&old);
    if (newNumber != nullptr && oldNumber != nullptr) {
        if (std::isnan(*newNumber) || std::isnan(*oldNumber)) {
            return std::isnan(*newNumber) != std::isnan(*oldNumber);
        }
        return std::fabs(*newNumber - *oldNumber) >= threshold;
    }

    return true;
}

void Bridge::sendToArduino(const std::string &cmd, const Value &value) {
    if (link.isConnected()) {
        link.send(cmd, protocol::buildOutgoing(cmd, value));
    }
}

void Bridge::resendState() {
    for (const auto &[cmd, previous] : previousValues) {
        sendToArduino(cmd, previous.parsed);
    }
    Logger::getInstance().debug("Resent %zu values to the Arduino", previousValues.size());
}

// ---------------------------------------------------------------------------------------------
// Arduino -> X-Plane
// ---------------------------------------------------------------------------------------------

void Bridge::handleSerialEvent(const SerialEvent &event) {
    switch (event.type) {
        case SerialEvent::Type::Connected:
        case SerialEvent::Type::Reset:
            resendState();
            break;
        case SerialEvent::Type::Disconnected:
            break;
        case SerialEvent::Type::Input:
            handleInput(event.text);
            break;
    }
}

void Bridge::handleInput(const std::string &name) {
    auto it = inputs.find(name);
    if (it == inputs.end()) {
        if (currentAircraft != nullptr) {
            Logger::getInstance().warn("Unknown Arduino input: %s", name.c_str());
        }
        return;
    }

    // Copied: running a command can make an aircraft plugin trigger a reload of the mappings
    const InputMapping mapping = it->second;
    if (mapping.type == InputMapping::Type::Command) {
        executeCommands(mapping.actions, mapping.duration);
    } else if (mapping.toggle) {
        setDataRefs(mapping.actions, std::nullopt);
    } else {
        setDataRefs(mapping.actions, mapping.value);
    }
}

void Bridge::executeCommands(const std::vector<std::string> &commands, double duration) {
    std::vector<XPLMCommandRef> refs;
    for (const auto &name : commands) {
        XPLMCommandRef ref = findCommand(name);
        if (ref == nullptr) {
            Logger::getInstance().error("Command \"%s\" not found in X-Plane", name.c_str());
            return;
        }
        refs.push_back(ref);
    }

    for (XPLMCommandRef ref : refs) {
        if (duration <= 0) {
            XPLMCommandOnce(ref);
        } else {
            XPLMCommandBegin(ref);
            auto hold = std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(duration));
            heldCommands.push_back({ref, Clock::now() + hold});
        }
    }

    Logger::getInstance().debug("X-Plane commands \"%s\" (duration %g)", joinNames(commands).c_str(), duration);
}

void Bridge::setDataRefs(const std::vector<std::string> &dataRefNames, std::optional<double> value) {
    std::vector<XPLMDataRef> refs;
    for (const auto &name : dataRefNames) {
        XPLMDataRef ref = findWritableDataRef(name);
        if (ref == nullptr) {
            Logger::getInstance().error("Dataref \"%s\" not found in X-Plane", name.c_str());
            return;
        }
        refs.push_back(ref);
    }

    if (refs.empty()) {
        return;
    }

    if (!value) {
        // Toggle: flip the first dataref and write the result to all of them, so they stay in step
        XPLMDataTypeID types = XPLMGetDataRefTypes(refs.front());
        double current;
        if (types & xplmType_Int) {
            current = XPLMGetDatai(refs.front());
        } else if (types & xplmType_Double) {
            current = XPLMGetDatad(refs.front());
        } else if (types & xplmType_Float) {
            current = XPLMGetDataf(refs.front());
        } else {
            Logger::getInstance().error("Dataref \"%s\" can't be toggled, it isn't a number", dataRefNames.front().c_str());
            return;
        }

        if (current != std::floor(current)) {
            Logger::getInstance().error("Dataref \"%s\" has a non-integer value: %g", dataRefNames.front().c_str(), current);
            return;
        }
        value = current == 0 ? 1.0 : 0.0;
    }

    for (size_t i = 0; i < refs.size(); i++) {
        XPLMDataRef ref = refs[i];
        XPLMDataTypeID types = XPLMGetDataRefTypes(ref);

        if (!XPLMCanWriteDataRef(ref)) {
            Logger::getInstance().warn("Dataref \"%s\" is read-only, writing anyway", dataRefNames[i].c_str());
        }

        if (types & xplmType_Int) {
            XPLMSetDatai(ref, static_cast<int>(std::lround(*value)));
        } else if (types & xplmType_Double) {
            XPLMSetDatad(ref, *value);
        } else if (types & xplmType_Float) {
            XPLMSetDataf(ref, static_cast<float>(*value));
        } else if (types & xplmType_IntArray) {
            int element = static_cast<int>(std::lround(*value));
            XPLMSetDatavi(ref, &element, 0, 1);
        } else if (types & xplmType_FloatArray) {
            float element = static_cast<float>(*value);
            XPLMSetDatavf(ref, &element, 0, 1);
        } else {
            Logger::getInstance().error("Dataref \"%s\" can't be set to a number", dataRefNames[i].c_str());
        }
    }

    Logger::getInstance().debug("X-Plane datarefs set: \"%s\" = %g", joinNames(dataRefNames).c_str(), *value);
}

XPLMCommandRef Bridge::findCommand(const std::string &name) {
    auto it = commandCache.find(name);
    if (it != commandCache.end()) {
        return it->second;
    }

    // Command refs stay valid for the whole session, so only found ones are cached
    XPLMCommandRef ref = XPLMFindCommand(name.c_str());
    if (ref != nullptr) {
        commandCache[name] = ref;
    }
    return ref;
}

XPLMDataRef Bridge::findWritableDataRef(const std::string &name) {
    auto it = dataRefCache.find(name);
    if (it != dataRefCache.end() && XPLMIsDataRefGood(it->second)) {
        return it->second;
    }

    XPLMDataRef ref = XPLMFindDataRef(name.c_str());
    if (ref == nullptr || !XPLMIsDataRefGood(ref)) {
        return nullptr;
    }
    dataRefCache[name] = ref;
    return ref;
}

void Bridge::releaseHeldCommands(bool all) {
    if (heldCommands.empty()) {
        return;
    }

    auto now = Clock::now();
    auto due = std::stable_partition(heldCommands.begin(), heldCommands.end(), [&](const HeldCommand &held) {
        return !all && held.releaseAt > now;
    });

    for (auto it = due; it != heldCommands.end(); ++it) {
        XPLMCommandEnd(it->ref);
    }
    heldCommands.erase(due, heldCommands.end());
}
