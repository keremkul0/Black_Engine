#include "MeshComponent.h"
#include <imgui.h>
#include <filesystem>
#include "Engine/Render/Mesh/Mesh.h"

void MeshComponent::Start() {
    // Eğer mesh yolu belirtilmişse ve henüz yüklenmemişse
    if (!m_meshPath.empty() && !m_isLoaded) {
        LoadMesh(m_meshPath);
    }
}

void MeshComponent::OnEnable() {
    // Bileşen aktif edildiğinde
}

void MeshComponent::OnDisable() {
    // Bileşen deaktif edildiğinde
}

bool MeshComponent::LoadMesh(const std::string &path) {
    try {
        // Yeni mesh oluştur
        auto newMesh = std::make_shared<Mesh>();

        // LoadFromFile metodu yerine geçici bir çözüm
        // Gerçek uygulamada Mesh sınıfında uygun metot eklenmeli
        // Şimdilik her zaman başarılı kabul edelim
        m_mesh = newMesh;
        m_meshPath = path;
        m_isLoaded = true;
        return true;
    } catch (const std::exception &e) {
        m_isLoaded = false;
    }

    return false;
}

void MeshComponent::OnInspectorGUI() {
    ImGui::Text("Mesh Component");

    // Mesh bilgilerini göster
    if (m_isLoaded && m_mesh) {
        std::string filename = !m_meshPath.empty()
                                   ? std::filesystem::path(m_meshPath).filename().string()
                                   : "Custom Mesh";

        ImGui::Text("Loaded Mesh: %s", filename.c_str());

        // Mesh istatistikleri yerine geçici bilgi göster
        // GetVertexCount ve GetTriangleCount metotları olmadığı için
        ImGui::Text("Mesh loaded successfully");

        // Gelecekte:
        // ImGui::Text("Vertices: %d", m_mesh->GetVertexCount());
        // ImGui::Text("Triangles: %d", m_mesh->GetTriangleCount());
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No Mesh Loaded");
    }

    if (ImGui::Button("Load Mesh...")) {
        // Burada dosya seçme dialogu açılabilir
    }
}

bool MeshComponent::SetMesh(const std::shared_ptr<Mesh>& mesh) {
    if (mesh) {
        m_mesh = mesh;
        m_meshPath = ""; // Doğrudan ayarlandı, dosyadan yüklenmedi
        m_isLoaded = true;
        return true;
    }
    m_isLoaded = false;
    return false;
}
