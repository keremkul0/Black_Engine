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

    // Create and position each primitive with increased spacing

    // 1. Cube
    auto cubeObj = CreateGameObject("Cube");
    auto cubeTransform = cubeObj->AddComponent<TransformComponent>();
    cubeTransform->position = glm::vec3(-10.0f, 0.0f, 0.0f);
    auto cubeMesh = cubeObj->AddComponent<MeshComponent>();
    cubeMesh->SetMesh(Primitives::CreateCube());
    auto cubeRenderer = cubeObj->AddComponent<MeshRendererComponent>();
    cubeRenderer->SetShader(defaultShader);

    // 2. Sphere
    auto sphereObj = CreateGameObject("Sphere");
    auto sphereTransform = sphereObj->AddComponent<TransformComponent>();
    sphereTransform->position = glm::vec3(-6.0f, 0.0f, 0.0f);
    auto sphereMesh = sphereObj->AddComponent<MeshComponent>();
    sphereMesh->SetMesh(Primitives::CreateSphere(1.0f, 32));
    auto sphereRenderer = sphereObj->AddComponent<MeshRendererComponent>();
    sphereRenderer->SetShader(defaultShader);

    // 3. Plane
    auto planeObj = CreateGameObject("Plane");
    auto planeTransform = planeObj->AddComponent<TransformComponent>();
    planeTransform->position = glm::vec3(-2.0f, -1.0f, 0.0f);
    planeTransform->scale = glm::vec3(2.0f, 1.0f, 2.0f);
    auto planeMesh = planeObj->AddComponent<MeshComponent>();
    planeMesh->SetMesh(Primitives::CreatePlane(2.0f, 2.0f, 1));
    auto planeRenderer = planeObj->AddComponent<MeshRendererComponent>();
    planeRenderer->SetShader(defaultShader);

    // 4. Quad
    auto quadObj = CreateGameObject("Quad");
    auto quadTransform = quadObj->AddComponent<TransformComponent>();
    quadTransform->position = glm::vec3(2.0f, 1.0f, 0.0f);
    auto quadMesh = quadObj->AddComponent<MeshComponent>();
    quadMesh->SetMesh(Primitives::CreateQuad(2.0f, 1.0f));
    auto quadRenderer = quadObj->AddComponent<MeshRendererComponent>();
    quadRenderer->SetShader(defaultShader);

    // 5. Cylinder
    auto cylinderObj = CreateGameObject("Cylinder");
    auto cylinderTransform = cylinderObj->AddComponent<TransformComponent>();
    cylinderTransform->position = glm::vec3(6.0f, 0.0f, 0.0f);
    auto cylinderMesh = cylinderObj->AddComponent<MeshComponent>();
    cylinderMesh->SetMesh(Primitives::CreateCylinder(1.0f, 2.0f, 32));
    auto cylinderRenderer = cylinderObj->AddComponent<MeshRendererComponent>();
    cylinderRenderer->SetShader(defaultShader);

    // 6. Capsule
    auto capsuleObj = CreateGameObject("Capsule");
    auto capsuleTransform = capsuleObj->AddComponent<TransformComponent>();
    capsuleTransform->position = glm::vec3(10.0f, 0.0f, 0.0f);
    auto capsuleMesh = capsuleObj->AddComponent<MeshComponent>();
    capsuleMesh->SetMesh(Primitives::CreateCapsule(1.0f, 2.0f, 32));
    auto capsuleRenderer = capsuleObj->AddComponent<MeshRendererComponent>();
    capsuleRenderer->SetShader(defaultShader);

    // You'll need to position your camera farther back to see all objects
    // Consider using a z-position of around -15 to -20
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
