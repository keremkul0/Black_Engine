#include "TransformComponent.h"
#include <imgui.h> // eğer kullanıyorsan, yoksa çıkart

void TransformComponent::Start()
{
    // Gerekirse
}

void TransformComponent::Update(float deltaTime)
{
    // Örneğin yavaşça dönmesini istersen:
    // rotation.y += 20.f * deltaTime;
}

glm::mat4 TransformComponent::GetModelMatrix() const
{
    auto model = glm::mat4(1.0f);

    // position
    model = glm::translate(model, position);

    // rotation (Euler angles -> mat4)
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1,0,0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0,1,0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0,0,1));

    // scale
    model = glm::scale(model, scale);

    return model;
}

void TransformComponent::OnInspectorGUI()
{
    // ImGui ile düzenlenebilir
    ImGui::Text("Transform");
    ImGui::DragFloat3("Position", &position[0], 0.1f);
    ImGui::DragFloat3("Rotation", &rotation[0], 0.5f);
    ImGui::DragFloat3("Scale",   &scale[0],   0.01f);
}
