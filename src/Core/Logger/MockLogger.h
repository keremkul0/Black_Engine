#pragma once

#include "ILoggerBackend.h"
#include <vector>
#include <sstream>
#include <mutex>
#include <spdlog/fmt/fmt.h>

namespace BlackEngine {
    /**
     * @brief Mock Logger class for testing purposes
     *
     * Stores log messages in memory and can be inspected by the test.
     */
    class MockLogger final : public ILoggerBackend {
    public:
        MockLogger() : m_initialized(false) {
        }

        ~MockLogger() override = default;

        bool Initialize() override {
            m_initialized = true;
            return true;
        }

        void Shutdown() override {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_logs.clear();
            m_formattedLogs.clear();
            m_initialized = false;
        }

        void Log(const LogMessage &message) override {
            if (!m_initialized || message.level == LogLevel::Off) {
                return;
            }

            std::lock_guard<std::mutex> lock(m_mutex);

            // Save the log
            m_logs.push_back(message);

            // Get only the file name from the file path
            std::string fileName = message.location.file_name();
            const size_t lastSlash = fileName.find_last_of("\\/");
            if (lastSlash != std::string::npos) {
                fileName = fileName.substr(lastSlash + 1);
            }

            // Formatted log message
            std::ostringstream formattedMsg;

            // Add repeat information
            std::string repeatInfo;
            if (message.repeatCount > 1) {
                repeatInfo = fmt::format(" (x{})", message.repeatCount);
            }

            // Format: "[Category] Message (file:line) repeatInfo"
            formattedMsg << "[" << message.category << "] "
                    << message.message
                    << repeatInfo
                    << " (" << fileName << ":" << message.location.line() << ")";

            m_formattedLogs.push_back(formattedMsg.str());
        }

        [[nodiscard]] bool IsInitialized() const override {
            return m_initialized;
        }

        // Test helper functions:

        int GetLogCount() const {
            std::lock_guard<std::mutex> lock(m_mutex);
            return static_cast<int>(m_logs.size());
        }

        LogMessage GetLastMessage() const {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_logs.empty()) {
                // Return an empty message
                return {};
            }
            return m_logs.back();
        }

        std::string GetLastFormattedMessage() const {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_formattedLogs.empty()) {
                return {};
            }
            return m_formattedLogs.back();
        }

        void ClearLogs() {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_logs.clear();
            m_formattedLogs.clear();
        }

        const std::vector<LogMessage> &GetAllLogs() const {
            return m_logs;
        }

    private:
        bool m_initialized;
        mutable std::mutex m_mutex;
        std::vector<LogMessage> m_logs;
        std::vector<std::string> m_formattedLogs;
    };
}
