// src/Editor/UI/Panels/Panel.cpp
    #include "Panel.h"
    #include "imgui.h"

    void Panel::Render() {
        if (!m_IsActive) return;

        // ImGui panel flags
        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoNavFocus;

        // Begin panel window
        if (ImGui::Begin(m_Title.c_str(), &m_IsOpen, windowFlags)) {
            DrawContent();
        }
        ImGui::End();
    }

    void Panel::SetActive(bool active) {
        m_IsActive = active;
    }

    bool Panel::IsActive() const {
        return m_IsActive;
    }

    const std::string& Panel::GetTitle() const {
        return m_Title;
    }