#include "Material.h"
#include "Engine/Render/Texture/Texture.h"
#include <glm/glm.hpp>

// Define static variable for camera position
glm::vec3 Material::s_CameraPosition = glm::vec3(0.0f);

Material::Material() {
    m_shader = std::make_shared<Shader>(
        "../src/shaders/simple.vert", 
        "../src/shaders/simple.frag"
    );
    m_texture = std::make_shared<Texture>(
        "../src/Engine/Render/Texture/TextureImages/brick.png",
        GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE
    );
}

// Implement SetCameraPosition to update the static camera position
void Material::SetCameraPosition(const glm::vec3& camPos) {
    s_CameraPosition = camPos;
}

void Material::Apply() {
    if (m_shader) {
        m_shader->use();
        m_shader->setVec3("camPos", s_CameraPosition); // Set uniform for camera position
    }
    if (m_texture) {
        m_texture->Bind();
    }
}
