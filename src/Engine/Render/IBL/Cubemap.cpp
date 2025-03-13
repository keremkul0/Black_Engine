#include "Cubemap.h"
#include "Engine/Render/Primitives/Primitives.h"
#include "Engine/Render/Shader/Shader.h"
#include <iostream>
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>

Cubemap::Cubemap() = default;

Cubemap::~Cubemap() {
    if (m_ID) {
        glDeleteTextures(1, &m_ID);
    }
}

bool Cubemap::LoadFromFiles(const std::vector<std::string> &facePaths) {
    if (facePaths.size() != 6) {
        std::cerr << "Cubemap requires exactly 6 face textures" << std::endl;
        return false;
    }

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

    int width, height, channels;
    for (unsigned int i = 0; i < facePaths.size(); i++) {
        unsigned char *data = stbi_load(facePaths[i].c_str(), &width, &height, &channels, 0);
        if (data) {
            GLenum format = channels == 3 ? GL_RGB : GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
                         width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load cubemap texture: " << facePaths[i] << std::endl;
            stbi_image_free(data);
            return false;
        }
    }

    m_Width = width;
    m_Height = height;

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return true;
}

bool Cubemap::LoadFromEquirectangular(const std::string& hdrPath) {
    // Load HDR image
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    float* data = stbi_loadf(hdrPath.c_str(), &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Failed to load HDR image: " << hdrPath << std::endl;
        return false;
    }

    // Create texture for equirectangular HDR
    unsigned int hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

    // Free image data once it's uploaded to GPU
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create cubemap texture
    const int cubeSize = 512; // Resolution of the cubemap faces
    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                    cubeSize, cubeSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set up projection and view matrices for each face of the cubemap
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    // Create and use equirectangular to cubemap shader
    auto equirectToCubemapShader = std::make_shared<Shader>("../src/shaders/cubemap.vert", "../src/shaders/equirectangular_to_cubemap.frag");
    equirectToCubemapShader->use();
    equirectToCubemapShader->setInt("equirectangularMap", 0);
    equirectToCubemapShader->setMat4("projection", captureProjection);

    // Use a cube to sample the environment
    auto cubeMesh = Primitives::CreateCube(2.0f);

    // Create a framebuffer for rendering to the cubemap
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeSize, cubeSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // Set viewport to cubemap face resolution
    glViewport(0, 0, cubeSize, cubeSize);

    // Bind HDR texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    // Render to each face of the cubemap
    for (unsigned int i = 0; i < 6; ++i) {
        equirectToCubemapShader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_ID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render cube
        glBindVertexArray(cubeMesh->GetVAO());
        glDrawElements(GL_TRIANGLES, cubeMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Generate mipmaps for the cubemap
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Clean up
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
    glDeleteTextures(1, &hdrTexture);

    m_Width = cubeSize;
    m_Height = cubeSize;

    return true;
}

void Cubemap::SetID(unsigned int id) {
    if (m_ID) {
        glDeleteTextures(1, &m_ID);
    }
    m_ID = id;
}

void Cubemap::Bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
}
