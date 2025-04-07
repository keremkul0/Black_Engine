#include "GamePanel.h"
#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <glm/fwd.hpp>

#include "imgui.h"
#include "Engine/Scene/Scene.h"

extern glm::mat4 gViewMatrix;

GamePanel::GamePanel(const std::string &title)
    : Panel(title) {
    SetupFramebuffer();
}

GamePanel::~GamePanel() {
    CleanupFramebuffer();
}

void GamePanel::SetScene(const std::shared_ptr<Scene> &scene) {
    m_Scene = scene;
}

void GamePanel::DrawContent() {
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

    // Only render if we have valid size
    if (contentRegionAvail.x <= 0 || contentRegionAvail.y <= 0)
        return;

    // Update the framebuffer size if needed
    if (m_ViewportWidth != static_cast<int>(contentRegionAvail.x) ||
        m_ViewportHeight != static_cast<int>(contentRegionAvail.y)) {
        // Resize the framebuffer
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     static_cast<int>(contentRegionAvail.x),
                     static_cast<int>(contentRegionAvail.y),
                     0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                              static_cast<int>(contentRegionAvail.x),
                              static_cast<int>(contentRegionAvail.y));

        m_ViewportWidth = static_cast<int>(contentRegionAvail.x);
        m_ViewportHeight = static_cast<int>(contentRegionAvail.y);

    }

    // Render scene to framebuffer
    if (m_FramebufferID > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
        glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the scene if available
        if (m_Scene) {
            m_Scene->DrawAll();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Display the framebuffer texture in ImGui
    if (m_TextureID != 0) {
        ImGui::Image(
            reinterpret_cast<ImTextureID>(reinterpret_cast<void *>(static_cast<intptr_t>(m_TextureID))),
            contentRegionAvail,
            ImVec2(0, 1), // UV0 (flipped Y)
            ImVec2(1, 0) // UV1
        );
    }
}

bool GamePanel::OnInputEvent(const InputEvent &event) {
    if (!IsFocused())
        return false;

    // Game-specific input handling can be added here
    // For now, this is a simple placeholder

    return false;
}

void GamePanel::SetupFramebuffer() {
    // Create framebuffer
    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    // Create a texture attachment
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // Set texture parameters
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_ViewportWidth, m_ViewportHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);

    // Create a renderbuffer for depth and stencil
    glGenRenderbuffers(1, &m_RenderbufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_ViewportWidth, m_ViewportHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR: Game framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void GamePanel::CleanupFramebuffer() {
    if (m_RenderbufferID) {
        glDeleteRenderbuffers(1, &m_RenderbufferID);
        m_RenderbufferID = 0;
    }

    if (m_TextureID) {
        glDeleteTextures(1, &m_TextureID);
        m_TextureID = 0;
    }

    if (m_FramebufferID) {
        glDeleteFramebuffers(1, &m_FramebufferID);
        m_FramebufferID = 0;
    }
}
