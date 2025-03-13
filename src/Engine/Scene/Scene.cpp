#include "Scene.h"

#include "Engine/Component/MeshRendererComponent.h"
#include "Engine/Component/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Render/Shader/Shader.h"
#include "Engine/Render/Primitives/Primitives.h"


std::shared_ptr<GameObject> Scene::CreateGameObject(const std::string &name) {
    auto obj = std::make_shared<GameObject>();
    obj->name = name;
    m_GameObjects.push_back(obj);
    return obj;
}

void Scene::LoadDefaultScene() {
    // Set up shader for all objects
    const std::string shaderPath = "../src/shaders/";
    const auto defaultShader = std::make_shared<Shader>(
        (shaderPath + "simple.vert").c_str(),
        (shaderPath + "simple.frag").c_str()
    );

}

void Scene::CreateTestScene() {
    // Create shader
    auto shader = std::make_shared<Shader>("shaders/pbr.vert", "shaders/pbr.frag");

    // Create textures
    const auto diffuseTexture = std::make_shared<Texture>();
    diffuseTexture->LoadFromFile("textures/bricks_diffuse.jpg");

    const auto specularTexture = std::make_shared<Texture>();
    specularTexture->LoadFromFile("textures/bricks_specular.jpg");

    // Create material
    const auto material = std::make_shared<Material>(shader);
    material->diffuseColor = glm::vec3(0.8f, 0.8f, 0.8f);
    material->specularColor = glm::vec3(1.0f);
    material->shininess = 64.0f;
    material->SetTexture(TextureType::Diffuse, diffuseTexture);
    material->SetTexture(TextureType::Specular, specularTexture);

    // Create cube with material
    auto cubeObject = CreateGameObject("Cube");

    const auto meshComp = cubeObject->AddComponent<MeshComponent>();
    meshComp->SetMesh(Primitives::CreateCube(1.0f));

    const auto rendererComp = cubeObject->AddComponent<MeshRendererComponent>();
    rendererComp->SetMaterial(material);

    const auto transform = cubeObject->GetComponent<TransformComponent>();
    transform->SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));
    transform->SetRotation(glm::vec3(45.0f, 45.0f, 0.0f));

    // Add to scene/game
    // yourScene->AddGameObject(cubeObject);
}

bool Scene::LoadSceneFromFile(const std::string &path) {
    // TODO: Implement this
    return false;
}

void Scene::UpdateAll(const float dt) {
    for (const auto &obj: m_GameObjects) {
        obj->Update(dt);
    }
}

void Scene::DrawAll() {
    for (const auto &obj: m_GameObjects) {
        obj->Draw();
    }
}
