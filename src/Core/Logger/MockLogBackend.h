#pragma once

#include "ILogBackend.h"
#include <vector>
#include <mutex>

namespace BlackEngine {

/**
 * @brief Mock implementation of ILogBackend for testing
 * 
 * This backend stores log messages in memory instead of outputting them,
 * making it useful for verifying log behavior in tests.
 */
class MockLogBackend : public ILogBackend {
public:
    struct StoredLogMessage {
        std::string message;
        LogLevel level;
        std::string categoryName;
        std::string filename;
        int line;
        std::string function;
        bool isRepeat = false;
        int repeatCount = 0;
    };

    MockLogBackend();
    ~MockLogBackend() override;

    bool Initialize() override;
    void Shutdown() override;
    void Log(const LogMessage& message) override;
    void LogRepeat(const LogMessage& message, int count) override;
    void Flush() override;
    bool IsInitialized() const override { return m_Initialized; }
    void SetMinLogLevel(LogLevel level) override { m_MinLogLevel = level; }
    LogLevel GetMinLogLevel() const override { return m_MinLogLevel; }
    std::string GetName() const override { return "MockLogBackend"; }

    // Mock-specific methods for testing
    std::vector<StoredLogMessage> GetLogMessages();
    void ClearLogMessages();
    
    size_t GetLogCount() const;
    size_t GetLogCount(LogLevel level) const;
    size_t GetLogCount(const std::string& categoryName) const;
    
    bool ContainsMessage(const std::string& messageSubstring);
    bool ContainsMessage(LogLevel level, const std::string& messageSubstring);

private:
    bool m_Initialized = false;
    LogLevel m_MinLogLevel = LogLevel::Trace;
    
    std::vector<StoredLogMessage> m_LogMessages;
    mutable std::mutex m_LogMutex;
};

} // namespace BlackEngine
