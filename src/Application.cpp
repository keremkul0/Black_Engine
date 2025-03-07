#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include "Application.h"

#include "Shader.h"

static void GLFWErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

Application::Application()
{
}

Application::~Application()
{
}

bool Application::Init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW\n";
        return false;
    }
    glfwSetErrorCallback(GLFWErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Black Engine", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    const char* vertexSrc = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        out vec3 vColor;
        uniform mat4 MVP;
        void main()
        {
            vColor = aColor;
            gl_Position = MVP * vec4(aPos, 1.0);
        }
        )";

            // Fragment shader (pseudo-code)
    const char* fragmentSrc = R"(
        #version 330 core
        in vec3 vColor;
        out vec4 FragColor;
        void main()
        {
            FragColor = vec4(vColor, 1.0);
        }
        )";

    // 3) Shader nesnesi oluşturalım (Application class içinde üye değişken olabilir)
    m_Shader = new Shader(vertexSrc, fragmentSrc);
    // Scene tarafında küp geometrisini hazırlat
    Scene::InitCubeGeometry();
    return true;
}

void Application::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ProcessInput();

        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        RenderFrame();
        RenderGUI();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    Shutdown();
}

void Application::ProcessInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    // Additional input handling, e.g. camera movement or object interaction
}

void Application::RenderFrame()
{
    // Update cube rotations based on current time
    float currentTime = glfwGetTime();
    scene.UpdateCubeRotations(currentTime);

    // Use the shader program
    m_Shader->use();

    // Draw the rotating cubes
    scene.DrawAllCubes(m_Shader->getID(), width, height);
}
void Application::Shutdown()
{
    delete m_Shader;
    m_Shader = nullptr;
    Scene::CleanUpCubeGeometry();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::RenderGUI()
{
    ImGui::Begin("Scene Controls");
    if (ImGui::Button("Add Cube"))
    {
        scene.AddCube(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    ImGui::Text("Cube Count: %d", (int)scene.GetCubes().size());
    ImGui::End();
}