#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H
#include <memory>
class GameObject;

class BaseComponent {
public:
    // Bileşenin bağlı olduğu varlık
    std::shared_ptr<GameObject> owner = nullptr;
    std::string fullName = typeid(*this).name();

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
    [[nodiscard]] virtual std::string GetTypeName() const {
        // Extract clean name from typeid
        const std::string fullName = typeid(*this).name();

        // Skip any leading numbers and "class " prefix
        size_t startPos = 0;
        while (startPos < fullName.size() && (std::isdigit(fullName[startPos]) || std::isspace(fullName[startPos])))
            startPos++;

        // Also skip "class " prefix if present
        if (fullName.substr(startPos, 6) == "class ")
            startPos += 6;

        return fullName.substr(startPos);
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
