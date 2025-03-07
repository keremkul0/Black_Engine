#include <gtest/gtest.h>
#include "Engine/Entity/GameObject.h"
#include "Engine/Component/BaseComponent.h"

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
