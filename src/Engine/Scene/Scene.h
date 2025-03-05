#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

// GameObject'i burada INLINE (yani header içinde) kullanacaksak
// tam tanımına ihtiyacımız var. O yüzden "GameObject.h" ekliyoruz.
#include "../Entity/GameObject.h"

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    // Inline olarak tanımlarsak, 'GameObject' tanımını görmesi gerekir.
    // Dolayısıyla "#include GameObject.h" bu dosyada olmalı.
    const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() const {
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

private:
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
    // Sahnedeki tüm objeler
    std::vector<std::shared_ptr<GameObject>> m_GameObjects;
};

#endif // SCENE_H
