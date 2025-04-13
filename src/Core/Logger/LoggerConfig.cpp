#include "LoggerConfig.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Convert string level to LogLevel enum
LogLevel LoggerConfig::StringToLogLevel(const std::string& levelStr) {
    if (levelStr == "trace" || levelStr == "Trace") return LogLevel::Trace;
    if (levelStr == "debug" || levelStr == "Debug") return LogLevel::Debug;
    if (levelStr == "info" || levelStr == "Info") return LogLevel::Info;
    if (levelStr == "warning" || levelStr == "Warning") return LogLevel::Warning;
    if (levelStr == "error" || levelStr == "Error") return LogLevel::Error;
    if (levelStr == "critical" || levelStr == "Critical") return LogLevel::Critical;
    if (levelStr == "off" || levelStr == "Off") return LogLevel::Off;
    
    // Default to Info if unknown
    return LogLevel::Info;
}

// Convert LogLevel enum to string
std::string LoggerConfig::LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "Trace";
        case LogLevel::Debug: return "Debug";
        case LogLevel::Info: return "Info";
        case LogLevel::Warning: return "Warning";
        case LogLevel::Error: return "Error";
        case LogLevel::Critical: return "Critical";
        case LogLevel::Off: return "Off";
        default: return "Info";
    }
}

// Parse JSON config file
std::optional<LoggerConfig> LoggerConfig::FromJson(const std::string& jsonFilePath) {
    try {
        // Open and parse JSON file
        std::ifstream file(jsonFilePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open logger config file: " << jsonFilePath << std::endl;
            return std::nullopt;
        }
        
        json configJson;
        file >> configJson;
        
        LoggerConfig config;
        
        // Parse global logging level
        if (configJson.contains("globalLevel") && configJson["globalLevel"].is_string()) {
            config.globalLevel = StringToLogLevel(configJson["globalLevel"]);
        }
        
        // Parse logging pattern
        if (configJson.contains("pattern") && configJson["pattern"].is_string()) {
            config.pattern = configJson["pattern"];
        }
        
        // Parse file logging settings
        if (configJson.contains("fileLogging") && configJson["fileLogging"].is_object()) {
            const auto& fileConfig = configJson["fileLogging"];
            if (fileConfig.contains("enabled") && fileConfig["enabled"].is_boolean()) {
                config.fileLogging.enabled = fileConfig["enabled"];
            }
            if (fileConfig.contains("path") && fileConfig["path"].is_string()) {
                config.fileLogging.path = fileConfig["path"];
            }
        }
        
        // Parse duplicate suppression settings
        if (configJson.contains("duplicateSuppression") && configJson["duplicateSuppression"].is_object()) {
            const auto& dupConfig = configJson["duplicateSuppression"];
            if (dupConfig.contains("enabled") && dupConfig["enabled"].is_boolean()) {
                config.duplicateSuppression.enabled = dupConfig["enabled"];
            }
            if (dupConfig.contains("suppressAfter") && dupConfig["suppressAfter"].is_number()) {
                config.duplicateSuppression.suppressAfter = dupConfig["suppressAfter"];
            }
            if (dupConfig.contains("summaryInterval") && dupConfig["summaryInterval"].is_number()) {
                config.duplicateSuppression.summaryInterval = dupConfig["summaryInterval"];
            }
        }
        
        // Parse category-specific levels
        if (configJson.contains("categories") && configJson["categories"].is_object()) {
            for (auto& [category, levelValue] : configJson["categories"].items()) {
                if (levelValue.is_string()) {
                    const std::string levelStr = levelValue;
                    config.categories[category] = StringToLogLevel(levelStr);
                }
            }
        }
        
        return config;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing logger config JSON: " << e.what() << std::endl;
        return std::nullopt;
    }
}
