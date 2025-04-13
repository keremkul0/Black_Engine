#pragma once

#include "ILogger.h"
#include <mutex>
#include <unordered_map>
#include <spdlog/fmt/fmt.h>
#include <chrono>

// Forward declaration for JSON config struct
struct LoggerConfig;

// Forward declarations to avoid including the entire spdlog in this header.
namespace spdlog {
    class logger;

    namespace level {
        enum level_enum : int;
    }

    namespace sinks {
        class sink;

        // Forward declaration of rotating_file_sink template
        template<typename Mutex>
        class rotating_file_sink;

        // Use the same definition style as spdlog
        using rotating_file_sink_mt = rotating_file_sink<std::mutex>;
    }
}

class SpdlogLogger final : public ILogger {
public:
    SpdlogLogger();

    ~SpdlogLogger() override;

    // Implementation of ILogger interface
    void Initialize() override;
    void Initialize(const std::string& jsonConfigPath);

    void Shutdown() override;
    
    // Duplicate suppression configuration
    void EnableDuplicateSuppression(bool enable) override;
    void SetDuplicateSuppressionOptions(size_t suppressAfter = 3, size_t summaryInterval = 0) override;

    void Log(LogLevel level, const std::string &message,
             const std::source_location &location) override;

    void LogTrace(const std::string &message,
                  const std::source_location &location) override;

    void LogDebug(const std::string &message,
                  const std::source_location &location) override;

    void LogInfo(const std::string &message,
                 const std::source_location &location) override;

    void LogWarning(const std::string &message,
                    const std::source_location &location) override;

    void LogError(const std::string &message,
                  const std::source_location &location) override;

    void LogCritical(const std::string &message,
                     const std::source_location &location) override;

    // Template specializations for formatted logging
    template<typename... Args>
    void LogFormat(LogLevel level, const std::string &formatString, Args &&... args);

    // Category logger management
    std::shared_ptr<ILogger> GetCategoryLogger(const std::string &category) override;

    // Configuration methods
    void SetGlobalLevel(LogLevel level) override;

    void SetCategoryLevel(const std::string &category, LogLevel level) override;

    void EnableFileLogging(const std::string &filePath) override;

    void DisableFileLogging() override;

    void SetPattern(const std::string &pattern) override;

    // Helper method for backwards compatibility and testing
    std::shared_ptr<spdlog::logger> GetSpdlogCategoryLogger(const std::string &category) const;

    static spdlog::level::level_enum ConvertToSpdlogLevel(LogLevel level);

private:
    // Internal helper methods
    static LogLevel ConvertFromSpdlogLevel(spdlog::level::level_enum level);
    void ApplyLoggerConfig(const LoggerConfig& config);
    
    // Message info for duplicate suppression
    struct MessageInfo {
        std::string message;
        LogLevel level;
        std::string categoryName;
        size_t repeatCount = 0;
        std::chrono::steady_clock::time_point lastTime;
        bool summarized = false;
    };
    
    // Handles duplicate message logic
    bool ProcessDuplicateMessage(const std::string& message, LogLevel level, const std::string& category);
    void ResetDuplicateTracking();

    // Private implementation details
    class LoggerImpl;
    std::unique_ptr<LoggerImpl> m_Impl;
    std::mutex m_LoggerMutex;
    std::mutex m_DuplicateMutex;
    std::unordered_map<std::string, std::shared_ptr<SpdlogLogger> > m_CategoryLoggers;
    
    // Duplicate suppression settings
    bool m_DuplicateSuppressionEnabled = false;
    size_t m_DuplicateSuppressAfter = 3;
    size_t m_DuplicateSummaryInterval = 0; // 0 means only summary at the end
    MessageInfo m_LastMessage;
    
    bool m_IsInitialized = false;
};

template<typename... Args>
void SpdlogLogger::LogFormat(LogLevel level, const std::string &formatString, Args &&... args) {
    try {
        auto formatted = fmt::format(fmt::runtime(formatString), std::forward<Args>(args)...);
        Log(level, formatted);
    } catch (const std::exception &e) {
        // Log a format error with the exception message
        LogError(std::string("Format error: ") + e.what(), std::source_location::current());
    }
}
