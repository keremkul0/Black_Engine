#pragma once

#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "Editor/UI/Panels/Panel.h"
#include "Engine/Scene/Scene.h"
#include "Core/Camera/Camera.h"
#include "Core/InputManager/InputEvent.h"
#include "Core/InputManager/InputManager.h"

class ScenePanel final : public Panel {
public:
    explicit ScenePanel(const std::string &title);
    ~ScenePanel() override;

    void SetScene(const std::shared_ptr<Scene> &scene);

    bool OnInputEvent(const InputEvent &event) override;
    void OnUpdate(float deltaTime) override;

protected:
    void DrawContent() override;

private:
    // Kamera özellikleri
    std::unique_ptr<Camera> m_Camera;
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

    // Kamera kontrol ayarları
    float m_CameraSpeed = 5.0f;
    float m_CameraRotationSpeed = 0.1f;
    glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    float m_CameraYaw = -90.0f;
    float m_CameraPitch = 0.0f;
    glm::vec3 m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Sahne referansı
    std::shared_ptr<Scene> m_Scene;

    // Kamera manipülasyon durumları
    bool m_IsRotating = false;
    bool m_IsPanning = false;

    // Framebuffer özellikleri
    unsigned int m_FramebufferID = 0;
    unsigned int m_TextureID = 0;
    unsigned int m_DepthRenderBuffer = 0;

    // Cursor durumu
    InputManager::CursorType m_CurrentCursor = InputManager::DEFAULT_CURSOR;

    // Yardımcı fonksiyonlar
    void SetupFramebuffer();
    void ResizeFramebuffer(int width, int height);
    void CleanupResources();
    void SetupCamera();
    void UpdateCamera(float deltaTime);
    void UpdateCursor();
};
