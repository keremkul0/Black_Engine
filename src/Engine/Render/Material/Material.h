#pragma once

#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include "../Shader/Shader.h"
#include "../Texture/Texture.h"

enum class TextureType {
    Albedo, // Replaces diffuse
    Metallic, // New
    Roughness, // New
    AO, // New
    Normal,
    // Legacy types for backward compatibility
    Diffuse,
    Specular
};

class Material {
public:
    Material();

    explicit Material(std::shared_ptr<Shader> shader);

    void SetShader(const std::shared_ptr<Shader> &shader);

    void SetTexture(TextureType type, const std::shared_ptr<Texture> &texture);

    std::shared_ptr<Shader> GetShader() const { return m_shader; }

    std::shared_ptr<Texture> GetTexture(TextureType type) const;

    void Apply() const;

    // PBR properties
    glm::vec3 albedo; // Base color (replaces diffuse)
    float metallic; // Metallic value [0-1]
    float roughness; // Surface roughness [0-1]
    float ao; // Ambient occlusion factor [0-1]

    // Legacy properties (for backward compatibility)
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    float shininess;

private:
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<TextureType, std::shared_ptr<Texture> > m_textures;
};
