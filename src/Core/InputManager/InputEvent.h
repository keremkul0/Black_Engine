#pragma once
#include <glm/vec2.hpp>

enum class InputEventType {
    MouseMove,
    MouseDown,
    MouseUp,
    MouseScroll,
    KeyDown,
    KeyUp,
    KeyHeld
};

struct InputEvent {
    InputEventType type;
    int key = 0;           // For keyboard events
    int button = 0;        // For mouse button events
    glm::vec2 mousePos{0}; // Mouse position
    glm::vec2 mouseDelta{0}; // Mouse movement delta
    float scrollDelta = 0;   // For scroll events
    bool consumed = false;   // Whether the event was handled

    // Mark event as handled to prevent further propagation
    void Consume() { consumed = true; }
};