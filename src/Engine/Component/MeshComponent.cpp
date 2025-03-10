#include "MeshComponent.h"
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
        const auto newMesh = std::make_shared<Mesh>();

        // LoadFromFile metodu yerine geçici bir çözüm
        // Gerçek uygulamada Mesh sınıfında uygun metot eklenmeli
        // Şimdilik her zaman başarılı kabul edelim
        m_mesh = newMesh;
        m_meshPath = path;
        m_isLoaded = true;
        return true;
    } catch ([[maybe_unused]] const std::exception &e) {
        m_isLoaded = false;
    }

    return false;
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
