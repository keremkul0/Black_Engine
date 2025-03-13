#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Engine/Render/Shader/Shader.h"
#include "Engine/Render/Texture/Texture.h"

enum class TextureType {
    Diffuse,
    Specular,
    Normal,
    // Add more types as needed
};

class Material {
public:
    Material();
    explicit Material(std::shared_ptr<Shader> shader);

    // Material properties
    glm::vec3 diffuseColor = glm::vec3(1.0f);
    glm::vec3 specularColor = glm::vec3(0.5f);
    float shininess = 32.0f;

    // Shader management
    void SetShader(const std::shared_ptr<Shader>& shader);
    std::shared_ptr<Shader> GetShader() const { return m_shader; }

    // Texture management
    void SetTexture(TextureType type, const std::shared_ptr<Texture>& texture);
    std::shared_ptr<Texture> GetTexture(TextureType type) const;

    // Apply material to current shader
    void Apply() const;

private:
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<TextureType, std::shared_ptr<Texture>> m_textures;
};

#endif // MATERIAL_H