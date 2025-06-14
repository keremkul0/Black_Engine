#include "Scene.h"

#include "Engine/Component/MeshRendererComponent.h"
#include "Engine/Component/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Render/Shader/Shader.h"
#include "Engine/Render/Primitives/Primitives.h"
#include "Editor/SelectionManager.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include "../external/glfw/include/GLFW/glfw3.h"
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

    // Bullet Physics init
    m_Broadphase = new btDbvtBroadphase();
    m_CollisionConfiguration = new btDefaultCollisionConfiguration();
    m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
    m_Solver = new btSequentialImpulseConstraintSolver();
    m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_CollisionConfiguration);
    m_DynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

    // Set up shader for all objects
    const std::string shaderPath = "../src/shaders/";
    const auto defaultShader = std::make_shared<Shader>(
        (shaderPath + "simple.vert").c_str(),
        (shaderPath + "simple.frag").c_str()
    );


    /*const auto defaultShader0 = std::make_shared<Shader>(
        (shaderPath + "default.vert").c_str(),
        (shaderPath + "default.frag").c_str()
    );*/


/*
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
        material->SetShadowMapShader(m_ShadowMapProgram);
        material->SetShader(defaultShader0);
        auto planeTexture = std::make_shared<Texture>("../src/Engine/Render/Texture/TextureImages/planks.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        material->SetTexture(planeTexture);
        planeRenderer->SetMaterial(material);
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
        material->SetShadowMapShader(m_ShadowMapProgram);
        material->SetShader(defaultShader0);
        sphereRenderer->SetMaterial(material);
    }*/



    // 1. Cube
    auto cubeObj = CreateGameObject("Cube");
    auto cubeTransform = cubeObj->AddComponent<TransformComponent>();
    cubeTransform->position = glm::vec3(-10.0f, 0.5f, 0.0f);
    auto cubeMesh = cubeObj->AddComponent<MeshComponent>();
    cubeMesh->SetMesh(Primitives::CreateCube());
    auto cubeRenderer = cubeObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShadowMapShader(m_ShadowMapProgram);
        material->SetShader(m_DefaultShader);
        cubeRenderer->SetMaterial(material);
    }

    // 2. Sphere
    auto sphereObj = CreateGameObject("Sphere");
    auto sphereTransform = sphereObj->AddComponent<TransformComponent>();
    sphereTransform->position = glm::vec3(-6.0f, 5.0f, 0.0f);
    auto sphereMesh = sphereObj->AddComponent<MeshComponent>();
    sphereMesh->SetMesh(Primitives::CreateSphere(1.0f, 32));
    auto sphereRenderer = sphereObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShadowMapShader(m_ShadowMapProgram);
        material->SetShader(m_DefaultShader);
        sphereRenderer->SetMaterial(material);
    }

    // --- 1. Çarpışma şekli
    btCollisionShape* sphereShape = new btSphereShape(1.0f);

    // --- 2. Başlangıç transform'u (kürenin pozisyonu)
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(sphereTransform->position.x, sphereTransform->position.y, sphereTransform->position.z));

    // --- 3. Kütle ve atalet hesaplama
    btScalar mass = 1.0f;
    btVector3 inertia(0, 0, 0);
    sphereShape->calculateLocalInertia(mass, inertia);

    // --- 4. MotionState ve RigidBody oluştur
    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, sphereShape, inertia);
    btRigidBody* sphereBody = new btRigidBody(rbInfo);

    // --- 5. Fiziksel özellikler (opsiyonel)
    sphereBody->setDamping(0.1f, 0.1f); // biraz hava sürtünmesi
    sphereBody->setActivationState(DISABLE_DEACTIVATION); // hep aktif kalsın
    //sürtünme
    sphereBody->setFriction(1.0f);            // yüzeyle sürtünme
    sphereBody->setRollingFriction(0.3f);     // yuvarlanmaya etki eder

    // --- 6. Bullet dünyasına ekle
    m_DynamicsWorld->addRigidBody(sphereBody);

    // --- 7. Listeye kaydet (Update'de erişebilmek için)
    m_RigidBodies.push_back(sphereBody);


    // 3. Plane
    auto planeObj = CreateGameObject("Plane");
    auto planeTransform = planeObj->AddComponent<TransformComponent>();
    planeTransform->position = glm::vec3(-2.0f, 0.0f, 0.0f);
    planeTransform->scale = glm::vec3(20.0f, 0.0f, 20.0f);
    auto planeMesh = planeObj->AddComponent<MeshComponent>();
    planeMesh->SetMesh(Primitives::CreatePlane(2.0f, 2.0f, 1));
    auto planeRenderer = planeObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShadowMapShader(m_ShadowMapProgram);
        material->SetShader(m_DefaultShader);
        auto planeTexture = std::make_shared<Texture>("../src/Engine/Render/Texture/TextureImages/planks.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        material->SetTexture(planeTexture);
        planeRenderer->SetMaterial(material);
    }

    // Plane için rigidbody oluşturma
    btCollisionShape* planeShape = new btStaticPlaneShape(btVector3(0, 1, 0), -planeTransform->position.y);
    btTransform planeBtTransform;
    planeBtTransform.setIdentity();
    planeBtTransform.setOrigin(btVector3(planeTransform->position.x, planeTransform->position.y, planeTransform->position.z));

    btScalar planeMass = 0.0f;
    btVector3 planeInertia(0, 0, 0);

    btDefaultMotionState* planeMotionState = new btDefaultMotionState(planeBtTransform);
    btRigidBody::btRigidBodyConstructionInfo planeRbInfo(planeMass, planeMotionState, planeShape, planeInertia);
    btRigidBody* planeBody = new btRigidBody(planeRbInfo);

    //sürtünme
    planeBody->setFriction(0.8f);

    m_DynamicsWorld->addRigidBody(planeBody);
    m_RigidBodies.push_back(planeBody);

    // 4. Quad
    auto quadObj = CreateGameObject("Quad");
    auto quadTransform = quadObj->AddComponent<TransformComponent>();
    quadTransform->position = glm::vec3(2.0f, 1.0f, 0.0f);
    auto quadMesh = quadObj->AddComponent<MeshComponent>();
    quadMesh->SetMesh(Primitives::CreateQuad(2.0f, 1.0f));
    auto quadRenderer = quadObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShadowMapShader(m_ShadowMapProgram);
        material->SetShader(m_DefaultShader);
        auto quadTexture = std::make_shared<Texture>("../src/Engine/Render/Texture/TextureImages/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        material->SetTexture(quadTexture);
        quadRenderer->SetMaterial(material);
    }

    // 5. Cylinder
    auto cylinderObj = CreateGameObject("Cylinder");
    auto cylinderTransform = cylinderObj->AddComponent<TransformComponent>();
    cylinderTransform->position = glm::vec3(6.0f, 1.0f, 0.0f);
    auto cylinderMesh = cylinderObj->AddComponent<MeshComponent>();
    cylinderMesh->SetMesh(Primitives::CreateCylinder(1.0f, 2.0f, 32));
    auto cylinderRenderer = cylinderObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShadowMapShader(m_ShadowMapProgram);
        material->SetShader(m_DefaultShader);
        cylinderRenderer->SetMaterial(material);
    }

    // 6. Capsule
    auto capsuleObj = CreateGameObject("Capsule");
    auto capsuleTransform = capsuleObj->AddComponent<TransformComponent>();
    capsuleTransform->position = glm::vec3(10.0f, 1.0f, 0.0f);
    auto capsuleMesh = capsuleObj->AddComponent<MeshComponent>();
    capsuleMesh->SetMesh(Primitives::CreateCapsule(1.0f, 2.0f, 32));
    auto capsuleRenderer = capsuleObj->AddComponent<MeshRendererComponent>();
    {
        auto material = std::make_shared<Material>();
        material->SetShadowMapShader(m_ShadowMapProgram);
        material->SetShader(m_DefaultShader);
        capsuleRenderer->SetMaterial(material);
    }

    // You'll need to position your camera farther back to see all objects
    // Consider using a z-position of around -15 to -20
}
std::shared_ptr<GameObject> Scene::CreatePrimitive(const std::string& primitiveType) {
    const std::string shaderPath = "../src/shaders/";
    const auto defaultShader = std::make_shared<Shader>(
        (shaderPath + "simple.vert").c_str(),
        (shaderPath + "simple.frag").c_str()
    );
    const auto defaultShader0 = std::make_shared<Shader>(
        (shaderPath + "default.vert").c_str(),
        (shaderPath + "default.frag").c_str()
    );

    auto obj = CreateGameObject(primitiveType);

    // Her primitive için mutlaka bir TransformComponent ekle
    obj->AddComponent<TransformComponent>();

    auto mesh = obj->AddComponent<MeshComponent>();
    auto renderer = obj->AddComponent<MeshRendererComponent>();
    auto material = std::make_shared<Material>();
    material->SetShader(defaultShader0);

    if (primitiveType == "Cube") {
        mesh->SetMesh(Primitives::CreateCube());
    } else if (primitiveType == "Sphere") {
        mesh->SetMesh(Primitives::CreateSphere(1.0f, 32));
    } else if (primitiveType == "Plane") {
        auto transform = obj->GetComponent<TransformComponent>();
        if (transform) {
            transform->scale = glm::vec3(20.0f, 0.0f, 20.0f);
        }
        auto mesh = obj->GetComponent<MeshComponent>();
        if (mesh) {
            mesh->SetMesh(Primitives::CreatePlane(2.0f, 2.0f, 1));
        }
        auto renderer = obj->GetComponent<MeshRendererComponent>();
        if (renderer) {
            renderer->SetShader(defaultShader0);
        }
    } else if (primitiveType == "Quad") {
        mesh->SetMesh(Primitives::CreateQuad(2.0f, 1.0f));
        auto quadTexture = std::make_shared<Texture>("../src/Engine/Render/Texture/TextureImages/brick.png",
                                                      GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        material->SetTexture(quadTexture);
    } else if (primitiveType == "Cylinder") {
        mesh->SetMesh(Primitives::CreateCylinder(1.0f, 2.0f, 32));
    } else if (primitiveType == "Capsule") {
        mesh->SetMesh(Primitives::CreateCapsule(1.0f, 2.0f, 32));
    } else {
        std::cerr << "Unknown primitive: " << primitiveType << std::endl;
        return obj;
    }

    // Finish up
    renderer->SetMaterial(material);

    return obj;
}

// Overload: CreatePrimitive with position
std::shared_ptr<GameObject> Scene::CreatePrimitive(const std::string& primitiveType, const glm::vec3& position) {
    auto obj = CreatePrimitive(primitiveType);
    if (obj) {
        auto transform = obj->GetComponent<TransformComponent>();
        if (transform) {
            transform->SetPosition(position);
        }
    }
    return obj;
}

bool Scene::LoadSceneFromFile(const std::string &path) {
    // TODO: Implement this
    return false;
}

void Scene::UpdateAll(const float dt) {

    // Fizik dünyasını güncelle
    if (m_DynamicsWorld)
        m_DynamicsWorld->stepSimulation(dt);

    for (const auto &obj: m_GameObjects) {
        obj->Update(dt);
    }

    if (!m_RigidBodies.empty()) {
        btTransform trans;
        m_RigidBodies[0]->getMotionState()->getWorldTransform(trans);
        btVector3 pos = trans.getOrigin();

        auto sphereTransform = m_GameObjects[1]->GetComponent<TransformComponent>();
        sphereTransform->SetPosition(glm::vec3(pos.getX(), pos.getY(), pos.getZ()));
        btQuaternion rot = trans.getRotation();
        glm::quat glmRot(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
        glm::vec3 euler = glm::eulerAngles(glmRot);
        sphereTransform->SetRotation(glm::degrees(euler));
    }


    // Kuvveti uygula (her karede sağa doğru)
    //m_RigidBodies[0]->applyCentralForce(btVector3(11.0f, 0.0f, 0.0f)); // sağa doğru kuvvet

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
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
void Scene::DrawAll2ShadowMap() {

    for (const auto &obj: m_GameObjects) {
        obj->Draw2ShadowMap();
    }
}
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//

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


void Scene::ProcessInput(const InputEvent& event) {
    if (event.type != InputEventType::KeyHeld) return;
    if (m_RigidBodies.empty()) return;

    auto* body = m_RigidBodies[0]; // İlk rigidbody top olarak kabul
    // Lokal yönlere göre dünya uzayı vektörleri
    btVector3 localForward = btVector3(0, 0, -1);
    btVector3 localBackward = btVector3(0, 0, 1);
    btVector3 localLeft = btVector3(-1, 0, 0);
    btVector3 localRight = btVector3(1, 0, 0);

    float forceMag = 11.0f;
    btVector3 force(0, 0, 0);

    switch (event.key) {
        case GLFW_KEY_W: force += localForward * forceMag; break;
        case GLFW_KEY_S: force += localBackward * forceMag; break;
        case GLFW_KEY_A: force += localLeft * forceMag; break;
        case GLFW_KEY_D: force += localRight * forceMag; break;
    }

    if (force.length2() > 0.0f) {
        body->activate(); // Uyandıralım
        body->applyCentralForce(force);
    }
}
