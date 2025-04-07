#include "Logger.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/async.h"
#include "spdlog/fmt/fmt.h"
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <ranges>

namespace
{
    bool s_LoggerInitialized = false;

    // Manages the core and category loggers internally.
    class LogManager
    {
    public:
        static LogManager& Get()
        {
            static LogManager instance;
            return instance;
        }

        // Returns the core (default) logger.
        std::shared_ptr<spdlog::logger> GetCoreLogger()
        {
            return m_CoreLogger;
        }

        // Creates or retrieves a logger associated with a specific category.
        std::shared_ptr<spdlog::logger> GetCategoryLogger(const std::string& category)
        {
            std::lock_guard lock(m_LoggerMutex);

            // Check if we already have a logger for this category.
            if (auto it = m_CategoryLoggers.find(category); it != m_CategoryLoggers.end())
            {
                return it->second;
            }

            // Ensure previous logger with the same name is dropped, if any.
            try
            {
                spdlog::drop(category);
            }
            catch (...)
            {
                // Ignore errors when dropping.
            }

            // Create a new logger that shares the same sinks as the core logger.
            auto categoryLogger = std::make_shared<spdlog::logger>(category, m_Sinks.begin(), m_Sinks.end());
            categoryLogger->set_level(m_CoreLogger->level());
            categoryLogger->flush_on(spdlog::level::err);

            // Store it in our map.
            m_CategoryLoggers[category] = categoryLogger;

            // Register globally so spdlog can manage it as well.
            try
            {
                spdlog::register_logger(categoryLogger);
            }
            catch (const std::exception& ex)
            {
                std::cerr << "Failed to register logger '" << category << "': " << ex.what() << std::endl;
            }

            return categoryLogger;
        }

        // Initializes sinks and creates the core logger if not already done.
        void SetupSinks()
        {
            if (m_IsInitialized)
                return;

            // Create a colored console sink.
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            consoleSink->set_pattern("[%^%l%$] %v");
            m_Sinks.push_back(consoleSink);

            // Create the core logger with the available sinks.
            m_CoreLogger = std::make_shared<spdlog::logger>("BLACK_ENGINE", m_Sinks.begin(), m_Sinks.end());
            m_CoreLogger->set_level(spdlog::level::trace);

            // Register and set as default.
            spdlog::register_logger(m_CoreLogger);
            spdlog::set_default_logger(m_CoreLogger);

            m_IsInitialized = true;
        }

        // Sets the global log level (core + categories).
        void SetGlobalLevel(spdlog::level::level_enum level)
        {
            std::lock_guard<std::mutex> lock(m_LoggerMutex);
            m_CoreLogger->set_level(level);

            for (auto& loggerPtr : m_CategoryLoggers | std::views::values)
            {
                loggerPtr->set_level(level);
            }
        }

        // Sets a pattern (spdlog format string) for all active sinks.
        void SetPattern(const std::string& pattern)
        {
            std::lock_guard<std::mutex> lock(m_LoggerMutex);

            for (auto& sink : m_Sinks)
            {
                sink->set_pattern(pattern);
            }
        }

        // Shuts down all loggers, flushes, and clears resources.
        void ShutdownLoggers()
        {
            std::lock_guard<std::mutex> lock(m_LoggerMutex);

            // Flush everything.
            m_CoreLogger->flush();
            for (auto& loggerPtr : m_CategoryLoggers | std::views::values)
            {
                loggerPtr->flush();
            }

            m_FileSink.reset();
            m_Sinks.clear();
            m_CategoryLoggers.clear();
            m_CoreLogger.reset();

            // Let spdlog handle its own shutdown.
            spdlog::shutdown();

            m_IsInitialized = false;
        }

        // Enables file logging via a rotating file sink.
        void EnableFileLogging(const std::string& path)
        {
            std::lock_guard<std::mutex> lock(m_LoggerMutex);

            // Drop the existing core logger from the registry.
            spdlog::drop("BLACK_ENGINE");

            // If we already have a file sink, remove it before re-adding.
            if (m_FileSink)
            {
                auto it = std::ranges::find(m_Sinks, m_FileSink);
                if (it != m_Sinks.end())
                {
                    m_Sinks.erase(it);
                }
                m_FileSink.reset();
            }

            // Create directory if it doesn't exist.
            if (auto filePath = std::filesystem::path(path); !std::filesystem::exists(filePath.parent_path()))
            {
                std::filesystem::create_directories(filePath.parent_path());
            }

            // Create a rotating file sink (5MB limit, 3 rotated files).
            m_FileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path, 5 * 1024 * 1024, 3, true);
            m_FileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
            m_Sinks.push_back(m_FileSink);

            // Recreate the core logger with the updated sinks.
            m_CoreLogger = std::make_shared<spdlog::logger>("BLACK_ENGINE", m_Sinks.begin(), m_Sinks.end());
            m_CoreLogger->set_level(spdlog::level::trace);
            spdlog::register_logger(m_CoreLogger);
            spdlog::set_default_logger(m_CoreLogger);

            // Recreate all category loggers with the new sinks.
            for (auto& [name, loggerPtr] : m_CategoryLoggers)
            {
                try
                {
                    spdlog::drop(name);
                }
                catch (const std::exception& ex)
                {
                    std::cerr << "Failed to drop logger '" << name << "': " << ex.what() << std::endl;
                }

                auto newLogger = std::make_shared<spdlog::logger>(name, m_Sinks.begin(), m_Sinks.end());
                newLogger->set_level(loggerPtr->level());
                m_CategoryLoggers[name] = newLogger;

                spdlog::register_logger(newLogger);
            }
        }

        // Disables file logging by removing the file sink.
        void DisableFileLogging()
        {
            std::lock_guard<std::mutex> lock(m_LoggerMutex);

            if (!m_FileSink)
                return;

            auto it = std::ranges::find(m_Sinks, m_FileSink);
            if (it != m_Sinks.end())
            {
                m_Sinks.erase(it);
            }
            m_FileSink.reset();

            // Recreate the core logger without a file sink.
            m_CoreLogger = std::make_shared<spdlog::logger>("BLACK_ENGINE", m_Sinks.begin(), m_Sinks.end());
            m_CoreLogger->set_level(spdlog::level::trace);
            spdlog::register_logger(m_CoreLogger);
            spdlog::set_default_logger(m_CoreLogger);

            // Update category loggers.
            for (auto& [name, loggerPtr] : m_CategoryLoggers)
            {
                auto newLogger = std::make_shared<spdlog::logger>(name, m_Sinks.begin(), m_Sinks.end());
                newLogger->set_level(loggerPtr->level());
                m_CategoryLoggers[name] = newLogger;
                spdlog::register_logger(newLogger);
            }
        }

    private:
        std::shared_ptr<spdlog::logger> m_CoreLogger;
        std::vector<std::shared_ptr<spdlog::sinks::sink>> m_Sinks;
        std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> m_FileSink;
        std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> m_CategoryLoggers;
        std::mutex m_LoggerMutex;
        bool m_IsInitialized = false;
    };
}

// ----------------------------------------------------------------------------
// Public Logger Interface Implementation
// ----------------------------------------------------------------------------

void Logger::Initialize()
{
    // Prevent multiple initializations.
    if (s_LoggerInitialized)
        return;

    // Make sure any previous spdlog state is cleared.
    spdlog::shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Set up async logging (optional, but recommended for performance).
    spdlog::init_thread_pool(8192, 1);

    // Create console sinks and set up the core logger.
    LogManager::Get().SetupSinks();

    // Example: set a default pattern.
    // This can be changed later by Logger::SetPattern().
    Logger::SetPattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

    // Create the Logs directory if it doesn't exist.
    if (auto logDir = std::filesystem::path("Logs"); !std::filesystem::exists(logDir))
    {
        std::filesystem::create_directory(logDir);
    }

    // Optionally enable file logging with a default path.
    EnableFileLogging("Logs/BlackEngine.log");

    // Log a message indicating logger initialization.
    LogInfo("Logger initialized");
    s_LoggerInitialized = true;
}

void Logger::Shutdown()
{
    // Example message indicating logger shutdown.
    LogInfo("Logger shutting down");

    // Clean up all spdlog resources.
    LogManager::Get().ShutdownLoggers();
}

void Logger::Log(LogLevel level, const std::string& message, const std::source_location& location)
{
    auto logger = LogManager::Get().GetCoreLogger();
    auto spdLevel = ConvertToSpdlogLevel(level);
    logger->log(spdLevel, "{}:{} - {}", location.file_name(), location.line(), message);
}

void Logger::LogTrace(const std::string& message, const std::source_location& location)
{
    auto logger = LogManager::Get().GetCoreLogger();
    logger->trace("{}:{} - {}", location.file_name(), location.line(), message);
}

void Logger::LogDebug(const std::string& message, const std::source_location& location)
{
    auto logger = LogManager::Get().GetCoreLogger();
    logger->debug("{}:{} - {}", location.file_name(), location.line(), message);
}

void Logger::LogInfo(const std::string& message, const std::source_location& location)
{
    auto logger = LogManager::Get().GetCoreLogger();
    logger->info("{}:{} - {}", location.file_name(), location.line(), message);
}

void Logger::LogWarning(const std::string& message, const std::source_location& location)
{
    auto logger = LogManager::Get().GetCoreLogger();
    logger->warn("{}:{} - {}", location.file_name(), location.line(), message);
}

void Logger::LogError(const std::string& message, const std::source_location& location)
{
    auto logger = LogManager::Get().GetCoreLogger();
    logger->error("{}:{} - {}", location.file_name(), location.line(), message);
}

void Logger::LogCritical(const std::string& message, const std::source_location& location)
{
    auto logger = LogManager::Get().GetCoreLogger();
    logger->critical("{}:{} - {}", location.file_name(), location.line(), message);
}

std::shared_ptr<spdlog::logger> Logger::GetCategoryLogger(const std::string& category)
{
    return LogManager::Get().GetCategoryLogger(category);
}

void Logger::SetGlobalLevel(LogLevel level)
{
    auto spdLevel = ConvertToSpdlogLevel(level);
    LogManager::Get().SetGlobalLevel(spdLevel);
}

void Logger::SetCategoryLevel(const std::string& category, LogLevel level)
{
    auto spdLevel = ConvertToSpdlogLevel(level);
    auto catLogger = LogManager::Get().GetCategoryLogger(category);
    catLogger->set_level(spdLevel);
}

void Logger::EnableFileLogging(const std::string& filePath)
{
    LogManager::Get().EnableFileLogging(filePath);
}

void Logger::DisableFileLogging()
{
    LogManager::Get().DisableFileLogging();
}

void Logger::SetPattern(const std::string& pattern)
{
    LogManager::Get().SetPattern(pattern);
}

spdlog::level::level_enum Logger::ConvertToSpdlogLevel(LogLevel level)
{
    switch (level)
    {
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
