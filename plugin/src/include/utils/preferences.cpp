#include "preferences.h"

#include "win-utils.h"

#include <filesystem>
#include <fstream>
#include <system_error>

static std::string trim(const std::string &text) {
    const char *whitespace = " \t\r\n";
    size_t start = text.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(whitespace);
    return text.substr(start, end - start + 1);
}

Preferences::Preferences(std::string filePath) : filePath(std::move(filePath)) {
    load();
}

void Preferences::load() {
    std::ifstream file(utf8ToPath(filePath));
    if (!file) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';' || line[0] == '[') {
            continue;
        }

        size_t separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, separator));
        if (!key.empty()) {
            values[key] = trim(line.substr(separator + 1));
        }
    }
}

bool Preferences::save() const {
    const std::filesystem::path path = utf8ToPath(filePath);
    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);

    std::ofstream file(path, std::ios::trunc);
    if (!file) {
        return false;
    }

    file << "# XPlane Bridge preferences\n";
    for (const auto &[key, value] : values) {
        file << key << "=" << value << "\n";
    }
    return static_cast<bool>(file);
}

std::string Preferences::get(const std::string &key, const std::string &defaultValue) const {
    auto it = values.find(key);
    return it != values.end() && !it->second.empty() ? it->second : defaultValue;
}

int Preferences::getInt(const std::string &key, int defaultValue) const {
    try {
        return std::stoi(get(key, std::to_string(defaultValue)));
    } catch (...) {
        return defaultValue;
    }
}

bool Preferences::getBool(const std::string &key, bool defaultValue) const {
    std::string value = get(key, defaultValue ? "1" : "0");
    return value == "1" || value == "true" || value == "yes" || value == "on";
}

bool Preferences::set(const std::string &key, const std::string &value) {
    values[key] = value;
    return save();
}
