#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Engine/Entity/GameObject.h"
#include "Core/Math/Ray.h"
#include "Engine/render/Texture/Texture.h"
#include "Core/Camera/Camera.h"

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    // Singleton pattern for accessing the active scene
    static Scene& Get();


    void LoadDefaultScene();
    static bool LoadSceneFromFile(const std::string& path);

    [[nodiscard]] const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() const {
        return m_GameObjects;
    }

    std::shared_ptr<GameObject> CreateGameObject(const std::string& name);

    bool HasGameObject(const std::shared_ptr<GameObject>& obj) const;
    
    // Create primitive game objects
    std::shared_ptr<GameObject> CreatePrimitive(const std::string& primitiveType);
    std::shared_ptr<GameObject> CreatePrimitive(const std::string& primitiveType, const glm::vec3& position);
    
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

    //shadowMap için çizim fonksiyonu
    void DrawAll2ShadowMap();
    
    const std::string& GetName() const { return m_SceneName; }
    void SetName(const std::string& name) { m_SceneName = name; }

    void RemoveGameObject(const std::shared_ptr<GameObject>& gameObject);


    // Ray casting for object selection
    std::shared_ptr<GameObject> PickObjectWithRay(const Math::Ray& ray) const;
    std::shared_ptr<GameObject> PickObjectWithRay(const glm::vec3& origin, const glm::vec3& direction) const;
    void SetCamera(Camera* camera) { m_Camera = camera; }

    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    // ScenePanel'ın shadowMapProgram'a erişebilmesi için getter
    std::shared_ptr<Shader> GetShadowMapShader() const { return m_ShadowMapProgram; }
    void SetShadowMapShader(const std::shared_ptr<Shader>& shader) { m_ShadowMapProgram = shader; }
    // ScenePanel'ın default shader'a erişebilmesi için getter
    std::shared_ptr<Shader> GetDefaultShader() const { return m_DefaultShader; }
    void SetDefaultShader(const std::shared_ptr<Shader>& shader) { m_DefaultShader = shader; }
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//

private:
    std::string m_SceneName = "New Scene";
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

    // Sahnedeki tüm objeler
    std::vector<std::shared_ptr<GameObject>> m_GameObjects;

    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    // Gölge haritası shader'ı için yeni üye değişken
    std::shared_ptr<Shader> m_ShadowMapProgram;
    //default shader'ı için yeni üye değişken
    std::shared_ptr<Shader> m_DefaultShader;
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//

    bool RemoveChildRecursive(const std::shared_ptr<GameObject>& parent, const std::shared_ptr<GameObject>& childToRemove);
    
    // Singleton instance
    static Scene* s_ActiveScene;

    Camera* m_Camera = nullptr; // Added camera pointer for scene
};

#endif // SCENE_H

