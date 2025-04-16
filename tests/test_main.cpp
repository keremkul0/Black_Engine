// file: test_main.cpp
#include <gtest/gtest.h>
#include "Core/Logger/LogManager.h"
#include "Core/Logger/MockLogBackend.h"

class GlobalTestEnvironment final : public testing::Environment {
public:
    void SetUp() override {
        // LogManager singleton üzerinden backend ekleyip log sistemini başlatıyoruz
        BlackEngine::LogManager::GetInstance().AddBackend(std::make_shared<BlackEngine::MockLogBackend>());
        BlackEngine::LogManager::GetInstance().Initialize();
    }

    void TearDown() override {
        BlackEngine::LogManager::GetInstance().Shutdown();
    }
};

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    AddGlobalTestEnvironment(new GlobalTestEnvironment());
    return RUN_ALL_TESTS();
}