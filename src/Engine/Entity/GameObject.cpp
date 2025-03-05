// GameObject.cpp
#include "GameObject.h"
#include "../Component/BaseComponent.h"

void GameObject::Update(float deltaTime)
{
    for (auto& comp : components)
    {
        comp->Update(deltaTime);
    }

    // Update children recursively
    for (auto& child : m_Children)
    {
        child->Update(deltaTime);
    }
}

void GameObject::Draw()
{
    for (auto& comp : components)
    {
        comp->Draw();
    }

    // Draw children recursively
    for (auto& child : m_Children)
    {
        child->Draw();
    }
}

void GameObject::OnInspectorGUI()
{
    for (auto& comp : components)
    {
        comp->OnInspectorGUI();
    }
}

void GameObject::AddChild(std::shared_ptr<GameObject> child) {
    if (child) {
        // If child has another parent, remove from that parent first
        if (auto parentPtr = child->GetParent()) {
            parentPtr->RemoveChild(child);
        }

        m_Children.push_back(child);
        child->m_Parent = shared_from_this();
    }
}

void GameObject::RemoveChild(std::shared_ptr<GameObject> child) {
    if (!child) return;

    auto it = std::find(m_Children.begin(), m_Children.end(), child);
    if (it != m_Children.end()) {
        child->m_Parent.reset(); // Clear the parent reference
        m_Children.erase(it);
    }
}