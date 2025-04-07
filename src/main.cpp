#include "Application.h"
#include "Core/Logger/Logger.h"
#include "Core/Logger/LogMacros.h"
#include "Core/ProjectManager/ProjectManager.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/SceneManager/SceneManager.h"
#include <cstdlib>
#include <nlohmann/json.hpp>

int main() {
    // Initialize the logging system
    Logger::Initialize();
    BE_INFO("Black Engine starting up");

    BE_DEBUG("Creating application instance");
    Application app;

    BE_INFO("Initializing application");
    if (!app.Initialize()) {
        BE_CRITICAL("Application initialization failed");
        Logger::Shutdown();
        return -1;
    }

    // Get user home directory in a cross-platform way
    std::string userHomeDir;
#ifdef _WIN32
    userHomeDir = std::getenv("USERPROFILE");
#else
    userHomeDir = std::getenv("HOME");
#endif

    std::string defaultProjectsDir = userHomeDir + "/BlackEngineProjects";
    const std::string defaultProjectPath = defaultProjectsDir + "/MyBlackEngineProject";

    BE_DEBUG_FORMAT("User home directory: {}", userHomeDir);
    BE_DEBUG_FORMAT("Default projects directory: {}", defaultProjectsDir);

    // Try to load the default project, create it if it doesn't exist
    if (!FileSystem::DirectoryExists(defaultProjectPath) ||
        !ProjectManager::GetInstance().LoadProject(defaultProjectPath)) {
        BE_INFO_FORMAT("Creating default project at: {}", defaultProjectPath);

        // Create default projects directory if it doesn't exist
        if (!FileSystem::DirectoryExists(defaultProjectsDir)) {
            BE_DEBUG("Creating default project structure");
            FileSystem::CreateDirectory(defaultProjectsDir);
        }

        // Create default project directory
        if (!FileSystem::DirectoryExists(defaultProjectPath)) {
            FileSystem::CreateDirectory(defaultProjectPath);
        }

        // Create basic project structure
        FileSystem::CreateDirectory(defaultProjectPath + "/Assets");
        FileSystem::CreateDirectory(defaultProjectPath + "/Assets/Scenes");
        FileSystem::CreateDirectory(defaultProjectPath + "/Assets/Meshes");
        FileSystem::CreateDirectory(defaultProjectPath + "/Assets/Textures");
        FileSystem::CreateDirectory(defaultProjectPath + "/Assets/Shaders");

        // Create project.json using nlohmann::json
        nlohmann::json projectJson;
        projectJson["name"] = "MyBlackEngineProject";
        projectJson["version"] = "1.0.0";
        projectJson["engine"] = "Black Engine";
        FileSystem::WriteTextFile(defaultProjectPath + "/project.json", projectJson.dump(4));

        // Load the default project
        if (!ProjectManager::GetInstance().LoadProject(defaultProjectPath)) {
            BE_ERROR("Failed to load default project");
            return 1;
        }

        // Create and save a default scene
        auto &sceneManager = SceneManager::GetInstance();
        sceneManager.CreateNewScene();
        sceneManager.GetActiveScene()->LoadDefaultScene();

        // Save the current scene to a default path
        const std::string scenePath = defaultProjectPath + "/Assets/Scenes/DefaultScene.scene";
        sceneManager.SaveCurrentScene(scenePath);
    }

    int result = app.Run();
    BE_INFO_FORMAT("Application exited with code: {}", result);
    Logger::Shutdown();
    return 0;
}
