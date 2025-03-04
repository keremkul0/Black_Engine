#include "InputManager.h"

GLFWwindow* InputManager::s_Window = nullptr;
GLFWcursor* InputManager::s_HandCursor = nullptr;
GLFWcursor* InputManager::s_EyeCursor = nullptr;
GLFWcursor* InputManager::s_DefaultCursor = nullptr;

void InputManager::Initialize(GLFWwindow* window) {
    s_Window = window;
    CreateCursors();
}

void InputManager::Cleanup() {
    DestroyCursors();
}

bool InputManager::IsKeyPressed(int key) {
    return glfwGetKey(s_Window, key) == GLFW_PRESS;
}

void InputManager::GetMousePosition(double& xpos, double& ypos) {
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