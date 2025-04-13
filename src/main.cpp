#include "Application.h"
#include "Core/Logger/LogMacros.h"
#include "Core/Logger/LoggerManager.h"  // Yeni log yönetim sistemi
#include "Core/ProjectManager/ProjectManager.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/SceneManager/SceneManager.h"
#include <cstdlib>
#include <nlohmann/json.hpp>

int main() {
    // Get the executable path to find config files relative to it
    std::string configPath = "c:/Users/Kerem/CLionProjects/Black_Engine/log_config.json";
    
    // Yeni log sistemini JSON konfigürasyon dosyasından başlat: Prod ortamı için false (test değil)
    LoggerManager::InitializeFromJson(configPath, false);

    BE_INFO("Black Engine starting up");

    BE_DEBUG("Creating application instance");
    Application app;    BE_CAT_INFO("Engine", "Initializing application");
    if (!app.Initialize()) {
        BE_CAT_CRITICAL("Engine", "Application initialization failed");
        LoggerManager::Shutdown();
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

    BE_DEBUG_FMT("User home directory: {}", userHomeDir);
    BE_DEBUG_FMT("Default projects directory: {}", defaultProjectsDir);

    // Default proje yüklenmeye çalışılıyor; mevcut değilse oluşturuluyor.
    if (!FileSystem::BE_Directory_Exists(defaultProjectPath) ||
        !ProjectManager::GetInstance().LoadProject(defaultProjectPath)) {
        BE_INFO_FMT("Creating default project at: {}", defaultProjectPath);

        // Default proje dizini yoksa oluştur
        if (!FileSystem::BE_Directory_Exists(defaultProjectsDir)) {
            BE_DEBUG("Creating default project structure");
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

        // project.json dosyasını oluştur (nlohmann::json kullanılarak)
        nlohmann::json projectJson;
        projectJson["name"] = "MyBlackEngineProject";
        projectJson["version"] = "1.0.0";
        projectJson["engine"] = "Black Engine";
        FileSystem::BE_Write_Text_File(defaultProjectPath + "/project.json", projectJson.dump(4));

        // Default projeyi yükle
        if (!ProjectManager::GetInstance().LoadProject(defaultProjectPath)) {
            BE_ERROR("Failed to load default project");
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
    BE_INFO_FMT("Application exited with code: {}", result);
    LoggerManager::Shutdown();
    return 0;
}
