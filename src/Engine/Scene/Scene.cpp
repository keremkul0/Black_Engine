#include "Scene.h"

#include "Engine/Component/MeshRendererComponent.h"
#include "Engine/Component/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Render/Shader/Shader.h"
#include "Engine/Render/Primitives/Primitives.h"
#include "Editor/SelectionManager.h"
#include <iostream>
#include <algorithm>

// Initialize static member
Scene* Scene::s_ActiveScene = nullptr;

// Singleton implementation
Scene& Scene::Get() {
    if (!s_ActiveScene) {
        s_ActiveScene = new Scene();
    }
    return *s_ActiveScene;
}

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


    const std::string shaderPath0 = "../src/shaders/";
    const auto defaultShader0 = std::make_shared<Shader>(
        (shaderPath + "default.vert").c_str(),
        (shaderPath + "default.frag").c_str()
    );

    // 1. Cube
    auto cubeObj = CreateGameObject("Cube");
    auto cubeTransform = cubeObj->AddComponent<TransformComponent>();
    cubeTransform->position = glm::vec3(-10.0f, 0.0f, 0.0f);
    auto cubeMesh = cubeObj->AddComponent<MeshComponent>();
    cubeMesh->SetMesh(Primitives::CreateCube());
    auto cubeRenderer = cubeObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShader(defaultShader0);
        cubeRenderer->SetMaterial(material);
    }

    // 2. Sphere
    auto sphereObj = CreateGameObject("Sphere");
    auto sphereTransform = sphereObj->AddComponent<TransformComponent>();
    sphereTransform->position = glm::vec3(-6.0f, 0.0f, 0.0f);
    auto sphereMesh = sphereObj->AddComponent<MeshComponent>();
    sphereMesh->SetMesh(Primitives::CreateSphere(1.0f, 32));
    auto sphereRenderer = sphereObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShader(defaultShader0);
        sphereRenderer->SetMaterial(material);
    }

    // 3. Plane
    auto planeObj = CreateGameObject("Plane");
    auto planeTransform = planeObj->AddComponent<TransformComponent>();
    planeTransform->position = glm::vec3(-2.0f, -1.0f, 0.0f);
    planeTransform->scale = glm::vec3(20.0f, 0.0f, 20.0f);
    auto planeMesh = planeObj->AddComponent<MeshComponent>();
    planeMesh->SetMesh(Primitives::CreatePlane(2.0f, 2.0f, 1));
    auto planeRenderer = planeObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShader(defaultShader0);
        auto planeTexture = std::make_shared<Texture>("../src/Engine/Render/Texture/TextureImages/planks.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        material->SetTexture(planeTexture);
        planeRenderer->SetMaterial(material);
    }

    // 4. Quad
    auto quadObj = CreateGameObject("Quad");
    auto quadTransform = quadObj->AddComponent<TransformComponent>();
    quadTransform->position = glm::vec3(2.0f, 1.0f, 0.0f);
    auto quadMesh = quadObj->AddComponent<MeshComponent>();
    quadMesh->SetMesh(Primitives::CreateQuad(2.0f, 1.0f));
    auto quadRenderer = quadObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShader(defaultShader0);
        auto quadTexture = std::make_shared<Texture>("../src/Engine/Render/Texture/TextureImages/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        material->SetTexture(quadTexture);
        quadRenderer->SetMaterial(material);
    }

    // 5. Cylinder
    auto cylinderObj = CreateGameObject("Cylinder");
    auto cylinderTransform = cylinderObj->AddComponent<TransformComponent>();
    cylinderTransform->position = glm::vec3(6.0f, 0.0f, 0.0f);
    auto cylinderMesh = cylinderObj->AddComponent<MeshComponent>();
    cylinderMesh->SetMesh(Primitives::CreateCylinder(1.0f, 2.0f, 32));
    auto cylinderRenderer = cylinderObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShader(defaultShader0);
        cylinderRenderer->SetMaterial(material);
    }

    // 6. Capsule
    auto capsuleObj = CreateGameObject("Capsule");
    auto capsuleTransform = capsuleObj->AddComponent<TransformComponent>();
    capsuleTransform->position = glm::vec3(10.0f, 0.0f, 0.0f);
    auto capsuleMesh = capsuleObj->AddComponent<MeshComponent>();
    capsuleMesh->SetMesh(Primitives::CreateCapsule(1.0f, 2.0f, 32));
    auto capsuleRenderer = capsuleObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShader(defaultShader0);
        capsuleRenderer->SetMaterial(material);
    }

    // You'll need to position your camera farther back to see all objects
    // Consider using a z-position of around -15 to -20
}

std::shared_ptr<GameObject> Scene::CreatePrimitive(const std::string& primitiveType) {
    // Get default shader
    const std::string shaderPath = "../src/shaders/";
    const auto defaultShader = std::make_shared<Shader>(
        (shaderPath + "simple.vert").c_str(),
        (shaderPath + "simple.frag").c_str()
    );
    
    std::shared_ptr<GameObject> newObject;
    
    // Create based on primitive type
    if (primitiveType == "Empty") {
        newObject = CreateGameObject("New GameObject");
        newObject->AddComponent<TransformComponent>();
    }
    else if (primitiveType == "Cube") {
        newObject = CreateGameObject("Cube");
        auto transform = newObject->AddComponent<TransformComponent>();
        auto mesh = newObject->AddComponent<MeshComponent>();
        mesh->SetMesh(Primitives::CreateCube());
        auto renderer = newObject->AddComponent<MeshRendererComponent>();
        renderer->SetShader(defaultShader);
    }
    else if (primitiveType == "Sphere") {
        newObject = CreateGameObject("Sphere");
        auto transform = newObject->AddComponent<TransformComponent>();
        auto mesh = newObject->AddComponent<MeshComponent>();
        mesh->SetMesh(Primitives::CreateSphere(1.0f, 32));
        auto renderer = newObject->AddComponent<MeshRendererComponent>();
        renderer->SetShader(defaultShader);
    }
    else if (primitiveType == "Plane") {
        newObject = CreateGameObject("Plane");
        auto transform = newObject->AddComponent<TransformComponent>();
        auto mesh = newObject->AddComponent<MeshComponent>();
        mesh->SetMesh(Primitives::CreatePlane(2.0f, 2.0f, 1));
        auto renderer = newObject->AddComponent<MeshRendererComponent>();
        renderer->SetShader(defaultShader);
    }
    else if (primitiveType == "Quad") {
        newObject = CreateGameObject("Quad");
        auto transform = newObject->AddComponent<TransformComponent>();
        auto mesh = newObject->AddComponent<MeshComponent>();
        mesh->SetMesh(Primitives::CreateQuad(2.0f, 2.0f));
        auto renderer = newObject->AddComponent<MeshRendererComponent>();
        renderer->SetShader(defaultShader);
    }
    else if (primitiveType == "Cylinder") {
        newObject = CreateGameObject("Cylinder");
        auto transform = newObject->AddComponent<TransformComponent>();
        auto mesh = newObject->AddComponent<MeshComponent>();
        mesh->SetMesh(Primitives::CreateCylinder(1.0f, 2.0f, 32));
        auto renderer = newObject->AddComponent<MeshRendererComponent>();
        renderer->SetShader(defaultShader);
    }
   /* else if (primitiveType == "Cone") {
        newObject = CreateGameObject("Cone");
        auto transform = newObject->AddComponent<TransformComponent>();
        auto mesh = newObject->AddComponent<MeshComponent>();
        mesh->SetMesh(Primitives::CreateCone(1.0f, 2.0f, 32));
        auto renderer = newObject->AddComponent<MeshRendererComponent>();
        renderer->SetShader(defaultShader);
    }*/
    else if (primitiveType == "Capsule") {
        newObject = CreateGameObject("Capsule");
        auto transform = newObject->AddComponent<TransformComponent>();
        auto mesh = newObject->AddComponent<MeshComponent>();
        mesh->SetMesh(Primitives::CreateCapsule(1.0f, 2.0f, 32));
        auto renderer = newObject->AddComponent<MeshRendererComponent>();
        renderer->SetShader(defaultShader);
    }
    else if (primitiveType == "DirectionalLight") {
        // Implement directional light creation
        newObject = CreateGameObject("Directional Light");
        auto transform = newObject->AddComponent<TransformComponent>();
        // Add light component logic here
    }
    else if (primitiveType == "PointLight") {
        // Implement point light creation
        newObject = CreateGameObject("Point Light");
        auto transform = newObject->AddComponent<TransformComponent>();
        // Add light component logic here
    }
    else if (primitiveType == "SpotLight") {
        // Implement spot light creation
        newObject = CreateGameObject("Spot Light");
        auto transform = newObject->AddComponent<TransformComponent>();
        // Add light component logic here
    }
    else if (primitiveType == "Camera") {
        // Implement camera creation
        newObject = CreateGameObject("Camera");
        auto transform = newObject->AddComponent<TransformComponent>();
        // Add camera component logic here
    }
    else {
        // Default empty object if type not recognized
        newObject = CreateGameObject("Unknown Object");
        auto transform = newObject->AddComponent<TransformComponent>();
    }
    
    // Return the newly created object
    return newObject;
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
    // Set the camera position for shaders if a camera is attached
    if (m_Camera) {
        Material::SetCameraPosition(m_Camera->GetPosition());
        // Updated line to correctly print camera position:

    }
    for (const auto &obj: m_GameObjects) {
        obj->Draw();
    }
}

bool Scene::HasGameObject(const std::shared_ptr<GameObject>& obj) const {
    for (const auto& existing : m_GameObjects) {
        if (existing == obj) return true;
    }
    return false;
}

void Scene::RemoveGameObject(const std::shared_ptr<GameObject>& gameObject) {
    if (!gameObject) {
        std::cout << "Hata: Silinecek nesne null!" << std::endl;
        return;
    }

    // Silmeden önce GameObject'in adını kaydedin
    std::string objName = gameObject->GetName();
    std::cout << "Nesneyi silme işlemi başlatılıyor: " << objName << std::endl;

    // 1. Seçili nesne kontrolü
    SelectionManager& selectionManager = SelectionManager::GetInstance();
    if (selectionManager.GetSelectedObject() == gameObject) {
        selectionManager.ClearSelection();
        std::cout << "Nesne seçimi temizlendi" << std::endl;
    }

    // 2. Kök seviyede mi kontrol et
    auto it = std::find(m_GameObjects.begin(), m_GameObjects.end(), gameObject);
    if (it != m_GameObjects.end()) {
        // Nesnenin çocukları varsa işlemden önce çocuklar için güvenli temizleme
        auto children = gameObject->GetChildren();

        // Güvenlik kontrolü: children vektörünü kopyalayalım çünkü işlem sırasında değişebilir
        std::vector<std::shared_ptr<GameObject>> childrenCopy = children;

        // Önce tüm çocukları sil
        for (const auto& child : childrenCopy) {
            // Recursive olarak her çocuğu sil
            RemoveGameObject(child);
        }

        // Artık nesneyi listeden güvenle kaldırabiliriz
        m_GameObjects.erase(it);
        std::cout << "Nesne başarıyla silindi: " << objName << std::endl;
        return;
    }

    // 3. Eğer kök seviyede değilse, parent-child ilişkisini kaldır
    auto parent = gameObject->GetParent();
    if (parent) {
        // Parent'tan çocuğu kaldır
        parent->RemoveChild(gameObject);
        std::cout << "Nesne, parent'ından ayrıldı: " << objName << std::endl;
    } else {
        std::cout << "Uyarı: Nesne bulunamadı veya zaten silinmiş: " << objName << std::endl;
    }
}

// Ray casting for object selection
std::shared_ptr<GameObject> Scene::PickObjectWithRay(const Math::Ray& ray) const {
    // If no objects in scene, return nullptr
    if (m_GameObjects.empty()) {
        std::cout << "[PickObjectWithRay] No objects in the scene." << std::endl;
        return nullptr;
    }
    
    std::shared_ptr<GameObject> closestObject = nullptr;
    float closestDistance = std::numeric_limits<float>::max();
    
    // Log all the objects that will be tested
    std::cout << "Testing ray intersection with " << m_GameObjects.size() << " objects:" << std::endl;
    
    // Iterate through all game objects
    for (const auto& gameObject : m_GameObjects) {
        // Skip inactive objects
        if (!gameObject->IsActive()) {
            std::cout << "  Skipping inactive object: " << gameObject->GetName() << std::endl;
            continue;
        }
        
        std::cout << "  Testing object: " << gameObject->GetName() << std::endl;
        
        // Test ray intersection with this object's transformed AABB
        float hitDistance = std::numeric_limits<float>::max();
        bool hit = false;
        
        try {
            hit = gameObject->IntersectsRay(ray, hitDistance);
        }
        catch (const std::exception& e) {
            std::cerr << "    Exception during intersection test: " << e.what() << std::endl;
            continue;
        }
        catch (...) {
            std::cerr << "    Unknown exception during intersection test" << std::endl;
            continue;
        }
        
        if (hit) {
            std::cout << "    HIT at distance: " << hitDistance << std::endl;
            
            // Keep track of the closest hit
            if (hitDistance < closestDistance) {
                closestDistance = hitDistance;
                closestObject = gameObject;
                std::cout << "    New closest object: " << gameObject->GetName() << 
                          " (distance: " << hitDistance << ")" << std::endl;
            }
        } else {
            std::cout << "    No intersection" << std::endl;
        }
    }
    
    if (closestObject) {
        std::cout << "Final selected object: " << closestObject->GetName() << 
                  " at distance " << closestDistance << std::endl;
    } else {
        std::cout << "No object was hit by the ray" << std::endl;
    }
    
    return closestObject;
}

std::shared_ptr<GameObject> Scene::PickObjectWithRay(const glm::vec3& origin, const glm::vec3& direction) const {
    // Create a Ray object from origin and direction
    Math::Ray ray(origin, glm::normalize(direction));
    return PickObjectWithRay(ray);
}
