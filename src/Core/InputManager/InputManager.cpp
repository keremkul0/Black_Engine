#include "InputManager.h"

GLFWwindow *InputManager::s_Window = nullptr;
GLFWcursor *InputManager::s_HandCursor = nullptr;
GLFWcursor *InputManager::s_EyeCursor = nullptr;
GLFWcursor *InputManager::s_DefaultCursor = nullptr;
float InputManager::s_ScrollOffset = 0.0f;
bool InputManager::s_MouseButtonsCurrentState[3] = {false, false, false};
bool InputManager::s_MouseButtonsPreviousState[3] = {false, false, false};

bool InputManager::IsMouseButtonJustPressed(const int button) {
    return s_MouseButtonsCurrentState[button] && !s_MouseButtonsPreviousState[button];
}

bool InputManager::IsMouseButtonJustReleased(const int button) {
    return !s_MouseButtonsCurrentState[button] && s_MouseButtonsPreviousState[button];
}

float InputManager::GetScrollOffset() {
    const float offset = s_ScrollOffset;
    s_ScrollOffset = 0.0f;
    return offset;
}

void InputManager::SetScrollOffset(const float offset) {
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
    for (int i = 0; i < 3; i++) {
        s_MouseButtonsPreviousState[i] = s_MouseButtonsCurrentState[i];
        s_MouseButtonsCurrentState[i] = glfwGetMouseButton(s_Window, i) == GLFW_PRESS;
    }
}

bool InputManager::IsKeyPressed(const int key) {
    return glfwGetKey(s_Window, key) == GLFW_PRESS;
}

void InputManager::GetMousePosition(double &xpos, double &ypos) {
    glfwGetCursorPos(s_Window, &xpos, &ypos);
}

bool InputManager::IsMouseButtonPressed(const int button) {
    return glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
}

void InputManager::SetCursor(const CursorType cursorType) {
    // Only change cursor if it's different from current
    if (s_CurrentCursorType != cursorType) {
        s_CurrentCursorType = cursorType;

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

//TODO: Implement additional cursor types if needed
