#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "BaseComponent.h"
#include <memory>
#include <string>

#include "Engine/Render/Mesh/Mesh.h"

class MeshComponent final : public BaseComponent {
private:
    std::shared_ptr<Mesh> m_mesh;
    std::string m_meshPath;
    bool m_isLoaded = false;

public:
    MeshComponent() = default;

    ~MeshComponent() override = default;

    // Mesh işlemleri
    bool LoadMesh(const std::string &path);

    bool SetMesh(const std::shared_ptr<Mesh> &mesh);

    [[nodiscard]] std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }
    [[nodiscard]] const std::string &GetMeshPath() const { return m_meshPath; }
    [[nodiscard]] bool IsLoaded() const { return m_isLoaded; }

    // Temel komponenet işlevleri
    void Start() override;

    void OnEnable() override;

    void OnDisable() override;

    // Bileşen tipi bilgisi
    [[nodiscard]] std::string GetTypeName() const override { return "MeshComponent"; }
};

#endif // MESH_COMPONENT_H
