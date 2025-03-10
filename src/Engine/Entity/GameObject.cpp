#include "GameObject.h"
#include "../Component/BaseComponent.h"

void GameObject::Update(const float deltaTime)
{
    for (const auto& comp : components)
    {
        comp->Update(deltaTime);
    }

    // Update children recursively
    for (const auto& child : m_Children)
    {
        child->Update(deltaTime);
    }
}

void GameObject::Draw()
{
    for (const auto& comp : components)
    {
        comp->Draw();
    }

    // Draw children recursively
    for (const auto& child : m_Children)
    {
        child->Draw();
    }
}

void GameObject::OnInspectorGUI()
{
    for (const auto& comp : components)
    {
        comp->OnInspectorGUI();
    }
}

void GameObject::AddChild(const std::shared_ptr<GameObject>& child) {
    if (child) {
        // If child has another parent, remove from that parent first
        if (const auto parentPtr = child->GetParent()) {
            parentPtr->RemoveChild(child);
        }

        m_Children.push_back(child);
        child->m_Parent = shared_from_this();
    }
}

void GameObject::RemoveChild(const std::shared_ptr<GameObject>& child) {
    if (!child) return;

    if (const auto it = std::ranges::find(m_Children, child); it != m_Children.end()) {
        child->m_Parent.reset(); // Clear the parent reference
        m_Children.erase(it);
    }
}