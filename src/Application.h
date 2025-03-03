#pragma once
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "Shader.h"

struct GLFWwindow;

class Application
{
public:
    Application();
    ~Application();

    bool Init();
    void Run();
    void Shutdown();

private:
    // Pencere göstergesi
    GLFWwindow* window = nullptr;

    // Sahne
    Scene scene;

    Shader* m_Shader = nullptr;
    // Ekran boyutu
    int width = 1280;
    int height = 720;

    // private fonksiyonlar
    void ProcessInput();
    void RenderFrame();
    void RenderGUI();
};
