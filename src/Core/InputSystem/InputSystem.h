#pragma once
#include "Core/InputManager/IInputEventReceiver.h"
#include <GLFW/glfw3.h>
#include <vector>

class InputSystem {
public:
    InputSystem();
    ~InputSystem() = default;

    // Girdi sistemini başlatır
    void Initialize(GLFWwindow *window);

    // Olay alıcıları kaydetme
    void RegisterEventReceiver(IInputEventReceiver *receiver);
    void UnregisterEventReceiver(IInputEventReceiver *receiver);

    // Mevcut kare için girdi olaylarını işler
    void ProcessInput(float deltaTime);

    // ImGui'nin girdiyi yakalamak isteyip istemediğini kontrol etme yardımcısı
    [[nodiscard]] static bool ShouldImGuiProcessEvent(const InputEvent &event);

    // GLFW geri çağrısı olarak ayarlanacak kaydırma geri çağrısı
    static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

private:
    // Son bilinen fare konumunu saklar
    glm::vec2 m_LastMousePos{0.0f, 0.0f};

    // Girdi olaylarını alacak alıcılar
    std::vector<IInputEventReceiver*> m_EventReceivers;
};