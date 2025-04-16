#include "MockLogger.h"
#include <algorithm>

namespace BlackEngine {
    MockLogger::MockLogger()
        : m_initialized(false) {
    }

    bool MockLogger::Initialize() {
        m_initialized = true;
        return true;
    }

    void MockLogger::Shutdown() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.clear();
        m_initialized = false;
    }

    void MockLogger::Log(const LogMessage &message) {
        if (!m_initialized || message.level == LogLevel::Off) {
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.push_back(message);
    }

    std::vector<LogMessage> MockLogger::GetMessages() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_messages;
    }

    void MockLogger::Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.clear();
    }

    bool MockLogger::ContainsMessage(const LogLevel level, const std::string &category,
                                     const std::string &messageContains) const {
        std::lock_guard<std::mutex> lock(m_mutex);

        return std::ranges::any_of(m_messages,
                                   [&](const LogMessage &msg) {
                                       return msg.level == level &&
                                              msg.category == category &&
                                              msg.message.find(messageContains) != std::string::npos;
                                   });
    }

    int MockLogger::CountMessages(const LogLevel level, const std::string &category) const {
        std::lock_guard<std::mutex> lock(m_mutex);

        return static_cast<int>(std::ranges::count_if(m_messages,
                                                      [&](const LogMessage &msg) {
                                                          return msg.level == level &&
                                                                 (category.empty() || msg.category == category);
                                                      }));
    }
} // namespace BlackEngine
