#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "BaseComponent.h"
#include <glm/glm.hpp>      // Eğer GLM kullanıyorsanız
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class TransformComponent : public BaseComponent
{
public:
    glm::vec3 position  {0.f, 0.f, 0.f};
    glm::vec3 rotation  {0.f, 0.f, 0.f}; // Euler açıları (derece cinsinden tutabiliriz)
    glm::vec3 scale     {1.f, 1.f, 1.f};

public:
    TransformComponent() = default;
    virtual ~TransformComponent() = default;

    // Transform'la ilgili hesaplamalar
    // (Mesela bir "GetModelMatrix()" fonksiyonu)
    [[nodiscard]] glm::mat4 GetModelMatrix() const;

    // override metodlar
    void Start() override;
    void Update(float deltaTime) override;
    void OnInspectorGUI() override;
};

#endif
