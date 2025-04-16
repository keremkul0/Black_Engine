#pragma once

#include "ILoggerBackend.h"
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <mutex>

namespace BlackEngine {

/**
 * @brief Spdlog tabanlı konsol logger backend
 * 
 * Spdlog'un asenkron logger ve renkli stdout sink'ini kullanarak
 * loglama yapar.
 */
class ConsoleLoggerBackend final : public ILoggerBackend {
public:
    ConsoleLoggerBackend();

    ~ConsoleLoggerBackend() override = default;
    
    /**
     * @brief Backend'i başlatır ve spdlog yapılandırmasını yapar
     * @return Başarı durumu
     */
    bool Initialize() override;
    
    /**
     * @brief Backend'i kapatır
     */
    void Shutdown() override;
    
    /**
     * @brief Log mesajını spdlog üzerinden konsola yazar
     * @param message Log mesajı
     */
    void Log(const LogMessage& message) override;
    
    /**
     * @brief Backend'in başlatılma durumunu kontrol eder
     */
    [[nodiscard]] bool IsInitialized() const override { return m_initialized; }
    
    /**
     * @brief Async thread pool ve kuyruk ayarlarını yapılandırır
     * @param queueSize Async kuyruk boyutu
     * @param threadCount Worker thread sayısı
     * @return Başarı durumu
     */
    bool ConfigureAsync(size_t queueSize = 8192, size_t threadCount = 1);

private:
    bool m_initialized;
    std::shared_ptr<spdlog::logger> m_logger;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> m_consoleSink;
    std::mutex m_mutex;
    bool m_asyncConfigured;
};

} // namespace BlackEngine
