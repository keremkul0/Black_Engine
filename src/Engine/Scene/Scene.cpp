#include "Scene.h"

#include "Engine/Component/MeshRendererComponent.h"
#include "Engine/Render/Primitives.h"
#include "Engine/Render/Shader.h"


std::shared_ptr<GameObject> Scene::CreateGameObject(const std::string &name) {
    auto obj = std::make_shared<GameObject>();
    obj->name = name;
    m_GameObjects.push_back(obj); // Changed from objects to m_GameObjects
    return obj;
}

void Scene::LoadDefaultScene() {
    // Add a cube
    const std::string shaderPath = "../src/shaders/";
    const auto defaultShader = std::make_shared<Shader>(
        (shaderPath + "simple.vert").c_str(),
        (shaderPath + "simple.frag").c_str()
    );

    auto cubeObj = CreateGameObject("MyCube");
    auto transform = cubeObj->AddComponent<TransformComponent>();
    transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

    auto renderer = cubeObj->AddComponent<MeshRendererComponent>();
    renderer->mesh = Primitives::CreateCube();
    renderer->shader = defaultShader;
}

bool Scene::LoadSceneFromFile(const std::string &path) {
    // TODO: Implement this
    return false;
}

void Scene::UpdateAll(float dt) {
    for (auto &obj: m_GameObjects) // Changed from objects to m_GameObjects
    {
        obj->Update(dt);
    }
}

void Scene::DrawAll() {
    for (auto &obj: m_GameObjects) // Changed from objects to m_GameObjects
    {
        obj->Draw();
    }
}
