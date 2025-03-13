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
    auto iblManager = std::make_shared<IBLManager>();
    iblManager->GenerateEnvironmentMap("../src/Assets/environments/studio.hdr");
    m_IBLManager = iblManager;
    const std::string shaderPath = "../src/shaders/";
    // Create shader
    const auto shader = std::make_shared<Shader>(
        (shaderPath + "pbr.vert").c_str(),
        (shaderPath + "pbr.frag").c_str()
    );

    shader->setFloat("material.metallic", 0.9f); // More metallic
    shader->setFloat("material.roughness", 0.1f); // Less rough
    shader->setInt("useIBL", 1); // Enable IBL

    // Create material
    const auto material = std::make_shared<Material>(shader);

    // Load textures with error handling
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

    // If scene has IBL, bind the textures before rendering
    if (m_IBLManager && !m_GameObjects.empty()) {
        // Get the shader from the first object's material
        // This is a simplification - you might need a more robust approach
        auto renderer = m_GameObjects[0]->GetComponent<MeshRendererComponent>();
        if (renderer && renderer->GetMaterial() && renderer->GetMaterial()->GetShader()) {
            int textureUnit = 10; // Start at a high texture unit to avoid conflicts
            m_IBLManager->BindTextures(renderer->GetMaterial()->GetShader(), textureUnit);
        }
    }

    for (const auto &obj: m_GameObjects) {
        obj->Draw();
    }
}
