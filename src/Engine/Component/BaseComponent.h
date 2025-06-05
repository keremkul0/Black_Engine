#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H
#include <memory>
class GameObject;

class BaseComponent {
public:
    // Bileşenin bağlı olduğu varlık
    std::shared_ptr<GameObject> owner = nullptr;
    std::string fullName = typeid(*this).name();

    // Return the GameObject this component is attached to
    GameObject* GetGameObject() const {
        return owner.get();
    }

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

    // Her karede çağrılabilecek tel çerçeve çizim metodu
    virtual void DrawWireframe() {
    }

    // Bileşen aktif edildiğinde çağrılır
    virtual void OnEnable() {
    }

    // Bileşen deaktif edildiğinde çağrılır
    virtual void OnDisable() {
    }

    // Bileşenin tipi için sabit dize döndürür
    [[nodiscard]] virtual std::string GetTypeName() const {
        std::string name = typeid(*this).name();
        // Sadece son kısmı al (ör: "class TransformComponent" -> "TransformComponent")
        size_t pos = name.find_last_of(" :");
        if (pos != std::string::npos)
            name = name.substr(pos + 1);
        // Eğer başta "class " veya "struct " varsa onları da temizle
        if (name.rfind("class ", 0) == 0)
            name = name.substr(6);
        else if (name.rfind("struct ", 0) == 0)
            name = name.substr(7);
        return name;
    }

    // Bileşenin aktif/inaktif durumunu değiştirmek için yardımcı fonksiyon
    void SetEnabled(const bool enabled) {
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
