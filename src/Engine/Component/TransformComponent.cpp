#include "TransformComponent.h"
#include <imgui.h>

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

void TransformComponent::OnInspectorGUI()
{
    ImGui::Text("Transform");

    // Position değişikliği
    if (ImGui::DragFloat3("Position", &position[0], 0.1f)) {
        matrixDirty = true;
    }

    // Rotation değişikliği
    if (ImGui::DragFloat3("Rotation", &rotation[0], 0.5f)) {
        matrixDirty = true;
    }

    // Scale değişikliği
    if (ImGui::DragFloat3("Scale", &scale[0], 0.01f)) {
        matrixDirty = true;
    }
}