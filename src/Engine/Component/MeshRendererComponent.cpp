#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "../Entity/GameObject.h"
#include <glm/glm.hpp>

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

    // Bileşenler önbellekte yoksa, tekrar dene
    if (!m_cachedMeshComponent || !m_cachedTransform) {
        CacheComponents();
    }

    // Önbelleğe alınmış bileşenleri kullan
    if (!m_cachedMeshComponent || !m_cachedMeshComponent->IsLoaded()) return;

    const auto mesh = m_cachedMeshComponent->GetMesh();
    if (!mesh || !m_cachedTransform) return;

    // Material'ı aktif et (shader'ı aktif eder ve varsa texture'u bağlar)
    m_material->Apply();

    // Material'ın shader'ı üzerinden uniform'ları güncelle
    if (auto shader = m_material->GetShader()) {
        const glm::mat4 model = m_cachedTransform->GetModelMatrix();
        shader->setMat4("model", model);
        shader->setMat4("view", gViewMatrix);
        shader->setMat4("projection", gProjectionMatrix);
    }

    // Mesh'i çiz
    mesh->Draw();
}
