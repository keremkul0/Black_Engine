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

    [[nodiscard]] GLFWwindow* GetWindow() const { return m_Window; }
    [[nodiscard]] bool ShouldClose() const;
    void SwapBuffers() const;

    static void PollEvents();

    void SetScrollCallback(GLFWscrollfun callback) const;

    [[nodiscard]] int GetWidth() const { return m_Width; }
    [[nodiscard]] int GetHeight() const { return m_Height; }

private:
    GLFWwindow* m_Window;
    int m_Width;
    int m_Height;
};