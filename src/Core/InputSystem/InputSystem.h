#pragma once
#include "Editor/UI/EditorLayout.h"
#include <GLFW/glfw3.h>

class InputSystem {
public:
    InputSystem();

    ~InputSystem() = default;

    // Initialize the input system
    void Initialize(GLFWwindow *window);

    // Register input event listeners
    void RegisterUIEventHandler(EditorLayout *layout);

    // Process input events for current frame
    void ProcessInput(float deltaTime);

    // Helper to check if ImGui wants to capture input
    bool ShouldImGuiProcessEvent(const InputEvent &event) const;

    // Scroll callback to be set as GLFW callback
    static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

private:
    // Stores the last known mouse position
    glm::vec2 m_LastMousePos{0.0f, 0.0f};

    // UI layout to receive input events
    EditorLayout *m_EditorLayout = nullptr;
};
