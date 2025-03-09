#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "../Entity/GameObject.h"
#include <imgui.h>
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
    if (!owner || !m_shader) return;

    // Bileşenler önbellekte yoksa, tekrar dene
    if (!m_cachedMeshComponent || !m_cachedTransform) {
        CacheComponents();
    }

    // Önbelleğe alınmış bileşenleri kullan
    if (!m_cachedMeshComponent || !m_cachedMeshComponent->IsLoaded()) return;

    const auto mesh = m_cachedMeshComponent->GetMesh();
    if (!mesh) return;

    if (!m_cachedTransform) return;

    // Shader'ı aktif et
    m_shader->use();

    // Model matrisi
    const glm::mat4 model = m_cachedTransform->GetModelMatrix();
    m_shader->setMat4("model", model);

    // Global kamera matrislerini uniform olarak ayarla
    m_shader->setMat4("view", gViewMatrix);
    m_shader->setMat4("projection", gProjectionMatrix);

    // Mesh'i çiz
    mesh->Draw();
}

void MeshRendererComponent::OnInspectorGUI() {
    ImGui::Text("Mesh Renderer Component");

    // Shader durumunu göster
    if (m_shader) {
        ImGui::Text("Shader: Loaded");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No Shader Assigned");
    }

    if (ImGui::Button("Select Shader...")) {
        // Burada shader seçme dialogu açılabilir
    }

    // Bağımlılık durumunu göster
    if (owner) {
        const auto meshComp = owner->GetComponent<MeshComponent>();
        const auto transformComp = owner->GetComponent<TransformComponent>();

        ImGui::Separator();
        ImGui::Text("Dependencies:");
        ImGui::Text("Mesh Component: %s", meshComp ? "Found" : "Missing");
        ImGui::Text("Transform Component: %s", transformComp ? "Found" : "Missing");
    }
}
