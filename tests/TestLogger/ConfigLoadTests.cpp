#include <gtest/gtest.h>
#include "Core/Logger/LogManager.h"
#include "Core/Logger/LogTypes.h"
#include "Core/Logger/MockLogger.h"
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace BlackEngine::Tests {

using namespace std::chrono_literals;

class ConfigLoadTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Test için geçici bir dosya yolu oluştur
        tempConfigPath = std::filesystem::temp_directory_path() / "temp_log_config.json";
        
        // Test için bir mock logger oluştur
        mockLogger = std::make_shared<MockLogger>();
        mockLogger->Initialize();
        
        // Daha önce eklenen backend'leri temizle
        auto& logManager = LogManager::GetInstance();
        auto backends = logManager.GetBackends();
        for (const auto& backend : backends) {
            logManager.RemoveBackend(backend);
        }
        
        // Kendi test logger'ımızı ekle
        logManager.AddBackend(mockLogger);
        
        // LogManager'ı sıfırla
        logManager.ResetSpamControl();
    }

    void TearDown() override {
        // Test dosyasını temizle
        if (std::filesystem::exists(tempConfigPath)) {
            std::filesystem::remove(tempConfigPath);
        }
        
        // LogManager'ı sıfırla
        LogManager::GetInstance().SetDefaultLogLevel(LogLevel::Debug);
        LogManager::GetInstance().ResetSpamControl();
        
        // Mock logger'ı kaldır
        LogManager::GetInstance().RemoveBackend(mockLogger);
        mockLogger->Shutdown();
        mockLogger = nullptr;
    }
    
    // Belirtilen içerikle geçici bir config JSON dosyası oluşturur
    void CreateTempConfigFile(const nlohmann::json& configJson) const {
        std::ofstream configFile(tempConfigPath);
        configFile << configJson.dump(4); // 4 space indentation
        configFile.close();
        
        // Dosya oluşturulduğundan emin ol
        ASSERT_TRUE(std::filesystem::exists(tempConfigPath)) << "Geçici config dosyası oluşturulamadı";
    }

    std::filesystem::path tempConfigPath;
    std::shared_ptr<MockLogger> mockLogger;
    
    // Spam kontrolünü dolaylı olarak test etmek için yardımcı fonksiyon
    void TestCategorySpamControl(const std::string& categoryName, bool expectedEnabled, 
                               std::chrono::milliseconds expectedInterval) const {
        // LogManager'ı temizle/sıfırla
        LogManager& logManager = LogManager::GetInstance();
        std::source_location location = std::source_location::current();
          // İlk log
        mockLogger->ClearLogs();
        logManager.Log(LogLevel::Critical, categoryName, location, "Spam test message");
        EXPECT_EQ(1, mockLogger->GetLogCount()) << "İlk mesaj backend'e iletilmedi: " << categoryName;
        
        // Hemen aynı mesajı tekrar dene
        mockLogger->ClearLogs();
        logManager.Log(LogLevel::Critical, categoryName, location, "Spam test message");
        
        if (expectedEnabled) {
            // Rate limit etkinse, mesaj engellenmeli
            EXPECT_EQ(0, mockLogger->GetLogCount()) << "Rate-limit etkin olmasına rağmen mesaj engellenmedi: " << categoryName;
              // Bekleme süresi dolunca mesaj geçmeli
            std::this_thread::sleep_for(expectedInterval + 1ms); // Sadece eşik + 1 ms
            mockLogger->ClearLogs();
            logManager.Log(LogLevel::Critical, categoryName, location, "Spam test message");
            EXPECT_EQ(1, mockLogger->GetLogCount()) << "Süre dolduktan sonra mesaj backend'e iletilmedi: " << categoryName;
        } else {
            // Rate limit etkin değilse, mesaj engellenmemeli
            EXPECT_EQ(1, mockLogger->GetLogCount()) << "Rate-limit etkin olmamasına rağmen mesaj engellendi: " << categoryName;
        }
    }
};

// Temel konfigürasyon yükleme testi
TEST_F(ConfigLoadTests, BasicConfigLoad_SetsLevelsCorrectly) {
    // Test için config JSON'ı oluştur - gerçek projedeki formata uygun
    nlohmann::json configJson = {
        {"defaultLogLevel", "Info"},
        {"categories", {
            {"TestCategory1", "Debug"},
            {"TestCategory2", "Warning"},
            {"TestCategory3", "Error"}
        }},
        {"spamControl", {
            {"TestCategory1", {{"enabled", true}, {"intervalMs", 100}}},
            {"TestCategory2", {{"enabled", false}, {"intervalMs", 0}}}
        }},
        {"async", {
            {"queueSize", 8192},
            {"threadCount", 2},
            {"overflowPolicy", "overrun_oldest"}
        }}
    };
    
    // Config dosyasını oluştur
    CreateTempConfigFile(configJson);
    
    // LogManager ile config'i yükle
    LogManager& logManager = LogManager::GetInstance();
    ASSERT_TRUE(logManager.LoadConfig(tempConfigPath.string())) << "Config dosyası yüklenemedi";
    
    // Varsayılan seviye kontrolü
    EXPECT_EQ(LogLevel::Info, logManager.GetDefaultLogLevel());
    
    // Kategori seviyeleri kontrolü
    EXPECT_EQ(LogLevel::Debug, logManager.GetCategoryLevel("TestCategory1"));
    EXPECT_EQ(LogLevel::Warning, logManager.GetCategoryLevel("TestCategory2"));
    EXPECT_EQ(LogLevel::Error, logManager.GetCategoryLevel("TestCategory3"));
    
    // Spam kontrolünü dolaylı olarak test et
    TestCategorySpamControl("TestCategory1", true, 100ms);
    TestCategorySpamControl("TestCategory2", false, 0ms);
}

// Eksik spamControl alanı testi
TEST_F(ConfigLoadTests, MissingSpamControl_RateLimitDisabled) {
    // spamControl alanı olmayan config
    nlohmann::json configJson = {
        {"defaultLogLevel", "Info"},
        {"categories", {
            {"TestCategory", "Debug"}
        }}
    };
    
    // Config dosyasını oluştur
    CreateTempConfigFile(configJson);
    
    // LogManager ile config'i yükle
    LogManager& logManager = LogManager::GetInstance();
    ASSERT_TRUE(logManager.LoadConfig(tempConfigPath.string())) << "Config dosyası yüklenemedi";
    
    // Spam kontrolünün varsayılan olarak kapalı olduğunu doğrula
    TestCategorySpamControl("TestCategory", false, 0ms);
}

// Geçersiz overflowPolicy testi - sadece geçerliyse test et, LogManager API'sini bilmiyoruz
TEST_F(ConfigLoadTests, InvalidOverflowPolicy_DoesNotThrowException) {
    // Geçersiz overflowPolicy içeren config
    nlohmann::json configJson = {
        {"defaultLogLevel", "Info"},
        {"categories", {
            {"TestCategory", "Debug"}
        }},
        {"spamControl", {
            {"TestCategory", {{"enabled", true}, {"intervalMs", 100}}}
        }},
        {"async", {
            {"queueSize", 8192},
            {"threadCount", 2},
            {"overflowPolicy", "invalid_policy"}
        }}
    };
    
    // Config dosyasını oluştur
    CreateTempConfigFile(configJson);
    
    // LogManager ile config'i yükle - hata fırlatılmamalı
    LogManager& logManager = LogManager::GetInstance();
    EXPECT_NO_THROW({
        bool result = logManager.LoadConfig(tempConfigPath.string());
        EXPECT_TRUE(result) << "Geçersiz policy ile config yüklenemedi"; 
    }) << "Geçersiz policy ile config yüklemesi hata fırlattı";
}

// Full entegrasyon konfigürasyon testi
TEST_F(ConfigLoadTests, FullConfigIntegration_AllSettingsApplied) {
    // Tüm özellikleri içeren tam bir config
    nlohmann::json configJson = {
        {"defaultLogLevel", "Info"},
        {"categories", {
            {"System", "Error"},
            {"Network", "Warning"},
            {"Render", "Debug"},
            {"Physics", "Info"},
            {"Audio", "Critical"}
        }},
        {"spamControl", {
            {"System", {{"enabled", true}, {"intervalMs", 50}}},
            {"Render", {{"enabled", true}, {"intervalMs", 50}}},
            {"Physics", {{"enabled", false}, {"intervalMs", 0}}},
            {"Audio", {{"enabled", true}, {"intervalMs", 100}}}
        }},
        {"async", {
            {"queueSize", 8192},
            {"threadCount", 2},
            {"overflowPolicy", "block"}
        }}
    };
    
    // Config dosyasını oluştur
    CreateTempConfigFile(configJson);
    
    // LogManager ile config'i yükle
    LogManager& logManager = LogManager::GetInstance();
    ASSERT_TRUE(logManager.LoadConfig(tempConfigPath.string())) << "Config dosyası yüklenemedi";
    
    // Varsayılan seviye kontrolü
    EXPECT_EQ(LogLevel::Info, logManager.GetDefaultLogLevel());
    
    // Kategori seviyeleri kontrolü
    EXPECT_EQ(LogLevel::Error, logManager.GetCategoryLevel("System"));
    EXPECT_EQ(LogLevel::Warning, logManager.GetCategoryLevel("Network"));
    EXPECT_EQ(LogLevel::Debug, logManager.GetCategoryLevel("Render"));
    EXPECT_EQ(LogLevel::Info, logManager.GetCategoryLevel("Physics"));
    EXPECT_EQ(LogLevel::Critical, logManager.GetCategoryLevel("Audio"));
    
    // Spam kontrolünü kategoriler için kontrol et
    TestCategorySpamControl("System", true, 50ms);
    TestCategorySpamControl("Render", true, 50ms);
    TestCategorySpamControl("Physics", false, 0ms);
}

} // namespace BlackEngine::Tests
