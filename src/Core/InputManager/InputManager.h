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
    static void Update();

    static bool IsKeyPressed(int key);
    static void GetMousePosition(double& xpos, double& ypos);
    static bool IsMouseButtonPressed(int button);
    static bool IsMouseButtonJustPressed(int button);
    static bool IsMouseButtonJustReleased(int button);
    static float GetScrollOffset();
    static void SetScrollOffset(float offset);
    static void ResetScrollOffset();
    // Cursor management
    static void SetCursor(CursorType cursorType);

private:
    static bool s_MouseButtonsCurrentState[3];
    static bool s_MouseButtonsPreviousState[3];
    static void CreateCursors();
    static void DestroyCursors();
    static float s_ScrollOffset;
    static GLFWwindow* s_Window;
    static GLFWcursor* s_HandCursor;
    static GLFWcursor* s_EyeCursor;
    static GLFWcursor* s_DefaultCursor;
};