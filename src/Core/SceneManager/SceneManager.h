#pragma once

#include "Engine/Scene/Scene.h"
#include <memory>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class SceneManager {
public:
    static SceneManager &GetInstance();

    // Scene operations
    bool LoadScene(const std::string &scenePath);

    void SaveCurrentScene();

    void SaveCurrentScene(const std::string &path);

    void CreateNewScene();

    // Scene access
    Scene *GetActiveScene() { return m_ActiveScene.get(); }
    [[nodiscard]] const Scene *GetActiveScene() const { return m_ActiveScene.get(); }

    // Scene list management
    [[nodiscard]] std::vector<std::string> GetAvailableScenes() const;

    void RegisterScene(const std::string &scenePath);

    bool LoadSceneFromJSON(const std::string &jsonData);

private:
    SceneManager() = default;

    ~SceneManager() = default;

    bool DeserializeScene(const std::string &jsonData);

    [[nodiscard]] std::string SerializeCurrentScene() const;

    void ClearCurrentScene();

    std::shared_ptr<Scene> m_ActiveScene;
    std::vector<std::string> m_RegisteredScenes;
    std::string m_CurrentScenePath;
};
