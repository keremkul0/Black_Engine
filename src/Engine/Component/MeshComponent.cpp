#include "MeshComponent.h"
#include <filesystem>
#include "Engine/Render/Mesh/Mesh.h"
#include "TransformComponent.h"
#include "../Entity/GameObject.h"
#include <limits>
#include <iostream>

void MeshComponent::Start() {
    // Eğer mesh yolu belirtilmişse ve henüz yüklenmemişse
    if (!m_meshPath.empty() && !m_isLoaded) {
        LoadMesh(m_meshPath);
    }
    
    // Calculate bounding sphere during startup
    CalculateBoundingSphere();
}

void MeshComponent::Update(float deltaTime) {
    // If the transform has changed, mark the bounding sphere as dirty
    if (owner) {
        auto transform = owner->GetComponent<TransformComponent>();
        if (transform && transform->GetTransformDirty()) {
            m_boundingSphereDirty = true;
        }
    }
    
    // Recalculate bounding sphere if necessary
    if (m_boundingSphereDirty) {
        CalculateBoundingSphere();
    }
}

void MeshComponent::OnEnable() {
    // Bileşen aktif edildiğinde
}

void MeshComponent::OnDisable() {
    // Bileşen deaktif edildiğinde
}

bool MeshComponent::LoadMesh(const std::string &path) {
    try {
        // Yeni mesh oluştur
        const auto newMesh = std::make_shared<Mesh>();

        // LoadFromFile metodu yerine geçici bir çözüm
        // Gerçek uygulamada Mesh sınıfında uygun metot eklenmeli
        // Şimdilik her zaman başarılı kabul edelim
        m_mesh = newMesh;
        m_meshPath = path;
        m_isLoaded = true;
        
        // Mark the bounding sphere as dirty since we have a new mesh
        m_boundingSphereDirty = true;
        
        return true;
    } catch ([[maybe_unused]] const std::exception &e) {
        m_isLoaded = false;
    }

    return false;
}

bool MeshComponent::SetMesh(const std::shared_ptr<Mesh>& mesh) {
    if (mesh) {
        m_mesh = mesh;
        m_meshPath = ""; // Doğrudan ayarlandı, dosyadan yüklenmedi
        m_isLoaded = true;
        
        // Mark the bounding sphere as dirty since we have a new mesh
        m_boundingSphereDirty = true;
        
        return true;
    }
    m_isLoaded = false;
    return false;
}

void MeshComponent::CalculateBoundingSphere() {
    if (!m_mesh || !owner) {
        // Default to a unit sphere at origin if no mesh or owner
        m_boundingSphere.SetCenter(glm::vec3(0.0f));
        m_boundingSphere.SetRadius(1.0f);
        m_boundingSphereDirty = false;
        return;
    }
    
    auto transform = owner->GetComponent<TransformComponent>();
    if (!transform) {
        // Default to a unit sphere if no transform component
        m_boundingSphere.SetCenter(glm::vec3(0.0f));
        m_boundingSphere.SetRadius(1.0f);
        m_boundingSphereDirty = false;
        return;
    }
    
    // Get mesh vertices to calculate bounding sphere
    const auto& vertices = m_mesh->GetVertices();
    if (vertices.empty()) {
        // Default sphere if no vertices
        m_boundingSphere.SetCenter(transform->position);
        m_boundingSphere.SetRadius(1.0f);
        m_boundingSphereDirty = false;
        return;
    }
    
    // Calculate bounds in local space
    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
    
    for (const auto& vertex : vertices) {
        min = glm::min(min, vertex.position);
        max = glm::max(max, vertex.position);
    }
    
    // Calculate center and radius in local space
    glm::vec3 localCenter = (min + max) * 0.5f;
    float localRadius = glm::length(max - localCenter);
    
    // Transform to world space
    const glm::mat4& modelMatrix = transform->GetModelMatrix();
    glm::vec3 worldCenter = glm::vec3(modelMatrix * glm::vec4(localCenter, 1.0f));
    
    // Scale radius by the maximum scale component
    const glm::vec3& scale = transform->scale;
    float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
    float worldRadius = localRadius * maxScale;
    
    // Set the bounding sphere
    m_boundingSphere.SetCenter(worldCenter);
    m_boundingSphere.SetRadius(worldRadius);
    m_boundingSphereDirty = false;
}

bool MeshComponent::IntersectsRay(const Math::Ray& ray, float& distance) const {
    return IntersectsRay(ray.GetOrigin(), ray.GetDirection(), distance);
}

bool MeshComponent::IntersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const {
    if (!m_mesh || !owner) {
        std::cout << "[IntersectsRay] Mesh or owner is null" << std::endl;
        return false;
    }

    // Get the transform component from owner game object
    auto transform = owner->GetComponent<TransformComponent>();
    if (!transform) {
        std::cout << "[IntersectsRay] No transform component found" << std::endl;
        return false;
    }
    
    // First test against bounding sphere for quick rejection
    Math::Ray worldRay(rayOrigin, rayDirection);
    
    // BUG FIX: Convert bounding sphere to world space before testing
    glm::vec3 worldCenter = glm::vec3(transform->GetModelMatrix() * glm::vec4(m_boundingSphere.GetCenter(), 1.0f));
    float worldRadius = m_boundingSphere.GetRadius() * std::max(std::max(transform->scale.x, transform->scale.y), transform->scale.z);
    
    Math::BoundingSphere worldSphere(worldCenter, worldRadius);
    bool sphereHit = worldSphere.IntersectsRay(worldRay, distance);
    
    // Log bounding sphere test results
    std::cout << "  Bounding sphere test for " << owner->GetName() << ":" << std::endl;
    std::cout << "    Local Center: (" 
              << m_boundingSphere.GetCenter().x << ", " 
              << m_boundingSphere.GetCenter().y << ", " 
              << m_boundingSphere.GetCenter().z << "), Local Radius: " 
              << m_boundingSphere.GetRadius() << std::endl;
    std::cout << "    World Center: (" 
              << worldCenter.x << ", " 
              << worldCenter.y << ", " 
              << worldCenter.z << "), World Radius: " 
              << worldRadius << std::endl;
    std::cout << "    Result: " << (sphereHit ? "HIT" : "MISS") << std::endl;
    
    if (!sphereHit) {
        return false;
    }
    
    std::cout << "    Passed bounding sphere test, continuing to detailed intersection test..." << std::endl;
    
    // If bounding sphere test passes, proceed with detailed mesh intersection test
    
    // Get model matrix from transform
    const glm::mat4& modelMatrix = transform->GetModelMatrix();
    
    // Debug log to check the model matrix
    std::cout << "    Model matrix for " << owner->GetName() << ":\n" 
              << "    [" << modelMatrix[0][0] << ", " << modelMatrix[0][1] << ", " << modelMatrix[0][2] << ", " << modelMatrix[0][3] << "]\n"
              << "    [" << modelMatrix[1][0] << ", " << modelMatrix[1][1] << ", " << modelMatrix[1][2] << ", " << modelMatrix[1][3] << "]\n"
              << "    [" << modelMatrix[2][0] << ", " << modelMatrix[2][1] << ", " << modelMatrix[2][2] << ", " << modelMatrix[2][3] << "]\n"
              << "    [" << modelMatrix[3][0] << ", " << modelMatrix[3][1] << ", " << modelMatrix[3][2] << ", " << modelMatrix[3][3] << "]" << std::endl;
    
    // Get mesh data
    const auto& vertices = m_mesh->GetVertices();
    if (vertices.empty()) {
        std::cout << "    Mesh has no vertices" << std::endl;
        return false;
    }

    // Get indices for triangles
    const auto& indices = m_mesh->GetIndices();
    std::cout << "    Testing " << (indices.size() / 3) << " triangles" << std::endl;
    
    // Test ray against each triangle in the mesh
    bool hitFound = false;
    float closestHit = std::numeric_limits<float>::max();
    int triangleHitCount = 0;
    
    for (size_t i = 0; i < indices.size(); i += 3) {
        // Make sure we have valid indices
        if (i + 2 >= indices.size() || 
            indices[i] >= vertices.size() ||
            indices[i+1] >= vertices.size() ||
            indices[i+2] >= vertices.size()) {
            std::cout << "    Invalid triangle indices at " << i << std::endl;
            continue;
        }
        
        // Get the triangle vertices in world space
        glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i]].position, 1.0f));
        glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i+1]].position, 1.0f));
        glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[i+2]].position, 1.0f));
        
        // Möller–Trumbore intersection algorithm
        const glm::vec3 edge1 = v1 - v0;
        const glm::vec3 edge2 = v2 - v0;
        const glm::vec3 h = glm::cross(rayDirection, edge2);
        const float a = glm::dot(edge1, h);
        
        // Check if ray is parallel to triangle
        if (a > -0.00001f && a < 0.00001f) {
            continue;
        }
        
        const float f = 1.0f / a;
        const glm::vec3 s = rayOrigin - v0;
        const float u = f * glm::dot(s, h);
        
        // Check if hit point is outside triangle
        if (u < 0.0f || u > 1.0f) {
            continue;
        }
        
        const glm::vec3 q = glm::cross(s, edge1);
        const float v = f * glm::dot(rayDirection, q);
        
        // Check if hit point is outside triangle
        if (v < 0.0f || u + v > 1.0f) {
            continue;
        }
        
        // Compute distance to hit point
        const float t = f * glm::dot(edge2, q);
        
        // Check if hit point is in front of ray origin
        if (t > 0.00001f) {
            if (t < closestHit) {
                closestHit = t;
                hitFound = true;
                triangleHitCount++;
                
                // Log triangle hit details (only for the closest triangle so far)
                std::cout << "      Triangle hit at distance: " << t << std::endl;
            }
        }
    }
    
    // BUG FIX: Set the output distance parameter to the closest hit distance
    if (hitFound) {
        distance = closestHit;
        std::cout << "    Found " << triangleHitCount << " triangle intersections" << std::endl;
        std::cout << "    Closest hit at distance: " << closestHit << std::endl;
    } else {
        std::cout << "    No triangle intersections found" << std::endl;
    }
    
    return hitFound;
}
