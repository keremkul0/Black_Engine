#pragma once

// First include the type definitions
#include "LogTypes.h"
#include "ILogBackend.h"

// Then include standard headers
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <source_location>
#include <format>

namespace BlackEngine {
    /**
     * @brief Manages and configures the logging backends
     */
    class LogBackendManager {
    public:
        LogBackendManager() = default;

        ~LogBackendManager() = default;

        void AddBackend(const LogBackendPtr &backend);

        void RemoveBackend(const std::string &backendName);

        void RemoveAllBackends();

        LogBackendPtr GetBackend(const std::string &backendName);

        std::vector<LogBackendPtr> GetAllBackends();

        bool Initialize();

        void Shutdown();

        void Log(const LogMessage &message);

        void LogRepeat(const LogMessage &message, int count);

        void Flush();

        void SetMinLogLevel(LogLevel level);

    private:
        std::vector<LogBackendPtr> m_Backends;
        std::mutex m_BackendMutex;
        LogLevel m_MinLogLevel = LogLevel::Trace;
    };

    /**
     * @brief Manages log categories and their configuration
     */
    class CategoryRegistry {
    public:
        CategoryRegistry() = default;

        ~CategoryRegistry() = default;

        void RegisterCategory(const std::string &name, LogLevel defaultLevel = LogLevel::Info);

        CategoryInfo *GetCategory(const std::string &name);

        std::vector<std::string> GetAllCategoryNames() const;

        bool SetCategoryLevel(const std::string &name, LogLevel level);

        bool SetCategoryEnabled(const std::string &name, bool enabled);

        // For handling repeated logs
        bool IsRepeatedMessage(const std::string &categoryName, const std::string &message, LogLevel level);

        void UpdateRepeatCount(const std::string &categoryName, const std::string &message, LogLevel level);

        int GetAndResetRepeatCount(const std::string &categoryName, const std::string &message, LogLevel level);

    private:
        std::unordered_map<std::string, CategoryInfo> m_Categories;
        std::unordered_map<LogMessageIdentifier, std::atomic<int> > m_RepeatCounts;
        mutable std::mutex m_CategoryMutex;
        mutable std::mutex m_RepeatMutex;
    };

    /**
     * @brief Main log manager singleton class that coordinates the entire logging system
     */
    class LogManager {
    public:
        static LogManager &GetInstance();

        // Delete copy/move constructors and assignment operators
        LogManager(const LogManager &) = delete;

        LogManager(LogManager &&) = delete;

        LogManager &operator=(const LogManager &) = delete;

        LogManager &operator=(LogManager &&) = delete;

        bool Initialize();

        void Shutdown();

        // Category Management
        void RegisterCategory(const std::string &name, LogLevel defaultLevel = LogLevel::Info);

        CategoryInfo *GetCategory(const std::string &name);

        bool SetCategoryLevel(const std::string &name, LogLevel level);

        bool SetCategoryEnabled(const std::string &name, bool enabled);

        // Backend Management
        void AddBackend(const LogBackendPtr &backend);

        void RemoveBackend(const std::string &backendName);

        LogBackendPtr GetBackend(const std::string &backendName);

        // Configuration
        bool LoadConfig(const std::string &configFile = "log_config.json");

        bool SaveConfig(const std::string &configFile = "log_config.json");

        void SetDefaultLogLevel(LogLevel level);

        LogLevel GetDefaultLogLevel() const { return m_DefaultLogLevel; }

        // Logging Operations
        template<typename... Args>
        void LogMessage(
            LogLevel level,
            const std::string &categoryName,
            const std::source_location &location,
            std::string_view fmt,
            Args &&... args);

        void Flush();

    private:
        LogManager();

        ~LogManager();

        static constexpr int REPEAT_LOG_THRESHOLD = 5; // Number of repeats before switching to condensed format

        void HandleRepeatedLog(const BlackEngine::LogMessage &message);

        // Core components
        CategoryRegistry m_CategoryRegistry;
        LogBackendManager m_BackendManager;

        // Default settings
        LogLevel m_DefaultLogLevel = LogLevel::Info;

        // For repeated log detection
        std::unordered_map<std::string, CategoryInfo::RepeatInfo> m_LastCategoryLogs;
        std::mutex m_LastLogMutex;

        // Current state
        bool m_Initialized = false;
        std::mutex m_ConfigMutex;
    };

    // Template method implementation
    template<typename... Args>
    void LogManager::LogMessage(
        LogLevel level,
        const std::string &categoryName,
        const std::source_location &location,
        std::string_view fmt,
        Args &&... args) {
        // Skip if not initialized
        if (!m_Initialized) return;

        // Skip if level is below default
        if (level < m_DefaultLogLevel) return;

        // Get category info
        CategoryInfo *category = GetCategory(categoryName);
        if (!category || !category->enabled || level < category->level) {
            return; // Skip if category doesn't exist, is disabled, or level is too low
        }

        // Format the message
        std::string formattedMessage;
        try {
            if constexpr (sizeof...(args) > 0) {
                // Çalışma zamanı format dizesini desteklemek için std::vformat kullanma
                std::string formatStr{fmt};

                // Format argümanlarını std::make_format_args ile paketleme
                auto formatArgs = std::make_format_args(std::forward<Args>(args)...);

                // vformat ile çalışma zamanında formatlama
                formattedMessage = std::vformat(formatStr, formatArgs);
            } else {
                formattedMessage = fmt;
            }
        } catch (const std::exception &e) {
            // If formatting fails, log the raw format string and error
            formattedMessage = std::string(fmt) + " [FORMAT ERROR: " + e.what() + "]";
        }

        // Create the log message
        BlackEngine::LogMessage message{
            formattedMessage,
            level,
            categoryName,
            location
        };

        // Handle repeated logs
        HandleRepeatedLog(message);
    }
}
