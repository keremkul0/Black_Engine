#ifndef GAME_OBJECT_H
    #define GAME_OBJECT_H

    #include <vector>
    #include <memory>
    #include <string>
    #include <algorithm>
    #include <type_traits>
    #include "Engine/Component/BaseComponent.h"

    class GameObject : public std::enable_shared_from_this<GameObject> {
    public:
        std::string name;
        bool isSelected = false;

        std::vector<std::shared_ptr<BaseComponent>> components;

        // Use the public name property consistently
        const std::string &GetName() const { return name; }

        GameObject() = default;

        virtual ~GameObject() = default;

        // Template methods
        template<typename T>
        std::shared_ptr<T> AddComponent() {
            static_assert(std::is_base_of<BaseComponent, T>::value, "T must derive from BaseComponent");

            // Create new component
            auto newComponent = std::make_shared<T>();
            newComponent->owner = shared_from_this();
            components.push_back(newComponent);  // Use components instead of m_components

            // Initialize component
            newComponent->Start();

            return newComponent;
        }

        template<typename T>
        std::shared_ptr<T> GetComponent() const {
            static_assert(std::is_base_of_v<BaseComponent, T>, "T must derive from BaseComponent");

            for (const auto& component : components) {  // Use components instead of m_components
                auto cast = std::dynamic_pointer_cast<T>(component);
                if (cast) return cast;
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

        void AddChild(std::shared_ptr<GameObject> child);

        void RemoveChild(std::shared_ptr<GameObject> child);

        const std::vector<std::shared_ptr<GameObject>> &GetChildren() const { return m_Children; }
        std::shared_ptr<GameObject> GetParent() const { return m_Parent.lock(); }

        virtual void Update(float deltaTime);

        virtual void Draw();

        virtual void OnInspectorGUI();

    private:
        std::vector<std::shared_ptr<GameObject>> m_Children;
        std::weak_ptr<GameObject> m_Parent; // Weak reference to avoid circular dependencies
    };

    #endif