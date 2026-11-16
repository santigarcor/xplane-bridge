#ifndef LOGGER_H
#define LOGGER_H

#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <XPLMProcessing.h>

#if defined(__MINGW32__)
#define LOGGER_PRINTF_FORMAT __attribute__((format(__MINGW_PRINTF_FORMAT, 2, 3)))
#elif defined(__GNUC__) || defined(__clang__)
#define LOGGER_PRINTF_FORMAT __attribute__((format(printf, 2, 3)))
#else
#define LOGGER_PRINTF_FORMAT
#endif

enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warn = 2,
    Error = 3,
};

// Thread-safe logger, writing to X-Plane's Log.txt and to a session file in <plugin>/logs.
//
// XPLMDebugString may only be called from the main (sim) thread: calling it from another thread
// aborts X-Plane. Lines logged on any other thread are queued and written by a flight loop on the
// main thread. The session file is written from the main thread too, so the serial worker never
// touches files or the SDK.
//
// initialize() runs once from XPluginStart and destroy() from XPluginStop, both on the main thread.
// A trailing newline is added to every message, so callers don't pass one.
class Logger {
    public:
        static Logger &getInstance();

        void initialize(const std::string &logDirectory);
        void destroy();

        void setLogLevel(LogLevel level) {
            currentLogLevel.store(level, std::memory_order_relaxed);
        }

        LogLevel getLogLevel() const {
            return currentLogLevel.load(std::memory_order_relaxed);
        }

        std::string getLogFilePath() const {
            return logFilePath;
        }

        void debug(const char *format, ...) LOGGER_PRINTF_FORMAT;
        void info(const char *format, ...) LOGGER_PRINTF_FORMAT;
        void warn(const char *format, ...) LOGGER_PRINTF_FORMAT;
        void error(const char *format, ...) LOGGER_PRINTF_FORMAT;

    private:
        struct Entry {
                LogLevel level;
                std::string timestamp;
                std::string message;
        };

        Logger();
        ~Logger() = default;
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        void log(LogLevel level, const char *format, va_list args);
        void emit(const Entry &entry);
        void flush();
        void openLogFile(const std::string &logDirectory);
        static float flushCallback(float, float, int, void *);

        // Queued off-main-thread lines beyond this are dropped (and counted), so a runaway
        // producer can't grow memory without bound.
        static constexpr size_t kMaxPending = 2000;

        std::mutex queueMutex;
        std::vector<Entry> pending;
        size_t droppedEntries = 0;

        std::thread::id mainThreadId;
        std::atomic<bool> initialized{false};
        std::atomic<LogLevel> currentLogLevel{LogLevel::Info};

        XPLMFlightLoopID flushLoop = nullptr;
        FILE *logFile = nullptr;
        std::string logFilePath;
};

#endif
