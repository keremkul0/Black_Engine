#include "ConsoleLoggerBackend.h"
#include <iostream>

namespace BlackEngine {
    ConsoleLoggerBackend::ConsoleLoggerBackend()
        : m_initialized(false)
          , m_asyncConfigured(false)
          , m_queueSize(8192)
          , m_threadCount(1)
          , m_overflowPolicy(spdlog::async_overflow_policy::block) {
    }

    bool ConsoleLoggerBackend::Initialize() {
        std::lock_guard<std::mutex> lock(m_mutex);
        try {
            if (!m_asyncConfigured) {
                ConfigureAsync(m_queueSize, m_threadCount, "block");
            }
            m_consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            m_logger = std::make_shared<spdlog::async_logger>(
                "console_logger",
                m_consoleSink,
                spdlog::thread_pool(),
                m_overflowPolicy
            );
            m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
            m_logger->flush_on(spdlog::level::warn);
            m_initialized = true;
            return true;
        } catch (const std::exception &e) {
            std::cerr << "ConsoleLoggerBackend başlatılamadı: " << e.what() << std::endl;
            return false;
        }
    }

    void ConsoleLoggerBackend::Shutdown() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_logger) {
            m_logger->flush();
            m_logger.reset();
        }
        m_consoleSink.reset();
        m_initialized = false;
    }

    void ConsoleLoggerBackend::Log(const LogMessage &message) {
        if (!m_initialized || message.level == LogLevel::Off) return;
        try {
            const auto spdlogLevel = ToSpdlogLevel(message.level);
            std::string fileName = message.location.file_name();
            if (const size_t lastSlash = fileName.find_last_of("\\/"); lastSlash != std::string::npos)
                fileName = fileName.substr(lastSlash + 1);
            std::string repeatInfo;
            if (message.repeatCount > 1)
                repeatInfo = fmt::format(" (x{})", message.repeatCount);
            const std::string formattedMsg = fmt::format(
                "[{}] {}{} ({}:{})",
                message.category,
                message.message,
                repeatInfo,
                fileName,
                message.location.line()
            );
            m_logger->log(spdlogLevel, formattedMsg);
        } catch (const std::exception &e) {
            std::cerr << "Logging error: " << e.what() << std::endl;
        }
    }

    bool ConsoleLoggerBackend::ConfigureAsync(
        const size_t queueSize,
        const size_t threadCount,
        const std::string &overflowPolicy
    ) {
        try {
            m_queueSize = queueSize;
            m_threadCount = threadCount;
            if (overflowPolicy == "overrun_oldest") {
                m_overflowPolicy = spdlog::async_overflow_policy::overrun_oldest;
            } else if (overflowPolicy == "discard") {
                m_overflowPolicy = spdlog::async_overflow_policy::discard_new;
            } else {
                m_overflowPolicy = spdlog::async_overflow_policy::block;
            }
            if (!m_asyncConfigured) {
                spdlog::init_thread_pool(m_queueSize, m_threadCount);
                m_asyncConfigured = true;
            } else if (m_initialized && m_consoleSink) {
                m_logger = std::make_shared<spdlog::async_logger>(
                    m_logger->name(),
                    m_consoleSink,
                    spdlog::thread_pool(),
                    m_overflowPolicy
                );
                m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
                m_logger->flush_on(spdlog::level::warn);
            }
            return true;
        } catch (const std::exception &e) {
            std::cerr << "Async configuration error: " << e.what() << std::endl;
            return false;
        }
    }

    spdlog::async_overflow_policy ConsoleLoggerBackend::GetOverflowPolicyForTest() const {
        return m_overflowPolicy;
    }
} // namespace BlackEngine
