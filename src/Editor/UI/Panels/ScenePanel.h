#pragma once
#include <imgui.h>
#include "Panel.h"
#include "Engine/Scene/Scene.h"
#include "Core/Camera/Camera.h"
#include <memory>
#include <glm/vec3.hpp>

class ScenePanel : public Panel {
public:
    ScenePanel(const std::string &title);
    ~ScenePanel();

    void SetScene(const std::shared_ptr<Scene> &scene);
    bool OnInputEvent(const InputEvent &event) override;
    void OnUpdate(float deltaTime) override;

protected:
    void DrawContent() override;

private:
    // Camera properties
    std::unique_ptr<Camera> m_Camera;
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

    // Camera control properties
    float m_CameraSpeed = 5.0f;
    float m_CameraRotationSpeed = 0.1f;
    glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    float m_CameraYaw = -90.0f; // Look at -Z initially
    float m_CameraPitch = 0.0f;
    glm::vec3 m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Scene properties
    std::shared_ptr<Scene> m_Scene;

    // Viewport properties
    bool m_ViewportHovered = false;
    bool m_ViewportFocused = false;
    ImVec2 m_LastMousePos = {0.0f, 0.0f};

    // Framebuffer properties
    unsigned int m_FramebufferID = 0;
    unsigned int m_TextureID = 0;
    unsigned int m_DepthRenderBuffer = 0;

    // Helper methods
    void SetupFramebuffer();
    void SetupCamera();
    void UpdateCamera(float deltaTime);
    void ResizeFramebuffer(int width, int height);
    void CleanupResources();
};