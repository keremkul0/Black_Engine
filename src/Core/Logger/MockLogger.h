#pragma once

#include "ILoggerBackend.h"
#include <vector>
#include <mutex>

namespace BlackEngine {

/**
 * @brief Test amaçlı mock logger backend
 * 
 * Log mesajlarını belleğe kaydeder ve test sırasında sorgular yapmaya olanak tanır
 */
class MockLogger final : public ILoggerBackend {
public:
    MockLogger();
    ~MockLogger() override = default;
    
    /**
     * @brief Mock logger'ı başlatır
     * @return Başarı durumu
     */
    bool Initialize() override;
    
    /**
     * @brief Mock logger'ı kapatır
     */
    void Shutdown() override;
    
    /**
     * @brief Log mesajını belleğe kaydeder
     * @param message Log mesajı
     */
    void Log(const LogMessage& message) override;
    
    /**
     * @brief Backend'in başlatılma durumunu kontrol eder
     */
    bool IsInitialized() const override { return m_initialized; }
    
    /**
     * @brief Tüm kayıtlı log mesajlarını döndürür
     * @return Log mesajları
     */
    std::vector<LogMessage> GetMessages() const;
    
    /**
     * @brief Kayıtlı log mesajlarını temizler
     */
    void Clear();
    
    /**
     * @brief Belirli kriterlere göre mesaj arar
     * @param level Log seviyesi
     * @param category Kategori adı
     * @param messageContains Mesaj içeriği
     * @return Mesaj bulundu mu
     */
    bool ContainsMessage(LogLevel level, const std::string& category, const std::string& messageContains) const;
    
    /**
     * @brief Belirli kriterlere göre mesaj sayısını döndürür
     * @param level Log seviyesi
     * @param category Kategori adı (boş ise tüm kategoriler)
     * @return Mesaj sayısı
     */
    int CountMessages(LogLevel level, const std::string& category = "") const;

private:
    bool m_initialized;
    mutable std::mutex m_mutex;
    std::vector<LogMessage> m_messages;
};

} // namespace BlackEngine
