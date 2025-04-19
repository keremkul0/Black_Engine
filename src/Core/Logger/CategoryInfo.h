#pragma once

#include "LogTypes.h"
#include <string>
#include <mutex>
#include <unordered_map>
#include <chrono>

namespace BlackEngine {

/**
 * @brief Mesaj istatistikleri için yapı
 */
struct MessageStats {
    uint32_t counter{0};                       // Mesaj sayacı
    std::chrono::steady_clock::time_point lastPrint{};  // Son basım zamanı
};

/**
 * @brief Kategori bilgilerini ve spam kontrolünü yöneten sınıf
 */
class CategoryInfo {
public:
    /**
     * @brief Constructor
     * @param name Kategori adı
     * @param level Başlangıç log seviyesi
     */
    CategoryInfo(std::string  name, LogLevel level);
    
    /**
     * @brief Kategori adını döndürür
     */
    const std::string& GetName() const { return m_name; }
    
    /**
     * @brief Kategori log seviyesini döndürür
     */
    LogLevel GetLevel() const { return m_level; }
    
    /**
     * @brief Kategori log seviyesini ayarlar
     */
    void SetLevel(const LogLevel level) { m_level = level; }
    
    /**
     * @brief Spam kontrolüne göre mesajın loglanıp loglanmayacağını kontrol eder
     * @param messageKey Mesaj için benzersiz anahtar
     * @return true: mesaj loglanmalı, false: spam kontrolü engelledi
     */
    bool ShouldLog(const std::string& messageKey);
    
    /**
     * @brief Belirli bir mesajın tekrar sayısını döndürür
     * @param messageKey Mesaj için benzersiz anahtar
     * @return Tekrar sayısı
     */
    int GetMessageRepeatCount(const std::string& messageKey);
    
    /**
     * @brief Spam kontrol sayaçlarını sıfırlar
     */
    void ResetSpamControl();
    
    /**
     * @brief Frekans sınırı kontrolünü ayarlar
     * @param enabled Frekans sınırı aktif mi?
     * @param interval Minimum basım aralığı (ms)
     */
    void SetRateLimit(bool enabled, std::chrono::milliseconds interval);
    
    /**
     * @brief Frekans sınırı durumunu kontrol eder
     * @return Frekans sınırı etkin mi?
     */
    bool IsRateLimitEnabled() const { return m_rateLimitEnabled; }
    
    /**
     * @brief Frekans sınırı aralığını döndürür
     * @return Frekans sınırı aralığı (ms)
     */
    std::chrono::milliseconds GetRateLimit() const { return m_rateLimit; }

private:
    std::string m_name;
    LogLevel m_level;
    
    // Frekans sınırı ayarları
    bool m_rateLimitEnabled{false};
    std::chrono::milliseconds m_rateLimit{0};
    
    // Spam kontrol - her mesaj için istatistik tutar
    std::mutex m_mutex;
    std::unordered_map<std::string, MessageStats> m_messageCounters;
};

} // namespace BlackEngine
