#include "MockLogBackend.h"
#include <algorithm>

namespace BlackEngine {
    MockLogBackend::MockLogBackend()
        : m_Initialized(false)
          , m_MinLogLevel(LogLevel::Trace) {
    }

    MockLogBackend::~MockLogBackend() {
        MockLogBackend::Shutdown();
    }

    bool MockLogBackend::Initialize() {
        m_Initialized = true;
        return true;
    }

    void MockLogBackend::Shutdown() {
        if (!m_Initialized) {
            return;
        }

        ClearLogMessages();
        m_Initialized = false;
    }

    void MockLogBackend::Log(const LogMessage &message) {
        if (!m_Initialized || message.level < m_MinLogLevel) {
            return;
        }

        StoredLogMessage storedMsg;
        storedMsg.message = message.message;
        storedMsg.level = message.level;
        storedMsg.categoryName = message.categoryName;
        storedMsg.filename = message.location.file_name();
        storedMsg.line = static_cast<int>(message.location.line());
        storedMsg.function = message.location.function_name();
        storedMsg.isRepeat = false;
        storedMsg.repeatCount = 1;

        std::lock_guard<std::mutex> lock(m_LogMutex);
        m_LogMessages.push_back(std::move(storedMsg));
    }

    void MockLogBackend::LogRepeat(const LogMessage &message, int count) {
        if (!m_Initialized || message.level < m_MinLogLevel) {
            return;
        }

        StoredLogMessage storedMsg;
        storedMsg.message = message.message;
        storedMsg.level = message.level;
        storedMsg.categoryName = message.categoryName;
        storedMsg.filename = message.location.file_name();
        storedMsg.line = static_cast<int>(message.location.line());
        storedMsg.function = message.location.function_name();
        storedMsg.isRepeat = true;
        storedMsg.repeatCount = count;

        std::lock_guard<std::mutex> lock(m_LogMutex);
        m_LogMessages.push_back(std::move(storedMsg));
    }

    void MockLogBackend::Flush() {
        // No-op for mock backend
    }

    std::vector<MockLogBackend::StoredLogMessage> MockLogBackend::GetLogMessages() {
        std::lock_guard<std::mutex> lock(m_LogMutex);
        return m_LogMessages;
    }

    void MockLogBackend::ClearLogMessages() {
        std::lock_guard<std::mutex> lock(m_LogMutex);
        m_LogMessages.clear();
    }

    size_t MockLogBackend::GetLogCount() const {
        std::lock_guard<std::mutex> lock(m_LogMutex);
        return m_LogMessages.size();
    }

    size_t MockLogBackend::GetLogCount(LogLevel level) const {
        std::lock_guard<std::mutex> lock(m_LogMutex);

        return std::ranges::count_if(m_LogMessages,
                                     [level](const StoredLogMessage &msg) {
                                         return msg.level == level;
                                     }
        );
    }

    size_t MockLogBackend::GetLogCount(const std::string &categoryName) const {
        std::lock_guard<std::mutex> lock(m_LogMutex);

        return std::ranges::count_if(m_LogMessages,
                                     [&categoryName](const StoredLogMessage &msg) {
                                         return msg.categoryName == categoryName;
                                     }
        );
    }

    bool MockLogBackend::ContainsMessage(const std::string &messageSubstring) {
        std::lock_guard<std::mutex> lock(m_LogMutex);

        return std::ranges::any_of(m_LogMessages,
                                   [&messageSubstring](const StoredLogMessage &msg) {
                                       return msg.message.find(messageSubstring) != std::string::npos;
                                   }
        );
    }

    bool MockLogBackend::ContainsMessage(LogLevel level, const std::string &messageSubstring) {
        std::lock_guard<std::mutex> lock(m_LogMutex);

        return std::ranges::any_of(m_LogMessages,
                                   [level, &messageSubstring](const StoredLogMessage &msg) {
                                       return msg.level == level &&
                                              msg.message.find(messageSubstring) != std::string::npos;
                                   }
        );
    }
} // namespace BlackEngine
