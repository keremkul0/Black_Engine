// ConsoleLoggerBackend.h
#pragma once

#include "ILoggerBackend.h"
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <mutex>

namespace BlackEngine {

    class ConsoleLoggerBackend final : public ILoggerBackend {
    public:
        ConsoleLoggerBackend();
        ~ConsoleLoggerBackend() override = default;

        bool Initialize() override;
        void Shutdown() override;
        void Log(const LogMessage& message) override;
        [[nodiscard]] bool IsInitialized() const override { return m_initialized; }

        bool ConfigureAsync(size_t queueSize = 8192,
                            size_t threadCount = 1,
                            const std::string& overflowPolicy = "block");

        [[nodiscard]] spdlog::async_overflow_policy GetOverflowPolicyForTest() const;

    private:
        bool m_initialized;
        bool m_asyncConfigured;
        size_t m_queueSize;
        size_t m_threadCount;
        spdlog::async_overflow_policy m_overflowPolicy;

        std::shared_ptr<spdlog::logger> m_logger;
        std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> m_consoleSink;
        std::mutex m_mutex;
    };

} // namespace BlackEngine