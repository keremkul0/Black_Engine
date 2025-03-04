#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    bool Initialize(int width, int height, const std::string& title);
    void Shutdown();

    GLFWwindow* GetWindow() const { return m_Window; }
    bool ShouldClose() const;
    void SwapBuffers();
    void PollEvents();

    void SetScrollCallback(GLFWscrollfun callback);

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    GLFWwindow* m_Window;
    int m_Width;
    int m_Height;
};