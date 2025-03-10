#include "Panel.h"
#include "imgui.h"
#include <iostream>

void Panel::Render() {
    if (!m_IsActive || !m_IsOpen)
        return;

    // Begin window with docking enabled

    // Start panel window
    if (constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None; ImGui::Begin(m_Title.c_str(), &m_IsOpen, windowFlags)) {
        // Update focus and hover states with more thorough checks
        m_IsFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows |
                                             ImGuiFocusedFlags_RootWindow);
        m_IsHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows |
                                             ImGuiHoveredFlags_AllowWhenBlockedByPopup |
                                             ImGuiHoveredFlags_RootWindow);

        // Draw panel-specific content
        DrawContent();
    }
    ImGui::End();
}

// Moved implementation from header to cpp file
bool Panel::IsActiveForInput() const {
    // For floating windows, check if any part of the window hierarchy is focused
    if (m_IsOpen && m_IsFocused) {
        return true;
    }
    // Also check if mouse is hovering over this panel
    return m_IsOpen && m_IsHovered;
}

void Panel::SetActive(bool active) {
    m_IsActive = active;
}

bool Panel::IsActive() const {
    return m_IsActive;
}

const std::string &Panel::GetTitle() const {
    return m_Title;
}


