// src/Editor/UI/GamePanel.cpp
#include "GamePanel.h"

#include <glad/glad.h>

#include "imgui.h"

GamePanel::GamePanel(const std::string& title)
    : Panel(title) {
    SetupFramebuffer();
}

GamePanel::~GamePanel() {
    // Clean up OpenGL resources
    if (m_FramebufferID != 0) {
        glDeleteFramebuffers(1, &m_FramebufferID);
        m_FramebufferID = 0;
    }
    if (m_TextureID != 0) {
        glDeleteTextures(1, &m_TextureID);
        m_TextureID = 0;
    }
}

void GamePanel::DrawContent() {
    // Calculate content area size
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

    // Check if framebuffer size needs to be updated
    if (contentRegionAvail.x > 0 && contentRegionAvail.y > 0 &&
        (m_LastSize.x != contentRegionAvail.x || m_LastSize.y != contentRegionAvail.y)) {
        ResizeFramebuffer(contentRegionAvail.x, contentRegionAvail.y);
        m_LastSize = contentRegionAvail;
        }

    // If we have a valid texture ID, display it
    if (m_TextureID != 0) {
        // Fixed casting to match ScenePanel approach
        ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(m_TextureID)), contentRegionAvail);
    } else {
        ImGui::TextWrapped("No camera view available. Add a camera to the scene to see the game view.");
    }
}

void GamePanel::SetupFramebuffer() {
    // Start with a small default size - will resize as needed
    int width = 800;
    int height = 600;

    // Generate framebuffer
    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    // Generate texture
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // Setup texture parameters
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);

    // Create renderbuffer for depth and stencil
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // Handle error - reset IDs to avoid using incomplete framebuffer
        glDeleteFramebuffers(1, &m_FramebufferID);
        glDeleteTextures(1, &m_TextureID);
        m_FramebufferID = 0;
        m_TextureID = 0;
    }

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_LastSize = ImVec2(width, height);
}

void GamePanel::ResizeFramebuffer(float width, float height) {
    if (width <= 0 || height <= 0 || m_FramebufferID == 0)
        return;

    // Resize the texture
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)width, (int)height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    // Resize renderbuffer if you have one
    // ...
}