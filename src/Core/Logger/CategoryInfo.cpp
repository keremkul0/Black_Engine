#include "CategoryInfo.h"

#include <utility>

namespace BlackEngine {
    CategoryInfo::CategoryInfo(std::string name, const LogLevel level)
        : m_name(std::move(name)), m_level(level) {
    }

    bool CategoryInfo::ShouldLog(const std::string &messageKey) {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Update message statistics.
        auto &[counter, lastPrint] = m_messageCounters[messageKey];
        ++counter;

        // Log every time if rate limiting is not enabled.
        if (!m_rateLimitEnabled) {
            return true;
        }

        // Time-based check - get the current time.
        const auto now = std::chrono::steady_clock::now();

        // Log if it is the first message or enough time has passed.
        if (lastPrint.time_since_epoch().count() == 0 ||
            (now - lastPrint) >= m_rateLimit) {
            lastPrint = now;
            return true;
        }

        // Logging blocked due to frequency limitation.
        return false;
    }

    void CategoryInfo::ResetSpamControl() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messageCounters.clear();
    }

    int CategoryInfo::GetMessageRepeatCount(const std::string &messageKey) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (const auto it = m_messageCounters.find(messageKey); it != m_messageCounters.end()) {
            return static_cast<int>(it->second.counter);
        }
        return 0; // No repetition.
    }

    void CategoryInfo::SetRateLimit(const bool enabled, const std::chrono::milliseconds interval) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_rateLimitEnabled = enabled;
        m_rateLimit = interval;
    }
} // namespace BlackEngine
