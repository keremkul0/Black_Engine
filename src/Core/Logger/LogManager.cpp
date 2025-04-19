#include "LogManager.h"
#include "ConsoleLoggerBackend.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace BlackEngine {

// Singleton implementasyonu
std::atomic<LogManager*> LogManager::s_instance{nullptr};
std::mutex LogManager::s_instanceMutex;

LogManager& LogManager::GetInstance() {
    LogManager* instance = s_instance.load(std::memory_order_acquire);
    if (!instance) {
        std::lock_guard<std::mutex> lock(s_instanceMutex);
        instance = s_instance.load(std::memory_order_relaxed);
        if (!instance) {
            instance = new LogManager();
            s_instance.store(instance, std::memory_order_release);
        }
    }
    return *instance;
}

bool LogManager::IsAvailable() {
    return s_instance.load(std::memory_order_acquire) != nullptr;
}

LogManager::LogManager()
    : m_defaultLogLevel(LogLevel::Info), m_initialized(false) {
}

LogManager::~LogManager() {
    Shutdown();
}

bool LogManager::Initialize(const std::string& configPath) {
    if (m_initialized) {
        return true; // Zaten başlatılmış
    }
    
    // Varsayılan konsol backend'i ekle
    if (m_backends.empty()) {
        if (const auto consoleBackend = std::make_shared<ConsoleLoggerBackend>(); !AddBackend(consoleBackend)) {
            std::cerr << "Varsayılan console backend eklenemedi." << std::endl;
            return false;
        }
    }
    
    // Yapılandırma dosyasını yükle
    if (!configPath.empty()) {
        if (!LoadConfig(configPath)) {
            std::cerr << "Yapılandırma dosyası yüklenemedi: " << configPath << std::endl;
            // Yapılandırma başarısız olsa bile devam edebiliriz, varsayılan değerlerle
        }
    }
    
    m_initialized = true;
    return true;
}

void LogManager::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    // Tüm backend'leri kapat
    {
        std::lock_guard<std::mutex> lock(m_backendMutex);
        for (const auto& backend : m_backends) {
            backend->Shutdown();
        }
        m_backends.clear();
    }
    
    // Kategorileri temizle
    {
        std::lock_guard<std::mutex> lock(m_categoryMutex);
        m_categories.clear();
    }
    
    m_initialized = false;
}

void LogManager::SetDefaultLogLevel(const LogLevel level) {
    m_defaultLogLevel = level;
}

LogLevel LogManager::GetDefaultLogLevel() const {
    return m_defaultLogLevel;
}

void LogManager::SetCategoryLevel(const std::string& categoryName, LogLevel level) {
    std::lock_guard<std::mutex> lock(m_categoryMutex);

    if (const auto it = m_categories.find(categoryName); it != m_categories.end()) {
        it->second.SetLevel(level);
    } else {
        // Insert kullanarak in-place oluşturma (emplace yerine)
        m_categories.try_emplace(categoryName, categoryName, level);
    }
}

void LogManager::SetCategorySpamControl(const std::string& categoryName, 
                                        bool enabled, 
                                        std::chrono::milliseconds interval) {
    CategoryInfo& category = GetOrCreateCategory(categoryName);
    category.SetRateLimit(enabled, interval);
}

LogLevel LogManager::GetCategoryLevel(const std::string& categoryName) {
    const CategoryInfo& category = GetOrCreateCategory(categoryName);
    return category.GetLevel();
}

bool LogManager::AddBackend(const std::shared_ptr<ILoggerBackend>& backend) {
    if (!backend) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_backendMutex);
    
    // Zaten eklenmiş mi kontrol et
    if (std::ranges::find(m_backends, backend) != m_backends.end()) {
        return true; // Zaten eklenmiş
    }
    
    // Başlat ve ekle
    if (!backend->Initialize()) {
        return false;
    }
    
    m_backends.push_back(backend);
    return true;
}

bool LogManager::RemoveBackend(const std::shared_ptr<ILoggerBackend>& backend) {
    if (!backend) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_backendMutex);

    if (const auto it = std::ranges::find(m_backends, backend); it != m_backends.end()) {
        (*it)->Shutdown();
        m_backends.erase(it);
        return true;
    }
    
    return false;
}

std::vector<std::shared_ptr<ILoggerBackend>> LogManager::GetBackends() const {
    std::lock_guard<std::mutex> lock(m_backendMutex);
    return m_backends;
}

bool LogManager::LoadConfig(const std::string& configPath) {
    try {
        std::ifstream configFile(configPath);
        if (!configFile.is_open()) {
            std::cerr << "Yapılandırma dosyası açılamadı: " << configPath << std::endl;
            return false;
        }
        
        nlohmann::json config;
        configFile >> config;
        
        // Varsayılan log seviyesi
        if (config.contains("defaultLogLevel") && config["defaultLogLevel"].is_string()) {
            const std::string defaultLevelStr = config["defaultLogLevel"];
            SetDefaultLogLevel(StringToLogLevel(defaultLevelStr));
        }
        
        // Kategori seviyeleri
        if (config.contains("categories") && config["categories"].is_object()) {
            for (auto& [key, value] : config["categories"].items()) {
                if (value.is_string()) {
                    std::string levelStr = value;
                    SetCategoryLevel(key, StringToLogLevel(levelStr));
                }
            }
        }
        
        // Spam kontrol ayarları
        if (config.contains("spamControl") && config["spamControl"].is_object()) {
            for (auto& [categoryName, settings] : config["spamControl"].items()) {
                if (settings.is_object()) {
                    bool enabled = false;
                    int intervalMs = 0;
                    
                    if (settings.contains("enabled") && settings["enabled"].is_boolean()) {
                        enabled = settings["enabled"];
                    }
                    
                    if (settings.contains("intervalMs") && settings["intervalMs"].is_number()) {
                        intervalMs = settings["intervalMs"];
                    }
                    
                    if (enabled) {
                        SetCategorySpamControl(categoryName, enabled, std::chrono::milliseconds(intervalMs));
                    }
                }
            }
        }
        
        // Spam kontrol ayarları
        if (config.contains("spamControl") && config["spamControl"].is_object()) {
            for (auto& [categoryName, settings] : config["spamControl"].items()) {
                if (settings.is_object()) {
                    bool enabled = false;
                    int intervalMs = 0;
                    
                    if (settings.contains("enabled") && settings["enabled"].is_boolean()) {
                        enabled = settings["enabled"];
                    }
                    
                    if (settings.contains("intervalMs") && settings["intervalMs"].is_number()) {
                        intervalMs = settings["intervalMs"];
                    }
                    
                    if (enabled) {
                        SetCategorySpamControl(categoryName, enabled, std::chrono::milliseconds(intervalMs));
                    }
                }
            }
        }
        
        // Asenkron yapılandırma
        size_t queueSize = 8192;
        size_t threadCount = 1;
        std::string overflowPolicy = "block"; // Varsayılan
        
        if (config.contains("async") && config["async"].is_object()) {
            auto& async = config["async"];
            if (async.contains("queueSize") && async["queueSize"].is_number()) {
                queueSize = async["queueSize"];
            }
            if (async.contains("threadCount") && async["threadCount"].is_number()) {
                threadCount = async["threadCount"];
            }
            if (async.contains("overflowPolicy") && async["overflowPolicy"].is_string()) {
                overflowPolicy = async["overflowPolicy"];
            }
        }
        
        // ConsoleLoggerBackend için async yapılandırmayı uygula
        std::lock_guard<std::mutex> lock(m_backendMutex);
        for (auto& backend : m_backends) {
            // ConsoleLoggerBackend mi?
            if (const auto consoleBackend = std::dynamic_pointer_cast<ConsoleLoggerBackend>(backend)) {
                consoleBackend->ConfigureAsync(queueSize, threadCount, overflowPolicy);
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Yapılandırma dosyası yükleme hatası: " << e.what() << std::endl;
        return false;
    }
}

void LogManager::ResetSpamControl() {
    std::lock_guard<std::mutex> lock(m_categoryMutex);
    for (auto &category: m_categories | std::views::values) {
        category.ResetSpamControl();
    }
}

bool LogManager::ShouldLog(LogLevel level, const std::string& categoryName) {
    if (level == LogLevel::Off) {
        return false;
    }
    
    LogLevel categoryLevel = GetCategoryLevel(categoryName);
    if (categoryLevel == LogLevel::Off) {
        return false;
    }
    
    return static_cast<int>(level) <= static_cast<int>(categoryLevel);
}

bool LogManager::PassesSpamControl(const std::string& categoryName, 
                                   const std::string& message, 
                                   const std::source_location& location, 
                                   int& repeatCount) {
    // Mesaj anahtarını dosya adı + satır numarası ile oluştur
    std::stringstream keyBuilder;
    keyBuilder << location.file_name() << ':' << location.line();
    const std::string messageKey = keyBuilder.str();
    
    // Kategori için spam kontrolü uygula
    CategoryInfo& category = GetOrCreateCategory(categoryName);
    
    // Tekrar sayısını al
    repeatCount = category.GetMessageRepeatCount(messageKey);
    
    // Spam kontrolünü uygula
    return category.ShouldLog(messageKey);
}

CategoryInfo& LogManager::GetOrCreateCategory(const std::string& categoryName) {
    std::lock_guard<std::mutex> lock(m_categoryMutex);

    if (const auto it = m_categories.find(categoryName); it != m_categories.end()) {
        return it->second;
    }
    
    // Kategori yoksa varsayılan seviye ile oluştur
    auto [newIt, inserted] = m_categories.try_emplace(
        categoryName, 
        categoryName,
        m_defaultLogLevel
    );
    
    return newIt->second;
}

} // namespace BlackEngine
