#include "Scene.h"

#include <iostream>

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
    const std::string shaderPath = "../src/shaders/";
    // Create shader
    const auto shader = std::make_shared<Shader>(
        (shaderPath + "pbr.vert").c_str(),
        (shaderPath + "pbr.frag").c_str()
    );

    // Create material
    const auto material = std::make_shared<Material>(shader);

    // Load textures with error handling
    if (const auto diffuseTexture = std::make_shared<Texture>(); !diffuseTexture->LoadFromFile("../src/textures/bricks_diffuse.jpg")) {
        std::cout << "Warning: Failed to load diffuse texture, using default\n";
        material->diffuseColor = glm::vec3(0.8f, 0.2f, 0.2f);
    } else {
        material->SetTexture(TextureType::Diffuse, diffuseTexture);
    }

    if (const auto specularTexture = std::make_shared<Texture>(); !specularTexture->LoadFromFile("../src/textures/bricks_specular.jpg")) {
        std::cout << "Warning: Failed to load specular texture, using default\n";
    } else {
        material->SetTexture(TextureType::Specular, specularTexture);
    }

    material->diffuseColor = glm::vec3(0.8f, 0.8f, 0.8f);
    material->specularColor = glm::vec3(1.0f);
    material->shininess = 64.0f;

    // Create cube with material
    const auto cubeObject = CreateGameObject("Cube");

    // Use AddComponent instead of GetComponent - this is the key fix
    const auto transform = cubeObject->AddComponent<TransformComponent>();
    transform->SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));
    transform->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));

    const auto meshComp = cubeObject->AddComponent<MeshComponent>();
    meshComp->SetMesh(Primitives::CreateCube(1.0f));

    const auto rendererComp = cubeObject->AddComponent<MeshRendererComponent>();
    rendererComp->SetMaterial(material);

    std::cout << "Test scene created successfully" << std::endl;
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
