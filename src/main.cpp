#include "Application.h"
#include "Core/Logger/LogMacros.h"
#include "Core/Logger/LogManager.h"
#include "Core/ProjectManager/ProjectManager.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/SceneManager/SceneManager.h"
#include <cstdlib>
#include <nlohmann/json.hpp>

// Ana log kategorisi tanımlama
BE_DEFINE_LOG_CATEGORY(MainLog, "Main");

int main() {
    // Log yapılandırma dosyası yolu
    const std::string configPath = "../log_config.json";

    // Log sistemini başlat
    BlackEngine::LogManager::GetInstance().Initialize();
    BlackEngine::LogManager::GetInstance().LoadConfig(configPath);

    BE_LOG_INFO(MainLog, "Black Engine starting up");

    BE_LOG_DEBUG(MainLog, "Creating application instance");
    Application app;
    BE_LOG_INFO(MainLog, "Initializing application");
    if (!app.Initialize()) {
        BE_LOG_CRITICAL(MainLog, "Application initialization failed");
        BlackEngine::LogManager::GetInstance().Shutdown();
        return -1;
    }

    // Kullanıcı ev dizinini platformdan bağımsız alma
    std::string userHomeDir;
#ifdef _WIN32
    userHomeDir = std::getenv("USERPROFILE");
#else
    userHomeDir = std::getenv("HOME");
#endif

    const std::string defaultProjectsDir = userHomeDir + "/BlackEngineProjects";
    const std::string defaultProjectPath = defaultProjectsDir + "/MyBlackEngineProject";

    BE_LOG_DEBUG(MainLog, "User home directory: {}", userHomeDir);
    BE_LOG_DEBUG(MainLog, "Default projects directory: {}", defaultProjectsDir);

    // Default proje yüklenmeye çalışılıyor; mevcut değilse oluşturuluyor
    if (!FileSystem::BE_Directory_Exists(defaultProjectPath) ||
        !ProjectManager::GetInstance().LoadProject(defaultProjectPath)) {
        BE_LOG_INFO(MainLog, "Creating default project at: {}", defaultProjectPath);

        // Default proje dizini yoksa oluştur
        if (!FileSystem::BE_Directory_Exists(defaultProjectsDir)) {
            BE_LOG_DEBUG(MainLog, "Creating default project structure");
            FileSystem::BE_Create_Directory(defaultProjectsDir);
        }

        if (!FileSystem::BE_Directory_Exists(defaultProjectPath)) {
            FileSystem::BE_Create_Directory(defaultProjectPath);
        }

        // Temel proje yapısını oluştur
        FileSystem::BE_Create_Directory(defaultProjectPath + "/Assets");
        FileSystem::BE_Create_Directory(defaultProjectPath + "/Assets/Scenes");
        FileSystem::BE_Create_Directory(defaultProjectPath + "/Assets/Meshes");
        FileSystem::BE_Create_Directory(defaultProjectPath + "/Assets/Textures");
        FileSystem::BE_Create_Directory(defaultProjectPath + "/Assets/Shaders");

        // project.json dosyasını oluştur
        nlohmann::json projectJson;
        projectJson["name"] = "MyBlackEngineProject";
        projectJson["version"] = "1.0.0";
        projectJson["engine"] = "Black Engine";
        FileSystem::BE_Write_Text_File(defaultProjectPath + "/project.json", projectJson.dump(4));

        // Default projeyi yükle
        if (!ProjectManager::GetInstance().LoadProject(defaultProjectPath)) {
            BE_LOG_ERROR(MainLog, "Failed to load default project");
            return 1;
        }

        // Yeni bir sahne oluştur ve varsayılan sahneyi yükle
        auto &sceneManager = SceneManager::GetInstance();
        sceneManager.CreateNewScene();
        sceneManager.GetActiveScene()->LoadDefaultScene();

        // Varsayılan sahneyi belirlenmiş bir yola kaydet
        const std::string scenePath = defaultProjectPath + "/Assets/Scenes/DefaultScene.scene";
        sceneManager.SaveCurrentScene(scenePath);
    }

    int result = app.Run();
    BE_LOG_INFO(MainLog, "Application exited with code: {}", result);
    BlackEngine::LogManager::GetInstance().Shutdown();
    return 0;
}