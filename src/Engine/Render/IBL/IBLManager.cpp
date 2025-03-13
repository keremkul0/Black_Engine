#include "IBLManager.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Shader/Shader.h"
#include "../Primitives/Primitives.h"

IBLManager::IBLManager() {
    // Initialize the maps as needed
    m_IrradianceMap = std::make_shared<Cubemap>();
    m_PrefilteredMap = std::make_shared<Cubemap>();

    // Generate the BRDF LUT once
    GenerateBRDFLookUpTexture();
}

IBLManager::~IBLManager() {
    if (m_BRDFLutTexture) {
        glDeleteTextures(1, &m_BRDFLutTexture);
    }
}

void IBLManager::BindMaps(unsigned int irradianceUnit,
                         unsigned int prefilteredUnit,
                         unsigned int brdfLutUnit) const {
    m_IrradianceMap->Bind(irradianceUnit);
    m_PrefilteredMap->Bind(prefilteredUnit);

    glActiveTexture(GL_TEXTURE0 + brdfLutUnit);
    glBindTexture(GL_TEXTURE_2D, m_BRDFLutTexture);
}

void IBLManager::BindTextures(std::shared_ptr<Shader> shader, int& startTextureUnit) {
    shader->setInt("useIBL", 1);

    // Bind irradiance map
    m_IrradianceMap->Bind(startTextureUnit);
    shader->setInt("irradianceMap", startTextureUnit++);

    // Bind prefiltered map
    m_PrefilteredMap->Bind(startTextureUnit);
    shader->setInt("prefilteredMap", startTextureUnit++);

    // Bind BRDF LUT
    glActiveTexture(GL_TEXTURE0 + startTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_BRDFLutTexture);
    shader->setInt("brdfLUT", startTextureUnit++);
}

void IBLManager::GenerateEnvironmentMap(const std::string& hdrPath, int resolution) {
    // Create and load environment map from equirectangular HDR
    m_EnvironmentMap = std::make_shared<Cubemap>();
    if (!m_EnvironmentMap->LoadFromEquirectangular(hdrPath)) {
        std::cerr << "Failed to load HDR environment map: " << hdrPath << std::endl;
        return;
    }

    // Generate irradiance map for diffuse IBL component
    GenerateIrradianceMap(m_EnvironmentMap);

    // Generate prefiltered environment map for specular IBL component
    GeneratePrefilteredEnvMap(m_EnvironmentMap);

    std::cout << "IBL environment maps generated successfully" << std::endl;
}

void IBLManager::GenerateIrradianceMap(const std::shared_ptr<Cubemap>& envMap) {
    // Create irradiance map with lower resolution
    const int irradianceSize = 32; // Lower resolution is sufficient for diffuse

    // Create cubemap texture
    unsigned int irradianceMapID;
    glGenTextures(1, &irradianceMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                    irradianceSize, irradianceSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create framebuffer for cubemap rendering
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceSize, irradianceSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // Set up projection and view matrices for each cubemap face
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    // Load irradiance convolution shader
    auto irradianceShader = std::make_shared<Shader>("../src/shaders/cubemap.vert", "../src/shaders/irradiance_convolution.frag");
    irradianceShader->use();
    irradianceShader->setInt("environmentMap", 0);
    irradianceShader->setMat4("projection", captureProjection);

    // Use a cube to sample the environment
    auto cubeMesh = Primitives::CreateCube(2.0f); // Size 2 means vertices from -1 to 1

    // Set viewport to capture resolution
    glViewport(0, 0, irradianceSize, irradianceSize);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    // Render to 6 cubemap faces
    for (unsigned int i = 0; i < 6; ++i) {
        irradianceShader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMapID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind environment map
        envMap->Bind(0);

        // Render cube
        glBindVertexArray(cubeMesh->GetVAO());
        glDrawElements(GL_TRIANGLES, cubeMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Clean up
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);

    // Set our irradiance map
    m_IrradianceMap = std::make_shared<Cubemap>();
    m_IrradianceMap->SetID(irradianceMapID);
}

void IBLManager::GeneratePrefilteredEnvMap(const std::shared_ptr<Cubemap>& envMap) {
    // Create prefiltered map with mipmap levels
    const int prefilteredSize = 128; // Size for prefiltered environment map

    // Create cubemap texture with mips
    unsigned int prefilteredMapID;
    glGenTextures(1, &prefilteredMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMapID);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                    prefilteredSize, prefilteredSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Create framebuffer for cubemap rendering
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    // Set up projection and views (same as for irradiance map)
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    // Load prefiltering shader
    auto prefilterShader = std::make_shared<Shader>("../src/shaders/cubemap.vert", "../src/shaders/prefilter.frag");
    prefilterShader->use();
    prefilterShader->setInt("environmentMap", 0);
    prefilterShader->setMat4("projection", captureProjection);

    // Use a cube to sample the environment
    auto cubeMesh = Primitives::CreateCube(2.0f);

    // Bind environment map
    envMap->Bind(0);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    // Generate prefiltered mipmaps for different roughness levels
    const unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        // Resize framebuffer according to mip level
        const unsigned int mipSize = prefilteredSize * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipSize, mipSize);
        glViewport(0, 0, mipSize, mipSize);

        // Calculate roughness for this mip level
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader->setFloat("roughness", roughness);

        // Render to 6 cubemap faces
        for (unsigned int i = 0; i < 6; ++i) {
            prefilterShader->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilteredMapID, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Render cube
            glBindVertexArray(cubeMesh->GetVAO());
            glDrawElements(GL_TRIANGLES, cubeMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    // Clean up
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);

    // Set our prefiltered map
    m_PrefilteredMap = std::make_shared<Cubemap>();
    m_PrefilteredMap->SetID(prefilteredMapID);
}

void IBLManager::GenerateBRDFLookUpTexture() {
    // Create BRDF lookup texture
    const int brdfSize = 512;

    glGenTextures(1, &m_BRDFLutTexture);
    glBindTexture(GL_TEXTURE_2D, m_BRDFLutTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, brdfSize, brdfSize, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create framebuffer
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, brdfSize, brdfSize);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BRDFLutTexture, 0);

    // Load BRDF shader
    auto brdfShader = std::make_shared<Shader>("../src/shaders/brdf.vert", "../src/shaders/brdf.frag");

    // Use a quad to render the LUT
    auto quadMesh = Primitives::CreateQuad(2.0f, 2.0f); // Size 2 means vertices from -1 to 1

    // Set viewport to LUT resolution
    glViewport(0, 0, brdfSize, brdfSize);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render quad with BRDF shader
    brdfShader->use();
    glBindVertexArray(quadMesh->GetVAO());
    glDrawElements(GL_TRIANGLES, quadMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Clean up
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);

    std::cout << "BRDF LUT generated" << std::endl;
}