#include "SpdlogLogger.h"
#include "LoggerConfig.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/async.h"
#include "spdlog/fmt/fmt.h"
#include <filesystem>
#include <iostream>
#include <thread>
#include <ranges>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

// Private implementation class (PIMPL pattern)
class SpdlogLogger::LoggerImpl {
public:
    std::shared_ptr<spdlog::logger> m_CoreLogger;
    std::vector<std::shared_ptr<spdlog::sinks::sink> > m_Sinks;
    std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> m_FileSink;
    std::unordered_map<std::string, std::shared_ptr<spdlog::logger> > m_SpdlogCategoryLoggers;
};

SpdlogLogger::SpdlogLogger()
    : m_Impl(std::make_unique<LoggerImpl>()), m_LastMessage() {
}

SpdlogLogger::~SpdlogLogger() {
    if (m_IsInitialized) {
        try {
            SpdlogLogger::Shutdown();
        } catch (...) {
            // Suppress exceptions in destructor
        }
    }
}

void SpdlogLogger::Initialize() {
    std::lock_guard<std::mutex> lock(m_LoggerMutex);

    // Prevent multiple initializations
    if (m_IsInitialized)
        return;

    // Make sure any previous spdlog state is cleared
    spdlog::shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Set up async logging (optional, but recommended for performance)
    spdlog::init_thread_pool(8192, 1);

    // Create console sinks
    const auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("[%^%l%$] %v");
    m_Impl->m_Sinks.push_back(consoleSink);

    // Create the core logger with the available sinks
    m_Impl->m_CoreLogger = std::make_shared<spdlog::logger>("BLACK_ENGINE", m_Impl->m_Sinks.begin(),
                                                            m_Impl->m_Sinks.end());
    m_Impl->m_CoreLogger->set_level(spdlog::level::trace);

    // Register and set as default
    spdlog::register_logger(m_Impl->m_CoreLogger);
    spdlog::set_default_logger(m_Impl->m_CoreLogger);

    // Set a default pattern
    SetPattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

    // Create the Logs directory if it doesn't exist
    if (const auto logDir = std::filesystem::path("Logs"); !std::filesystem::exists(logDir)) {
        std::filesystem::create_directory(logDir);
    }

    // Enable file logging with default path
    EnableFileLogging("Logs/BlackEngine.log");

    // Log a message indicating logger initialization
    LogInfo("SpdlogLogger initialized",
            std::source_location::current());

    m_IsInitialized = true;
}

void SpdlogLogger::Shutdown() {
    std::lock_guard<std::mutex> lock(m_LoggerMutex);

    if (!m_IsInitialized)
        return;

    // Log a message indicating logger shutdown
    LogInfo("SpdlogLogger shutting down",
            std::source_location::current());

    // Flush everything
    m_Impl->m_CoreLogger->flush();
    for (const auto &loggerPtr: m_Impl->m_SpdlogCategoryLoggers | std::views::values) {
        loggerPtr->flush();
    }

    // Clear resources
    m_Impl->m_FileSink.reset();
    m_Impl->m_Sinks.clear();
    m_Impl->m_SpdlogCategoryLoggers.clear();
    m_Impl->m_CoreLogger.reset();

    // Clear category wrappers
    m_CategoryLoggers.clear();

    // Let spdlog handle its own shutdown
    spdlog::shutdown();

    m_IsInitialized = false;
}

void SpdlogLogger::Log(LogLevel level, const std::string &message, const std::source_location &location) {
    if (!m_IsInitialized)
        return;

    // Check for duplicate message suppression
    if (ProcessDuplicateMessage(message, level, ""))
        return;  // This message is suppressed

    auto spdLevel = ConvertToSpdlogLevel(level);
    m_Impl->m_CoreLogger->log(
        spdlog::source_loc{location.file_name(), static_cast<int>(location.line()), location.function_name()},
        spdLevel, "{}", message);
}

void SpdlogLogger::LogTrace(const std::string &message, const std::source_location &location) {
    if (!m_IsInitialized)
        return;

    // Check for duplicate message suppression
    if (ProcessDuplicateMessage(message, LogLevel::Trace, ""))
        return;  // This message is suppressed

    m_Impl->m_CoreLogger->log(spdlog::source_loc{
                                  location.file_name(), static_cast<int>(location.line()), location.function_name()
                              },
                              spdlog::level::trace, "{}", message);
}

void SpdlogLogger::LogDebug(const std::string &message, const std::source_location &location) {
    if (!m_IsInitialized)
        return;
        
    // Check for duplicate message suppression
    if (ProcessDuplicateMessage(message, LogLevel::Debug, ""))
        return;  // This message is suppressed

    m_Impl->m_CoreLogger->log(spdlog::source_loc{
                                  location.file_name(), static_cast<int>(location.line()), location.function_name()
                              },
                              spdlog::level::debug, "{}", message);
}

void SpdlogLogger::LogInfo(const std::string &message, const std::source_location &location) {
    if (!m_IsInitialized)
        return;
        
    // Check for duplicate message suppression
    if (ProcessDuplicateMessage(message, LogLevel::Info, ""))
        return;  // This message is suppressed

    m_Impl->m_CoreLogger->log(spdlog::source_loc{
                                  location.file_name(), static_cast<int>(location.line()), location.function_name()
                              },
                              spdlog::level::info, "{}", message);
}

void SpdlogLogger::LogWarning(const std::string &message, const std::source_location &location) {
    if (!m_IsInitialized)
        return;
        
    // Check for duplicate message suppression
    if (ProcessDuplicateMessage(message, LogLevel::Warning, ""))
        return;  // This message is suppressed

    m_Impl->m_CoreLogger->log(spdlog::source_loc{
                                  location.file_name(), static_cast<int>(location.line()), location.function_name()
                              },
                              spdlog::level::warn, "{}", message);
}

void SpdlogLogger::LogError(const std::string &message, const std::source_location &location) {
    if (!m_IsInitialized)
        return;
        
    // Check for duplicate message suppression
    if (ProcessDuplicateMessage(message, LogLevel::Error, ""))
        return;  // This message is suppressed

    m_Impl->m_CoreLogger->log(spdlog::source_loc{
                                  location.file_name(), static_cast<int>(location.line()), location.function_name()
                              },
                              spdlog::level::err, "{}", message);
}

void SpdlogLogger::LogCritical(const std::string &message, const std::source_location &location) {
    if (!m_IsInitialized)
        return;
        
    // Check for duplicate message suppression
    if (ProcessDuplicateMessage(message, LogLevel::Critical, ""))
        return;  // This message is suppressed

    m_Impl->m_CoreLogger->log(spdlog::source_loc{
                                  location.file_name(), static_cast<int>(location.line()), location.function_name()
                              },
                              spdlog::level::critical, "{}", message);
}

std::shared_ptr<ILogger> SpdlogLogger::GetCategoryLogger(const std::string &category) {
    std::lock_guard<std::mutex> lock(m_LoggerMutex);

    // Check if we already have a category wrapper
    if (const auto it = m_CategoryLoggers.find(category); it != m_CategoryLoggers.end()) {
        return it->second;
    }

    // Create a new spdlog category logger
    const auto spdlogCategoryLogger = GetSpdlogCategoryLogger(category);

    // Create a new SpdlogLogger wrapper for this category
    auto categoryLogger = std::make_shared<SpdlogLogger>();
    categoryLogger->m_IsInitialized = true;
    categoryLogger->m_Impl = std::make_unique<LoggerImpl>();
    categoryLogger->m_Impl->m_CoreLogger = spdlogCategoryLogger;
    categoryLogger->m_Impl->m_Sinks = m_Impl->m_Sinks;
    categoryLogger->m_Impl->m_FileSink = m_Impl->m_FileSink;

    // Store and return the wrapper
    m_CategoryLoggers[category] = categoryLogger;
    return categoryLogger;
}

std::shared_ptr<spdlog::logger> SpdlogLogger::GetSpdlogCategoryLogger(const std::string &category) const {
    if (!m_IsInitialized)
        return nullptr;

    // Check if we already have a logger for this category
    auto it = m_Impl->m_SpdlogCategoryLoggers.find(category);
    if (it != m_Impl->m_SpdlogCategoryLoggers.end()) {
        return it->second;
    }

    // Ensure previous logger with the same name is dropped, if any
    try {
        spdlog::drop(category);
    } catch (...) {
        // Ignore errors when dropping
    }

    // Create a new logger that shares the same sinks as the core logger
    auto categoryLogger = std::make_shared<spdlog::logger>(category, m_Impl->m_Sinks.begin(), m_Impl->m_Sinks.end());
    categoryLogger->set_level(m_Impl->m_CoreLogger->level());
    categoryLogger->flush_on(spdlog::level::err);

    // Store it in our map
    m_Impl->m_SpdlogCategoryLoggers[category] = categoryLogger;

    // Register globally so spdlog can manage it as well
    try {
        spdlog::register_logger(categoryLogger);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to register logger '" << category << "': " << ex.what() << std::endl;
    }

    return categoryLogger;
}

void SpdlogLogger::SetGlobalLevel(const LogLevel level) {
    if (!m_IsInitialized)
        return;

    auto spdLevel = ConvertToSpdlogLevel(level);

    std::lock_guard<std::mutex> lock(m_LoggerMutex);
    m_Impl->m_CoreLogger->set_level(spdLevel);

    // Update all category loggers
    for (const auto &logger: m_Impl->m_SpdlogCategoryLoggers | std::views::values) {
        logger->set_level(spdLevel);
    }
}

void SpdlogLogger::SetCategoryLevel(const std::string &category, LogLevel level) {
    if (!m_IsInitialized)
        return;

    const auto spdLevel = ConvertToSpdlogLevel(level);
    if (const auto catLogger = GetSpdlogCategoryLogger(category)) {
        catLogger->set_level(spdLevel);
    }
}

void SpdlogLogger::EnableFileLogging(const std::string &filePath) {
    if (!m_IsInitialized)
        return;

    std::lock_guard lock(m_LoggerMutex);

    // Drop the existing core logger from the registry
    spdlog::drop("BLACK_ENGINE");

    // If we already have a file sink, remove it before re-adding
    if (m_Impl->m_FileSink) {
        if (const auto it = std::ranges::find(m_Impl->m_Sinks, m_Impl->m_FileSink); it != m_Impl->m_Sinks.end()) {
            m_Impl->m_Sinks.erase(it);
        }
        m_Impl->m_FileSink.reset();
    }

    // Create directory if it doesn't exist
    if (const auto fileDirPath = std::filesystem::path(filePath).parent_path();
        !fileDirPath.empty() && !exists(fileDirPath)) {
        create_directories(fileDirPath);
    } // Create a rotating file sink (5MB limit, 3 rotated files)
    const auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filePath, 5 * 1024 * 1024, 3, true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    // Store the file sink as a base sink type to avoid incomplete type issues
    m_Impl->m_FileSink = std::static_pointer_cast<spdlog::sinks::rotating_file_sink_mt>(file_sink);
    m_Impl->m_Sinks.push_back(std::static_pointer_cast<spdlog::sinks::sink>(file_sink));

    // Recreate the core logger with the updated sinks
    m_Impl->m_CoreLogger = std::make_shared<spdlog::logger>("BLACK_ENGINE", m_Impl->m_Sinks.begin(),
                                                            m_Impl->m_Sinks.end());
    m_Impl->m_CoreLogger->set_level(spdlog::level::trace);
    spdlog::register_logger(m_Impl->m_CoreLogger);
    spdlog::set_default_logger(m_Impl->m_CoreLogger);

    // Recreate all category loggers with the new sinks
    for (auto &[name, loggerPtr]: m_Impl->m_SpdlogCategoryLoggers) {
        try {
            spdlog::drop(name);
        } catch (const std::exception &ex) {
            std::cerr << "Failed to drop logger '" << name << "': " << ex.what() << std::endl;
        }

        auto newLogger = std::make_shared<spdlog::logger>(name, m_Impl->m_Sinks.begin(), m_Impl->m_Sinks.end());
        newLogger->set_level(loggerPtr->level());
        m_Impl->m_SpdlogCategoryLoggers[name] = newLogger;

        spdlog::register_logger(newLogger);
    }
}

void SpdlogLogger::DisableFileLogging() {
    if (!m_IsInitialized)
        return;

    std::lock_guard<std::mutex> lock(m_LoggerMutex);

    if (!m_Impl->m_FileSink)
        return;

    if (const auto it = std::ranges::find(m_Impl->m_Sinks, m_Impl->m_FileSink); it != m_Impl->m_Sinks.end()) {
        m_Impl->m_Sinks.erase(it);
    }
    m_Impl->m_FileSink.reset();

    // Recreate the core logger without a file sink
    m_Impl->m_CoreLogger = std::make_shared<spdlog::logger>("BLACK_ENGINE", m_Impl->m_Sinks.begin(),
                                                            m_Impl->m_Sinks.end());
    m_Impl->m_CoreLogger->set_level(spdlog::level::trace);
    spdlog::register_logger(m_Impl->m_CoreLogger);
    spdlog::set_default_logger(m_Impl->m_CoreLogger);

    // Update category loggers
    for (auto &[name, loggerPtr]: m_Impl->m_SpdlogCategoryLoggers) {
        auto newLogger = std::make_shared<spdlog::logger>(name, m_Impl->m_Sinks.begin(), m_Impl->m_Sinks.end());
        newLogger->set_level(loggerPtr->level());
        m_Impl->m_SpdlogCategoryLoggers[name] = newLogger;
        spdlog::register_logger(newLogger);
    }
}

void SpdlogLogger::SetPattern(const std::string &pattern) {
    if (!m_IsInitialized)
        return;

    std::lock_guard<std::mutex> lock(m_LoggerMutex);

    for (auto &sink: m_Impl->m_Sinks) {
        sink->set_pattern(pattern);
    }
}

spdlog::level::level_enum SpdlogLogger::ConvertToSpdlogLevel(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return spdlog::level::trace;
        case LogLevel::Debug: return spdlog::level::debug;
        case LogLevel::Info: return spdlog::level::info;
        case LogLevel::Warning: return spdlog::level::warn;
        case LogLevel::Error: return spdlog::level::err;
        case LogLevel::Critical: return spdlog::level::critical;
        case LogLevel::Off: return spdlog::level::off;
        default: return spdlog::level::info;
    }
}

LogLevel SpdlogLogger::ConvertFromSpdlogLevel(spdlog::level::level_enum level) {
    switch (level) {
        case spdlog::level::trace: return LogLevel::Trace;
        case spdlog::level::debug: return LogLevel::Debug;
        case spdlog::level::info: return LogLevel::Info;
        case spdlog::level::warn: return LogLevel::Warning;
        case spdlog::level::err: return LogLevel::Error;
        case spdlog::level::critical: return LogLevel::Critical;
        case spdlog::level::off: return LogLevel::Off;
        default: return LogLevel::Info;
    }
}

void SpdlogLogger::EnableDuplicateSuppression(bool enable) {
    std::lock_guard<std::mutex> lock(m_DuplicateMutex);
    m_DuplicateSuppressionEnabled = enable;
    
    if (!enable) {
        // When disabling, reset tracking
        ResetDuplicateTracking();
    }
}

void SpdlogLogger::SetDuplicateSuppressionOptions(size_t suppressAfter, size_t summaryInterval) {
    std::lock_guard<std::mutex> lock(m_DuplicateMutex);
    m_DuplicateSuppressAfter = suppressAfter;
    m_DuplicateSummaryInterval = summaryInterval;
    
    // Reset tracking when options change
    ResetDuplicateTracking();
}

bool SpdlogLogger::ProcessDuplicateMessage(const std::string& message, LogLevel level, const std::string& category) {
    if (!m_DuplicateSuppressionEnabled) {
        return false; // Not suppressed
    }
    
    std::lock_guard<std::mutex> lock(m_DuplicateMutex);
    
    auto now = std::chrono::steady_clock::now();
    
    // If this is a different message than the last one, or from a different category or level
    if (message != m_LastMessage.message || 
        level != m_LastMessage.level || 
        category != m_LastMessage.categoryName) {
            
        // If we had a previous message with repeats, log a final summary if it wasn't summarized yet
        if (m_LastMessage.repeatCount > m_DuplicateSuppressAfter && !m_LastMessage.summarized) {
            const std::string summaryMsg = fmt::format("Last message repeated {} times", m_LastMessage.repeatCount);
            
            // Get the appropriate logger
            const auto spdlogLevel = ConvertToSpdlogLevel(m_LastMessage.level);
            if (m_LastMessage.categoryName.empty()) {
                // Use core logger
                m_Impl->m_CoreLogger->log(spdlogLevel, summaryMsg);
            } else if (auto catLogger = GetSpdlogCategoryLogger(m_LastMessage.categoryName)) {
                // Use category logger
                catLogger->log(spdlogLevel, summaryMsg);
            }
        }
        
        // Set the new message as current
        m_LastMessage.message = message;
        m_LastMessage.level = level;
        m_LastMessage.categoryName = category;
        m_LastMessage.repeatCount = 1;
        m_LastMessage.lastTime = now;
        m_LastMessage.summarized = false;
        
        return false; // Not suppressed
    }
    
    // Same message, increment counter
    m_LastMessage.repeatCount++;
    m_LastMessage.lastTime = now;
    
    // Check if we should show a periodic summary
    if (m_DuplicateSummaryInterval > 0 && 
        m_LastMessage.repeatCount > m_DuplicateSuppressAfter &&
        m_LastMessage.repeatCount % m_DuplicateSummaryInterval == 0) {
          // Show periodic summary
        const std::string summaryMsg = fmt::format("Last message repeated {} times so far", m_LastMessage.repeatCount);
        
        // Get the appropriate logger
        const auto spdlogLevel = ConvertToSpdlogLevel(m_LastMessage.level);
        if (m_LastMessage.categoryName.empty()) {
            // Use core logger
            m_Impl->m_CoreLogger->log(spdlogLevel, summaryMsg);
        } else if (const auto catLogger = GetSpdlogCategoryLogger(m_LastMessage.categoryName)) {
            // Use category logger
            catLogger->log(spdlogLevel, summaryMsg);
        }
        
        m_LastMessage.summarized = true;
    }
    
    // Suppress if we've exceeded the threshold
    return m_LastMessage.repeatCount > m_DuplicateSuppressAfter;
}

void SpdlogLogger::ResetDuplicateTracking() {
    m_LastMessage = MessageInfo{};
}

void SpdlogLogger::ApplyLoggerConfig(const LoggerConfig& config) {
    // Apply global level setting
    SetGlobalLevel(config.globalLevel);
    
    // Apply pattern setting
    if (!config.pattern.empty()) {
        SetPattern(config.pattern);
    }
    
    // Apply file logging settings
    if (config.fileLogging.enabled) {
        EnableFileLogging(config.fileLogging.path);
    } else {
        DisableFileLogging();
    }
    
    // Apply duplicate suppression settings
    EnableDuplicateSuppression(config.duplicateSuppression.enabled);
    if (config.duplicateSuppression.enabled) {
        SetDuplicateSuppressionOptions(
            config.duplicateSuppression.suppressAfter,
            config.duplicateSuppression.summaryInterval
        );
    }
    
    // Apply category-specific log levels
    for (const auto& [category, level] : config.categories) {
        SetCategoryLevel(category, level);
    }
      // Log that configuration was applied
    LogInfo("Applied logger configuration: Global level=" + 
            LoggerConfig::LogLevelToString(config.globalLevel) +
            ", File logging=" + (config.fileLogging.enabled ? "enabled" : "disabled") +
            ", Categories=" + std::to_string(config.categories.size()),
            std::source_location::current());
}

void SpdlogLogger::Initialize(const std::string& jsonConfigPath) {
    // Completely redesigned implementation to avoid deadlocks
    
    // Check if JSON file exists first
    if (std::ifstream configFile(jsonConfigPath); !configFile.is_open()) {
        // File doesn't exist or can't be opened - do standard initialization and log the error
        std::cerr << "Cannot open config file: " << jsonConfigPath << std::endl;
        Initialize(); // Use standard initialization
        
        // Now it's safe to log the error since we're initialized
        if (m_IsInitialized) {
            LogError("Failed to open logger configuration file: " + jsonConfigPath, 
                     std::source_location::current());
        }
        return;
    }
    
    // If we get here, the file exists - do standard initialization first
    Initialize();
    
    // Now we're initialized, so we can parse and apply the JSON config
    try {
        if (const auto config = LoggerConfig::FromJson(jsonConfigPath)) {
            // Apply configuration
            ApplyLoggerConfig(*config);
            
            // Log success
            LogInfo("Logger configured from JSON file: " + jsonConfigPath,
                    std::source_location::current());
        }
    } catch (const std::exception& e) {
        // We're already initialized so we can log the error
        LogError("Error parsing logger configuration: " + std::string(e.what()),
                 std::source_location::current());
    }
}
