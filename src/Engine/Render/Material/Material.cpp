#include "Material.h"

#include <iostream>
#include <utility>


Material::Material()
    : albedo(1.0f), metallic(0.0f), roughness(0.5f), ao(1.0f),
      diffuseColor(1.0f), specularColor(0.5f), shininess(32.0f) {
}

Material::Material(std::shared_ptr<Shader> shader)
    : albedo(1.0f), metallic(0.0f), roughness(0.5f), ao(1.0f),
      diffuseColor(1.0f), specularColor(0.5f), shininess(32.0f),
      m_shader(std::move(shader)) {
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

    // Set PBR material properties
    m_shader->setVec3("material.albedo", albedo);
    m_shader->setFloat("material.metallic", metallic);
    m_shader->setFloat("material.roughness", roughness);
    m_shader->setFloat("material.ao", ao);

    // Legacy properties for backward compatibility
    m_shader->setVec3("material.diffuse", diffuseColor);
    m_shader->setVec3("material.specular", specularColor);
    m_shader->setFloat("material.shininess", shininess);

    // Bind textures
    int textureUnit = 0;

    // PBR textures
    // Albedo/Base color map
    if (auto albedoTex = GetTexture(TextureType::Albedo)) {
        albedoTex->Bind(textureUnit);
        m_shader->setInt("material.albedoMap", textureUnit);
        m_shader->setInt("material.hasAlbedoMap", 1);
        textureUnit++;
    } else if (auto diffuseTex = GetTexture(TextureType::Diffuse)) { // Fallback to legacy
        diffuseTex->Bind(textureUnit);
        m_shader->setInt("material.albedoMap", textureUnit);
        m_shader->setInt("material.hasAlbedoMap", 1);
        textureUnit++;
    } else {
        m_shader->setInt("material.hasAlbedoMap", 0);
    }

    // Metallic map
    if (auto metallicTex = GetTexture(TextureType::Metallic)) {
        metallicTex->Bind(textureUnit);
        m_shader->setInt("material.metallicMap", textureUnit);
        m_shader->setInt("material.hasMetallicMap", 1);
        textureUnit++;
    } else {
        m_shader->setInt("material.hasMetallicMap", 0);
    }

    // Roughness map
    if (auto roughnessTex = GetTexture(TextureType::Roughness)) {
        roughnessTex->Bind(textureUnit);
        m_shader->setInt("material.roughnessMap", textureUnit);
        m_shader->setInt("material.hasRoughnessMap", 1);
        textureUnit++;
    } else {
        m_shader->setInt("material.hasRoughnessMap", 0);
    }

    // AO map
    if (auto aoTex = GetTexture(TextureType::AO)) {
        aoTex->Bind(textureUnit);
        m_shader->setInt("material.aoMap", textureUnit);
        m_shader->setInt("material.hasAoMap", 1);
        textureUnit++;
    } else {
        m_shader->setInt("material.hasAoMap", 0);
    }

    // Normal map
    if (auto normalTex = GetTexture(TextureType::Normal)) {
        normalTex->Bind(textureUnit);
        m_shader->setInt("material.normalMap", textureUnit);
        m_shader->setInt("material.hasNormalMap", 1);
        textureUnit++;
    } else {
        m_shader->setInt("material.hasNormalMap", 0);
    }

    // Legacy textures for backward compatibility
    if (!GetTexture(TextureType::Albedo) && !GetTexture(TextureType::Diffuse)) {
        if (auto diffuseTex = GetTexture(TextureType::Diffuse)) {
            diffuseTex->Bind(textureUnit);
            m_shader->setInt("material.diffuseMap", textureUnit);
            m_shader->setInt("material.hasDiffuseMap", 1);
            textureUnit++;
        } else {
            m_shader->setInt("material.hasDiffuseMap", 0);
        }
    }

    if (auto specularTex = GetTexture(TextureType::Specular)) {
        specularTex->Bind(textureUnit);
        m_shader->setInt("material.specularMap", textureUnit);
        m_shader->setInt("material.hasSpecularMap", 1);
    } else {
        m_shader->setInt("material.hasSpecularMap", 0);
    }
}