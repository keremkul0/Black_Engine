#include "InputSystem.h"

#include <algorithm>

#include "imgui.h"
#include "Core/InputManager/InputManager.h"

InputSystem::InputSystem() : m_LastMousePos(0.0f, 0.0f) {
}

void InputSystem::Initialize(GLFWwindow *window) {
    // Girdi yöneticisini başlat
    InputManager::Initialize(window);

    // Pencere geri çağrılarını ayarla
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, ScrollCallback);

    // Son fare konumunu başlat
    double mouseX, mouseY;
    InputManager::GetMousePosition(mouseX, mouseY);
    m_LastMousePos = glm::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
}

void InputSystem::RegisterEventReceiver(IInputEventReceiver *receiver) {
    if (receiver) {
        m_EventReceivers.push_back(receiver);
    }
}

void InputSystem::UnregisterEventReceiver(IInputEventReceiver *receiver) {
    if (const auto it = std::ranges::find(m_EventReceivers, receiver); it != m_EventReceivers.end()) {
        m_EventReceivers.erase(it);
    }
}

void InputSystem::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    InputManager::SetScrollOffset(static_cast<float>(yoffset));
}

bool InputSystem::ShouldImGuiProcessEvent(const InputEvent &event) {
    const ImGuiIO &io = ImGui::GetIO();

    // For mouse events, check if ImGui wants the mouse
    if (event.type == InputEventType::MouseDown ||
        event.type == InputEventType::MouseUp ||
        event.type == InputEventType::MouseMove ||
        event.type == InputEventType::MouseScroll) {
        return io.WantCaptureMouse;
    }

    // For keyboard events, check if ImGui wants the keyboard
    if (event.type == InputEventType::KeyDown ||
        event.type == InputEventType::KeyUp ||
        event.type == InputEventType::KeyHeld) {
        return io.WantCaptureKeyboard;
    }

    return false;
}

void InputSystem::ProcessInput(float deltaTime) {
    // Girdi yöneticisi durumunu güncelle
    InputManager::Update();

    // Kayıtlı alıcı yoksa olayları işleme
    if (m_EventReceivers.empty()) return;

    // Fare konumunu al
    double mouseX, mouseY;
    InputManager::GetMousePosition(mouseX, mouseY);
    glm::vec2 currentMousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
    glm::vec2 mouseDelta = currentMousePos - m_LastMousePos;

    // Fare hareketi olayını işle
    if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {
        InputEvent mouseMoveEvent;
        mouseMoveEvent.type = InputEventType::MouseMove;
        mouseMoveEvent.mousePos = currentMousePos;
        mouseMoveEvent.mouseDelta = mouseDelta;

        // Tüm alıcılara gönder
        for (auto receiver: m_EventReceivers) {
            if (!mouseMoveEvent.consumed) {
                receiver->ProcessInput(mouseMoveEvent);
            }
        }
    }

    // Fare düğmesi basılma olaylarını işle
    for (int button = 0; button < 3; ++button) {
        if (InputManager::IsMouseButtonJustPressed(button)) {
            InputEvent mouseDownEvent;
            mouseDownEvent.type = InputEventType::MouseDown;
            mouseDownEvent.button = button;
            mouseDownEvent.mousePos = currentMousePos;

            for (auto receiver: m_EventReceivers) {
                if (!mouseDownEvent.consumed) {
                    receiver->ProcessInput(mouseDownEvent);
                }
            }
        }

        if (InputManager::IsMouseButtonJustReleased(button)) {
            InputEvent mouseUpEvent;
            mouseUpEvent.type = InputEventType::MouseUp;
            mouseUpEvent.button = button;
            mouseUpEvent.mousePos = currentMousePos;

            for (auto receiver: m_EventReceivers) {
                if (!mouseUpEvent.consumed) {
                    receiver->ProcessInput(mouseUpEvent);
                }
            }
        }
    }

    // Fare kaydırma olayını işle
    float scrollOffset = InputManager::GetScrollOffset();
    if (scrollOffset != 0.0f) {
        InputEvent scrollEvent;
        scrollEvent.type = InputEventType::MouseScroll;
        scrollEvent.scrollDelta = scrollOffset;
        scrollEvent.mousePos = currentMousePos;

        for (auto receiver: m_EventReceivers) {
            if (!scrollEvent.consumed) {
                receiver->ProcessInput(scrollEvent);
            }
        }
    }

    // Klavye olaylarını işle
    // Genel olarak, tüm tuşları kontrol etmek için bir döngü kullanabiliriz
    // Not: Bu, performansı etkileyebilir; optimizasyon gerekebilir
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
        bool isPressed = InputManager::IsKeyPressed(key);

        // Tuş basma ve basılı tutma olaylarını ayırt etmek için
        // daha fazla durum bilgisine ihtiyaç olabilir
        // Şu anda sadece basılı tuşlar için olay oluşturuyoruz

        if (isPressed) {
            InputEvent keyEvent;
            keyEvent.type = InputEventType::KeyHeld; // Veya KeyDown olabilir
            keyEvent.key = key;

            for (auto receiver: m_EventReceivers) {
                if (!keyEvent.consumed) {
                    receiver->ProcessInput(keyEvent);
                }
            }
        }
    }

    // Son fare konumunu güncelle
    m_LastMousePos = currentMousePos;
}
