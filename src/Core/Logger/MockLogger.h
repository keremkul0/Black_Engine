#pragma once

#include "ILogger.h"

class MockLogger : public ILogger {
public:
    MockLogger() = default;
    ~MockLogger() override = default;

    // Implementation of ILogger interface with no-op methods for testing
    void Initialize() override {}
    void Shutdown() override {}
    
    void Log(LogLevel level, const std::string& message, 
             const std::source_location& location = std::source_location::current()) override {}
    void LogTrace(const std::string& message,
                 const std::source_location& location = std::source_location::current()) override {}
    void LogDebug(const std::string& message,
                 const std::source_location& location = std::source_location::current()) override {}
    void LogInfo(const std::string& message,
                const std::source_location& location = std::source_location::current()) override {}    void LogWarning(const std::string& message,
                   const std::source_location& location = std::source_location::current()) override {}
    void LogError(const std::string& message,
                 const std::source_location& location = std::source_location::current()) override {}
    void LogCritical(const std::string& message,
                    const std::source_location& location = std::source_location::current()) override {}
    
    // Template specialization for formatted logging
    template<typename... Args>
    void LogFormat(LogLevel level, const std::string& formatString, Args&&... args) {}

    // Category logger management (returns self as a mock category)
    std::shared_ptr<ILogger> GetCategoryLogger(const std::string& category) override;
    
    // Configuration methods (all no-op)
    void SetGlobalLevel(LogLevel level) override {}
    void SetCategoryLevel(const std::string& category, LogLevel level) override {}
    void EnableFileLogging(const std::string& filePath = "Logs/BlackEngine.log") override {}
    void DisableFileLogging() override {}
    void SetPattern(const std::string& pattern) override {}
};