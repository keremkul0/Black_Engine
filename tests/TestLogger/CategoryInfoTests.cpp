#include <gtest/gtest.h>
#include "Core/Logger/CategoryInfo.h"
#include <thread>

namespace BlackEngine {
namespace Tests {

class CategoryInfoTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Her test için temiz bir CategoryInfo oluştur
        testCategory = std::make_unique<CategoryInfo>("TestCategory", LogLevel::Debug);
    }

    void TearDown() override {
        testCategory.reset();
    }

    std::unique_ptr<CategoryInfo> testCategory;
};

// Rate-limit kapalı durumda her mesaj loglanmalı ve sayaç artmalı
TEST_F(CategoryInfoTests, NoRateLimit_AllMessagesPass) {
    const std::string messageKey = "test_message_key";
    
    // Rate-limit kapalı (varsayılan)
    EXPECT_FALSE(testCategory->IsRateLimitEnabled());
    
    // İlk mesaj
    EXPECT_TRUE(testCategory->ShouldLog(messageKey));
    EXPECT_EQ(1, testCategory->GetMessageRepeatCount(messageKey));
    
    // İkinci mesaj - hemen sonra
    EXPECT_TRUE(testCategory->ShouldLog(messageKey));
    EXPECT_EQ(2, testCategory->GetMessageRepeatCount(messageKey));
    
    // Üçüncü mesaj - hemen sonra yine
    EXPECT_TRUE(testCategory->ShouldLog(messageKey));
    EXPECT_EQ(3, testCategory->GetMessageRepeatCount(messageKey));
}

// Rate-limit açık durumda, süre dolmadan gelen mesajlar engellenmeli
TEST_F(CategoryInfoTests, WithRateLimit_MessagesBlocked) {
    const std::string messageKey = "test_message_key";
    const auto rateLimit = std::chrono::milliseconds(10);
    
    // Rate-limit'i aktif et (10ms)
    testCategory->SetRateLimit(true, rateLimit);
    EXPECT_TRUE(testCategory->IsRateLimitEnabled());
    EXPECT_EQ(rateLimit, testCategory->GetRateLimit());
    
    // İlk mesaj her zaman geçmeli
    EXPECT_TRUE(testCategory->ShouldLog(messageKey));
    EXPECT_EQ(1, testCategory->GetMessageRepeatCount(messageKey));
    
    // Hemen bir mesaj daha - bu engellenmeli
    EXPECT_FALSE(testCategory->ShouldLog(messageKey));
    EXPECT_EQ(2, testCategory->GetMessageRepeatCount(messageKey));
    
    // Süre dolunca mesaj geçmeli
    std::this_thread::sleep_for(rateLimit + std::chrono::milliseconds(1));
    EXPECT_TRUE(testCategory->ShouldLog(messageKey));
    EXPECT_EQ(3, testCategory->GetMessageRepeatCount(messageKey));
}

// Farklı message key'ler birbirlerinden bağımsız çalışmalı
TEST_F(CategoryInfoTests, DifferentMessageKeys_IndependentRateLimits) {
    const std::string messageKey1 = "test_key_1";
    const std::string messageKey2 = "test_key_2";
    const auto rateLimit = std::chrono::milliseconds(10);
    
    // Rate-limit'i aktif et
    testCategory->SetRateLimit(true, rateLimit);
    
    // İlk anahtar için ilk mesaj
    EXPECT_TRUE(testCategory->ShouldLog(messageKey1));
    EXPECT_EQ(1, testCategory->GetMessageRepeatCount(messageKey1));
    
    // İkinci anahtar için ilk mesaj - bu da geçmeli, farklı anahtar
    EXPECT_TRUE(testCategory->ShouldLog(messageKey2));
    EXPECT_EQ(1, testCategory->GetMessageRepeatCount(messageKey2));
    
    // İlk anahtar için tekrar mesaj - engellenmeli
    EXPECT_FALSE(testCategory->ShouldLog(messageKey1));
    EXPECT_EQ(2, testCategory->GetMessageRepeatCount(messageKey1));
    
    // İkinci anahtar için tekrar mesaj - engellenmeli
    EXPECT_FALSE(testCategory->ShouldLog(messageKey2));
    EXPECT_EQ(2, testCategory->GetMessageRepeatCount(messageKey2));
}

// ResetSpamControl tüm sayaçları temizlemeli
TEST_F(CategoryInfoTests, ResetSpamControl_ClearsAllCounters) {
    const std::string messageKey1 = "test_key_1";
    const std::string messageKey2 = "test_key_2";
    const auto rateLimit = std::chrono::milliseconds(10);
    
    // Rate-limit'i aktif et
    testCategory->SetRateLimit(true, rateLimit);
    
    // Her iki anahtar için de mesajlar gönder
    testCategory->ShouldLog(messageKey1);
    testCategory->ShouldLog(messageKey1);
    testCategory->ShouldLog(messageKey2);
    
    // Sayaçları kontrol et
    EXPECT_EQ(2, testCategory->GetMessageRepeatCount(messageKey1));
    EXPECT_EQ(1, testCategory->GetMessageRepeatCount(messageKey2));
    
    // Spam kontrolünü sıfırla
    testCategory->ResetSpamControl();
    
    // Sayaçlar sıfırlanmış olmalı
    EXPECT_EQ(0, testCategory->GetMessageRepeatCount(messageKey1));
    EXPECT_EQ(0, testCategory->GetMessageRepeatCount(messageKey2));
    
    // Yeni mesajlar yine ilk mesaj gibi davranmalı
    EXPECT_TRUE(testCategory->ShouldLog(messageKey1));
    EXPECT_EQ(1, testCategory->GetMessageRepeatCount(messageKey1));
}

// Dinamik aralık değişikliği anında etkili olmalı
TEST_F(CategoryInfoTests, DynamicRateLimitChange_TakesEffectImmediately) {
    const std::string messageKey = "test_message_key";
    const auto initialRateLimit = std::chrono::milliseconds(50);
    const auto newRateLimit = std::chrono::milliseconds(5);
    
    // Başlangıç rate-limit ayarı (50ms)
    testCategory->SetRateLimit(true, initialRateLimit);
    
    // İlk mesaj
    EXPECT_TRUE(testCategory->ShouldLog(messageKey));
    
    // Rate-limit'i küçült (5ms)
    testCategory->SetRateLimit(true, newRateLimit);
    EXPECT_EQ(newRateLimit, testCategory->GetRateLimit());
    
    // Kısa bekleme (yeni limitten biraz fazla)
    std::this_thread::sleep_for(newRateLimit + std::chrono::milliseconds(1));
    
    // Yeni limite göre mesaj geçmeli
    EXPECT_TRUE(testCategory->ShouldLog(messageKey));
    
    // Rate-limit'i kapat
    testCategory->SetRateLimit(false, newRateLimit);
    EXPECT_FALSE(testCategory->IsRateLimitEnabled());
    
    // Hemen mesaj gönder - rate-limit kapalı olduğundan geçmeli
    EXPECT_TRUE(testCategory->ShouldLog(messageKey));
}

} // namespace Tests
} // namespace BlackEngine
