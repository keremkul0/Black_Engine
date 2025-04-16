#pragma once

#include <string>
#include <source_location>
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/common.h>

namespace BlackEngine {

/**
 * @brief Log seviyeleri
 */
enum class LogLevel {
    Off = 0,
    Critical,
    Error,
    Warning,
    Info,
    Debug,
    Trace
};

/**
 * @brief LogLevel -> string dönüşümü
 */
inline std::string LogLevelToString(const LogLevel level) {
    switch (level) {
        case LogLevel::Off:      return "Off";
        case LogLevel::Critical: return "Critical";
        case LogLevel::Error:    return "Error";
        case LogLevel::Warning:  return "Warning";
        case LogLevel::Info:     return "Info";
        case LogLevel::Debug:    return "Debug";
        case LogLevel::Trace:    return "Trace";
        default:                 return "Unknown";
    }
}

/**
 * @brief String -> LogLevel dönüşümü
 */
inline LogLevel StringToLogLevel(const std::string& levelStr) {
    if (levelStr == "Off" || levelStr == "off")           return LogLevel::Off;
    else if (levelStr == "Critical" || levelStr == "critical") return LogLevel::Critical;
    else if (levelStr == "Error" || levelStr == "error")     return LogLevel::Error;
    else if (levelStr == "Warning" || levelStr == "warning")  return LogLevel::Warning;
    else if (levelStr == "Debug" || levelStr == "debug")     return LogLevel::Debug;
    else if (levelStr == "Trace" || levelStr == "trace")     return LogLevel::Trace;
    else return LogLevel::Info; // Varsayılan
}

/**
 * @brief BlackEngine LogLevel -> spdlog level dönüşümü
 */
inline spdlog::level::level_enum ToSpdlogLevel(const LogLevel level) {
    switch (level) {
        case LogLevel::Off:      return spdlog::level::off;
        case LogLevel::Critical: return spdlog::level::critical;
        case LogLevel::Error:    return spdlog::level::err;
        case LogLevel::Warning:  return spdlog::level::warn;
        case LogLevel::Info:     return spdlog::level::info;
        case LogLevel::Debug:    return spdlog::level::debug;
        case LogLevel::Trace:    return spdlog::level::trace;
        default:                 return spdlog::level::info;
    }
}

/**
 * @brief spdlog level -> BlackEngine LogLevel dönüşümü
 */
inline LogLevel FromSpdlogLevel(const spdlog::level::level_enum level) {
    switch (level) {
        case spdlog::level::off:      return LogLevel::Off;
        case spdlog::level::critical: return LogLevel::Critical;
        case spdlog::level::err:      return LogLevel::Error;
        case spdlog::level::warn:     return LogLevel::Warning;
        case spdlog::level::info:     return LogLevel::Info;
        case spdlog::level::debug:    return LogLevel::Debug;
        case spdlog::level::trace:    return LogLevel::Trace;
        default:                      return LogLevel::Info;
    }
}

/**
 * @brief Log mesaj yapısı
 */
struct LogMessage {
    LogLevel level;
    std::string category;
    std::string message;
    std::source_location location;
    std::chrono::system_clock::time_point timestamp;
    
    // Test için eşitlik operatörü
    bool operator==(const LogMessage& other) const {
        return level == other.level && 
               category == other.category && 
               message == other.message;
    }
};

// Forward declarations
class CategoryInfo;
class ILoggerBackend;
class LogManager;
class MockLogger;
class ConsoleLoggerBackend;

} // namespace BlackEngine
