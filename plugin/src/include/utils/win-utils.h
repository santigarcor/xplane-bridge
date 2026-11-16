#ifndef WIN_UTILS_H
#define WIN_UTILS_H

#include <filesystem>
#include <string>
#include <windows.h>

// Owns a Win32 HANDLE and closes it once. Both INVALID_HANDLE_VALUE and nullptr mean "no handle",
// since CreateFile returns the first and most other APIs return the second.
class UniqueHandle {
    public:
        UniqueHandle() = default;
        explicit UniqueHandle(HANDLE handle) : handle(handle) {}
        ~UniqueHandle() {
            reset();
        }

        UniqueHandle(const UniqueHandle &) = delete;
        UniqueHandle &operator=(const UniqueHandle &) = delete;

        UniqueHandle(UniqueHandle &&other) noexcept : handle(other.release()) {}
        UniqueHandle &operator=(UniqueHandle &&other) noexcept {
            if (this != &other) {
                reset(other.release());
            }
            return *this;
        }

        HANDLE get() const {
            return handle;
        }

        bool valid() const {
            return handle != nullptr && handle != INVALID_HANDLE_VALUE;
        }

        HANDLE release() {
            HANDLE released = handle;
            handle = INVALID_HANDLE_VALUE;
            return released;
        }

        void reset(HANDLE newHandle = INVALID_HANDLE_VALUE) {
            if (valid()) {
                CloseHandle(handle);
            }
            handle = newHandle;
        }

    private:
        HANDLE handle = INVALID_HANDLE_VALUE;
};

// X-Plane hands out UTF-8 paths, but a narrow std::filesystem::path decodes with the ANSI code page
// on Windows, so anything that touches the filesystem goes through UTF-16.
std::wstring utf8ToWide(const std::string &utf8);
std::string wideToUtf8(const std::wstring &wide);

inline std::filesystem::path utf8ToPath(const std::string &utf8) {
    return std::filesystem::path(utf8ToWide(utf8));
}

inline std::string pathToUtf8(const std::filesystem::path &path) {
    return wideToUtf8(path.native());
}

// Message for a GetLastError() code, without the trailing newline Windows adds
std::string win32ErrorMessage(DWORD error);

#endif
