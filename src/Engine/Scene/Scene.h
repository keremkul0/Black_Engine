#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Engine/Entity/GameObject.h"
<<<<<<< HEAD
#include "Core/Math/Ray.h"
=======
#include "Engine/render/Texture/Texture.h"
#include "Core/Camera/Camera.h"
>>>>>>> 2c7472b480e34724b9cb0c0c9d3a71e9720ac2f2

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

<<<<<<< HEAD
    // Singleton pattern for accessing the active scene
    static Scene& Get();
=======
>>>>>>> 2c7472b480e34724b9cb0c0c9d3a71e9720ac2f2

    void LoadDefaultScene();
    static bool LoadSceneFromFile(const std::string& path);

    [[nodiscard]] const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() const {
        return m_GameObjects;
    }

    std::shared_ptr<GameObject> CreateGameObject(const std::string& name);

    bool HasGameObject(const std::shared_ptr<GameObject>& obj) const;
    
    // Create primitive game objects
    std::shared_ptr<GameObject> CreatePrimitive(const std::string& primitiveType);
    
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

<<<<<<< HEAD
    void RemoveGameObject(const std::shared_ptr<GameObject>& gameObject);


    // Ray casting for object selection
    std::shared_ptr<GameObject> PickObjectWithRay(const Math::Ray& ray) const;
    std::shared_ptr<GameObject> PickObjectWithRay(const glm::vec3& origin, const glm::vec3& direction) const;
=======
    void SetCamera(Camera* camera) { m_Camera = camera; }
>>>>>>> 2c7472b480e34724b9cb0c0c9d3a71e9720ac2f2

private:
    std::string m_SceneName = "New Scene";
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

    // Sahnedeki tüm objeler
    std::vector<std::shared_ptr<GameObject>> m_GameObjects;
<<<<<<< HEAD
    

    bool RemoveChildRecursive(const std::shared_ptr<GameObject>& parent, const std::shared_ptr<GameObject>& childToRemove);
    
    // Singleton instance
    static Scene* s_ActiveScene;
=======

    Camera* m_Camera = nullptr; // Added camera pointer for scene
>>>>>>> 2c7472b480e34724b9cb0c0c9d3a71e9720ac2f2
};

#endif // SCENE_H

