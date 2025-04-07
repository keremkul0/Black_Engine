#pragma once

#include <string>
#include <memory>
#include <source_location>
#include "spdlog/fmt/fmt.h"

// Forward declarations to avoid including the entire spdlog in this header.
namespace spdlog {
    class logger;
    namespace level {
        enum level_enum : int;
    }
}

// Represents the log levels supported by this logging system.
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Off = 6
};

class Logger {
public:
    // Initializes the logging system (e.g., sinks, patterns).
    // This should be called once at the start of the application.
    static void Initialize();

    // Shuts down the logging system and flushes all loggers.
    // Call this before application exit.
    static void Shutdown();

    // Logs a message with the specified log level and source location info.
    static void Log(
        LogLevel level,
        const std::string& message,
        const std::source_location& location = std::source_location::current()
    );

    // Logs a message with Trace level.
    static void LogTrace(
        const std::string& message,
        const std::source_location& location = std::source_location::current()
    );

    // Logs a message with Debug level.
    static void LogDebug(
        const std::string& message,
        const std::source_location& location = std::source_location::current()
    );

    // Logs a message with Info level.
    static void LogInfo(
        const std::string& message,
        const std::source_location& location = std::source_location::current()
    );

    // Logs a message with Warning level.
    static void LogWarning(
        const std::string& message,
        const std::source_location& location = std::source_location::current()
    );

    // Logs a message with Error level.
    static void LogError(
        const std::string& message,
        const std::source_location& location = std::source_location::current()
    );

    // Logs a message with Critical level.
    static void LogCritical(
        const std::string& message,
        const std::source_location& location = std::source_location::current()
    );

    // Retrieves or creates a category-specific logger.
    // Each category logger can have its own level or pattern.
    static std::shared_ptr<spdlog::logger> GetCategoryLogger(const std::string& category);

    // Sets the global logging level for the core and all category loggers.
    static void SetGlobalLevel(LogLevel level);

    // Sets the logging level for a specific category logger.
    static void SetCategoryLevel(const std::string& category, LogLevel level);

    // Enables file logging with a rotating file sink (default path is Logs/BlackEngine.log).
    static void EnableFileLogging(const std::string& filePath = "Logs/BlackEngine.log");

    // Disables file logging and removes the file sink if it exists.
    static void DisableFileLogging();

    // Sets a pattern string (spdlog-compatible) for all current sinks.
    static void SetPattern(const std::string& pattern);

    // Logs a formatted message at the specified log level.
    template<typename... Args>
    static void LogFormat(
        LogLevel level,
        const std::string& formatString,
        Args&&... args
    );

    // Logs a formatted Trace-level message.
    template<typename... Args>
    static void LogTraceFormat(
        const std::string& formatString,
        Args&&... args
    );

    // Logs a formatted Debug-level message.
    template<typename... Args>
    static void LogDebugFormat(
        const std::string& formatString,
        Args&&... args
    );

    // Logs a formatted Info-level message.
    template<typename... Args>
    static void LogInfoFormat(
        const std::string& formatString,
        Args&&... args
    );

    // Logs a formatted Warning-level message.
    template<typename... Args>
    static void LogWarningFormat(
        const std::string& formatString,
        Args&&... args
    );

    // Logs a formatted Error-level message.
    template<typename... Args>
    static void LogErrorFormat(
        const std::string& formatString,
        Args&&... args
    );

    // Logs a formatted Critical-level message.
    template<typename... Args>
    static void LogCriticalFormat(
        const std::string& formatString,
        Args&&... args
    );

private:
    // Converts LogLevel to the corresponding spdlog level.
    static spdlog::level::level_enum ConvertToSpdlogLevel(LogLevel level);
};

// Implementation of formatted logging methods.
// They are placed here to allow inlining and avoid separate template definitions.

template<typename... Args>
void Logger::LogFormat(LogLevel level, const std::string &formatString, Args&&... args) {
    try {
        auto formatted = fmt::format(fmt::runtime(formatString), std::forward<Args>(args)...);
        Log(level, formatted);
    } catch (const fmt::format_error &e) {
        LogError(std::string("Format error: ") + e.what());
    }
}


template<typename... Args>
void Logger::LogTraceFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Trace, formatString, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::LogDebugFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Debug, formatString, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::LogInfoFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Info, formatString, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::LogWarningFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Warning, formatString, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::LogErrorFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Error, formatString, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::LogCriticalFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Critical, formatString, std::forward<Args>(args)...);
}
