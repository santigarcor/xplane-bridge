#ifndef VALUE_H
#define VALUE_H

#include <cmath>
#include <functional>
#include <string>
#include <variant>
#include <vector>

// A dataref value as the bridge sees it: a number (int, float and double datarefs), an array (int and
// float array datarefs) or a string (byte datarefs). Parsers map one Value to another before it's
// sent to the Arduino.
using Value = std::variant<double, std::vector<double>, std::string>;

using Parser = std::function<Value(const Value &)>;

// The number in a Value, or NaN if it holds an array or a string. NaN compares false with
// everything, so `num(v) > 0` behaves like JavaScript's `v > 0` did for non-numbers.
inline double num(const Value &value) {
    if (const double *number = std::get_if<double>(&value)) {
        return *number;
    }
    return std::nan("");
}

// For log lines
std::string describeValue(const Value &value);

#endif
