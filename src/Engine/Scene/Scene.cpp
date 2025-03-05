#include "Scene.h"


std::shared_ptr<GameObject> Scene::CreateGameObject(const std::string& name)
{
    auto obj = std::make_shared<GameObject>();
    obj->name = name;
    m_GameObjects.push_back(obj); // Changed from objects to m_GameObjects
    return obj;
}

void Scene::UpdateAll(float dt)
{
    for (auto& obj : m_GameObjects) // Changed from objects to m_GameObjects
    {
        obj->Update(dt);
    }
}

void Scene::DrawAll()
{
    for (auto& obj : m_GameObjects) // Changed from objects to m_GameObjects
    {
        obj->Draw();
    }
}