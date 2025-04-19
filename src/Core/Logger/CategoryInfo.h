#pragma once

#include "LogTypes.h"
#include <string>
#include <mutex>
#include <unordered_map>
#include <chrono>

namespace BlackEngine {
    /**
     * @brief Structure for message statistics
     */
    struct MessageStats {
        uint32_t counter{0}; // Message counter
        std::chrono::steady_clock::time_point lastPrint{}; // Last print time
    };

    /**
     * @brief Class that manages category information and spam control
     */
    class CategoryInfo {
    public:
        /**
         * @brief Constructor
         * @param name Category name
         * @param level Initial log level
         */
        CategoryInfo(std::string name, LogLevel level);

        /**
         * @brief Returns the category name
         */
        const std::string &GetName() const { return m_name; }

        /**
         * @brief Returns the category log level
         */
        LogLevel GetLevel() const { return m_level; }

        /**
         * @brief Sets the category log level
         */
        void SetLevel(const LogLevel level) { m_level = level; }

        /**
         * @brief Checks whether the message should be logged according to spam control
         * @param messageKey Unique key for the message
         * @return true: message should be logged, false: spam control blocked it
         */
        bool ShouldLog(const std::string &messageKey);

        /**
         * @brief Returns the repeat count of a specific message
         * @param messageKey Unique key for the message
         * @return Repeat count
         */
        int GetMessageRepeatCount(const std::string &messageKey);

        /**
         * @brief Resets spam control counters
         */
        void ResetSpamControl();

        /**
         * @brief Sets the frequency limit control
         * @param enabled Is frequency limit active?
         * @param interval Minimum print interval (ms)
         */
        void SetRateLimit(bool enabled, std::chrono::milliseconds interval);

        /**
         * @brief Checks the frequency limit status
         * @return Is frequency limit enabled?
         */
        bool IsRateLimitEnabled() const { return m_rateLimitEnabled; }

        /**
         * @brief Returns the frequency limit interval
         * @return Frequency limit interval (ms)
         */
        std::chrono::milliseconds GetRateLimit() const { return m_rateLimit; }

    private:
        std::string m_name;
        LogLevel m_level;

        // Frequency limit settings
        bool m_rateLimitEnabled{false};
        std::chrono::milliseconds m_rateLimit{0};

        // Spam control - keeps statistics for each message
        std::mutex m_mutex;
        std::unordered_map<std::string, MessageStats> m_messageCounters;
    };
} // namespace BlackEngine
