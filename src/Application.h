#pragma once

#include "Core/WindowManager/WindowManager.h"
#include "Core/Camera/Camera.h"
#include "Engine/Scene/Scene.h"
#include "Editor/UI/Layout/EditorLayout.h"
#include "Core/InputSystem/InputSystem.h"
#include <memory>

class Application {
public:
    Application();
    ~Application();

    bool Initialize();
    [[nodiscard]] [[nodiscard]] int Run() const;

private:
    // Window and core systems
    std::unique_ptr<WindowManager> m_WindowManager;
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<EditorLayout> m_EditorLayout;
    std::unique_ptr<InputSystem> m_InputSystem;

    // Projection matrix
    glm::mat4 m_ProjectionMatrix{};
};