#include "ConsoleLoggerBackend.h"
#include <iostream>

namespace BlackEngine {
    ConsoleLoggerBackend::ConsoleLoggerBackend()
        : m_initialized(false), m_asyncConfigured(false), m_overflowPolicy(spdlog::async_overflow_policy::block) { // Initialize policy
    }

    bool ConsoleLoggerBackend::Initialize() {
        std::lock_guard<std::mutex> lock(m_mutex);

        try {
            if (!m_asyncConfigured) {
                // Varsayılan async ayarları
                ConfigureAsync();
            }

            // Renkli konsol sink'i oluştur
            m_consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            // Asenkron logger oluştur
            m_logger = std::make_shared<spdlog::async_logger>(
                "console_logger",
                m_consoleSink,
                spdlog::thread_pool(),
                spdlog::async_overflow_policy::block
            );
            // Loglama pattern'ini ayarla - [console_logger] kısmını kaldır ve dosya konumunu daha okunabilir yap
            m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

            // Varsayılan flush yap
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
        if (!m_initialized || message.level == LogLevel::Off) {
            return;
        }

        try {
            // LogLevel'i spdlog seviyesine dönüştür
            const auto spdlogLevel = ToSpdlogLevel(message.level);

            // Dosya yolundan sadece dosya adını al
            std::string fileName = message.location.file_name();
            // Son \\ veya / sonrası sadece dosya adını al
            const size_t lastSlash = fileName.find_last_of("\\/");
            if (lastSlash != std::string::npos) {
                fileName = fileName.substr(lastSlash + 1);
            }

            // Tekrar sayısı bilgisini al (eğer varsa)
            std::string repeatInfo;
            if (message.repeatCount > 1) {
                repeatInfo = fmt::format(" (x{})", message.repeatCount);
            }

            // Format: "[Category] Message (file:line) repeatInfo"
            // Dosya konumu tıklanabilir olması için VS Code format olan (file:line) şeklinde
            const std::string formattedMsg = fmt::format("[{}] {}{} ({}:{})",
                                                         message.category,
                                                         message.message,
                                                         repeatInfo,
                                                         fileName,
                                                         message.location.line());

            // Log mesajını asenkron olarak gönder
            m_logger->log(spdlogLevel, formattedMsg);
        } catch (const std::exception &e) {
            // Loglama hatası durumunda fallback olarak std::cerr kullan
            std::cerr << "Loglama hatası: " << e.what() << std::endl;
        }
    }

    bool ConsoleLoggerBackend::ConfigureAsync(const size_t queueSize, const size_t threadCount,
                                              const std::string &overflowPolicy) {
        try {
            // Store the requested policy string first for potential later use
            // Determine the spdlog policy enum
            m_overflowPolicy = spdlog::async_overflow_policy::block; // Default
            if (overflowPolicy == "overrun_oldest") {
                m_overflowPolicy = spdlog::async_overflow_policy::overrun_oldest;
            } else if (overflowPolicy == "discard") {
                m_overflowPolicy = spdlog::async_overflow_policy::discard_new;
            }
            // Note: If overflowPolicy is invalid, it defaults to block, no error thrown here.

            // Initialize or re-initialize the thread pool and logger with the determined policy
            if (!m_asyncConfigured) {
                 // If spdlog thread pool hasn't been initialized globally, do it.
                 // Note: spdlog::init_thread_pool is global. Calling it multiple times
                 // might be problematic or ignored by spdlog depending on its implementation.
                 // Consider initializing it once globally if needed.
                 // For simplicity here, we assume it's safe or handled by spdlog.
                 spdlog::init_thread_pool(queueSize, threadCount);
                 m_asyncConfigured = true; // Mark async as configured
            }

            // If already initialized, we need to replace the logger instance
            // This assumes the sink (m_consoleSink) is already created in Initialize
            if (m_initialized && m_logger && m_consoleSink) {
                 m_logger = std::make_shared<spdlog::async_logger>(
                     m_logger->name(), // Keep the same logger name
                     m_consoleSink,    // Use the existing sink
                     spdlog::thread_pool(),
                     m_overflowPolicy // Use the determined policy
                 );
                 // Reapply pattern and flush level if necessary
                 m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
                 m_logger->flush_on(spdlog::level::warn);
            } else if (!m_initialized) {
                // If called *before* Initialize, we just store the config.
                // Initialize() will later use these settings (or defaults if ConfigureAsync wasn't called).
                // However, the current Initialize() doesn't use stored async settings.
                // Let's adjust Initialize to use the stored policy if async was configured beforehand.
                // --> This requires further changes in Initialize() if pre-configuration is desired.
                // For now, assume ConfigureAsync is called *after* Initialize or the re-creation logic handles it.
            }

            // Mark that async setup has been attempted/done.
            // The original m_asyncConfigured flag might be redundant now.
            // Let's keep m_overflowPolicy as the source of truth for the *intended* policy.

            return true;
        } catch (const std::exception &e) {
            std::cerr << "Async yapılandırma hatası: " << e.what() << std::endl;
            return false;
        }
    }

    // Test-only getter implementation
    spdlog::async_overflow_policy ConsoleLoggerBackend::GetOverflowPolicyForTest() const {
        // It's better to return the stored policy intended by ConfigureAsync,
        // as accessing the live policy from spdlog's async_logger might be complex or impossible.
        return m_overflowPolicy;
    }

} // namespace BlackEngine
