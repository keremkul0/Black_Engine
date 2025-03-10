#include "ScenePanel.h"

#include <iostream>

#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Core/Camera/Camera.h"
#include "Core/InputManager/InputManager.h"
#include "Core/InputManager/InputEvent.h"

extern glm::mat4 gViewMatrix;

ScenePanel::ScenePanel(const std::string &title) : Panel(title) {
    m_Camera = std::make_unique<Camera>(m_CameraPosition);
    SetupFramebuffer();
    SetupCamera();
}

ScenePanel::~ScenePanel() {
    CleanupResources();
}

void ScenePanel::CleanupResources() {
    if (m_FramebufferID != 0) {
        glDeleteFramebuffers(1, &m_FramebufferID);
        m_FramebufferID = 0;
    }

    if (m_TextureID != 0) {
        glDeleteTextures(1, &m_TextureID);
        m_TextureID = 0;
    }

    if (m_DepthRenderBuffer != 0) {
        glDeleteRenderbuffers(1, &m_DepthRenderBuffer);
        m_DepthRenderBuffer = 0;
    }
}

void ScenePanel::SetScene(const std::shared_ptr<Scene> &scene) {
    m_Scene = scene;
}

void ScenePanel::OnUpdate(const float deltaTime) {
    UpdateCursorForCurrentAction();
    // Update camera properties based on accumulated input
    if (m_IsActive && m_IsOpen && (m_IsFocused || m_IsHovered) && InputManager::IsMouseButtonPressed(
            GLFW_MOUSE_BUTTON_RIGHT)) {
        // Apply shift key speed multiplier
        const float speedMultiplier = (InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ||
                                       InputManager::IsKeyPressed(GLFW_KEY_RIGHT_SHIFT))
                                          ? 3.0f
                                          : 1.0f;
        const float adjustedSpeed = m_CameraSpeed * speedMultiplier * deltaTime;

        // Apply continuous movement for held keys
        if (InputManager::IsKeyPressed(GLFW_KEY_W)) {
            m_CameraPosition += adjustedSpeed * m_CameraFront;
            UpdateCamera(deltaTime);
        }

        if (InputManager::IsKeyPressed(GLFW_KEY_S)) {
            m_CameraPosition -= adjustedSpeed * m_CameraFront;
            UpdateCamera(deltaTime);
        }

        if (InputManager::IsKeyPressed(GLFW_KEY_A)) {
            m_CameraPosition -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * adjustedSpeed;
            UpdateCamera(deltaTime);
        }

        if (InputManager::IsKeyPressed(GLFW_KEY_D)) {
            m_CameraPosition += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * adjustedSpeed;
            UpdateCamera(deltaTime);
        }

        if (InputManager::IsKeyPressed(GLFW_KEY_Q)) {
            m_CameraPosition -= m_CameraUp * adjustedSpeed;
            UpdateCamera(deltaTime);
        }

        if (InputManager::IsKeyPressed(GLFW_KEY_E)) {
            m_CameraPosition += m_CameraUp * adjustedSpeed;
            UpdateCamera(deltaTime);
        }
    }
}


bool ScenePanel::OnInputEvent(const InputEvent &event) {
    switch (event.type) {
        case InputEventType::KeyDown:
        case InputEventType::KeyHeld: {
            // Only handle R key for camera reset
            if (event.key == GLFW_KEY_R) {
                m_CameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
                m_CameraYaw = -90.0f;
                m_CameraPitch = 0.0f;
                m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
                UpdateCamera(0.0f);
                return true;
            }
            return false;
        }

        case InputEventType::MouseMove: {
            // Camera rotation with right mouse button
            if (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
                if (!m_IsRotating) {
                    m_IsRotating = true;
                    UpdateCursorForCurrentAction();
                }

                m_CameraYaw += event.mouseDelta.x * m_CameraRotationSpeed;
                m_CameraPitch -= event.mouseDelta.y * m_CameraRotationSpeed;

                // Limit pitch to avoid flipping
                m_CameraPitch = glm::clamp(m_CameraPitch, -89.0f, 89.0f);

                // Calculate front vector
                glm::vec3 front;
                front.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
                front.y = sin(glm::radians(m_CameraPitch));
                front.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
                m_CameraFront = glm::normalize(front);

                UpdateCamera(0.0f);
                return true;
            } else if (m_IsRotating) {
                // Button was released outside an event
                m_IsRotating = false;
                UpdateCursorForCurrentAction();
            }

            // Camera panning with middle mouse button
            if (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE)) {
                if (!m_IsPanning) {
                    m_IsPanning = true;
                    UpdateCursorForCurrentAction();
                }

                constexpr float panSpeed = 0.01f;

                // Calculate camera-relative right vector
                const glm::vec3 right = glm::normalize(glm::cross(m_CameraFront, m_CameraUp));

                // Calculate camera-relative up vector (perpendicular to both front and right)
                const glm::vec3 cameraRelativeUp = glm::normalize(glm::cross(right, m_CameraFront));

                // Pan horizontally using camera right vector
                m_CameraPosition -= right * event.mouseDelta.x * panSpeed;

                // Pan vertically using camera-relative up vector
                m_CameraPosition += cameraRelativeUp * event.mouseDelta.y * panSpeed;

                UpdateCamera(0.0f);
                return true;
            } else if (m_IsPanning) {
                // Button was released outside an event
                m_IsPanning = false;
                UpdateCursorForCurrentAction();
            }
            break;
        }

        case InputEventType::MouseScroll: {
            // Zoom with mouse wheel
            constexpr float zoomSpeed = 0.5f;
            m_CameraPosition += m_CameraFront * event.scrollDelta * zoomSpeed;

            UpdateCamera(0.0f);
            return true;
        }

        default: ;
    }

    return false;
}


void ScenePanel::SetupCamera() {
    if (!m_Camera) {
        m_Camera = std::make_unique<Camera>(m_CameraPosition);
    }

    // Set initial camera properties
    m_Camera->SetPosition(m_CameraPosition);

    // Update camera front based on yaw and pitch
    glm::vec3 front;
    front.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
    front.y = sin(glm::radians(m_CameraPitch));
    front.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
    m_CameraFront = glm::normalize(front);

    m_Camera->SetFront(m_CameraFront);
}


void ScenePanel::UpdateCamera(float deltaTime) {
    // Update view matrix for rendering
    m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraPosition + m_CameraFront, m_CameraUp);

    // Update global view matrix
    gViewMatrix = m_ViewMatrix;

    // Update camera object
    if (m_Camera) {
        m_Camera->SetPosition(m_CameraPosition);
        m_Camera->SetFront(m_CameraFront);
    }
}

void ScenePanel::UpdateCursorForCurrentAction() const {
    if (m_IsRotating) {
        InputManager::SetCursor(InputManager::EYE_CURSOR);
    } else if (m_IsPanning) {
        InputManager::SetCursor(InputManager::HAND_CURSOR);
    } else {
        InputManager::SetCursor(InputManager::DEFAULT_CURSOR);
    }
}

void ScenePanel::SetupFramebuffer() {
    // Create framebuffer
    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    // Create a texture attachment
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // Set texture parameters
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);

    // Create a renderbuffer for depth and stencil
    glGenRenderbuffers(1, &m_DepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ScenePanel::DrawContent() {
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

    // Update viewport state
    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();

    // Only render if we have valid size
    if (contentRegionAvail.x <= 0 || contentRegionAvail.y <= 0)
        return;

    // Update the framebuffer size if needed
    static int lastWidth = 0, lastHeight = 0;
    if (lastWidth != static_cast<int>(contentRegionAvail.x) || lastHeight != static_cast<int>(contentRegionAvail.y)) {
        ResizeFramebuffer(static_cast<int>(contentRegionAvail.x), static_cast<int>(contentRegionAvail.y));
        lastWidth = static_cast<int>(contentRegionAvail.x);
        lastHeight = static_cast<int>(contentRegionAvail.y);
    }

    // Render scene to framebuffer
    if (m_FramebufferID > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
        glViewport(0, 0, static_cast<int>(contentRegionAvail.x), static_cast<int>(contentRegionAvail.y));
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_Scene) {
            // Always use our internal camera view matrix
            gViewMatrix = glm::lookAt(m_CameraPosition,
                                      m_CameraPosition + m_CameraFront,
                                      m_CameraUp);

            m_Scene->DrawAll();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ImGui::Image(reinterpret_cast<ImTextureID>(reinterpret_cast<void *>(static_cast<intptr_t>(m_TextureID))),
                     contentRegionAvail, ImVec2(0, 1), ImVec2(1, 0));
    }

    // Debug info
    ImGui::SetCursorPos(ImVec2(10, 10));
    ImGui::Text("Camera: %.1f, %.1f, %.1f", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
}

void ScenePanel::ResizeFramebuffer(const int width, const int height) {
    // Update texture size
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    // Update renderbuffer size
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    // Update projection matrix for aspect ratio
    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
}
