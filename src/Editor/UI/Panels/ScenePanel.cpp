// src/Editor/UI/ScenePanel.cpp
#include "ScenePanel.h"

#include <iostream>

#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Camera/Camera.h"

ScenePanel::ScenePanel(const std::string &title)
    : Panel(title) {
    SetupFramebuffer();
    SetupCamera();
}

void ScenePanel::DrawContent() {
    // Calculate viewport size
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

    // Update viewport state
    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();

    // Handle input and camera update
    if (m_ViewportHovered) {
        HandleCameraInput();
    }
    UpdateCamera(ImGui::GetIO().DeltaTime);

    // Render scene to framebuffer
    if (m_FramebufferID > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_Scene) {
            // Create view and projection matrices
            glm::mat4 viewMatrix = glm::lookAt(m_CameraPosition,
                                              m_CameraPosition + m_CameraFront,
                                              m_CameraUp);
            glm::mat4 projMatrix = glm::perspective(glm::radians(45.0f),
                                                   contentRegionAvail.x / std::max(contentRegionAvail.y, 1.0f),
                                                   0.1f, 100.0f);

            // Instead of calling SetViewMatrix/SetProjectionMatrix, directly use DrawAll
            // or a proper rendering method that your Scene class supports
            m_Scene->DrawAll(); // Your Scene class might need a different approach
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Display the rendered image in ImGui
        ImGui::Image(reinterpret_cast<ImTextureID>(reinterpret_cast<void *>(static_cast<intptr_t>(m_TextureID))), contentRegionAvail, ImVec2(0, 1), ImVec2(1, 0));
    }
}

void ScenePanel::SetupCamera() {
    m_Camera = std::make_unique<Camera>();

    // Set initial camera position and orientation
    m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    m_Camera->SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));

    // Also update our internal tracking variables to match
    m_CameraPosition = m_Camera->GetPosition();
    m_CameraFront = m_Camera->GetFront();
}

void ScenePanel::SetupFramebuffer() {
    // Create framebuffer
    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    // Create a texture attachment
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // Set texture parameters
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);

    // Create a renderbuffer for depth and stencil
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ScenePanel::HandleCameraInput() {
    // Handle mouse input for camera rotation
    ImVec2 mousePos = ImGui::GetMousePos();

    // If right mouse button is held down
    if (ImGui::IsMouseDown(1)) {
        // First frame of click, capture position
        if (ImGui::IsMouseClicked(1)) {
            m_LastMousePos = mousePos;
        } else {
            // Calculate mouse offset
            float xOffset = mousePos.x - m_LastMousePos.x;
            float yOffset = m_LastMousePos.y - mousePos.y; // Reversed: y ranges bottom to top

            m_LastMousePos = mousePos;

            // Apply rotation
            m_CameraYaw += xOffset * m_CameraRotationSpeed;
            m_CameraPitch += yOffset * m_CameraRotationSpeed;

            // Constrain pitch
            if (m_CameraPitch > 89.0f) m_CameraPitch = 89.0f;
            if (m_CameraPitch < -89.0f) m_CameraPitch = -89.0f;
        }
    }
}

void ScenePanel::UpdateCamera(float deltaTime) {
    // Calculate camera front vector from yaw and pitch
    glm::vec3 front;
    front.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
    front.y = sin(glm::radians(m_CameraPitch));
    front.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
    m_CameraFront = glm::normalize(front);

    // Handle keyboard input for camera movement
    if (m_ViewportFocused) {
        // Forward/backward
        if (ImGui::IsKeyDown(ImGuiKey_W))
            m_CameraPosition += m_CameraFront * m_CameraSpeed * deltaTime;
        if (ImGui::IsKeyDown(ImGuiKey_S))
            m_CameraPosition -= m_CameraFront * m_CameraSpeed * deltaTime;

        // Left/right
        if (ImGui::IsKeyDown(ImGuiKey_A)) {
            glm::vec3 right = glm::normalize(glm::cross(m_CameraFront, m_CameraUp));
            m_CameraPosition -= right * m_CameraSpeed * deltaTime;
        }
        if (ImGui::IsKeyDown(ImGuiKey_D)) {
            glm::vec3 right = glm::normalize(glm::cross(m_CameraFront, m_CameraUp));
            m_CameraPosition += right * m_CameraSpeed * deltaTime;
        }

        // Up/down
        if (ImGui::IsKeyDown(ImGuiKey_E))
            m_CameraPosition += m_CameraUp * m_CameraSpeed * deltaTime;
        if (ImGui::IsKeyDown(ImGuiKey_Q))
            m_CameraPosition -= m_CameraUp * m_CameraSpeed * deltaTime;
    }

    // Update view matrix in scene camera (placeholder)
    // Will need to be implemented when camera integration is ready
}
