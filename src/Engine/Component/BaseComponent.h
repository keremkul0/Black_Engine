#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H

#include <memory>

// Öne deklarasyon
class GameObject;

class BaseComponent
{
public:
    // Bileşenin bağlı olduğu varlık
    GameObject* owner = nullptr;

    // Sanal yıkıcı (miras için gerekli)
    virtual ~BaseComponent() = default;

    // Bileşen hayata geçirildiğinde bir seferlik çağrılabilir
    virtual void Start() {}

    // Her karede çağrılabilecek güncelleme metodu
    virtual void Update(float deltaTime) {}

    // Her karede çağrılabilecek çizim metodu (şimdilik boş bırakabiliriz)
    virtual void Draw() {}

    virtual void OnEnable() {}
    virtual void OnDisable() {}
    [[nodiscard]] virtual const char* GetTypeName() const { return "BaseComponent"; }
    // İleride Inspector'da göstereceğimiz parametreler
    virtual void OnInspectorGUI() {}
};

#endif // BASE_COMPONENT_H
