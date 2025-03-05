#pragma once
#include <imgui.h>
#include "Panel.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Entity/GameObject.h"
#include "Core/Camera/Camera.h"
#include <memory>
#include <glm/vec3.hpp>

class ScenePanel : public Panel {
public:
    ScenePanel(const std::string &title);

    void SetScene(std::shared_ptr<Scene> scene) {
        m_Scene = scene;
    }


protected:
    void DrawContent() override;



private:
    std::shared_ptr<Scene> m_Scene;
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

    // Mouse state for camera control
    bool m_ViewportHovered = false;
    bool m_ViewportFocused = false;
    ImVec2 m_LastMousePos = {0.0f, 0.0f};

    // Scene view specific members
    unsigned int m_FramebufferID = 0;
    unsigned int m_TextureID = 0;

    void SetupFramebuffer();

    void SetupCamera();

    void HandleCameraInput();

    void UpdateCamera(float deltaTime);
};
