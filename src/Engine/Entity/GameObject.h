#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <type_traits>
#include "Engine/Component/BaseComponent.h"

class GameObject final : public std::enable_shared_from_this<GameObject> {
public:
    std::string name;
    bool isSelected = false;
    bool active = true;
    std::vector<std::shared_ptr<BaseComponent> > components;

    // Use the public name property consistently
    const std::string &GetName() const { return name; }
    void SetName(const std::string &newName) { name = newName; }

    GameObject() = default;

    ~GameObject() = default;

    // Template methods
    template<typename T>
    std::shared_ptr<T> AddComponent() {
        static_assert(std::is_base_of_v<BaseComponent, T>, "T must derive from BaseComponent");

        // Create new component
        auto newComponent = std::make_shared<T>();
        newComponent->owner = shared_from_this();
        components.push_back(newComponent); // Use components instead of m_components

        // Initialize component
        newComponent->Start();

        return newComponent;
    }

    const std::vector<std::shared_ptr<BaseComponent>>& GetComponents() const {
        return components;  // Assuming 'components' is the member variable name
    }

    template<typename T>
    std::shared_ptr<T> GetComponent() const {
        static_assert(std::is_base_of_v<BaseComponent, T>, "T must derive from BaseComponent");

        for (const auto &component: components) {
            // Use components instead of m_components
            if (auto cast = std::dynamic_pointer_cast<T>(component)) return cast;
        }

        return nullptr;
    }

    template<typename T>
    bool RemoveComponent() {
        auto it = std::find_if(components.begin(), components.end(),
                               [](const std::shared_ptr<BaseComponent> &comp) {
                                   return std::dynamic_pointer_cast<T>(comp) != nullptr;
                               });

        if (it != components.end()) {
            components.erase(it);
            return true;
        }
        return false;
    }

    void AddChild(const std::shared_ptr<GameObject> &child);

    void RemoveChild(const std::shared_ptr<GameObject> &child);

    bool IsActive() const { return active; }
    void SetActive(bool isActive);

    const std::vector<std::shared_ptr<GameObject> > &GetChildren() const { return m_Children; }
    std::shared_ptr<GameObject> GetParent() const { return m_Parent.lock(); }

    void Update(float deltaTime);

    void Draw();

private:
    std::vector<std::shared_ptr<GameObject> > m_Children;
    std::weak_ptr<GameObject> m_Parent; // Weak reference to avoid circular dependencies
};

#endif
