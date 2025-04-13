#pragma once

#include "ILogger.h"
#include <string>
#include <unordered_map>
#include <optional>

// A structure to hold logger configuration settings
struct LoggerConfig {
    // Global logging level
    LogLevel globalLevel = LogLevel::Info;
    
    // Category-specific logging levels
    std::unordered_map<std::string, LogLevel> categories;
    
    // Logging pattern
    std::string pattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v";
    
    // File logging configuration
    struct FileLogging {
        bool enabled = false;
        std::string path = "Logs/BlackEngine.log";
    } fileLogging;
    
    // Duplicate suppression settings
    struct DuplicateSuppression {
        bool enabled = false;
        size_t suppressAfter = 3; // After this many duplicates, start suppressing
        size_t summaryInterval = 0; // How often to show summary (0 = only at change)
    } duplicateSuppression;

    // Helper method to parse JSON config file
    static std::optional<LoggerConfig> FromJson(const std::string& jsonFilePath);
    
    // Convert string level to LogLevel enum
    static LogLevel StringToLogLevel(const std::string& levelStr);
    
    // Convert LogLevel enum to string
    static std::string LogLevelToString(LogLevel level);
};
