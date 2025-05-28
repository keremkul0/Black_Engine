#include "GameObject.h"
#include "../Component/BaseComponent.h"
#include "../Component/TransformComponent.h"
#include "../Component/MeshComponent.h"
#include "../Component/MeshRendererComponent.h"
#include <iostream>

// Initialize GameObject with a default constructor that creates a default bounding box
GameObject::GameObject() : name("GameObject"), isSelected(false), active(true) {
    // Create a default local AABB with a small size
    Math::AABB localAABB(glm::vec3(-0.5f), glm::vec3(0.5f));
    m_BoundingBox.SetLocalAABB(localAABB);
    m_BoundingBoxDirty = true;
}

void GameObject::Update(float deltaTime) {
    if (!active) return;
    
    // Check if we need to update the bounding box
    auto transform = GetComponent<TransformComponent>();
    if (transform && transform->GetTransformDirty()) {
        UpdateBoundingBox();
        transform->ClearTransformDirty();
    }

    for (const auto& comp : components) {
        comp->Update(deltaTime);
    }

    // Update children recursively
    for (const auto& child : m_Children) {
        child->Update(deltaTime);
    }
}

void GameObject::Draw() {
    if (!active) return;
    
    for (const auto& comp : components) {
        comp->Draw();
    }

    // Draw children recursively
    for (const auto& child : m_Children) {
        child->Draw();
    }
}

void GameObject::DrawWireframe() {
    if (!active) return;
    
    // Draw this object's components in wireframe mode
    for (const auto& comp : components) {
        comp->DrawWireframe();
    }

    // Draw children recursively
    for (const auto& child : m_Children) {
        child->DrawWireframe();
    }
}

void GameObject::AddChild(const std::shared_ptr<GameObject>& child) {
    if (!child) return;
    
    // Eğer child zaten bu nesnenin çocuğuysa, bir şey yapma
    for (const auto& existingChild : m_Children) {
        if (existingChild == child) {
            return;
        }
    }
    
    // Eğer child başka bir nesnenin çocuğuysa, eski parent'tan kaldır
    auto oldParent = child->GetParent();
    if (oldParent && oldParent != shared_from_this()) {
        oldParent->RemoveChild(child);
    }
    
    // Child'ın parent'ını bu nesne olarak ayarla
    child->m_Parent = shared_from_this();
    
    // Bu nesnenin çocuk listesine ekle
    m_Children.push_back(child);
}

void GameObject::RemoveChild(const std::shared_ptr<GameObject>& child) {
    if (!child) return;

    auto it = std::find(m_Children.begin(), m_Children.end(), child);
    if (it != m_Children.end()) {
        // Child'ın parent referansını temizle
        (*it)->m_Parent.reset();
        
        // Listeden kaldır
        m_Children.erase(it);
    }
}

void GameObject::SetParent(const std::shared_ptr<GameObject>& parent) {
    // Eğer parent aynıysa hiçbir şey yapma
    if (m_Parent.lock() == parent) {
        return;
    }
    
    // Eski parent'tan kendimizi kaldır
    if (auto oldParent = m_Parent.lock()) {
        oldParent->RemoveChild(shared_from_this());
    }

    // Yeni parent'ı ayarla
    m_Parent = parent;

    // Eğer yeni bir parent varsa kendimizi onun çocuklarına ekle
    if (parent) {
        bool alreadyChild = false;
        for (const auto& child : parent->m_Children) {
            if (child.get() == this) {
                alreadyChild = true;
                break;
            }
        }

        if (!alreadyChild) {
            parent->m_Children.push_back(shared_from_this());
        }
    }
}

void GameObject::SetActive(bool isActive) {
    active = isActive;
    
    // Çocukları da aktif/pasif hale getir
    for (const auto& child : m_Children) {
        child->SetActive(isActive);
    }
    
    // Bileşenleri de aktif/pasif hale getir
    for (const auto& component : components) {
        component->SetEnabled(isActive);
    }
}

void GameObject::UpdateBoundingBox() {
    if (!IsActive()) return;

    auto transform = GetComponent<TransformComponent>();
    if (!transform) return;

    // Get the world transform matrix
    glm::mat4 worldTransform = transform->GetModelMatrix();
    
    // Create or update the local AABB based on mesh components
    auto meshComp = GetComponent<MeshComponent>();
    auto meshRendererComp = GetComponent<MeshRendererComponent>();
    
    if (meshComp) {
        // Use the mesh's AABB directly if available
        auto mesh = meshComp->GetMesh();
        if (mesh) {
            glm::vec3 min = mesh->GetMinBounds();
            glm::vec3 max = mesh->GetMaxBounds();
            Math::AABB localAABB(min, max);
            m_BoundingBox.SetLocalAABB(localAABB);
        }
    } else if (meshRendererComp) {
        // If using MeshRendererComponent, try to get bounds from it
        auto mesh = meshRendererComp->GetMesh();
        if (mesh) {
            glm::vec3 min = mesh->GetMinBounds();
            glm::vec3 max = mesh->GetMaxBounds();
            Math::AABB localAABB(min, max);
            m_BoundingBox.SetLocalAABB(localAABB);
        }
    }
    
    // Update the transform of the bounding box
    m_BoundingBox.UpdateTransform(worldTransform);
    m_BoundingBoxDirty = false;
}

bool GameObject::IntersectsRay(const Math::Ray& ray, float& t) const {
    if (!IsActive()) return false;
    
    // Eğer bounding box güncellenmediyse, güncelle
    if (m_BoundingBoxDirty) {
        const_cast<GameObject*>(this)->UpdateBoundingBox();
    }
    
    // Check intersection with this object's bounding box
    if (m_BoundingBox.IntersectsRay(ray, t)) {
        return true;
    }
    
    // Check intersections with children
    for (const auto& child : m_Children) {
        float childT;
        if (child->IntersectsRay(ray, childT)) {
            // If this child is closer than the current closest hit, update t
            if (childT < t) {
                t = childT;
            }
            return true;
        }
    }
    
    return false;
}