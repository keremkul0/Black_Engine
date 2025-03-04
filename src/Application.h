#pragma once

#include "Core/WindowManager/WindowManager.h"
#include "Core/Camera/Camera.h"
#include "Engine/Scene/Scene.h"
#include <memory>

class Application {
public:
    Application();
    ~Application();

    int Run();

    // Getter for camera (used by callbacks)
    Camera* GetCamera() { return m_Camera.get(); }

private:
    // Initialize the scene with objects
    void SetupScene();

    // Process input based on delta time
    void ProcessInput(float deltaTime);

    // Window and core systems
    std::unique_ptr<WindowManager> m_WindowManager;
    std::unique_ptr<Camera> m_Camera;

    // Scene management
    std::unique_ptr<Scene> m_Scene;

    // Projection matrix
    glm::mat4 m_ProjectionMatrix;
};