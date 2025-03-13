#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "../Entity/GameObject.h"
#include <glm/glm.hpp>

#include "Engine/Scene/Scene.h"

// Uygulama tarafında tanımlanacak.
extern glm::mat4 gViewMatrix;
extern glm::mat4 gProjectionMatrix;

void MeshRendererComponent::Start() {
    // Başlangıç işlemleri
}

void MeshRendererComponent::OnEnable() {
    // Bileşen aktif edildiğinde
}

void MeshRendererComponent::OnDisable() {
    // Bileşen deaktif edildiğinde
}

void MeshRendererComponent::CacheComponents() {
    if (!owner) return;

    m_cachedMeshComponent = owner->GetComponent<MeshComponent>().get();
    m_cachedTransform = owner->GetComponent<TransformComponent>().get();
}

void MeshRendererComponent::Draw() {
    if (!owner || !m_material) return;

    // Cache components if needed
    if (!m_cachedMeshComponent || !m_cachedTransform) {
        CacheComponents();
    }

    // Use cached components
    if (!m_cachedMeshComponent || !m_cachedMeshComponent->IsLoaded()) return;

    const auto mesh = m_cachedMeshComponent->GetMesh();
    if (!mesh) return;

    if (!m_cachedTransform) return;

    // Apply material (activates shader and sets properties)
    m_material->Apply();

    // Get shader from material
    const auto shader = m_material->GetShader();

    // Set matrices
    const glm::mat4 model = m_cachedTransform->GetModelMatrix();
    shader->setMat4("model", model);
    shader->setMat4("view", gViewMatrix);
    shader->setMat4("projection", gProjectionMatrix);

    // Enable IBL if available in the scene
    if (owner->HasScene()) {
        auto scene = owner->GetScene();
        if (scene && scene->HasIBLManager()) {
            if (shader->HasUniform("useIBL")) {
                auto iblManager = scene->GetIBLManager();

                // Enable IBL in shader
                shader->setInt("useIBL", 1);

                // Bind IBL textures to specific texture units
                iblManager->BindMaps(3, 4, 5);

                // Tell shader which units to use
                if (shader->HasUniform("irradianceMap")) shader->setInt("irradianceMap", 3);
                if (shader->HasUniform("prefilteredMap")) shader->setInt("prefilteredMap", 4);
                if (shader->HasUniform("brdfLUT")) shader->setInt("brdfLUT", 5);
            }
        } else if (shader->HasUniform("useIBL")) {
            // Disable IBL in shader if scene has no IBL manager
            shader->setInt("useIBL", 0);
        }
    } else if (shader->HasUniform("useIBL")) {
        // Disable IBL in shader if GameObject has no scene
        shader->setInt("useIBL", 0);
    }

    // Draw mesh
    mesh->Draw();
}