#include "Application.h"
#include "Core/InputManager/InputManager.h"
#include "Engine/Component/TransformComponent.h"
#include "Core/ImGui/ImGuiLayer.h"
#include "Editor/UI/EditorLayout.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

// ImGui için ana pencere referansı - ImGuiLayer.cpp için gerekli
GLFWwindow *g_Window = nullptr;

// External references to maintain compatibility with existing code
glm::mat4 gViewMatrix(1.0f);
glm::mat4 gProjectionMatrix(1.0f);

// Scroll callback function
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    // Only store the scroll offset, don't directly control camera
    InputManager::SetScrollOffset(static_cast<float>(yoffset));
}

Application::Application()
    : m_WindowManager(std::make_unique<WindowManager>()),
      m_Camera(std::make_unique<Camera>()),
      m_EditorLayout(nullptr),
      m_InputSystem(std::make_unique<InputSystem>()),
      m_Scene(std::make_shared<Scene>()) {
}

Application::~Application() {
    // Input manager cleanup
    InputManager::Cleanup();

    // ImGui temizleme işlemi
    ImGuiLayer::Shutdown();
}

bool Application::Initialize() {
    // Initialize window
    if (!m_WindowManager->Initialize(1280, 720, "Black Engine")) {
        return false;
    }

    // Global window reference for ImGui
    g_Window = m_WindowManager->GetWindow();

    // Initialize input manager
    InputManager::Initialize(m_WindowManager->GetWindow());
    m_InputSystem->Initialize(m_WindowManager->GetWindow());
    // Set up window callbacks
    GLFWwindow *window = m_WindowManager->GetWindow();
    glfwSetWindowUserPointer(window, this);
    m_WindowManager->SetScrollCallback(scroll_callback);

    // Set up projection matrix
    const float aspectRatio = static_cast<float>(m_WindowManager->GetWidth()) /
                              static_cast<float>(m_WindowManager->GetHeight());
    m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    gProjectionMatrix = m_ProjectionMatrix;

    // Initialize ImGui
    ImGuiLayer::Init();

    // Initialize editor UI
    m_EditorLayout = std::make_unique<EditorLayout>();
    m_EditorLayout->SetupDefaultLayout(m_Scene);

    // Register editor layout with input system
    m_InputSystem->RegisterUIEventHandler(m_EditorLayout.get());

    // Load default scene
    m_Scene->LoadDefaultScene();
    // Set up the scene
    return true;
}

int Application::Run() {
    // Main loop variables
    auto lastTime = static_cast<float>(glfwGetTime());

    // Main loop
    while (!m_WindowManager->ShouldClose()) {
        const auto currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Process input
        m_WindowManager->PollEvents();
        m_InputSystem->ProcessInput(deltaTime);

        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update and draw the scene
        m_Scene->UpdateAll(deltaTime);
        m_Scene->DrawAll();

        // Render UI
        ImGuiLayer::Begin();
        if (m_EditorLayout) {
            m_EditorLayout->UpdateAllPanels(deltaTime); // Add this line
            m_EditorLayout->RenderLayout();
        }
        ImGuiLayer::End();

        m_WindowManager->SwapBuffers();
    }

    return 0;
}
