#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <vector>
#include <memory>
#include <string>
#include <algorithm>

class BaseComponent; // sadece işaretçi tutmak için forward declare

class GameObject
{
public:
    std::string name;
    bool isSelected = false;

    std::vector<std::shared_ptr<BaseComponent>> components;

    GameObject() = default;
    virtual ~GameObject() = default;

    // Template metodlar da mecburen header'da kalacak
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

    // Sadece imzalar (signature):
    virtual void Update(float deltaTime);
    virtual void Draw();
    virtual void OnInspectorGUI();
};

#endif
