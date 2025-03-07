#include "TransformComponent.h"
#include <imgui.h>  // eğer inspector'da ImGui kullanacaksan

glm::mat4 TransformComponent::GetModelMatrix() const
{
    // Basit bir model matrisi oluşturuyoruz
    glm::mat4 model = glm::mat4(1.0f);
    // position
    model = glm::translate(model, position);

    // rotation (Euler -> mat4)
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1,0,0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0,1,0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0,0,1));

    // scale
    model = glm::scale(model, scale);

    return model;
}

void TransformComponent::Start()
{
    // Başlangıçta bir şey yapacaksak buraya koyabiliriz.
    // Örneğin console log, ya da position varsayılanları atama vb.
}

void TransformComponent::Update(float deltaTime)
{
    // Her frame çalışacak transform ile ilgili kod varsa
    // (Ör: basit bir animasyon, sin/cos dalgası vs.)
    // Şimdilik boş bırakabiliriz.
}

void TransformComponent::OnInspectorGUI()
{
    // ImGui kullanarak position/rotation/scale düzenleme
    // Her frame inspector'da bu görünecek
    ImGui::Text("Transform");
    ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
    ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.5f);
    ImGui::DragFloat3("Scale",   glm::value_ptr(scale),   0.01f);
}
