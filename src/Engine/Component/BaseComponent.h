#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H
#include <memory>
class GameObject;

class BaseComponent {
public:
    // Bileşenin bağlı olduğu varlık
    std::shared_ptr<GameObject> owner = nullptr;


    // Sanal yıkıcı (miras için gerekli)
    virtual ~BaseComponent() = default;

    // Bileşen aktif/inaktif durumu
    bool isEnabled = true;

    // Bileşen hayata geçirildiğinde bir seferlik çağrılabilir
    virtual void Start() {
    }

    // Her karede çağrılabilecek güncelleme metodu
    virtual void Update(float deltaTime) {
    }

    // Her karede çağrılabilecek çizim metodu
    virtual void Draw() {
    }

    // Bileşen aktif edildiğinde çağrılır
    virtual void OnEnable() {
    }

    // Bileşen deaktif edildiğinde çağrılır
    virtual void OnDisable() {
    }

    // Bileşenin tipi için sabit dize döndürür
    [[nodiscard]] virtual const char *GetTypeName() const { return "BaseComponent"; }

    // İleride Inspector'da göstereceğimiz parametreler
    virtual void OnInspectorGUI() {
    }

    // Bileşenin aktif/inaktif durumunu değiştirmek için yardımcı fonksiyon
    void SetEnabled(bool enabled) {
        if (isEnabled != enabled) {
            isEnabled = enabled;
            if (isEnabled) {
                OnEnable();
            } else {
                OnDisable();
            }
        }
    }
};

#endif // BASE_COMPONENT_H
