#include "TransformComponent.h"

void TransformComponent::Start()
{
    // Başlangıç işlemleri
}

void TransformComponent::Update(float deltaTime)
{
    // Güncelleme işlemleri
}

void TransformComponent::OnEnable()
{
    // Bileşen aktif edildiğinde
}

void TransformComponent::OnDisable()
{
    // Bileşen deaktif edildiğinde
}

glm::mat4 TransformComponent::GetModelMatrix() const
{
    if (matrixDirty) {
        RecalculateModelMatrix();
        matrixDirty = false;
    }
    return cachedModelMatrix;
}

void TransformComponent::RecalculateModelMatrix() const
{
    cachedModelMatrix = glm::mat4(1.0f);

    // Position
    cachedModelMatrix = translate(cachedModelMatrix, position);

    // Rotation (Euler angles -> mat4)
    cachedModelMatrix = rotate(cachedModelMatrix, glm::radians(rotation.x), glm::vec3(1,0,0));
    cachedModelMatrix = rotate(cachedModelMatrix, glm::radians(rotation.y), glm::vec3(0,1,0));
    cachedModelMatrix = rotate(cachedModelMatrix, glm::radians(rotation.z), glm::vec3(0,0,1));

    // Scale
    cachedModelMatrix = glm::scale(cachedModelMatrix, scale);
}
