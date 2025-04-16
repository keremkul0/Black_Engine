#pragma once

#include "LogTypes.h"
#include "CategoryInfo.h"
#include "ILoggerBackend.h"
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <memory>
#include <source_location>
#include <vector>
#include <spdlog/fmt/fmt.h>
#include <nlohmann/json.hpp>

namespace BlackEngine {

/**
 * @brief Ana log yönetim sınıfı
 * 
 * Kategorileri ve backend'leri yönetir, log mesajlarını filtreleyip yönlendirir.
 */
class LogManager {
public:
    /**
     * @brief Singleton örneği alır
     */
    static LogManager& GetInstance();
    
    /**
     * @brief LogManager'ın kullanılabilir olup olmadığını kontrol eder
     */
    static bool IsAvailable();
    
    // Silinen üye fonksiyonlar - sınıfın kopyalanması engellendi
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    
    /**
     * @brief Log sistemini başlatır
     * @param configPath Konfigürasyon dosya yolu
     * @return Başarı durumu
     */
    bool Initialize(const std::string& configPath = "log_config.json");
    
    /**
     * @brief Log sistemini kapatır
     */
    void Shutdown();
    
    /**
     * @brief Varsayılan log seviyesini ayarlar
     */
    void SetDefaultLogLevel(LogLevel level);
    
    /**
     * @brief Varsayılan log seviyesini döndürür
     */
    LogLevel GetDefaultLogLevel() const;
    
    /**
     * @brief Kategori seviyesini ayarlar
     */
    void SetCategoryLevel(const std::string& categoryName, LogLevel level);
    
    /**
     * @brief Kategori seviyesini döndürür, yoksa varsayılan seviyeyi kullanır
     */
    LogLevel GetCategoryLevel(const std::string& categoryName);
    
    /**
     * @brief Ana loglama fonksiyonu (makrolar üzerinden çağrılır)
     */
    template<typename... Args>
    void Log(const LogLevel level, const std::string& categoryName,
             const std::source_location& location, fmt::format_string<Args...> fmt, Args&&... args) {
        
        // Hızlı çıkış kontrolü
        if (level == LogLevel::Off || !ShouldLog(level, categoryName)) {
            return;
        }
        
        // Mesajı biçimlendir
        std::string formattedMessage;
        try {
            formattedMessage = fmt::format(fmt, std::forward<Args>(args)...);
        } catch (const std::exception& e) {
            formattedMessage = fmt::format("Mesaj biçimlendirme hatası: {}", e.what());
        }
        
        // Spam kontrolü
        if (!PassesSpamControl(categoryName, formattedMessage)) {
            return;
        }
        
        // Log mesajını oluştur
        LogMessage message;
        message.level = level;
        message.category = categoryName;
        message.message = std::move(formattedMessage);
        message.location = location;
        message.timestamp = std::chrono::system_clock::now();
        
        // Tüm backend'lere gönder
        std::lock_guard<std::mutex> lock(m_backendMutex);
        for (const auto& backend : m_backends) {
            backend->Log(message);
        }
    }
    
    /**
     * @brief Backend ekler
     * @param backend Eklenecek backend
     * @return Başarı durumu
     */
    bool AddBackend(const std::shared_ptr<ILoggerBackend>& backend);
    
    /**
     * @brief Backend kaldırır
     * @param backend Kaldırılacak backend
     * @return Başarı durumu
     */
    bool RemoveBackend(const std::shared_ptr<ILoggerBackend>& backend);
    
    /**
     * @brief Tüm backend'leri döndürür
     */
    std::vector<std::shared_ptr<ILoggerBackend>> GetBackends() const;
    
    /**
     * @brief JSON yapılandırma dosyasını yükler
     * @param configPath Dosya yolu
     * @return Başarı durumu
     */
    bool LoadConfig(const std::string& configPath);
    
    /**
     * @brief Tüm spam kontrollerini sıfırlar
     */
    void ResetSpamControl();

private:
    // Singleton implementasyonu
    LogManager();
    ~LogManager();
    
    /**
     * @brief Mesajın log level'a göre loglanıp loglanmayacağını kontrol eder
     */
    bool ShouldLog(LogLevel level, const std::string& categoryName);
    
    /**
     * @brief Mesajın spam kontrolüne göre loglanıp loglanmayacağını kontrol eder
     */
    bool PassesSpamControl(const std::string& categoryName, const std::string& message);
    
    /**
     * @brief Kategori bilgisini alır veya yeni oluşturur
     */
    CategoryInfo& GetOrCreateCategory(const std::string& categoryName);
    
    static std::atomic<LogManager*> s_instance;
    static std::mutex s_instanceMutex;
    
    std::mutex m_categoryMutex;
    std::unordered_map<std::string, CategoryInfo> m_categories;
    
    LogLevel m_defaultLogLevel;
    
    std::vector<std::shared_ptr<ILoggerBackend>> m_backends;
    mutable std::mutex m_backendMutex;
    
    bool m_initialized;
};

} // namespace BlackEngine
