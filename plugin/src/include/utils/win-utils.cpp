#include "win-utils.h"

std::wstring utf8ToWide(const std::string &utf8) {
    if (utf8.empty()) {
        return {};
    }

    int length = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
    if (length <= 0) {
        return {};
    }

    std::wstring wide(static_cast<size_t>(length), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), wide.data(), length);
    return wide;
}

std::string wideToUtf8(const std::wstring &wide) {
    if (wide.empty()) {
        return {};
    }

    int length = WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);
    if (length <= 0) {
        return {};
    }

    std::string utf8(static_cast<size_t>(length), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), utf8.data(), length, nullptr, nullptr);
    return utf8;
}

std::string win32ErrorMessage(DWORD error) {
    wchar_t *buffer = nullptr;
    DWORD length = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error, 0, reinterpret_cast<wchar_t *>(&buffer), 0, nullptr);

    std::string message;
    if (length > 0 && buffer != nullptr) {
        message = wideToUtf8(std::wstring(buffer, length));
    }
    if (buffer != nullptr) {
        LocalFree(buffer);
    }

    while (!message.empty() && (message.back() == '\n' || message.back() == '\r' || message.back() == ' ' || message.back() == '.')) {
        message.pop_back();
    }

    return "code " + std::to_string(error) + (message.empty() ? "" : ", " + message);
}
