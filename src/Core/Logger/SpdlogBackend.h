#pragma once

#include "ILogBackend.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace BlackEngine {

/**
 * @brief Implementation of ILogBackend using the spdlog library
 */
class SpdlogBackend : public ILogBackend {
public:
    enum class OutputType {
        Console,
        File,
        RotatingFile,
        Both  // Console and File
    };
    
    struct Config {
        OutputType outputType;
        std::string logFilePath;
        size_t maxFileSize;
        size_t maxFiles;
        bool async;
        size_t asyncQueueSize;
        size_t asyncThreadCount;
        spdlog::level::level_enum flushLevel;

        // Yapıcı metot ile değerleri başlatma
        Config() :
            outputType(OutputType::Both),
            logFilePath("Logs/BlackEngine.log"),
            maxFileSize(5 * 1024 * 1024),  // 5MB
            maxFiles(3),
            async(true),
            asyncQueueSize(8192),
            asyncThreadCount(1),
            flushLevel(spdlog::level::err) {}
    };
    
    explicit SpdlogBackend(Config  config = Config{});
    ~SpdlogBackend() override;
    
    bool Initialize() override;
    void Shutdown() override;
    void Log(const LogMessage& message) override;
    void LogRepeat(const LogMessage& message, int count) override;
    void Flush() override;
    bool IsInitialized() const override { return m_Initialized; }
    void SetMinLogLevel(LogLevel level) override;
    LogLevel GetMinLogLevel() const override { return m_MinLogLevel; }
    std::string GetName() const override { return "SpdlogBackend"; }

private:
    static spdlog::level::level_enum ConvertToSpdlogLevel(LogLevel level) ;
    std::shared_ptr<spdlog::logger> GetOrCreateCategoryLogger(const std::string& categoryName);
    void SetupAsyncLogger() const;

    Config m_Config;
    bool m_Initialized = false;
    LogLevel m_MinLogLevel = LogLevel::Trace;
    std::shared_ptr<spdlog::logger> m_DefaultLogger;
    
    // Category specific loggers
    std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> m_CategoryLoggers;
    std::mutex m_LoggerMutex;  // Protects m_CategoryLoggers
    
    // Sinks that will be shared between all loggers
    std::vector<spdlog::sink_ptr> m_Sinks;
};

} // namespace BlackEngine
