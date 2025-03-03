#include "Scene.h"
#include <algorithm> // remove_if vb. kullanıyorsanız
#include <iostream>  // std::cout vb. kullanırsanız

// "CreateGameObject" burada inline yazmak yerine .cpp'ye de alabilirsiniz.
// Fakat yukarıda inline tanımlarsanız burada tekrar yazmanıza gerek yok.

// Örnek: inline değil, burada tanımlayalım derseniz:

/*
std::shared_ptr<GameObject> Scene::CreateGameObject(const std::string& name)
{
    auto obj = std::make_shared<GameObject>();
    obj->name = name;
    objects.push_back(obj);
    return obj;
}
*/

std::shared_ptr<GameObject> Scene::CreateGameObject(const std::string& name)
{
    // Eğer fonksiyonun gövdesini .cpp'ye taşırsak,
    // burada da "GameObject" tanımı gerekiyor.
    // Ama Scene.h içinde include "GameObject.h" yaptığımız için sorun yok.
    auto obj = std::make_shared<GameObject>();
    obj->name = name;
    objects.push_back(obj);
    return obj;
}

void Scene::UpdateAll(float dt)
{
    for (auto& obj : objects)
    {
        obj->Update(dt);
    }
}

void Scene::DrawAll()
{
    for (auto& obj : objects)
    {
        obj->Draw();
    }
}
