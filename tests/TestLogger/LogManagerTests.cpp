#include <gtest/gtest.h>
#include "Core/Logger/LogManager.h"
#include "Core/Logger/LogTypes.h"
#include "Core/Logger/MockLogger.h"
#include <thread>
#include <future>
#include <vector>

namespace BlackEngine {
namespace Tests {

using namespace std::chrono_literals;

class LogManagerTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Her test için LogManager'ı sıfırla
        LogManager::GetInstance().ResetSpamControl();
        
        // Test için bir mock logger oluştur
        mockLogger = std::make_shared<MockLogger>();
        mockLogger->Initialize();
        
        // Daha önce eklenen backend'leri temizle
        auto& logManager = LogManager::GetInstance();
        auto backends = logManager.GetBackends();
        for (const auto& backend : backends) {
            logManager.RemoveBackend(backend);
        }
        
        // Mock logger'ı ekle
        logManager.AddBackend(mockLogger);
    }

    void TearDown() override {
        // Kategorileri test seviyesine geri çevir
        LogManager::GetInstance().SetDefaultLogLevel(LogLevel::Debug);
        LogManager::GetInstance().ResetSpamControl();
        
        // Mock logger'ı kaldır
        LogManager::GetInstance().RemoveBackend(mockLogger);
        mockLogger->Shutdown();
        mockLogger = nullptr;
    }
    
    std::shared_ptr<MockLogger> mockLogger;
};

// Seviye filtreleme testi (ShouldLog dolaylı olarak test edilir)
TEST_F(LogManagerTests, LevelFiltering_LogicalLevels) {
    LogManager& logManager = LogManager::GetInstance();
    const std::string testCategory = "TestCategory";
    
    // Önce varsayılan seviyeyi Debug olarak ayarla
    logManager.SetDefaultLogLevel(LogLevel::Debug);
    
    // Debug seviyeli bir kategori oluştur
    logManager.SetCategoryLevel(testCategory, LogLevel::Debug);
    
    // Seviye filtrelemeyi test etmek için doğrudan ShouldLog'u çağıramayız
    // Bunun yerine, Log fonksiyonu aracılığıyla test ediyoruz
    
    // Critical seviyeli bir log - geçmeli
    mockLogger->ClearLogs();
    logManager.Log(LogLevel::Critical, testCategory, std::source_location::current(), "Critical message");
    EXPECT_EQ(1, mockLogger->GetLogCount()) << "Critical seviyeli log engellendi";
    
    // Error seviyeli bir log - geçmeli
    mockLogger->ClearLogs();
    logManager.Log(LogLevel::Error, testCategory, std::source_location::current(), "Error message");
    EXPECT_EQ(1, mockLogger->GetLogCount()) << "Error seviyeli log engellendi";
    
    // Debug seviyeli bir log - geçmeli
    mockLogger->ClearLogs();
    logManager.Log(LogLevel::Debug, testCategory, std::source_location::current(), "Debug message");
    EXPECT_EQ(1, mockLogger->GetLogCount()) << "Debug seviyeli log engellendi";
    
    // Trace seviyeli bir log - engellenmeli
    mockLogger->ClearLogs();
    logManager.Log(LogLevel::Trace, testCategory, std::source_location::current(), "Trace message");
    EXPECT_EQ(0, mockLogger->GetLogCount()) << "Trace seviyeli log engellenmedi";
    
    // Kategoriyi Info seviyesine değiştir
    logManager.SetCategoryLevel(testCategory, LogLevel::Info);
    
    // Warning seviyeli bir log - geçmeli
    mockLogger->ClearLogs();
    logManager.Log(LogLevel::Warning, testCategory, std::source_location::current(), "Warning message");
    EXPECT_EQ(1, mockLogger->GetLogCount()) << "Warning seviyeli log engellendi";
    
    // Debug seviyeli bir log - artık engellenmeli
    mockLogger->ClearLogs();
    logManager.Log(LogLevel::Debug, testCategory, std::source_location::current(), "Debug message");
    EXPECT_EQ(0, mockLogger->GetLogCount()) << "Debug seviyeli log engellenmedi";
    
    // Kategoriyi Off seviyesine ayarla
    logManager.SetCategoryLevel(testCategory, LogLevel::Off);
    
    // Hiçbir mesaj geçmemeli
    mockLogger->ClearLogs();
    logManager.Log(LogLevel::Critical, testCategory, std::source_location::current(), "Critical message");
    EXPECT_EQ(0, mockLogger->GetLogCount()) << "OFF kategorisinde Critical mesaj engellenmedi";
}

// Spam kontrolü testi - dolaylı olarak test edilir
TEST_F(LogManagerTests, SpamControl_RepeatCountAndRateLimit) {
    LogManager& logManager = LogManager::GetInstance();
    const std::string testCategory = "TestCategorySpam";
    
    // Rate-limit ayarla
    const auto rateLimit = std::chrono::milliseconds(20);
    
    // Rate limit'i aktif et
    logManager.SetCategorySpamControl(testCategory, true, rateLimit);
    
    // Test locationı
    std::source_location location = std::source_location::current();
    
    // İlk log - MockLogger'ı temizle ve ilk mesajı logla
    mockLogger->ClearLogs();
    logManager.Log(LogLevel::Info, testCategory, location, "Spam test message");
    
    // MockLogger'da mesaj olduğunu doğrula
    EXPECT_EQ(1, mockLogger->GetLogCount()) << "İlk mesaj backend'e iletilmedi";
    
    // İlk mesajın LogMessage objesini al ve repeat count kontrolü yap
    if (mockLogger->GetLogCount() > 0) {
        // İlk mesajda repeat count 0 olmalı - sayaç ShouldLog sonrası artıyor
        LogMessage firstMessage = mockLogger->GetLastMessage();
        EXPECT_EQ(0, firstMessage.repeatCount) << "İlk mesajın tekrar sayısı 0 olmalı";
    }
    
    // Hemen aynı mesajı tekrar dene - spam kontrolü nedeniyle engellenmeli
    mockLogger->ClearLogs();
    logManager.Log(LogLevel::Info, testCategory, location, "Spam test message");
    EXPECT_EQ(0, mockLogger->GetLogCount()) << "Rate-limit nedeniyle mesaj engellenmedi";
    
    // Süre dolduktan sonra tekrar dene
    std::this_thread::sleep_for(rateLimit + 1ms); // Sadece eşik + 1 ms
    mockLogger->ClearLogs();
    logManager.Log(LogLevel::Info, testCategory, location, "Spam test message");
    EXPECT_EQ(1, mockLogger->GetLogCount()) << "Süre dolduktan sonra mesaj backend'e iletilmedi";
    
    // Sonraki mesajda repeat count en az 1 olmalı (artmış)
    if (mockLogger->GetLogCount() > 0) {
        LogMessage nextMessage = mockLogger->GetLastMessage();
        EXPECT_GE(nextMessage.repeatCount, 1) << "Tekrar sayısı artmamış";
    }
}

// Çok iş parçacıklı senaryo testi
TEST_F(LogManagerTests, MultiThreaded_ThreadSafety) {
    LogManager& logManager = LogManager::GetInstance();
    const std::string testCategory = "TestMultiThreadCategory";
    constexpr int threadCount = 20;
    constexpr int iterationCount = 5; // Her thread 5 mesaj göndersin
    
    // Rate limit'i kapat - bu testte sadece mesajların güvenli şekilde iletilmesine bakacağız
    logManager.SetCategorySpamControl(testCategory, false, 0ms);
    
    // İlk mesajları temizle
    mockLogger->ClearLogs();
    
    std::vector<std::future<void>> futures;
    
    for (int i = 0; i < threadCount; ++i) {
        futures.push_back(std::async(std::launch::async, [&logManager, testCategory, i]() {
            for (int j = 0; j < 5; ++j) {
                // Her thread kendine özel bir mesaj gönderiyor
                // Not: Compile-time string olarak fmt::format kullanıyoruz
                logManager.Log(LogLevel::Info, testCategory, std::source_location::current(), 
                             "Thread {} - Iteration {}", i, j);
                
                // Kısa bir süre bekleyelim - thread'lerin gerçekten paralel çalışması için
                std::this_thread::sleep_for(std::chrono::nanoseconds(100));
            }
        }));
    }
    
    // Tüm thread'lerin tamamlanmasını bekle
    for (auto& future : futures) {
        future.wait();
    }
    
    // Toplam log sayısını kontrol et - threadCount * iterationCount kadar olmalı
    const int expectedLogCount = threadCount * iterationCount;
    EXPECT_EQ(expectedLogCount, mockLogger->GetLogCount()) << 
        "Beklenen " << expectedLogCount << " log yerine " << 
        mockLogger->GetLogCount() << " log var";
}

} // namespace Tests
} // namespace BlackEngine

