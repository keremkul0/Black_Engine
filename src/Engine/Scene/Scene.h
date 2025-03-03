#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>

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
    std::shared_ptr<GameObject> CreateGameObject(const std::string& name = "");

    // Tüm objeleri update et
    void UpdateAll(float dt);

    // Tüm objeleri draw et
    void DrawAll();

private:
    // Sahnedeki tüm objeler
    std::vector<std::shared_ptr<GameObject>> objects;
};

#endif // SCENE_H
