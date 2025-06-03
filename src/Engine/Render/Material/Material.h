#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include "Engine/Render/Shader/Shader.h"
#include "Engine/Render/Texture/Texture.h"
#include <glm/glm.hpp>

class Material {
public:
    Material(); // Custom constructor to assign default shader and texture
    ~Material() = default;

    void SetShader(const std::shared_ptr<Shader>& shader) { m_shader = shader; }
    std::shared_ptr<Shader> GetShader() const { return m_shader; }

    void SetTexture(const std::shared_ptr<Texture>& texture) { m_texture = texture; }
    std::shared_ptr<Texture> GetTexture() const { return m_texture; }

    // Activates the material: activates the shader and binds the texture (if available)
    void Apply();

    // Added to update the camera position for all materials
    static void SetCameraPosition(const glm::vec3& camPos);

private:
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Texture> m_texture;

    static glm::vec3 s_CameraPosition; // Static camera position
};

#endif // MATERIAL_H

