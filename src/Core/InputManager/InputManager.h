#pragma once
#include <GLFW/glfw3.h>
#include <memory>

class InputManager {
public:
    enum CursorType {
        DEFAULT_CURSOR,
        HAND_CURSOR,
        EYE_CURSOR
    };

    static void Initialize(GLFWwindow* window);
    static void Cleanup();

    static bool IsKeyPressed(int key);
    static void GetMousePosition(double& xpos, double& ypos);
    static bool IsMouseButtonPressed(int button);

    // Cursor management
    static void SetCursor(CursorType cursorType);

private:
    static void CreateCursors();
    static void DestroyCursors();

    static GLFWwindow* s_Window;
    static GLFWcursor* s_HandCursor;
    static GLFWcursor* s_EyeCursor;
    static GLFWcursor* s_DefaultCursor;
};