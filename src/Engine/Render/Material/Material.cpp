#include "Material.h"
#include "Engine/Render/Texture/Texture.h"
#include <glm/glm.hpp>
#include <filesystem>
#include <iostream>

// Define static variable for camera position
glm::vec3 Material::s_CameraPosition = glm::vec3(0.0f);

Material::Material() {
    // Use direct path to the Black_Engine root directory
    std::filesystem::path projectRoot = "C:/Users/EREN/CLionProjects/Black_Engine";
    std::filesystem::path vertPath = projectRoot / "src" / "shaders" / "simple.vert";
    std::filesystem::path fragPath = projectRoot / "src" / "shaders" / "simple.frag"; 
    std::cout << "Loading shaders from: " << vertPath.string() << " and " << fragPath.string() << std::endl;
    
    try {
        m_shader = std::make_shared<Shader>(
            vertPath.string().c_str(),
            fragPath.string().c_str()
        );
    } 
    catch (const std::exception& e) {
        std::cerr << "Failed to load shader: " << e.what() << std::endl;
        // Create a default shader or handle the error
    }
    
    // Initialize with no texture
    m_texture = nullptr;
    
    // When proper textures are available, uncomment this code:

    // Use proper path relative to the project root
    std::filesystem::path texturePath = projectRoot / "src" / "Engine" / "Render" / "Texture" / "TextureImages" / "planksSpec.png";
    std::cout << "Loading texture from: " << texturePath.string() << std::endl;
    if (std::filesystem::exists(texturePath) && std::filesystem::file_size(texturePath) > 0) {
        m_texture = std::make_shared<Texture>(
            texturePath.string().c_str(),
            GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE
        );
    } else {
        std::cerr << "[Material] Texture file missing or empty: " << texturePath << std::endl;
        m_texture = nullptr;
    }
}

// Implement SetCameraPosition to update the static camera position
void Material::SetCameraPosition(const glm::vec3& camPos) {
    s_CameraPosition = camPos;
}

void Material::Apply() {
    if (m_shader) {
        m_shader->use();
        
        try {
            m_shader->setVec3("camPos", s_CameraPosition); // Set uniform for camera position
        } catch (const std::exception& e) {
            // Ignore errors if the uniform doesn't exist in the shader
            std::cerr << "Warning: " << e.what() << std::endl;
        }
        
        // hasTexture uniformunu ayarla
        m_shader->setBool("hasTexture", m_texture != nullptr);
    }
    
    if (m_texture) {
        try {
            m_texture->Bind();
        } catch (const std::exception& e) {
            // Ignore texture binding errors
            std::cerr << "Warning: Failed to bind texture: " << e.what() << std::endl;
        }
    }
}

//**//**//**//**//**//**//**//**//**//**//**//**//
void Material::Apply2ShadowMap() {
    if (m_shadowMapShader) {
        m_shadowMapShader->use();

    }

}
//**//**//**//**//**//**//**//**//**//**//**//**//
