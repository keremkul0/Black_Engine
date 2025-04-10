//
// Created by Kerem on 3.03.2025.
//
// tests/TestComponents.cpp
#include <gtest/gtest.h>
#include "../src/Engine/Entity/GameObject.h"
#include "../src/Engine/Component/BaseComponent.h"
#include <memory>

class MockComponent : public BaseComponent {
public:
    bool startCalled = false;
    float lastDeltaTime = 0.f;

    void Start() override {
        startCalled = true;
    }
    void Update(float deltaTime) override {
        lastDeltaTime = deltaTime;
    }
};

class ComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a new GameObject for each test
        gameObject = std::make_shared<GameObject>();
    }
    
    std::shared_ptr<GameObject> gameObject;
};

TEST_F(ComponentTest, AddComponent) {
    ASSERT_TRUE(gameObject->components.empty());

    auto comp = gameObject->AddComponent<MockComponent>();
    ASSERT_NE(comp, nullptr);
    ASSERT_FALSE(gameObject->components.empty());
    EXPECT_TRUE(comp->startCalled);
}

TEST_F(ComponentTest, UpdateCallsComponents) {
    auto comp = gameObject->AddComponent<MockComponent>();
    ASSERT_NE(comp, nullptr);

    gameObject->Update(0.016f);
    EXPECT_FLOAT_EQ(comp->lastDeltaTime, 0.016f);
}

