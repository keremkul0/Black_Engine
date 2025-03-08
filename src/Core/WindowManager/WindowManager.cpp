#include "WindowManager.h"
#include <iostream>

WindowManager::WindowManager() : m_Window(nullptr), m_Width(0), m_Height(0) {
}

WindowManager::~WindowManager() {
    Shutdown();
}

bool WindowManager::Initialize(const int width, const int height, const std::string& title) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    m_Width = width;
    m_Height = height;

    // Create the window
    m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_Window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Make the window's context current
    glfwMakeContextCurrent(m_Window);

    // Initialize GLAD
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    return true;
}

void WindowManager::Shutdown() {
    if (m_Window) {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
    glfwTerminate();
}

bool WindowManager::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void WindowManager::SwapBuffers() const {
    glfwSwapBuffers(m_Window);
}

void WindowManager::PollEvents() {
    glfwPollEvents();
}

void WindowManager::SetScrollCallback(const GLFWscrollfun callback) const {
    glfwSetScrollCallback(m_Window, callback);
}
