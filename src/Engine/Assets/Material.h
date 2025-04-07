#pragma once
#include "Core/AssetDatabase/AssetDatabase.h"
#include <glm/glm.hpp>
#include <unordered_map>

class Shader;

class Material : public Asset {
public:
    Material() = default;
    ~Material() override;

    void Bind() const;
    void Unbind() const;

    // Shader reference
    void SetShader(const std::string& shaderPath);
    Shader* GetShader() const;

    // Property setters
    void SetFloat(const std::string& name, float value);
    void SetInt(const std::string& name, int value);
    void SetVector2(const std::string& name, const glm::vec2& value);
    void SetVector3(const std::string& name, const glm::vec3& value);
    void SetVector4(const std::string& name, const glm::vec4& value);
    void SetMatrix4(const std::string& name, const glm::mat4& value);
    void SetTexture(const std::string& name, const std::string& texturePath);

    // Property getters
    float GetFloat(const std::string& name) const;
    int GetInt(const std::string& name) const;
    glm::vec2 GetVector2(const std::string& name) const;
    glm::vec3 GetVector3(const std::string& name) const;
    glm::vec4 GetVector4(const std::string& name) const;
    glm::mat4 GetMatrix4(const std::string& name) const;
    std::string GetTexture(const std::string& name) const;

private:
    AssetRef<Shader> m_Shader;

    // Material properties
    std::unordered_map<std::string, float> m_FloatProperties;
    std::unordered_map<std::string, int> m_IntProperties;
    std::unordered_map<std::string, glm::vec2> m_Vec2Properties;
    std::unordered_map<std::string, glm::vec3> m_Vec3Properties;
    std::unordered_map<std::string, glm::vec4> m_Vec4Properties;
    std::unordered_map<std::string, glm::mat4> m_Mat4Properties;
    std::unordered_map<std::string, std::string> m_TextureProperties;
};