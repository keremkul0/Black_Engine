#include "Material.h"

#include <iostream>
#include <utility>

Material::Material() : diffuseColor(1.0f), specularColor(0.5f), shininess(32.0f) {
}

Material::Material(std::shared_ptr<Shader> shader)
    : diffuseColor(1.0f), specularColor(0.5f), shininess(32.0f), m_shader(std::move(shader)) {
}

void Material::SetShader(const std::shared_ptr<Shader>& shader) {
    m_shader = shader;
}

void Material::SetTexture(TextureType type, const std::shared_ptr<Texture>& texture) {
    m_textures[type] = texture;
}

std::shared_ptr<Texture> Material::GetTexture(const TextureType type) const {
    if (const auto it = m_textures.find(type); it != m_textures.end()) {
        return it->second;
    }
    return nullptr;
}

void Material::Apply() const {
    if (!m_shader) {
        std::cerr << "Error: No shader assigned to material!" << std::endl;
        return;
    }

    m_shader->use();

    if (m_shader->HasUniform("material.hasNormalMap")) {
        const bool hasNormalMapTexture = (GetTexture(TextureType::Normal) != nullptr);
        m_shader->setBool("material.hasNormalMap", hasNormalMapTexture);
    }

    // Set material properties
    m_shader->setVec3("material.diffuse", diffuseColor);
    m_shader->setVec3("material.specular", specularColor);
    m_shader->setFloat("material.shininess", shininess);

    // Bind textures
    int textureUnit = 0;

    // Check for diffuse texture
    if (const auto diffuseTex = GetTexture(TextureType::Diffuse)) {
        diffuseTex->Bind(textureUnit);
        m_shader->setInt("material.diffuseMap", textureUnit);
        m_shader->setInt("material.hasDiffuseMap", 1);
        textureUnit++;
    } else {
        m_shader->setInt("material.hasDiffuseMap", 0);
    }

    // Check for specular texture
    if (const auto specularTex = GetTexture(TextureType::Specular)) {
        specularTex->Bind(textureUnit);
        m_shader->setInt("material.specularMap", textureUnit);
        m_shader->setInt("material.hasSpecularMap", 1);
        textureUnit++;
    } else {
        m_shader->setInt("material.hasSpecularMap", 0);
    }

    // Check for normal texture
    if (const auto normalTex = GetTexture(TextureType::Normal)) {
        normalTex->Bind(textureUnit);
        m_shader->setInt("material.normalMap", textureUnit);
        m_shader->setInt("material.hasNormalMap", 1);
    } else {
        m_shader->setInt("material.hasNormalMap", 0);
    }
}