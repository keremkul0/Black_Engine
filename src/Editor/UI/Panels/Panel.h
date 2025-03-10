#pragma once
#include <string>
#include <utility>
#include "Core/InputManager/InputEvent.h"
#include "imgui.h"

class Panel {
public:
    explicit Panel(std::string title) : m_Title(std::move(title)), m_IsOpen(true), m_IsActive(true), m_IsFocused(false),
                                               m_IsHovered(false) {
    }

    virtual ~Panel() = default;

    virtual void Render();

    // Returns true if the event was handled
    virtual bool OnInputEvent(const InputEvent &event) { return false; }

    // Declaration only - implementation moved to .cpp
    virtual bool IsActiveForInput() const;

    bool IsFocused() const { return m_IsFocused; }
    bool IsHovered() const { return m_IsHovered; }

    void SetActive(bool active);

    bool IsActive() const;

    virtual void OnUpdate(float deltaTime) {
    }

    bool &IsOpen() { return m_IsOpen; }

    const std::string &GetTitle() const;

protected:
    // This is what derived panels will implement
    virtual void DrawContent() = 0;

    std::string m_Title;
    bool m_IsOpen;
    bool m_IsActive;
    bool m_IsFocused;
    bool m_IsHovered;
};
