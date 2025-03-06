#include "InputSystem.h"
#include "imgui.h"
#include "Core/InputManager/InputManager.h"

InputSystem::InputSystem() : m_LastMousePos(0.0f, 0.0f), m_EditorLayout(nullptr) {}

void InputSystem::Initialize(GLFWwindow* window) {
    // Initialize the input manager
    InputManager::Initialize(window);

    // Set up window callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, ScrollCallback);

    // Initialize last mouse position
    double mouseX, mouseY;
    InputManager::GetMousePosition(mouseX, mouseY);
    m_LastMousePos = glm::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
}

void InputSystem::RegisterUIEventHandler(EditorLayout* layout) {
    m_EditorLayout = layout;
}

void InputSystem::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    InputManager::SetScrollOffset(static_cast<float>(yoffset));
}

bool InputSystem::ShouldImGuiProcessEvent(const InputEvent& event) const {
    ImGuiIO& io = ImGui::GetIO();

    // For mouse events, check if ImGui wants the mouse
    if (event.type == InputEventType::MouseDown ||
        event.type == InputEventType::MouseUp ||
        event.type == InputEventType::MouseMove ||
        event.type == InputEventType::MouseScroll) {
        return io.WantCaptureMouse;
    }

    // For keyboard events, check if ImGui wants the keyboard
    if (event.type == InputEventType::KeyDown ||
        event.type == InputEventType::KeyUp ||
        event.type == InputEventType::KeyHeld) {
        return io.WantCaptureKeyboard;
    }

    return false;
}

void InputSystem::ProcessInput(float deltaTime) {
    // Update input manager state
    InputManager::Update();

    // No UI layout registered to receive events
    if (!m_EditorLayout) return;

    // Get mouse position
    double mouseX, mouseY;
    InputManager::GetMousePosition(mouseX, mouseY);
    glm::vec2 currentMousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
    glm::vec2 mouseDelta = currentMousePos - m_LastMousePos;

    // Mouse move event - always create it
    if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {
        InputEvent mouseMoveEvent;
        mouseMoveEvent.type = InputEventType::MouseMove;
        mouseMoveEvent.mousePos = currentMousePos;
        mouseMoveEvent.mouseDelta = mouseDelta;

        // Always pass mouse move events to the layout - it will determine which panel receives it
        m_EditorLayout->ProcessInput(mouseMoveEvent);
    }

    // Mouse scroll event
    float scrollOffset = InputManager::GetScrollOffset();
    if (scrollOffset != 0.0f) {
        InputEvent scrollEvent;
        scrollEvent.type = InputEventType::MouseScroll;
        scrollEvent.scrollDelta = scrollOffset;
        m_EditorLayout->ProcessInput(scrollEvent);
    }

    // Mouse button events
    for (int button = 0; button < 3; button++) {
        if (InputManager::IsMouseButtonJustPressed(button)) {
            InputEvent mouseDownEvent;
            mouseDownEvent.type = InputEventType::MouseDown;
            mouseDownEvent.button = button;
            mouseDownEvent.mousePos = currentMousePos;
            m_EditorLayout->ProcessInput(mouseDownEvent);
        }
        if (InputManager::IsMouseButtonJustReleased(button)) {
            InputEvent mouseUpEvent;
            mouseUpEvent.type = InputEventType::MouseUp;
            mouseUpEvent.button = button;
            mouseUpEvent.mousePos = currentMousePos;
            m_EditorLayout->ProcessInput(mouseUpEvent);
        }
    }

    // Keyboard events - create individual events for each key
    static const int cameraKeys[] = {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_R};
    for (int key : cameraKeys) {
        if (InputManager::IsKeyPressed(key)) {
            InputEvent keyEvent;
            keyEvent.type = InputEventType::KeyHeld;
            keyEvent.key = key;
            m_EditorLayout->ProcessInput(keyEvent);
        }
    }

    // Update last mouse position
    m_LastMousePos = currentMousePos;
}