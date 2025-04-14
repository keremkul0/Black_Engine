#include "gtest/gtest.h"
#include "Core/Logger/LogManager.h"
#include "Core/Logger/MockLogBackend.h"

using namespace BlackEngine;

// Define a test category
BE_DEFINE_LOG_CATEGORY(TestCategory, "Testing", LogLevel::Trace);

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize the log manager with a mock backend
        LogManager::GetInstance().Initialize();
        
        // Add a mock backend for testing
        m_MockBackend = std::make_shared<MockLogBackend>();
        m_MockBackend->Initialize();
        LogManager::GetInstance().AddBackend(m_MockBackend);
    }
    
    void TearDown() override {
        // Clean up
        LogManager::GetInstance().Shutdown();
    }
    
    std::shared_ptr<MockLogBackend> m_MockBackend;
};

// Test basic logging functionality
TEST_F(LoggerTest, BasicLogging) {
    // Log some messages
    BE_LOG_INFO(TestCategory, "This is an info message");
    BE_LOG_ERROR(TestCategory, "This is an error message");
    
    // Verify the messages were logged
    auto messages = m_MockBackend->GetLogMessages();
    ASSERT_EQ(messages.size(), 2);
    
    // Check the first message
    EXPECT_EQ(messages[0].level, LogLevel::Info);
    EXPECT_EQ(messages[0].categoryName, "Testing");
    EXPECT_STREQ(messages[0].message.c_str(), "This is an info message");
    
    // Check the second message
    EXPECT_EQ(messages[1].level, LogLevel::Error);
    EXPECT_EQ(messages[1].categoryName, "Testing");
    EXPECT_STREQ(messages[1].message.c_str(), "This is an error message");
}

// Test log filtering by level
TEST_F(LoggerTest, LogFiltering) {
    // Set category level to Warning
    LogManager::GetInstance().SetCategoryLevel("Testing", LogLevel::Warning);
    
    // These should be filtered out
    BE_LOG_INFO(TestCategory, "This info should be filtered");
    BE_LOG_DEBUG(TestCategory, "This debug should be filtered");
    
    // These should be logged
    BE_LOG_WARNING(TestCategory, "This warning should be logged");
    BE_LOG_ERROR(TestCategory, "This error should be logged");
    
    // Verify filtering
    auto messages = m_MockBackend->GetLogMessages();
    ASSERT_EQ(messages.size(), 2);
    EXPECT_EQ(messages[0].level, LogLevel::Warning);
    EXPECT_EQ(messages[1].level, LogLevel::Error);
}

// Test repeated logs
TEST_F(LoggerTest, RepeatedLogs) {
    // Log the same message multiple times
    for (int i = 0; i < 10; i++) {
        BE_LOG_INFO(TestCategory, "This is a repeated message");
    }
    
    // Force flush to ensure all messages are processed
    LogManager::GetInstance().Flush();
    
    // Get the messages - we should see a single message with repeat count
    auto messages = m_MockBackend->GetLogMessages();
    
    // Look for the repeat message
    bool foundRepeat = false;
    for (const auto& msg : messages) {
        if (msg.isRepeat && msg.repeatCount > 1) {
            foundRepeat = true;
            break;
        }
    }
    
    EXPECT_TRUE(foundRepeat);
}

// Test format string support
TEST_F(LoggerTest, FormatString) {
    // Log with format parameters
    BE_LOG_INFO(TestCategory, "Testing format: {} and {}", 42, "string");
    
    // Verify the formatted message
    auto messages = m_MockBackend->GetLogMessages();
    ASSERT_GE(messages.size(), 1);
    EXPECT_EQ(messages[0].message, "Testing format: 42 and string");
}
