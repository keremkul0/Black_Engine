#include "Application.h"
#include "Core/InputManager/InputManager.h"
#include "Engine/Component/TransformComponent.h"
#include "Core/ImGui/ImGuiLayer.h"
#include "Editor/UI/Layout/EditorLayout.h"
#include "Core/SceneManager/SceneManager.h"
#include "Core/Logger/LogMacros.h"
#include "Core/AssetImporter/AssetImporterRegistry.h"
#include <iostream>

// Log kategorisini doğru formatla tanımla
BE_DEFINE_LOG_CATEGORY(EngineLog, "Engine");

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

// ImGui için ana pencere referansı - ImGuiLayer.cpp için gerekli
GLFWwindow *g_Window = nullptr;

// Mevcut kodla uyumluluğu korumak için harici referanslar
glm::mat4 gViewMatrix(1.0f);
glm::mat4 gProjectionMatrix(1.0f);

// Kaydırma geri çağrı fonksiyonu
static void scroll_callback(GLFWwindow *window, double xoffset, const double yoffset) {
    InputManager::SetScrollOffset(static_cast<float>(yoffset));
}

Application::Application()
    : m_WindowManager(std::make_unique<WindowManager>()),
      m_Camera(std::make_unique<Camera>()),
      m_EditorLayout(nullptr),
      m_InputSystem(std::make_unique<InputSystem>()) {
}

Application::~Application() {
    // Kapatma mesajını logla
    BE_LOG_INFO(EngineLog, "Shutting down application");

    // Input manager temizliği
    InputManager::Cleanup();

    // ImGui temizliği
    ImGuiLayer::Shutdown();

    // Log sistemini en son kapat
    BlackEngine::LogManager::GetInstance().Shutdown();
}

bool Application::Initialize() {
    // Önce log sistemini başlat
    if (!BlackEngine::LogManager::GetInstance().Initialize()) {
        // Log makroları henüz kullanılamaz, doğrudan hata çıktısı
        std::cerr << "Failed to initialize logging system!" << std::endl;
        return false;
    }

    BE_LOG_INFO(EngineLog, "Initializing application components");
    
    // Initialize asset importers
    BlackEngine::InitializeAssetImporters();

    // Pencereyi başlat
    BE_LOG_DEBUG(EngineLog, "Initializing window manager");
    if (!m_WindowManager->Initialize(1280, 720, "Black Engine")) {
        BE_LOG_ERROR(EngineLog, "Failed to initialize window manager");
        return false;
    }

    // ImGui için global pencere referansı
    g_Window = m_WindowManager->GetWindow();

    // Input manager'ı başlat
    BE_LOG_DEBUG(EngineLog, "Initializing input systems");
    InputManager::Initialize(m_WindowManager->GetWindow());
    m_InputSystem->Initialize(m_WindowManager->GetWindow());

    // Pencere geri çağrılarını ayarla
    GLFWwindow *window = m_WindowManager->GetWindow();
    glfwSetWindowUserPointer(window, this);
    m_WindowManager->SetScrollCallback(scroll_callback);

    // Projeksiyon matrisini ayarla
    const float aspectRatio = static_cast<float>(m_WindowManager->GetWidth()) /
                              static_cast<float>(m_WindowManager->GetHeight());
    m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    gProjectionMatrix = m_ProjectionMatrix;

    // ImGui'yi başlat
    ImGuiLayer::Init();

    // Editor arayüzünü SceneManager'ın aktif sahnesiyle başlat
    Scene* activeScene = SceneManager::GetInstance().GetActiveScene();
    m_EditorLayout = std::make_unique<EditorLayout>();
    m_EditorLayout->SetupDefaultLayout(std::shared_ptr<Scene>(activeScene, [](Scene*) {}));

    // Editor düzenini input sisteme kaydet
    m_InputSystem->RegisterEventReceiver(m_EditorLayout.get());

    return true;
}

int Application::Run() const {
    // Ana döngü değişkenleri
    auto lastTime = static_cast<float>(glfwGetTime());

    // Ana döngü
    while (!m_WindowManager->ShouldClose()) {
        const auto currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Girişi işle
        WindowManager::PollEvents();
        m_InputSystem->ProcessInput(deltaTime);

        // Ekranı temizle
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // SceneManager'dan aktif sahneyi güncelle ve çiz
        if (Scene* activeScene = SceneManager::GetInstance().GetActiveScene()) {
            activeScene->UpdateAll(deltaTime);
            activeScene->DrawAll();
        }

        // Arayüzü çiz
        ImGuiLayer::Begin();
        if (m_EditorLayout) {
            m_EditorLayout->UpdateAllPanels(deltaTime);
            m_EditorLayout->RenderLayout();
        }
        ImGuiLayer::End();
        m_WindowManager->SwapBuffers();
    }

    return 0;
}