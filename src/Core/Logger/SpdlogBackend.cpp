#include "SpdlogBackend.h"
#include <spdlog/details/fmt_helper.h>
#include <spdlog/pattern_formatter.h>
#include <filesystem>
#include <ranges>
#include <utility>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace BlackEngine {

SpdlogBackend::SpdlogBackend(Config  config)
    : m_Config(std::move(config))
{
}

SpdlogBackend::~SpdlogBackend() {
    SpdlogBackend::Shutdown();
}

bool SpdlogBackend::Initialize() {
    if (m_Initialized) {
        return true;
    }
    
    try {
        // Create directory for logs if it doesn't exist
        if (m_Config.outputType == OutputType::File || 
            m_Config.outputType == OutputType::RotatingFile || 
            m_Config.outputType == OutputType::Both) {
            std::filesystem::path logFilePath(m_Config.logFilePath);
            std::filesystem::create_directories(logFilePath.parent_path());
        }
        
        // Setup async logging if enabled
        if (m_Config.async) {
            SetupAsyncLogger();
        }
        
        // Setup sinks based on configuration
        if (m_Config.outputType == OutputType::Console || m_Config.outputType == OutputType::Both) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            m_Sinks.push_back(console_sink);
        }
        
        if (m_Config.outputType == OutputType::File || m_Config.outputType == OutputType::Both) {
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(m_Config.logFilePath, true);
            m_Sinks.push_back(file_sink);
        } else if (m_Config.outputType == OutputType::RotatingFile) {
            auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                m_Config.logFilePath, m_Config.maxFileSize, m_Config.maxFiles);
            m_Sinks.push_back(rotating_sink);
        }
        
        // Create default logger with all sinks
        m_DefaultLogger = std::make_shared<spdlog::logger>("BlackEngine", m_Sinks.begin(), m_Sinks.end());
        m_DefaultLogger->set_level(spdlog::level::trace);
        m_DefaultLogger->flush_on(static_cast<spdlog::level::level_enum>(m_Config.flushLevel));
        
        // Set as default for spdlog
        spdlog::set_default_logger(m_DefaultLogger);
        
        // Set pattern with source location
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%s:%#] %v");
        
        m_Initialized = true;
        return true;
    }
    catch (const std::exception& e) {
        // If initialization fails, print error directly to stderr
        fprintf(stderr, "Failed to initialize SpdlogBackend: %s\n", e.what());
        return false;
    }
}

void SpdlogBackend::SetupAsyncLogger() const {
    // Initialize the async logger with thread pool
    spdlog::init_thread_pool(m_Config.asyncQueueSize, m_Config.asyncThreadCount);
}

void SpdlogBackend::Shutdown() {
    if (!m_Initialized) {
        return;
    }
    
    // Flush all loggers
    Flush();
    
    // Clear category loggers
    {
        std::lock_guard<std::mutex> lock(m_LoggerMutex);
        m_CategoryLoggers.clear();
    }
    
    // Reset default logger
    m_DefaultLogger.reset();
    
    // Clear sinks
    m_Sinks.clear();
    
    // Shutdown spdlog
    spdlog::shutdown();
    
    m_Initialized = false;
}

void SpdlogBackend::Log(const LogMessage& message) {
    if (!m_Initialized) {
        return;
    }
    
    // Skip logs below minimum level
    if (message.level < m_MinLogLevel) {
        return;
    }
    
    auto logger = GetOrCreateCategoryLogger(message.categoryName);
    auto spdlogLevel = ConvertToSpdlogLevel(message.level);
    
    // Set source location for this log message
    spdlog::source_loc sourceLoc{
        message.location.file_name(),
        static_cast<int>(message.location.line()),
        message.location.function_name()
    };
    
    // Log with source location
    logger->log(sourceLoc, spdlogLevel, "{}", message.message);
    
    // Automatically flush on critical logs
    if (message.level >= LogLevel::Error) {
        logger->flush();
    }
}

void SpdlogBackend::LogRepeat(const LogMessage& message, int count) {
    if (!m_Initialized || message.level < m_MinLogLevel) {
        return;
    }
    
    auto logger = GetOrCreateCategoryLogger(message.categoryName);
    auto spdlogLevel = ConvertToSpdlogLevel(message.level);
    
    spdlog::source_loc sourceLoc{
        message.location.file_name(),
        static_cast<int>(message.location.line()),
        message.location.function_name()
    };
    
    // Log with repeat count in Unity-style format
    logger->log(sourceLoc, spdlogLevel, "{} (x{})", message.message, count);
    
    // Automatically flush on critical logs
    if (message.level >= LogLevel::Error) {
        logger->flush();
    }
}

void SpdlogBackend::Flush() {
    if (!m_Initialized) {
        return;
    }
    
    m_DefaultLogger->flush();
    
    std::lock_guard<std::mutex> lock(m_LoggerMutex);
    for (const auto &logger: m_CategoryLoggers | std::views::values) {
        logger->flush();
    }
}

void SpdlogBackend::SetMinLogLevel(LogLevel level) {
    m_MinLogLevel = level;
    auto spdlogLevel = ConvertToSpdlogLevel(level);
    
    m_DefaultLogger->set_level(spdlogLevel);
    
    std::lock_guard<std::mutex> lock(m_LoggerMutex);
    for (const auto &logger: m_CategoryLoggers | std::views::values) {
        logger->set_level(spdlogLevel);
    }
}

spdlog::level::level_enum SpdlogBackend::ConvertToSpdlogLevel(const LogLevel level) {
    switch (level) {
        case LogLevel::Trace:    return spdlog::level::trace;
        case LogLevel::Debug:    return spdlog::level::debug;
        case LogLevel::Info:     return spdlog::level::info;
        case LogLevel::Warning:  return spdlog::level::warn;
        case LogLevel::Error:    return spdlog::level::err;
        case LogLevel::Critical: return spdlog::level::critical;
        case LogLevel::Off:      return spdlog::level::off;
        default:                 return spdlog::level::info;
    }
}

std::shared_ptr<spdlog::logger> SpdlogBackend::GetOrCreateCategoryLogger(const std::string& categoryName) {
    // Use default logger if category name is empty
    if (categoryName.empty()) {
        return m_DefaultLogger;
    }
    
    // Check if we already have a logger for this category
    {
        std::lock_guard<std::mutex> lock(m_LoggerMutex);
        auto it = m_CategoryLoggers.find(categoryName);
        if (it != m_CategoryLoggers.end()) {
            return it->second;
        }
    }
    
    // Create new logger for this category
    auto logger = std::make_shared<spdlog::logger>(categoryName, m_Sinks.begin(), m_Sinks.end());
    logger->set_level(m_DefaultLogger->level());
    logger->flush_on(m_DefaultLogger->flush_level());
    
    // Store in map
    {
        std::lock_guard<std::mutex> lock(m_LoggerMutex);
        m_CategoryLoggers[categoryName] = logger;
    }
    
    return logger;
}

} // namespace BlackEngine
