#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "BaseComponent.h"
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Core/Math/BoundingVolume.h"

#include "Engine/Render/Mesh/Mesh.h"

class MeshComponent final : public BaseComponent {
private:
    std::shared_ptr<Mesh> m_mesh;
    std::string m_meshPath;
    bool m_isLoaded = false;
    Math::BoundingSphere m_boundingSphere;
    bool m_boundingSphereDirty = true;

public:
    MeshComponent() = default;

    ~MeshComponent() override = default;

    // Mesh işlemleri
    bool LoadMesh(const std::string &path);

    bool SetMesh(const std::shared_ptr<Mesh> &mesh);

    [[nodiscard]] std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }
    [[nodiscard]] const std::string &GetMeshPath() const { return m_meshPath; }
    [[nodiscard]] bool IsLoaded() const { return m_isLoaded; }

    // Bounding sphere methods
    void CalculateBoundingSphere();
    [[nodiscard]] const Math::BoundingSphere& GetBoundingSphere() const { return m_boundingSphere; }
    void SetBoundingSphereDirty() { m_boundingSphereDirty = true; }

    // Ray intersection testing
    bool IntersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const;
    bool IntersectsRay(const Math::Ray& ray, float& distance) const;

    // Temel komponenet işlevleri
    void Start() override;
    void Update(float deltaTime) override;
    void OnEnable() override;
    void OnDisable() override;

    // Bileşen tipi bilgisi
    [[nodiscard]] std::string GetTypeName() const override { return "MeshComponent"; }
};

#endif // MESH_COMPONENT_H
