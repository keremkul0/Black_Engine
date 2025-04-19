#pragma once

#include "LogTypes.h"
#include "CategoryInfo.h"
#include "ILoggerBackend.h"
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <memory>
#include <source_location>
#include <vector>
#include <spdlog/fmt/fmt.h>
#include <nlohmann/json.hpp>

namespace BlackEngine {
    /**
     * @brief Main log management class
     *
     * Manages categories and backends, filters and routes log messages.
     */
    class LogManager {
    public:
        /**
         * @brief Gets the singleton instance
         */
        static LogManager &GetInstance();

        /**
         * @brief Checks whether the LogManager is available
         */
        static bool IsAvailable();

        // Deleted member functions - prevents copying of the class
        LogManager(const LogManager &) = delete;

        LogManager &operator=(const LogManager &) = delete;

        /**
         * @brief Initializes the log system
         * @param configPath Configuration file path
         * @return Success status
         */
        bool Initialize(const std::string &configPath = "log_config.json");

        /**
         * @brief Shuts down the log system
         */
        void Shutdown();

        /**
         * @brief Sets the default log level
         */
        void SetDefaultLogLevel(LogLevel level);

        /**
         * @brief Returns the default log level
         */
        LogLevel GetDefaultLogLevel() const;

        /**
         * @brief Sets the category level
         */
        void SetCategoryLevel(const std::string &categoryName, LogLevel level);

        /**
         * @brief Returns the category level, or uses the default level if it does not exist
         */
        LogLevel GetCategoryLevel(const std::string &categoryName);

        /**
         * @brief Sets the spam control settings for the category
         * @param categoryName Category name
         * @param enabled Is spam control enabled?
         * @param interval Minimum print interval (ms)
         */
        void SetCategorySpamControl(const std::string &categoryName,
                                    bool enabled,
                                    std::chrono::milliseconds interval);

        /**
         * @brief Main logging function (called via macros)
         */
        template<typename... Args>
        void Log(const LogLevel level, const std::string &categoryName,
                 const std::source_location &location, fmt::format_string<Args...> fmt, Args &&... args) {
            // Quick exit check
            if (level == LogLevel::Off || !ShouldLog(level, categoryName)) {
                return;
            }

            // Format the message
            std::string formattedMessage;
            try {
                formattedMessage = fmt::format(fmt, std::forward<Args>(args)...);
            } catch (const std::exception &e) {
                formattedMessage = fmt::format("Message formatting error: {}", e.what());
            }

            // Get spam control and repeat count
            int repeatCount = 1;
            if (!PassesSpamControl(categoryName, formattedMessage, location, repeatCount)) {
                return;
            }

            // Create log message
            LogMessage message;
            message.level = level;
            message.category = categoryName;
            message.message = std::move(formattedMessage);
            message.location = location;
            message.timestamp = std::chrono::system_clock::now();
            message.repeatCount = repeatCount; // Transfer repeat count to message

            // Send to all backends
            std::lock_guard<std::mutex> lock(m_backendMutex);
            for (const auto &backend: m_backends) {
                backend->Log(message);
            }
        }

        /**
         * @brief Adds a backend
         * @param backend Backend to add
         * @return Success status
         */
        bool AddBackend(const std::shared_ptr<ILoggerBackend> &backend);

        /**
         * @brief Removes a backend
         * @param backend Backend to remove
         * @return Success status
         */
        bool RemoveBackend(const std::shared_ptr<ILoggerBackend> &backend);

        /**
         * @brief Returns all backends
         */
        std::vector<std::shared_ptr<ILoggerBackend> > GetBackends() const;

        /**
         * @brief Loads the JSON configuration file
         * @param configPath File path
         * @return Success status
         */
        bool LoadConfig(const std::string &configPath);

        /**
         * @brief Resets all spam controls
         */
        void ResetSpamControl();

    private:
        // Singleton implementation
        LogManager();

        ~LogManager();

        /**
         * @brief Checks whether the message should be logged according to the log level
         */
        bool ShouldLog(LogLevel level, const std::string &categoryName);

        /**
         * @brief Checks whether the message should be logged according to spam control
         * @param categoryName Category name
         * @param message Message content
         * @param location Source file location
         * @param[out] repeatCount Message repeat count (returned by reference)
         * @return true: message should be logged, false: spam control blocked it
         */
        bool PassesSpamControl(const std::string &categoryName,
                               const std::string &message,
                               const std::source_location &location,
                               int &repeatCount);

        /**
         * @brief Gets the category information or creates a new one
         */
        CategoryInfo &GetOrCreateCategory(const std::string &categoryName);

        static std::atomic<LogManager *> s_instance;
        static std::mutex s_instanceMutex;

        std::mutex m_categoryMutex;
        std::unordered_map<std::string, CategoryInfo> m_categories;

        LogLevel m_defaultLogLevel;

        std::vector<std::shared_ptr<ILoggerBackend> > m_backends;
        mutable std::mutex m_backendMutex;

        bool m_initialized;
    };
} // namespace BlackEngine
