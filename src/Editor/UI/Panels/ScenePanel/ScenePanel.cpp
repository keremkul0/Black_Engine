#include <glad/glad.h>
#include "ScenePanel.h"

#include <iostream>

#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Camera/Camera.h"
#include "Core/InputManager/InputManager.h"
#include "Core/InputManager/InputEvent.h"

extern glm::mat4 gViewMatrix;

ScenePanel::ScenePanel(const std::string &title)
    : Panel(title), m_CurrentCursor(InputManager::DEFAULT_CURSOR)
{
    m_Camera = std::make_unique<Camera>(m_CameraPosition);
    SetupFramebuffer();
    SetupCamera();
}

ScenePanel::~ScenePanel()
{
    CleanupResources();
}

void ScenePanel::SetScene(const std::shared_ptr<Scene> &scene)
{
    m_Scene = scene;
}

void ScenePanel::OnUpdate(const float deltaTime)
{
    // Eğer panel aktif değilse veya açık değilse güncelleme yapılmasın.
    if (!m_IsActive || !m_IsOpen)
        return;

    // Panelin focus/hover durumunu, Panel::Render() içinde ImGui üzerinden güncellenen bayraklardan alıyoruz.

    if (m_IsFocused || m_IsHovered)
    {
        // Mouse buton durumlarını InputManager üzerinden oku.
        const bool rightMouseDown = InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        const bool middleMouseDown = InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE);

        // Kamera kontrol modlarını belirle.
        m_IsRotating = rightMouseDown;
        m_IsPanning  = middleMouseDown;

        // Kamera moduna göre cursor'u güncelle (durum değişiminde).
        UpdateCursor();

        // Yalnızca rotasyon modunda WASD hareketleri ile kamera hareketini işle.
        if (m_IsRotating)
        {
            const float speedMultiplier = (InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ||
                                             InputManager::IsKeyPressed(GLFW_KEY_RIGHT_SHIFT))
                                                ? 3.0f : 1.0f;
            const float adjustedSpeed = m_CameraSpeed * speedMultiplier * deltaTime;

            if (InputManager::IsKeyPressed(GLFW_KEY_W))
                m_CameraPosition += adjustedSpeed * m_CameraFront;
            if (InputManager::IsKeyPressed(GLFW_KEY_S))
                m_CameraPosition -= adjustedSpeed * m_CameraFront;
            if (InputManager::IsKeyPressed(GLFW_KEY_A))
                m_CameraPosition -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * adjustedSpeed;
            if (InputManager::IsKeyPressed(GLFW_KEY_D))
                m_CameraPosition += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * adjustedSpeed;
            if (InputManager::IsKeyPressed(GLFW_KEY_Q))
                m_CameraPosition -= m_CameraUp * adjustedSpeed;
            if (InputManager::IsKeyPressed(GLFW_KEY_E))
                m_CameraPosition += m_CameraUp * adjustedSpeed;

            UpdateCamera(deltaTime);
        }
    }
    else
    {
        // Panel focus veya hover değilse, kamera kontrol modlarını sıfırla ve cursor'u default yap.
        m_IsRotating = false;
        m_IsPanning = false;
        if (m_CurrentCursor != InputManager::DEFAULT_CURSOR)
        {
            m_CurrentCursor = InputManager::DEFAULT_CURSOR;
            InputManager::SetCursor(m_CurrentCursor);
        }
    }
}

bool ScenePanel::OnInputEvent(const InputEvent &event)
{
    // Input olaylarını yalnızca panel aktif ve focus/hover durumdaysa işleyelim.
    if (!m_IsActive || !m_IsOpen || (!m_IsFocused && !m_IsHovered))
        return false;

    switch (event.type)
    {
        case InputEventType::KeyDown:
        case InputEventType::KeyHeld:
        {
            // R tuşu ile kamera resetleme
            if (event.key == GLFW_KEY_R)
            {
                m_CameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
                m_CameraYaw = -90.0f;
                m_CameraPitch = 0.0f;
                m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
                UpdateCamera(0.0f);
                return true;
            }
            return false;
        }
        case InputEventType::MouseMove:
        {
            if (m_IsRotating)
            {
                m_CameraYaw += event.mouseDelta.x * m_CameraRotationSpeed;
                m_CameraPitch -= event.mouseDelta.y * m_CameraRotationSpeed;
                m_CameraPitch = glm::clamp(m_CameraPitch, -89.0f, 89.0f);

                glm::vec3 front;
                front.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
                front.y = sin(glm::radians(m_CameraPitch));
                front.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
                m_CameraFront = glm::normalize(front);

                UpdateCamera(0.0f);
                return true;
            }
            if (m_IsPanning)
            {
                constexpr float panSpeed = 0.01f;
                const glm::vec3 right = glm::normalize(glm::cross(m_CameraFront, m_CameraUp));
                const glm::vec3 cameraRelativeUp = glm::normalize(glm::cross(right, m_CameraFront));

                m_CameraPosition -= right * event.mouseDelta.x * panSpeed;
                m_CameraPosition += cameraRelativeUp * event.mouseDelta.y * panSpeed;

                UpdateCamera(0.0f);
                return true;
            }
            break;
        }
        case InputEventType::MouseScroll:
        {
            constexpr float zoomSpeed = 0.5f;
            m_CameraPosition += m_CameraFront * event.scrollDelta * zoomSpeed;
            UpdateCamera(0.0f);
            return true;
        }
        default:
            break;
    }
    return false;
}

void ScenePanel::UpdateCursor() {
    InputManager::CursorType newCursor;

    if (m_IsRotating) {
        newCursor = InputManager::EYE_CURSOR;
    } else if (m_IsPanning) {
        newCursor = InputManager::HAND_CURSOR;
    } else {
        newCursor = InputManager::DEFAULT_CURSOR;
    }

    // Only set cursor if it needs to change
    if (m_CurrentCursor != newCursor) {
        m_CurrentCursor = newCursor;
        InputManager::SetCursor(m_CurrentCursor);
    }
}

void ScenePanel::SetupCamera()
{
    if (!m_Camera)
        m_Camera = std::make_unique<Camera>(m_CameraPosition);

    m_Camera->SetPosition(m_CameraPosition);

    glm::vec3 front;
    front.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
    front.y = sin(glm::radians(m_CameraPitch));
    front.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
    m_CameraFront = glm::normalize(front);

    m_Camera->SetFront(m_CameraFront);
}

void ScenePanel::UpdateCamera(float deltaTime)
{
    m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraPosition + m_CameraFront, m_CameraUp);
    gViewMatrix = m_ViewMatrix;
    if (m_Camera)
    {
        m_Camera->SetPosition(m_CameraPosition);
        m_Camera->SetFront(m_CameraFront);
    }
}

void ScenePanel::SetupFramebuffer()
{
    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);

    glGenRenderbuffers(1, &m_DepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        // Framebuffer is not complete, but we're removing the console output
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ScenePanel::DrawContent()
{
    const ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
    if (contentRegionAvail.x <= 0 || contentRegionAvail.y <= 0)
        return;

    static int lastWidth = 0, lastHeight = 0;
    if (lastWidth != static_cast<int>(contentRegionAvail.x) || lastHeight != static_cast<int>(contentRegionAvail.y))
    {
        ResizeFramebuffer(static_cast<int>(contentRegionAvail.x), static_cast<int>(contentRegionAvail.y));
        lastWidth = static_cast<int>(contentRegionAvail.x);
        lastHeight = static_cast<int>(contentRegionAvail.y);
    }

    if (m_FramebufferID > 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
        glViewport(0, 0, static_cast<int>(contentRegionAvail.x), static_cast<int>(contentRegionAvail.y));
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_Scene)
        {
            gViewMatrix = glm::lookAt(m_CameraPosition, m_CameraPosition + m_CameraFront, m_CameraUp);
            m_Scene->DrawAll();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ImGui::Image(reinterpret_cast<ImTextureID>(reinterpret_cast<void *>(static_cast<intptr_t>(m_TextureID))),
                                  contentRegionAvail, ImVec2(0, 1), ImVec2(1, 0));
    }

    // Opsiyonel: Debug bilgileri çizilebilir.
    ImGui::SetCursorPos(ImVec2(10, 10));
    ImGui::Text("Camera: %.1f, %.1f, %.1f", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
}

void ScenePanel::ResizeFramebuffer(const int width, const int height)
{
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
}

void ScenePanel::CleanupResources()
{
    if (m_FramebufferID != 0)
    {
        glDeleteFramebuffers(1, &m_FramebufferID);
        m_FramebufferID = 0;
    }
    if (m_TextureID != 0)
    {
        glDeleteTextures(1, &m_TextureID);
        m_TextureID = 0;
    }
    if (m_DepthRenderBuffer != 0)
    {
        glDeleteRenderbuffers(1, &m_DepthRenderBuffer);
        m_DepthRenderBuffer = 0;
    }
}
