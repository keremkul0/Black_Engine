#pragma once

#include <string>
#include <memory>
#include <source_location>

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Off = 6
};

class ILogger {
public:
    virtual ~ILogger() = default;

    // Basic initialization and shutdown
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;    // Simple logging methods with source location
    virtual void Log(LogLevel level, const std::string& message, 
                     const std::source_location& location) = 0;
    virtual void Log(const LogLevel level, const std::string& message) {
        Log(level, message, std::source_location::current()); 
    }
    
    virtual void LogTrace(const std::string& message, const std::source_location& location) = 0;
    virtual void LogTrace(const std::string& message) { 
        LogTrace(message, std::source_location::current()); 
    }
    
    virtual void LogDebug(const std::string& message, const std::source_location& location) = 0;
    virtual void LogDebug(const std::string& message) { 
        LogDebug(message, std::source_location::current()); 
    }    virtual void LogInfo(const std::string& message, const std::source_location& location) = 0;
    virtual void LogInfo(const std::string& message) {
        LogInfo(message, std::source_location::current());
    }
    
    virtual void LogWarning(const std::string& message, const std::source_location& location) = 0;
    virtual void LogWarning(const std::string& message) {
        LogWarning(message, std::source_location::current());
    }
    
    virtual void LogError(const std::string& message, const std::source_location& location) = 0;
    virtual void LogError(const std::string& message) {
        LogError(message, std::source_location::current());
    }
    
    virtual void LogCritical(const std::string& message, const std::source_location& location) = 0;
    virtual void LogCritical(const std::string& message) {
        LogCritical(message, std::source_location::current());
    }

    // Formatted logging methods
    template<typename... Args>
    void LogFormat(LogLevel level, const std::string& formatString, Args&&... args);
    
    template<typename... Args>
    void LogTraceFormat(const std::string& formatString, Args&&... args);
    
    template<typename... Args>
    void LogDebugFormat(const std::string& formatString, Args&&... args);
    
    template<typename... Args>
    void LogInfoFormat(const std::string& formatString, Args&&... args);
    
    template<typename... Args>
    void LogWarningFormat(const std::string& formatString, Args&&... args);
    
    template<typename... Args>
    void LogErrorFormat(const std::string& formatString, Args&&... args);
    
    template<typename... Args>
    void LogCriticalFormat(const std::string& formatString, Args&&... args);

    // Category loggers
    virtual std::shared_ptr<ILogger> GetCategoryLogger(const std::string& category) = 0;
    
    // Configuration methods
    virtual void SetGlobalLevel(LogLevel level) = 0;
    virtual void SetCategoryLevel(const std::string& category, LogLevel level) = 0;
    virtual void EnableFileLogging(const std::string& filePath) = 0;
    virtual void DisableFileLogging() = 0;
    virtual void SetPattern(const std::string& pattern) = 0;
};

// Template method implementations
template<typename... Args>
void ILogger::LogFormat(LogLevel level, const std::string& formatString, Args&&... args) {
    // Each implementation will provide its own formatting logic
    // This is just a placeholder that derived classes should override
}

template<typename... Args>
void ILogger::LogTraceFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Trace, formatString, std::forward<Args>(args)...);
}

template<typename... Args>
void ILogger::LogDebugFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Debug, formatString, std::forward<Args>(args)...);
}

template<typename... Args>
void ILogger::LogInfoFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Info, formatString, std::forward<Args>(args)...);
}

template<typename... Args>
void ILogger::LogWarningFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Warning, formatString, std::forward<Args>(args)...);
}

template<typename... Args>
void ILogger::LogErrorFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Error, formatString, std::forward<Args>(args)...);
}

template<typename... Args>
void ILogger::LogCriticalFormat(const std::string& formatString, Args&&... args) {
    LogFormat(LogLevel::Critical, formatString, std::forward<Args>(args)...);
}