#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "BaseComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TransformComponent final : public BaseComponent
{
private:
    // Model matrisi önbellekleme için
    mutable glm::mat4 cachedModelMatrix = glm::mat4(1.0f);
    mutable bool matrixDirty = true;

public:
    glm::vec3 position {0.f, 0.f, 0.f};
    glm::vec3 rotation {0.f, 0.f, 0.f}; // Euler açı (derece)
    glm::vec3 scale    {1.f, 1.f, 1.f};

    TransformComponent() = default;
    ~TransformComponent() override = default;

    // Position setter
    void SetPosition(const glm::vec3& newPosition) {
        position = newPosition;
        matrixDirty = true;
    }

    // Rotation setter
    void SetRotation(const glm::vec3& newRotation) {
        rotation = newRotation;
        matrixDirty = true;
    }

    // Scale setter
    void SetScale(const glm::vec3& newScale) {
        scale = newScale;
        matrixDirty = true;
    }

    // Model matrix hesapla - önbellekleyen versiyon
    glm::mat4 GetModelMatrix() const;

    // Geçersiz kılınan metotlar
    void Start() override;
    void Update(float deltaTime) override;
    void OnEnable() override;
    void OnDisable() override;
    void OnInspectorGUI() override;

    // Bileşen tipi bilgisi
    [[nodiscard]] const char* GetTypeName() const override { return "TransformComponent"; }

private:
    // Model matrisini yeniden hesapla
    void RecalculateModelMatrix() const;
};

#endif // TRANSFORM_COMPONENT_H