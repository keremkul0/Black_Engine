#ifndef MESH_RENDERER_COMPONENT_H
#define MESH_RENDERER_COMPONENT_H

#include "BaseComponent.h"
#include "TransformComponent.h"
#include "../Render/Mesh.h"
#include "../Render/Shader.h"  // varsa
#include <memory>

class MeshRendererComponent : public BaseComponent
{
public:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> shader;  // Shader / Material vb.

    MeshRendererComponent() = default;
    virtual ~MeshRendererComponent() = default;

    void Draw() override;
};

#endif
