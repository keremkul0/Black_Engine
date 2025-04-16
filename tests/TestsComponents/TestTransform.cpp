#include <gtest/gtest.h>
#include "Engine/Entity/GameObject.h"
#include "Engine/Component/TransformComponent.h"
#include <glm/glm.hpp>
#include <memory>

class TransformTest : public testing::Test {
protected:
    void SetUp() override {
        // Create a new GameObject for each test
        gameObject = std::make_shared<GameObject>();
    }
    
    std::shared_ptr<GameObject> gameObject;
};

TEST_F(TransformTest, DefaultValues)
{
    // Use the shared_ptr-managed GameObject
    const auto transform = gameObject->AddComponent<TransformComponent>();
    
    // Make sure we got a valid component
    ASSERT_NE(transform, nullptr);

    EXPECT_FLOAT_EQ(transform->position.x, 0.f);
    EXPECT_FLOAT_EQ(transform->scale.x, 1.f);
}

TEST_F(TransformTest, UpdateMovesPosition)
{
    // Use the shared_ptr-managed GameObject
    const auto transform = gameObject->AddComponent<TransformComponent>();
    
    // Make sure we got a valid component
    ASSERT_NE(transform, nullptr);

    transform->position = glm::vec3(1,2,3);
    gameObject->Update(0.016f);
    EXPECT_FLOAT_EQ(transform->position.x, 1.f);
}
