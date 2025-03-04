#include "Application.h"
#include "Core/InputManager/InputManager.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Component/MeshRendererComponent.h"
#include "Engine/Render/Primitives.h"
#include "Engine/Render/Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

// External references to maintain compatibility with existing code
glm::mat4 gViewMatrix(1.0f);
glm::mat4 gProjectionMatrix(1.0f);

// Scroll callback function
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Get the Application instance from window user pointer
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->GetCamera()->ProcessMouseScroll(yoffset);
    }
}

Application::Application()
    : m_WindowManager(std::make_unique<WindowManager>()),
      m_Camera(std::make_unique<Camera>()),
      m_Scene(std::make_unique<Scene>()) {
}

Application::~Application() {
    // Input manager cleanup
    InputManager::Cleanup();
}

int Application::Run() {
    // Initialize window
    if (!m_WindowManager->Initialize(1280, 720, "Black Engine")) {
        return -1;
    }

    // Initialize input manager
    InputManager::Initialize(m_WindowManager->GetWindow());

    // Set up window user pointer for callbacks
    GLFWwindow* window = m_WindowManager->GetWindow();
    glfwSetWindowUserPointer(window, this);
    m_WindowManager->SetScrollCallback(scroll_callback);

    // Set up projection matrix
    float aspectRatio = static_cast<float>(m_WindowManager->GetWidth()) /
                        static_cast<float>(m_WindowManager->GetHeight());
    m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // Update global projection matrix for compatibility
    gProjectionMatrix = m_ProjectionMatrix;

    // Set up the scene
    SetupScene();

    // Main loop variables
    float lastTime = static_cast<float>(glfwGetTime());

    // Main loop
    while (!m_WindowManager->ShouldClose()) {
        // Calculate delta time
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Process input
        m_WindowManager->PollEvents();
        ProcessInput(deltaTime);

        // Update view matrix
        gViewMatrix = m_Camera->GetViewMatrix();

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update and draw the scene
        m_Scene->UpdateAll(deltaTime);
        m_Scene->DrawAll();

        m_WindowManager->SwapBuffers();
    }

    return 0;
}

void Application::SetupScene() {
    // Add a cube
    const std::string shaderPath = "../src/shaders/";
    const auto defaultShader = std::make_shared<Shader>(
        (shaderPath + "simple.vert").c_str(),
        (shaderPath + "simple.frag").c_str()
    );

    auto cubeObj = m_Scene->CreateGameObject("MyCube");
    auto transform = cubeObj->AddComponent<TransformComponent>();
    transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

    auto renderer = cubeObj->AddComponent<MeshRendererComponent>();
    renderer->mesh = Primitives::CreateCube();
    renderer->shader = defaultShader;
}

void Application::ProcessInput(float deltaTime) {
    // Keyboard movement
    if (InputManager::IsKeyPressed(GLFW_KEY_W))
        m_Camera->ProcessKeyboard(0 /* FORWARD */, deltaTime);
    if (InputManager::IsKeyPressed(GLFW_KEY_S))
        m_Camera->ProcessKeyboard(1 /* BACKWARD */, deltaTime);
    if (InputManager::IsKeyPressed(GLFW_KEY_A))
        m_Camera->ProcessKeyboard(2 /* LEFT */, deltaTime);
    if (InputManager::IsKeyPressed(GLFW_KEY_D))
        m_Camera->ProcessKeyboard(3 /* RIGHT */, deltaTime);

    // Mouse handling
    double mouseX, mouseY;
    static double lastX = 0.0, lastY = 0.0;
    static bool firstMouse = true;

    InputManager::GetMousePosition(mouseX, mouseY);

    if (firstMouse) {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
    }

    const double deltaX = mouseX - lastX;
    const double deltaY = mouseY - lastY;
    lastX = mouseX;
    lastY = mouseY;

    // Handle mouse buttons
    const bool rightPressed = InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
    const bool middlePressed = InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE);

    // Right-click: rotate camera
    if (rightPressed) {
        m_Camera->ProcessMouseMovement(static_cast<float>(deltaX), static_cast<float>(deltaY));
        InputManager::SetCursor(InputManager::EYE_CURSOR);
    }
    // Middle-click: pan camera
    else if (middlePressed) {
        float panSpeed = 0.01f;

        // Calculate the right and up vectors for proper screen-space panning
        glm::vec3 right = m_Camera->GetFront();
        right = glm::normalize(glm::cross(right, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::normalize(glm::cross(right, m_Camera->GetFront()));

        // Create pan vector based on mouse movement
        glm::vec3 panDelta = -right * static_cast<float>(deltaX) * panSpeed +
                              up * static_cast<float>(deltaY) * panSpeed;

        // Add a custom Pan method to directly move the camera
        m_Camera->Pan(panDelta);

        InputManager::SetCursor(InputManager::HAND_CURSOR);
    }
    else {
        InputManager::SetCursor(InputManager::DEFAULT_CURSOR);
    }
}