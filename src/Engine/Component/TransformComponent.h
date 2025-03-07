#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "BaseComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TransformComponent : public BaseComponent
{
public:
    glm::vec3 position {0.f, 0.f, 0.f};
    glm::vec3 rotation {0.f, 0.f, 0.f}; // Euler açı (derece)
    glm::vec3 scale    {1.f, 1.f, 1.f};

    TransformComponent() = default;
    virtual ~TransformComponent() = default;

    // Model matrix hesapla
    glm::mat4 GetModelMatrix() const;

    void Start() override;
    void Update(float deltaTime) override;
    void OnInspectorGUI() override; // Eğer ImGui ile ayarlayacaksak
};

#endif // TRANSFORM_COMPONENT_H
