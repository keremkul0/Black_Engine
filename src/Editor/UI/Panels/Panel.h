// src/Editor/UI/Panels/Panel.h
#pragma once
#include <string>

class Panel {
public:
    explicit Panel(const std::string& title) : m_Title(title), m_IsOpen(true), m_IsActive(true) {}
    virtual ~Panel() = default;

    virtual void Render();  // No longer pure virtual

    void SetActive(bool active);
    bool IsActive() const;
    const std::string& GetTitle() const;
    bool& IsOpen() { return m_IsOpen; }

protected:
    // This is what derived panels will implement
    virtual void DrawContent() = 0;

    std::string m_Title;
    bool m_IsOpen;
    bool m_IsActive;
};