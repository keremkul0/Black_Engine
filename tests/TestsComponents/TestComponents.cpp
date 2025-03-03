//
// Created by Kerem on 3.03.2025.
//
// tests/TestComponents.cpp
#include <gtest/gtest.h>
#include "../src/Engine/Entity/GameObject.h"
#include "../src/Engine/Component/BaseComponent.h"

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

TEST(ComponentTest, AddComponent) {
    GameObject obj;
    ASSERT_TRUE(obj.components.empty());

    auto comp = obj.AddComponent<MockComponent>();
    ASSERT_FALSE(obj.components.empty());
    EXPECT_TRUE(comp->startCalled);
}

TEST(ComponentTest, UpdateCallsComponents) {
    GameObject obj;
    auto comp = obj.AddComponent<MockComponent>();

    obj.Update(0.016f);
    EXPECT_FLOAT_EQ(comp->lastDeltaTime, 0.016f);
}

