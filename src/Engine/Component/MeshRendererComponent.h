#ifndef MESH_RENDERER_COMPONENT_H
#define MESH_RENDERER_COMPONENT_H

#include "BaseComponent.h"
#include "MeshComponent.h"
#include "../Render/Shader.h"
#include <memory>
#include "TransformComponent.h"

class MeshRendererComponent final : public BaseComponent
{
private:
    std::shared_ptr<Shader> m_shader;

public:
    MeshRendererComponent() = default;
    ~MeshRendererComponent() override = default;

    // Shader setter/getter
    void SetShader(const std::shared_ptr<Shader> &shader) { m_shader = shader; }
    [[nodiscard]] std::shared_ptr<Shader> GetShader() const { return m_shader; }
    // BaseComponent overrides
    void Start() override;
    void Draw() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnInspectorGUI() override;

    // Bileşen tipi bilgisi
    [[nodiscard]] const char* GetTypeName() const override { return "MeshRendererComponent"; }

private:
    MeshComponent* m_cachedMeshComponent = nullptr;
    TransformComponent* m_cachedTransform = nullptr;


    void CacheComponents();
};

#endif // MESH_RENDERER_COMPONENT_H