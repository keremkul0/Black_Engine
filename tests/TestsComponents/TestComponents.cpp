#include <gtest/gtest.h>
#include "Engine/Entity/GameObject.h"
#include "Engine/Component/BaseComponent.h"
#include <memory>

class MockComponent final : public BaseComponent {
public:
    bool startCalled = false;
    float lastDeltaTime = 0.f;

    void Start() override {
        startCalled = true;
    }
    void Update(const float deltaTime) override {
        lastDeltaTime = deltaTime;
    }
};

class ComponentTest : public testing::Test {
protected:
    void SetUp() override {
        // Create a new GameObject for each test
        gameObject = std::make_shared<GameObject>();
    }
    
    std::shared_ptr<GameObject> gameObject;
};

TEST_F(ComponentTest, AddComponent) {
    ASSERT_TRUE(gameObject->components.empty());

    const auto comp = gameObject->AddComponent<MockComponent>();
    ASSERT_NE(comp, nullptr);
    ASSERT_FALSE(gameObject->components.empty());
    EXPECT_TRUE(comp->startCalled);
}

TEST_F(ComponentTest, UpdateCallsComponents) {
    const auto comp = gameObject->AddComponent<MockComponent>();
    ASSERT_NE(comp, nullptr);

    gameObject->Update(0.016f);
    EXPECT_FLOAT_EQ(comp->lastDeltaTime, 0.016f);
}

