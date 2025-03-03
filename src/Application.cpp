#include "Application.h"

    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
    #include <iostream>
    #include <cmath>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>

    #include "Engine/Scene/Scene.h"
    #include "Engine/Component/TransformComponent.h"
    #include "Engine/Component/MeshRendererComponent.h"
    #include "Engine/Render/Primitives.h"
    #include "Engine/Render/Shader.h"

    // Global matrices
    glm::mat4 gViewMatrix = glm::mat4(1.f);
    glm::mat4 gProjectionMatrix = glm::mat4(1.f);


    GLFWcursor* handCursor = nullptr;
    GLFWcursor* eyeCursor = nullptr;
    GLFWcursor* defaultCursor = nullptr;
    // Camera state
    struct CameraState {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        float yaw = -90.0f;
        float pitch = 0.0f;
        float speed = 2.5f;

        // Mouse state
        double lastX = 0.0;
        double lastY = 0.0;
        bool firstMouse = true;
    } camera;

    // Forward declarations
    static void initializeCamera(int width, int height);
    static void processKeyboardInput(GLFWwindow* window, float deltaTime);
    static void updateCameraVectors();

// Initialize custom cursors
    void initializeCursors() {
    // Create standard cursors
    handCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    defaultCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

    // For eye cursor, you might need a custom image
    // This example uses the crosshair as a placeholder
    eyeCursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);

    // Other standard options:
    // GLFW_IBEAM_CURSOR (text input)
    // GLFW_CROSSHAIR_CURSOR
    // GLFW_VRESIZE_CURSOR, GLFW_HRESIZE_CURSOR
}
    // Scroll callback for zooming
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        // Move camera along view direction when scrolling
        float zoomSpeed = 0.5f;
        camera.position += camera.front * (float)yoffset * zoomSpeed;
    }

    int Application::Run()
    {
        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "GLFW initialization failed!\n";
            return -1;
        }

        // Create window
        int width = 1280, height = 720;
        GLFWwindow* window = glfwCreateWindow(width, height, "Black Engine", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create window\n";
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);

        // Initialize GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD\n";
            return -1;
        }

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);

        // Initialize camera and setup callbacks
        initializeCamera(width, height);
        glfwSetScrollCallback(window, scroll_callback);

        // Create scene and objects
        Scene scene;
        const std::string shaderPath = "../src/shaders/";

        // Add a cube
        {
            auto defaultShader = std::make_shared<Shader>(
                (shaderPath + "simple.vert").c_str(),
                (shaderPath + "simple.frag").c_str()
            );

            auto cubeObj = scene.CreateGameObject("MyCube");
            auto transform = cubeObj->AddComponent<TransformComponent>();
            transform->position = glm::vec3(0.f, 0.f, 0.f);

            auto renderer = cubeObj->AddComponent<MeshRendererComponent>();
            renderer->mesh = Primitives::CreateCube();
            renderer->shader = defaultShader;
        }

        // Time tracking
        float lastTime = (float)glfwGetTime();

        initializeCursors();
        // Main loop
        while (!glfwWindowShouldClose(window)) {
            // Calculate delta time
            float currentTime = (float)glfwGetTime();
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            // Process input
            glfwPollEvents();
            processKeyboardInput(window, deltaTime);

            // Get and process mouse movement
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            if (camera.firstMouse) {
                camera.lastX = mouseX;
                camera.lastY = mouseY;
                camera.firstMouse = false;
            }

            float deltaX = (float)(mouseX - camera.lastX);
            float deltaY = (float)(mouseY - camera.lastY);
            camera.lastX = mouseX;
            camera.lastY = mouseY;

            // Handle mouse buttons
            bool rightPressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
            bool middlePressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);

            // Right-click: rotate camera
            if (rightPressed) {
                float sensitivity = 0.1f;
                camera.yaw += deltaX * sensitivity;
                camera.pitch -= deltaY * sensitivity;

                // Limit pitch to avoid flipping
                if (camera.pitch > 89.0f) camera.pitch = 89.0f;
                if (camera.pitch < -89.0f) camera.pitch = -89.0f;
                glfwSetCursor(window, eyeCursor);
                updateCameraVectors();
            }

            // Middle-click: pan camera
            else if (middlePressed) {
                float panSpeed = 0.01f;
                glm::vec3 right = glm::normalize(glm::cross(camera.front, camera.up));
                glm::vec3 up = glm::normalize(glm::cross(right, camera.front));
                glfwSetCursor(window, handCursor);
                camera.position += (-deltaX * panSpeed) * right;
                camera.position += (deltaY * panSpeed) * up;
            }
            else {
                glfwSetCursor(window, defaultCursor);
            }

            // Update view matrix
            gViewMatrix = glm::lookAt(camera.position, camera.position + camera.front, camera.up);

            // Clear the screen
            glClearColor(0.2f, 0.3f, 0.3f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Update and draw the scene
            scene.UpdateAll(deltaTime);
            scene.DrawAll();

            // Swap buffers
            glfwSwapBuffers(window);
        }
        // Clean up cursors before terminating GLFW
        glfwDestroyCursor(handCursor);
        glfwDestroyCursor(eyeCursor);
        glfwDestroyCursor(defaultCursor);
        // Clean up
        glfwTerminate();
        return 0;
    }

    // Initialize camera and projection matrix
    static void initializeCamera(int width, int height) {
        // Setup projection matrix
        float fov = glm::radians(45.f);
        float aspectRatio = (float)width / (float)height;
        float nearPlane = 0.1f;
        float farPlane = 100.f;
        gProjectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);

        // Initialize camera vectors
        updateCameraVectors();
    }

    // Process keyboard input
    static void processKeyboardInput(GLFWwindow* window, float deltaTime) {
        float velocity = camera.speed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.position += camera.front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.position -= camera.front * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * velocity;
    }

    // Update camera direction vectors based on Euler angles
    static void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        front.y = sin(glm::radians(camera.pitch));
        front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.front = glm::normalize(front);
    }