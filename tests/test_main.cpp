#include <gtest/gtest.h>
#include "Core/Logger/LoggerManager.h"
#include "Core/Logger/MockLogger.h"

// Global test environment setup
class GlobalTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        // Directly set a MockLogger instance instead of using Initialize(true)
        LoggerManager::SetLogger(std::make_shared<MockLogger>());
    }

    void TearDown() override {
        // Clean up the logger at the end of all tests
        LoggerManager::Shutdown();
    }
};

// Main function for running tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    // Add global test environment
    ::testing::AddGlobalTestEnvironment(new GlobalTestEnvironment());
    return RUN_ALL_TESTS();
}
