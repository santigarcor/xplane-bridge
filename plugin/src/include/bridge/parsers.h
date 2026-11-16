#ifndef PARSERS_H
#define PARSERS_H

#include "value.h"

#include <cmath>
#include <initializer_list>
#include <map>
#include <numbers>
#include <utility>
#include <vector>

// Port of src/bridge/helpers.ts. Parsers turn a raw dataref value into what the Arduino expects.
// Combine them with pipe() when one step isn't enough, e.g. pipe(atIndex(0), threshold) for an
// array dataref.
namespace parsers {

    // Math.round: halves round up (towards +infinity), unlike std::round
    inline double jsRound(double value) {
        return std::floor(value + 0.5);
    }

    // v > 0 ? 1 : 0
    inline Value toBoolean(const Value &value) {
        return num(value) > 0 ? 1.0 : 0.0;
    }

    inline Value round(const Value &value) {
        if (!std::holds_alternative<double>(value)) {
            return value;
        }
        return jsRound(num(value));
    }

    // Radians to whole degrees
    inline Value toDegrees(const Value &value) {
        if (!std::holds_alternative<double>(value)) {
            return value;
        }
        return jsRound(num(value) * (180.0 / std::numbers::pi));
    }

    inline Value none(const Value &value) {
        return value;
    }

    // Element `index` of an array value; any other value passes through
    inline Parser atIndex(size_t index) {
        return [index](const Value &value) -> Value {
            if (const auto *array = std::get_if<std::vector<double>>(&value)) {
                return index < array->size() ? Value((*array)[index]) : Value(std::nan(""));
            }
            return value;
        };
    }

    // Replaces numbers found in the map, leaving any other value unchanged
    using ValueMap = std::map<double, double>;
    inline Parser valueMap(ValueMap map) {
        return [map = std::move(map)](const Value &value) -> Value {
            if (const double *number = std::get_if<double>(&value)) {
                auto it = map.find(*number);
                if (it != map.end()) {
                    return it->second;
                }
            }
            return value;
        };
    }

    // Runs the parsers left to right, feeding each one the previous result
    inline Parser pipe(std::initializer_list<Parser> steps) {
        return [steps = std::vector<Parser>(steps)](const Value &value) -> Value {
            Value result = value;
            for (const auto &step : steps) {
                result = step(result);
            }
            return result;
        };
    }

}

#endif
