#include "ConsoleLoggerBackend.h"
#include <iostream>

namespace BlackEngine {

ConsoleLoggerBackend::ConsoleLoggerBackend()
    : m_initialized(false), m_asyncConfigured(false) {
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
        
        // Loglama pattern'ini ayarla
        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v (%g:%#)");
        
        // Varsayılan flush yap
        m_logger->flush_on(spdlog::level::warn);
        
        m_initialized = true;
        return true;
    }
    catch (const std::exception& e) {
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

void ConsoleLoggerBackend::Log(const LogMessage& message) {
    if (!m_initialized || message.level == LogLevel::Off) {
        return;
    }
    
    try {
        // LogLevel'i spdlog seviyesine dönüştür
        const auto spdlogLevel = ToSpdlogLevel(message.level);
        
        // Format: "[Category] Message (file:line)"
        const std::string formattedMsg = fmt::format("[{}] {} ({}:{})",
            message.category,
            message.message,
            message.location.file_name(),
            message.location.line());
        
        // Log mesajını asenkron olarak gönder
        m_logger->log(spdlogLevel, formattedMsg);
    }
    catch (const std::exception& e) {
        // Loglama hatası durumunda fallback olarak std::cerr kullan
        std::cerr << "Loglama hatası: " << e.what() << std::endl;
    }
}

bool ConsoleLoggerBackend::ConfigureAsync(const size_t queueSize, const size_t threadCount) {
    try {
        if (!m_asyncConfigured) {
            // Eğer spdlog thread pool henüz oluşturulmamışsa oluştur
            if (!spdlog::thread_pool()) {
                spdlog::init_thread_pool(queueSize, threadCount);
            }
            m_asyncConfigured = true;
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Async yapılandırma hatası: " << e.what() << std::endl;
        return false;
    }
}

} // namespace BlackEngine
