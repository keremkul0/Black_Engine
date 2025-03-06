#include "InputManager.h"

GLFWwindow *InputManager::s_Window = nullptr;
GLFWcursor *InputManager::s_HandCursor = nullptr;
GLFWcursor *InputManager::s_EyeCursor = nullptr;
GLFWcursor *InputManager::s_DefaultCursor = nullptr;
float InputManager::s_ScrollOffset = 0.0f;
bool InputManager::s_MouseButtonsCurrentState[3] = {false, false, false};
bool InputManager::s_MouseButtonsPreviousState[3] = {false, false, false};

// Implement the new methods
bool InputManager::IsMouseButtonJustPressed(int button) {
    return s_MouseButtonsCurrentState[button] && !s_MouseButtonsPreviousState[button];
}

bool InputManager::IsMouseButtonJustReleased(int button) {
    return !s_MouseButtonsCurrentState[button] && s_MouseButtonsPreviousState[button];
}

float InputManager::GetScrollOffset() {
    float offset = s_ScrollOffset;
    s_ScrollOffset = 0.0f; // Reset after reading
    return offset;
}

void InputManager::SetScrollOffset(float offset) {
    s_ScrollOffset = offset;
}

void InputManager::ResetScrollOffset() {
    s_ScrollOffset = 0.0f;
}

void InputManager::Initialize(GLFWwindow *window) {
    s_Window = window;
    CreateCursors();
}

void InputManager::Cleanup() {
    DestroyCursors();
}

void InputManager::Update() {
    // Store previous frame button states
    for (int i = 0; i < 3; i++) {
        s_MouseButtonsPreviousState[i] = s_MouseButtonsCurrentState[i];
        s_MouseButtonsCurrentState[i] = glfwGetMouseButton(s_Window, i) == GLFW_PRESS;
    }
}

bool InputManager::IsKeyPressed(int key) {
    return glfwGetKey(s_Window, key) == GLFW_PRESS;
}

void InputManager::GetMousePosition(double &xpos, double &ypos) {
    glfwGetCursorPos(s_Window, &xpos, &ypos);
}

bool InputManager::IsMouseButtonPressed(int button) {
    return glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
}

void InputManager::SetCursor(CursorType cursorType) {
    switch (cursorType) {
        case DEFAULT_CURSOR:
            glfwSetCursor(s_Window, s_DefaultCursor);
            break;
        case HAND_CURSOR:
            glfwSetCursor(s_Window, s_HandCursor);
            break;
        case EYE_CURSOR:
            glfwSetCursor(s_Window, s_EyeCursor);
            break;
    }
}

void InputManager::CreateCursors() {
    s_HandCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    s_DefaultCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    s_EyeCursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
}

void InputManager::DestroyCursors() {
    glfwDestroyCursor(s_HandCursor);
    glfwDestroyCursor(s_EyeCursor);
    glfwDestroyCursor(s_DefaultCursor);
}
