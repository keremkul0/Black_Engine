#ifndef MESH_RENDERER_COMPONENT_H
#define MESH_RENDERER_COMPONENT_H

#include "BaseComponent.h"
#include "MeshComponent.h"
#include <memory>
#include "TransformComponent.h"
#include "Engine/Render/Material/Material.h"

class MeshRendererComponent final : public BaseComponent
{
private:
    std::shared_ptr<Material> m_material;

public:
    MeshRendererComponent() = default;
    ~MeshRendererComponent() override = default;    // Shader setter/getter - now retrieved from material
    void SetShader(const std::shared_ptr<Shader> &shader) { 
        if (m_material) m_material->SetShader(shader); 
    }
    [[nodiscard]] std::shared_ptr<Shader> GetShader() const { 
        return m_material ? m_material->GetShader() : nullptr; 
    }
    
    // Get the mesh from the associated MeshComponent
    [[nodiscard]] std::shared_ptr<Mesh> GetMesh() const;
    
    // Material setter/getter
    void SetMaterial(const std::shared_ptr<Material>& material) { m_material = material; }
    [[nodiscard]] std::shared_ptr<Material> GetMaterial() const { return m_material; }

    // BaseComponent overrides
    void Start() override;
    void Draw() override;
    void DrawWireframe() override; // Add wireframe drawing for selection highlighting
    void OnEnable() override;
    void OnDisable() override;

    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    void Draw2ShadowMap() override; // Override for shadow map drawing
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//

    // Bileşen tipi bilgisi
    [[nodiscard]] std::string GetTypeName() const override { return "MeshRendererComponent"; }

private:
    MeshComponent* m_cachedMeshComponent = nullptr;
    TransformComponent* m_cachedTransform = nullptr;

    void CacheComponents();
};

#endif // MESH_RENDERER_COMPONENT_H
