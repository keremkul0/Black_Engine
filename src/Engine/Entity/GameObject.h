#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <type_traits>
#include "Engine/Component/BaseComponent.h"
#include "Core/Math/BoundingVolume.h" // Added for TransformedAABB
#include "Core/Math/Ray.h"            // Added for Ray

class GameObject final : public std::enable_shared_from_this<GameObject> {
public:
    std::string name;
    bool isSelected = false;
    bool active = true;
    std::vector<std::shared_ptr<BaseComponent> > components;

    // Use the public name property consistently
    const std::string &GetName() const { return name; }
    void SetName(const std::string &newName) { name = newName; }

    GameObject(); // Non-default constructor
    ~GameObject() = default;

    void SetParent(const std::shared_ptr<GameObject>& parent);

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
    }    void AddChild(const std::shared_ptr<GameObject> &child);
    void RemoveChild(const std::shared_ptr<GameObject> &child);    // Special helper method for adding a RigidBodyComponent and registering it with the physics world

    // Special helper method for removing a RigidBodyComponent and unregistering it from the physics world
    bool RemoveRigidBodyComponent();

    bool IsActive() const { return active; }
    void SetActive(bool isActive);

    const std::vector<std::shared_ptr<GameObject> > &GetChildren() const { return m_Children; }
    std::shared_ptr<GameObject> GetParent() const { return m_Parent.lock(); }

    void Update(float deltaTime);
    void Draw();
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    void Draw2ShadowMap(); // Added declaration for Draw2ShadowMap method
    //**//**//**//**//**//**//**//**//**//**//**//**//**//
    void DrawWireframe();  // Added declaration for DrawWireframe method

    /**
     * @brief Create or update the bounding box for this GameObject
     * This should be called whenever the mesh or transform changes
     */
    void UpdateBoundingBox();

    /**
     * @brief Check if a ray intersects with this GameObject
     * 
     * @param ray The ray to test against in world space
     * @param t Output parameter: distance along ray to intersection point (if hit)
     * @return true if the ray intersects the object
     */
    bool IntersectsRay(const Math::Ray& ray, float& t) const;

    /**
     * @brief Get the world-space AABB of this GameObject
     */
    const Math::AABB& GetWorldAABB() const { return m_BoundingBox.GetWorldAABB(); }

    /**
     * @brief Get the transformed AABB of this GameObject
     */
    const Math::TransformedAABB& GetTransformedAABB() const { return m_BoundingBox; }

private:
    std::vector<std::shared_ptr<GameObject> > m_Children;
    std::weak_ptr<GameObject> m_Parent; // Weak reference to avoid circular dependencies
    Math::TransformedAABB m_BoundingBox; // The transformed bounding box for this object
    bool m_BoundingBoxDirty = true;     // Flag indicating if the bounding box needs updating
};

#endif
