#include <gtest/gtest.h>
#include "Engine/Entity/GameObject.h"
#include "Engine/Component/TransformComponent.h"
#include <glm/glm.hpp>

TEST(TransformTest, DefaultValues)
{
    GameObject obj;
    auto transform = obj.AddComponent<TransformComponent>();

    EXPECT_FLOAT_EQ(transform->position.x, 0.f);
    EXPECT_FLOAT_EQ(transform->scale.x, 1.f);
}

TEST(TransformTest, UpdateMovesPosition)
{
    GameObject obj;
    auto transform = obj.AddComponent<TransformComponent>();

    transform->position = glm::vec3(1,2,3);
    obj.Update(0.016f);
    EXPECT_FLOAT_EQ(transform->position.x, 1.f);
}
