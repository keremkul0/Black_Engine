
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <vector>
#include <memory> // This includes std::enable_shared_from_this
#include <string>
#include <algorithm>

class BaseComponent; // forward declaration

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
    std::string name;
    bool isSelected = false;

    std::vector<std::shared_ptr<BaseComponent>> components;

    // Use the public name property consistently
    const std::string& GetName() const { return name; }

    GameObject() = default;
    virtual ~GameObject() = default;

    // Template methods
    template<typename T, typename... TArgs>
    std::shared_ptr<T> AddComponent(TArgs&&... args) {
        auto component = std::make_shared<T>(std::forward<TArgs>(args)...);
        component->owner = this;
        components.push_back(component);
        component->Start();
        return component;
    }

    template<typename T>
    std::shared_ptr<T> GetComponent() const {
        for (const auto& comp : components) {
            if (auto castedComp = std::dynamic_pointer_cast<T>(comp)) {
                return castedComp;
            }
        }
        return nullptr;
    }

    template<typename T>
    bool RemoveComponent() {
        auto it = std::find_if(components.begin(), components.end(),
            [](const std::shared_ptr<BaseComponent>& comp) {
                return std::dynamic_pointer_cast<T>(comp) != nullptr;
            });

        if (it != components.end()) {
            components.erase(it);
            return true;
        }
        return false;
    }

    void AddChild(std::shared_ptr<GameObject> child);
    void RemoveChild(std::shared_ptr<GameObject> child);
    const std::vector<std::shared_ptr<GameObject>>& GetChildren() const { return m_Children; }
    std::shared_ptr<GameObject> GetParent() const { return m_Parent.lock(); }

    virtual void Update(float deltaTime);
    virtual void Draw();
    virtual void OnInspectorGUI();

private:
    // Removing m_Name as it's redundant with public name field
    std::vector<std::shared_ptr<GameObject>> m_Children;
    std::weak_ptr<GameObject> m_Parent; // Weak reference to avoid circular dependencies
};

#endif