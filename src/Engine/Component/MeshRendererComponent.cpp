#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "../Entity/GameObject.h"
#include "glm/glm.hpp"

// Uygulama tarafında tanımlanacak.
extern glm::mat4 gViewMatrix;
extern glm::mat4 gProjectionMatrix;

void MeshRendererComponent::Draw()
{
    if (!mesh || !owner || !shader) return;

    // Transform'u al
    auto transform = owner->GetComponent<TransformComponent>();
    if (!transform) return;

    // Shader'i aktif et
    shader->use();

    // Model matrisi
    glm::mat4 model = transform->GetModelMatrix();
    shader->setMat4("model", model);

    // Global kamera matrislerini uniform olarak ayarla
    shader->setMat4("view", gViewMatrix);
    shader->setMat4("projection", gProjectionMatrix);

    // Mesh'i çiz
    mesh->Draw();
}
