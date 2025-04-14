/**
 * @file SceneManager.cpp
 * @brief Implementation of the scene management system for Black Engine.
 *
 * This file handles scene loading, saving, and management of scene objects.
 */

#include "SceneManager.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/ProjectManager/ProjectManager.h"
#include "Engine/Component/MeshComponent.h"
#include "Engine/Component/MeshRendererComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Core/Logger/LogMacros.h"

BE_DEFINE_LOG_CATEGORY(SceneManagerLog, "SceneManager", ::BlackEngine::LogLevel::Info);

SceneManager &SceneManager::GetInstance() {
    static SceneManager instance;
    return instance;
}

bool SceneManager::LoadScene(const std::string &scenePath) {
    if (!FileSystem::BE_File_Exists(scenePath)) {
        return false;
    }

    const std::string fileContent = FileSystem::BE_Read_Text_File(scenePath);
    if (fileContent.empty()) {
        return false;
    }

    // Save current scene path for future saves
    m_CurrentScenePath = scenePath;

    // Deserialize data
    return DeserializeScene(fileContent);
}

void SceneManager::SaveCurrentScene() {
    if (m_CurrentScenePath.empty()) {
        return; // No scene path set; nothing to save
    }

    SaveCurrentScene(m_CurrentScenePath);
}

void SceneManager::SaveCurrentScene(const std::string &scenePath) {
    if (!m_ActiveScene) {
        return;
    }

    // Use SerializeCurrentScene() instead of old serialization methods
    if (const std::string jsonStr = SerializeCurrentScene(); FileSystem::BE_Write_Text_File(scenePath, jsonStr)) {
        m_CurrentScenePath = scenePath;
    }
}

void SceneManager::CreateNewScene() {
    // Clear current scene
    ClearCurrentScene();

    // Create a new scene
    m_ActiveScene = std::make_shared<Scene>();
    m_ActiveScene->SetName("New Scene");
    m_CurrentScenePath = "";
}

std::vector<std::string> SceneManager::GetAvailableScenes() const {
    return m_RegisteredScenes;
}

void SceneManager::RegisterScene(const std::string &scenePath) {
    // Check if already registered
    for (const auto &path: m_RegisteredScenes) {
        if (path == scenePath) return;
    }
    m_RegisteredScenes.push_back(scenePath);
}

bool SceneManager::DeserializeScene(const std::string &jsonData) {
    try {
        // Parse JSON
        json sceneJson = json::parse(jsonData);

        // Clear current scene
        ClearCurrentScene();

        // Create a new scene
        m_ActiveScene = std::make_shared<Scene>();

        // Set scene properties
        m_ActiveScene->SetName(sceneJson.value("name", "Untitled Scene"));

        // Load game objects
        if (sceneJson.contains("gameObjects") && sceneJson["gameObjects"].is_array()) {
            for (const auto &objJson: sceneJson["gameObjects"]) {
                std::string name = objJson.value("name", "GameObject");
                const auto gameObject = m_ActiveScene->CreateGameObject(name);

                // Add components based on their types
                if (objJson.contains("components") && objJson["components"].is_array()) {
                    for (const auto &compJson: objJson["components"]) {
                        std::string type = compJson["type"];

                        if (type == "TransformComponent") {
                            auto transform = gameObject->GetComponent<TransformComponent>();
                            if (!transform) {
                                transform = gameObject->AddComponent<TransformComponent>();
                            }

                            // Parse transform data
                            if (compJson.contains("position")) {
                                auto &posJson = compJson["position"];
                                transform->position = glm::vec3(
                                    posJson[0], posJson[1], posJson[2]
                                );
                            }

                            if (compJson.contains("rotation")) {
                                auto &rotJson = compJson["rotation"];
                                transform->rotation = glm::vec3(
                                    rotJson[0], rotJson[1], rotJson[2]
                                );
                            }

                            if (compJson.contains("scale")) {
                                auto &scaleJson = compJson["scale"];
                                transform->scale = glm::vec3(
                                    scaleJson[0], scaleJson[1], scaleJson[2]
                                );
                            }
                        }
                        // Process MeshComponent
                        else if (type == "MeshComponent") {
                            auto mesh = gameObject->AddComponent<MeshComponent>();
                            // TODO: Load mesh asset reference as needed
                        }
                        // Process MeshRendererComponent
                        else if (type == "MeshRendererComponent") {
                            auto renderer = gameObject->AddComponent<MeshRendererComponent>();
                            // TODO: Load shader asset reference as needed
                        }
                    }
                }
            }
        }

        // Kategori kullanarak loglama
        BE_LOG_INFO(SceneManagerLog, "Scene loaded: {}", m_ActiveScene->GetName());
        return true;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(SceneManagerLog, "Failed to load scene: {}", e.what());
        // Create an empty scene as fallback
        CreateNewScene();
        return false;
    }
}

std::string SceneManager::SerializeCurrentScene() const {
    if (!m_ActiveScene) {
        return "{}";
    }

    // Create scene JSON
    json sceneJson;
    sceneJson["name"] = m_ActiveScene->GetName();
    sceneJson["gameObjects"] = json::array();

    // Serialize game objects
    for (const auto &obj: m_ActiveScene->GetGameObjects()) {
        json objJson;
        objJson["name"] = obj->name;
        objJson["components"] = json::array();

        // Serialize TransformComponent
        if (const auto transform = obj->GetComponent<TransformComponent>()) {
            json compJson;
            compJson["type"] = "TransformComponent";

            compJson["position"] = {
                transform->position.x,
                transform->position.y,
                transform->position.z
            };

            compJson["rotation"] = {
                transform->rotation.x,
                transform->rotation.y,
                transform->rotation.z
            };

            compJson["scale"] = {
                transform->scale.x,
                transform->scale.y,
                transform->scale.z
            };

            objJson["components"].push_back(compJson);
        }

        // Serialize MeshComponent
        if (auto mesh = obj->GetComponent<MeshComponent>()) {
            json compJson;
            compJson["type"] = "MeshComponent";
            // TODO: Store mesh asset reference
            objJson["components"].push_back(compJson);
        }

        // Serialize MeshRendererComponent
        if (auto renderer = obj->GetComponent<MeshRendererComponent>()) {
            json compJson;
            compJson["type"] = "MeshRendererComponent";
            // TODO: Store shader asset reference
            objJson["components"].push_back(compJson);
        }

        sceneJson["gameObjects"].push_back(objJson);
    }

    return sceneJson.dump(4);
}

void SceneManager::ClearCurrentScene() {
    m_ActiveScene.reset();
}

bool SceneManager::LoadSceneFromJSON(const std::string &jsonData) {
    try {
        auto j = nlohmann::json::parse(jsonData);
        const auto scene = std::make_shared<Scene>();
        DeserializeScene(jsonData);
        m_ActiveScene = scene;
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}
