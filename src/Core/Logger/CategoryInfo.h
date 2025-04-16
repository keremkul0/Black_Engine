#pragma once

#include "LogTypes.h"
#include <string>
#include <mutex>
#include <unordered_map>

namespace BlackEngine {

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
     * @brief Spam kontrol sayaçlarını sıfırlar
     */
    void ResetSpamControl();

private:
    std::string m_name;
    LogLevel m_level;
    
    // Spam kontrol - her mesaj için sayaç tutar
    std::mutex m_mutex;
    std::unordered_map<std::string, int> m_messageCounters;
};

} // namespace BlackEngine
