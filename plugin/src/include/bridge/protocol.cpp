#include "protocol.h"

#include <charconv>
#include <cmath>
#include <cstdlib>

namespace protocol {

    namespace {

        // A small parser for the flat objects the sketch sends. Nested objects and arrays aren't
        // needed, so they make the whole line invalid.
        class FlatObjectParser {
            public:
                explicit FlatObjectParser(std::string_view text) : text(text) {}

                bool parse(IncomingMessage &message) {
                    skipWhitespace();
                    if (!consume('{')) {
                        return false;
                    }

                    skipWhitespace();
                    if (consume('}')) {
                        return true;
                    }

                    while (true) {
                        std::string key;
                        skipWhitespace();
                        if (!parseString(key)) {
                            return false;
                        }

                        skipWhitespace();
                        if (!consume(':')) {
                            return false;
                        }

                        skipWhitespace();
                        if (peek() == '"') {
                            std::string value;
                            if (!parseString(value)) {
                                return false;
                            }
                            if (key == "user_input") {
                                message.userInput = std::move(value);
                            }
                        } else if (peek() == 't' || peek() == 'f' || peek() == 'n') {
                            if (!skipLiteral()) {
                                return false;
                            }
                        } else {
                            double value = 0;
                            if (!parseNumber(value)) {
                                return false;
                            }
                            if (key == "hb") {
                                message.heartbeat = value;
                            }
                        }

                        skipWhitespace();
                        if (consume(',')) {
                            continue;
                        }
                        return consume('}');
                    }
                }

            private:
                char peek() const {
                    return position < text.size() ? text[position] : '\0';
                }

                bool consume(char expected) {
                    if (peek() != expected) {
                        return false;
                    }
                    position++;
                    return true;
                }

                void skipWhitespace() {
                    while (position < text.size() && (text[position] == ' ' || text[position] == '\t' || text[position] == '\r' || text[position] == '\n')) {
                        position++;
                    }
                }

                bool parseString(std::string &out) {
                    if (!consume('"')) {
                        return false;
                    }

                    while (position < text.size()) {
                        char c = text[position++];
                        if (c == '"') {
                            return true;
                        }
                        if (c != '\\') {
                            out.push_back(c);
                            continue;
                        }
                        if (position >= text.size()) {
                            return false;
                        }
                        char escaped = text[position++];
                        switch (escaped) {
                            case 'n':
                                out.push_back('\n');
                                break;
                            case 't':
                                out.push_back('\t');
                                break;
                            case 'r':
                                out.push_back('\r');
                                break;
                            case 'b':
                                out.push_back('\b');
                                break;
                            case 'f':
                                out.push_back('\f');
                                break;
                            case 'u':
                                // Not used by the sketch; keep the line parseable
                                if (position + 4 > text.size()) {
                                    return false;
                                }
                                position += 4;
                                out.push_back('?');
                                break;
                            default:
                                out.push_back(escaped);
                                break;
                        }
                    }
                    return false;
                }

                bool parseNumber(double &out) {
                    const char *begin = text.data() + position;
                    const char *end = text.data() + text.size();
                    auto [pointer, ec] = std::from_chars(begin, end, out);
                    if (ec != std::errc() || pointer == begin) {
                        return false;
                    }
                    position += static_cast<size_t>(pointer - begin);
                    return true;
                }

                bool skipLiteral() {
                    for (std::string_view literal : {"true", "false", "null"}) {
                        if (text.substr(position, literal.size()) == literal) {
                            position += literal.size();
                            return true;
                        }
                    }
                    return false;
                }

                std::string_view text;
                size_t position = 0;
        };

        void appendEscaped(std::string &out, const std::string &text) {
            out.push_back('"');
            for (char c : text) {
                switch (c) {
                    case '"':
                        out += "\\\"";
                        break;
                    case '\\':
                        out += "\\\\";
                        break;
                    case '\n':
                        out += "\\n";
                        break;
                    case '\r':
                        out += "\\r";
                        break;
                    case '\t':
                        out += "\\t";
                        break;
                    default:
                        if (static_cast<unsigned char>(c) < 0x20) {
                            char buffer[8];
                            snprintf(buffer, sizeof(buffer), "\\u%04x", static_cast<unsigned char>(c));
                            out += buffer;
                        } else {
                            out.push_back(c);
                        }
                        break;
                }
            }
            out.push_back('"');
        }

        void appendValue(std::string &out, const Value &value) {
            if (const double *number = std::get_if<double>(&value)) {
                out += formatNumber(*number);
            } else if (const auto *array = std::get_if<std::vector<double>>(&value)) {
                out.push_back('[');
                for (size_t i = 0; i < array->size(); i++) {
                    if (i > 0) {
                        out.push_back(',');
                    }
                    out += formatNumber((*array)[i]);
                }
                out.push_back(']');
            } else {
                appendEscaped(out, std::get<std::string>(value));
            }
        }

    }

    bool parseIncoming(std::string_view line, IncomingMessage &message) {
        message = {};
        return FlatObjectParser(line).parse(message);
    }

    std::string formatNumber(double value) {
        if (!std::isfinite(value)) {
            return "null";
        }
        if (value == 0) {
            // Also turns -0 into 0, as JavaScript does
            return "0";
        }

        char buffer[64];
        auto [pointer, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);
        if (ec != std::errc()) {
            return "null";
        }
        return std::string(buffer, pointer);
    }

    std::string buildOutgoing(const std::string &cmd, const Value &value) {
        std::string line = "{\"cmd\":";
        appendEscaped(line, cmd);
        line += ",\"value\":";
        appendValue(line, value);
        line += "}";
        return line;
    }

}

std::string describeValue(const Value &value) {
    if (const double *number = std::get_if<double>(&value)) {
        return protocol::formatNumber(*number);
    }
    if (const auto *array = std::get_if<std::vector<double>>(&value)) {
        std::string text = "[";
        for (size_t i = 0; i < array->size() && i < 8; i++) {
            text += (i > 0 ? "," : "") + protocol::formatNumber((*array)[i]);
        }
        return text + (array->size() > 8 ? ",...]" : "]");
    }
    return "\"" + std::get<std::string>(value) + "\"";
}
