#pragma once

#include <string>
#include <string_view>
#include <source_location>
#include <chrono>
#include <atomic>

namespace BlackEngine {

/**
 * @brief Defines the severity levels for log messages
 */
enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Off
};

/**
 * @brief Converts a LogLevel to its string representation
 */
inline std::string LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace:    return "Trace";
        case LogLevel::Debug:    return "Debug";
        case LogLevel::Info:     return "Info";
        case LogLevel::Warning:  return "Warning";
        case LogLevel::Error:    return "Error";
        case LogLevel::Critical: return "Critical";
        case LogLevel::Off:      return "Off";
        default:                 return "Unknown";
    }
}

/**
 * @brief Converts a string to corresponding LogLevel
 */
inline LogLevel StringToLogLevel(const std::string& levelStr) {
    if (levelStr == "Trace")    return LogLevel::Trace;
    if (levelStr == "Debug")    return LogLevel::Debug;
    if (levelStr == "Info")     return LogLevel::Info;
    if (levelStr == "Warning")  return LogLevel::Warning;
    if (levelStr == "Error")    return LogLevel::Error;
    if (levelStr == "Critical") return LogLevel::Critical;
    if (levelStr == "Off")      return LogLevel::Off;
    return LogLevel::Info; // Default
}

/**
 * @brief Holds information about a log message
 */
struct LogMessage {
    std::string message;
    LogLevel level;
    std::string categoryName;
    std::source_location location;
    std::chrono::system_clock::time_point timestamp;
    
    LogMessage(std::string msg,
              const LogLevel lvl,
              std::string category,
              const std::source_location& loc = std::source_location::current(),
              const std::chrono::system_clock::time_point time = std::chrono::system_clock::now())
        : message(std::move(msg))
        , level(lvl)
        , categoryName(std::move(category))
        , location(loc)
        , timestamp(time)
    {}
};

/**
 * @brief Stores configuration and state for a log category
 */
struct CategoryInfo {
    std::string name;
    LogLevel level = LogLevel::Info;  // Default log level for this category
    bool enabled = true;              // Whether logging is enabled for this category
    
    // For repeated log handling
    struct RepeatInfo {
        std::atomic<int> count{0};
        std::string lastMessage;
        LogLevel lastLevel{LogLevel::Off};
        std::chrono::system_clock::time_point lastTime;
    };
    
    RepeatInfo repeatInfo;
    
    CategoryInfo() = default;
    explicit CategoryInfo(std::string catName, LogLevel catLevel = LogLevel::Info)
        : name(std::move(catName)), level(catLevel) {}
};

/**
 * @brief Combines a category name and message for repeat detection hashing
 */
struct LogMessageIdentifier {
    std::string categoryName;
    std::string message;
    
    bool operator==(const LogMessageIdentifier& other) const {
        return categoryName == other.categoryName && message == other.message;
    }
};

} // namespace BlackEngine

// Hash function for LogMessageIdentifier
template<>
struct std::hash<BlackEngine::LogMessageIdentifier> {
    size_t operator()(const BlackEngine::LogMessageIdentifier& id) const noexcept {
        return hash<std::string>()(id.categoryName) ^ hash<std::string>()(id.message);
    }
};
