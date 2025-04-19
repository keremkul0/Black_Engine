#include <gtest/gtest.h>
#include "Core/Logger/LogManager.h"
#include "Core/Logger/LogTypes.h"
#include "Core/Logger/MockLogger.h"
#include "Core/Logger/ConsoleLoggerBackend.h"
#include <memory>
#include <string>


namespace BlackEngine::Tests {

class BackendTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Test için yeni bir MockLogger backend oluştur
        mockLogger = std::make_shared<MockLogger>();
        mockLogger->Initialize();
        
        // Önceki backend'leri LogManager'dan kaldır
        auto& logManager = LogManager::GetInstance();
        auto backends = logManager.GetBackends();
        for (const auto& backend : backends) {
            logManager.RemoveBackend(backend);
        }
        
        // Kendi test logger'ımızı ekle
        logManager.AddBackend(mockLogger);
    }

    void TearDown() override {
        // Oluşturduğumuz backend'i kaldır
        LogManager::GetInstance().RemoveBackend(mockLogger);
        
        // MockLogger'ı sıfırla
        mockLogger->Shutdown();
        mockLogger = nullptr;
    }

    std::shared_ptr<MockLogger> mockLogger;
};

// MockLogger'ın mesaj formatını doğrula
TEST_F(BackendTests, MockLogger_FormatsMessagesCorrectly) {
    LogManager::GetInstance();
    const std::string testCategory = "TestCategory";
    const std::string testMessage = "Test message content";
    
    // Önce log'ları temizle
    mockLogger->ClearLogs();
    
    // Mesajı logla
    std::source_location location = std::source_location::current();
    LogMessage message;
    message.level = LogLevel::Info;
    message.category = testCategory;
    message.message = testMessage;
    message.location = location;
    message.timestamp = std::chrono::system_clock::now();
    message.repeatCount = 1;
    
    mockLogger->Log(message);
    
    // Log sayısını kontrol et
    ASSERT_EQ(1, mockLogger->GetLogCount());
    
    // Son formatlı mesajı al
    std::string formattedLog = mockLogger->GetLastFormattedMessage();
    
    // Doğru format: "[Category] message (file:line)"
    std::string expectedPrefix = "[" + testCategory + "] " + testMessage;
    EXPECT_TRUE(formattedLog.find(expectedPrefix) == 0) << "Formatlı log beklenen prefixle başlamıyor: " << formattedLog;
    
    // Dosya ve satır numarası içermeli
    std::string fileName = std::string(location.file_name());
    const size_t lastSlash = fileName.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        fileName = fileName.substr(lastSlash + 1);
    }

    const std::string fileInfo = "(" + fileName + ":" + std::to_string(location.line()) + ")";
    EXPECT_TRUE(formattedLog.find(fileInfo) != std::string::npos) << "Formatlı log dosya bilgisi içermiyor: " << formattedLog;
    
    // Repeat count 1 olduğunda (xN) eki olmamalı
    EXPECT_TRUE(formattedLog.find("(x") == std::string::npos) << "Repeat count 1 olduğu halde (xN) eki var: " << formattedLog;
}

// Repeat count > 1 olduğunda (xN) eki kontrolü
TEST_F(BackendTests, MockLogger_AddsRepeatCountSuffix) {
    const std::string testCategory = "TestCategory";
    const std::string testMessage = "Repeated message";
    constexpr int repeatCount = 5;
    
    // Önce log'ları temizle
    mockLogger->ClearLogs();
    
    // Mesajı repeat count ile logla
    const std::source_location location = std::source_location::current();
    LogMessage message;
    message.level = LogLevel::Info;
    message.category = testCategory;
    message.message = testMessage;
    message.location = location;
    message.timestamp = std::chrono::system_clock::now();
    message.repeatCount = repeatCount;
    
    mockLogger->Log(message);
    
    // Log sayısını kontrol et
    ASSERT_EQ(1, mockLogger->GetLogCount());
    
    // Son formatlı mesajı al
    const std::string formattedLog = mockLogger->GetLastFormattedMessage();
    
    // Repeat count eki kontrol et: " (x5)"
    const std::string expectedRepeatSuffix = " (x" + std::to_string(repeatCount) + ")";
    EXPECT_TRUE(formattedLog.find(expectedRepeatSuffix) != std::string::npos) 
        << "Formatlı log repeat count bilgisi içermiyor: " << formattedLog 
        << ", beklenen ek: " << expectedRepeatSuffix;
}

// ConsoleLoggerBackend'in Async ayarı ve overflow policy testi
TEST_F(BackendTests, ConsoleLogger_ConfiguresAsyncWithOverflowPolicy) {
    // Yeni bir ConsoleLoggerBackend oluştur
    const auto consoleLogger = std::make_shared<ConsoleLoggerBackend>();
    
    // Async konfigürasyonunu discard policy ile ayarla
    constexpr size_t queueSize = 1024;
    constexpr size_t threadCount = 1;
    EXPECT_TRUE(consoleLogger->ConfigureAsync(queueSize, threadCount, "discard"));
    
    // İyileştirme: Initialize kontrolü
    EXPECT_TRUE(consoleLogger->Initialize()) << "ConsoleLogger başlatılamadı";
    
    // İyileştirme: Log çağrısı yaparak crash kontrolü
    std::source_location location = std::source_location::current();
    LogMessage testMessage;
    testMessage.level = LogLevel::Info;
    testMessage.category = "TestAsyncConfig";
    testMessage.message = "Test async configuration";
    testMessage.location = location;
    testMessage.timestamp = std::chrono::system_clock::now();
    testMessage.repeatCount = 1;
    
    EXPECT_NO_THROW(consoleLogger->Log(testMessage)) << "Async yapılandırması sonrası Log çağrısı hata fırlattı";
    
    // Temizlik
    consoleLogger->Shutdown();
}

} // namespace BlackEngine::Tests

