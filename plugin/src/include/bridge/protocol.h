#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "value.h"

#include <optional>
#include <string>
#include <string_view>

// The line-delimited JSON spoken with arduino/arduino.ino.
//   Arduino -> plugin: {"user_input":"<name>"}   a switch, button or encoder event
//                      {"hb":<uptime seconds>}   heartbeat, every 2 s
//   plugin -> Arduino: {"cmd":"<name>","value":<value>}
namespace protocol {

    struct IncomingMessage {
            std::optional<std::string> userInput;
            std::optional<double> heartbeat;
    };

    // False if the line isn't a JSON object (boot noise, debug prints)
    bool parseIncoming(std::string_view line, IncomingMessage &message);

    std::string buildOutgoing(const std::string &cmd, const Value &value);

    // Formats a number the way JavaScript's JSON.stringify does: integers have no decimal point,
    // other values use the shortest form that round-trips, NaN and infinities become null.
    std::string formatNumber(double value);

}

#endif
