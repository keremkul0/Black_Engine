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
    bool transformDirty = true; // Track any changes to transform properties

public:
    glm::vec3 position {0.f, 0.f, 0.f};
    glm::vec3 rotation {0.f, 0.f, 0.f}; // Euler açı (derece)
    glm::vec3 scale    {1.f, 1.f, 1.f};

    TransformComponent() = default;
    ~TransformComponent() override = default;
   
    
    // Get transform dirty flag
    [[nodiscard]] bool GetTransformDirty() const { return transformDirty; }
    
    // Clear dirty flag after consumers have updated
    void ClearTransformDirty() { transformDirty = false; }
    
    // Position setter
    void SetPosition(const glm::vec3& newPosition) {
        position = newPosition;
        matrixDirty = true;
        transformDirty = true;
        OnTransformChanged();
    }

    // Rotation setter
    void SetRotation(const glm::vec3& newRotation) {
        rotation = newRotation;
        matrixDirty = true;
        transformDirty = true;
        OnTransformChanged();
    }

    // Scale setter
    void SetScale(const glm::vec3& newScale) {
        scale = newScale;
        matrixDirty = true;
        transformDirty = true;
        OnTransformChanged();
    }

    // Eğer transform doğrudan değiştirildiyse collider güncellemesi için callback
    void NotifyColliderUpdate(GameObject* owner);

    // Model matrix hesapla - önbellekleyen versiyon
    glm::mat4 GetModelMatrix() const;

    // Geçersiz kılınan metotlar
    void Start() override;
    void Update(float deltaTime) override;
    void OnEnable() override;
    void OnDisable() override;

    // Bileşen tipi bilgisi
    [[nodiscard]] std::string GetTypeName() const override { return "TransformComponent"; }

    // Her yerden erişilebilen, public bir MarkDirty fonksiyonu
    void MarkDirty() {
        matrixDirty = true;
        transformDirty = true;
    }

private:
    // Model matrisini yeniden hesapla
    void RecalculateModelMatrix() const;
    void OnTransformChanged();
};

#endif // TRANSFORM_COMPONENT_H