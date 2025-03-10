#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Engine/Entity/GameObject.h"

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    void LoadDefaultScene();
    static bool LoadSceneFromFile(const std::string& path);

    [[nodiscard]] const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() const {
        return m_GameObjects;
    }

    std::shared_ptr<GameObject> CreateGameObject(const std::string& name);
    // Tüm objeleri update et
    void UpdateAll(float dt);

    // Tüm objeleri draw et
    void DrawAll();
    void SetViewMatrix(const glm::mat4& viewMatrix) {
        m_ViewMatrix = viewMatrix;
    }

    void SetProjectionMatrix(const glm::mat4& projMatrix) {
        m_ProjectionMatrix = projMatrix;
    }
    const std::string& GetName() const { return m_SceneName; }
    void SetName(const std::string& name) { m_SceneName = name; }

private:
    std::string m_SceneName = "New Scene";
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
    // Sahnedeki tüm objeler
    std::vector<std::shared_ptr<GameObject>> m_GameObjects;
};

#endif // SCENE_H
