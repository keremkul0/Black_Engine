// src/Editor/UI/EditorLayout.h
#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include "Panels/Panel.h"

class EditorLayout {
public:
    EditorLayout();
    ~EditorLayout() = default;

    // Render all active panels
    void RenderLayout();

    // Add a panel to the layout
    template<typename T, typename... Args>
    std::shared_ptr<T> AddPanel(const std::string& name, Args&&... args);

    // Get a panel by name
    template<typename T>
    std::shared_ptr<T> GetPanel(const std::string& name);

    // Show/hide specific panel
    void ShowPanel(const std::string& name, bool show = true);

    // Toggle dockspace mode
    void EnableDockspace(bool enable);

    // Save/load layout configuration
    void SaveLayoutConfig(const std::string& filename);
    void LoadLayoutConfig(const std::string& filename);

private:
    void SetupDockspace();

    std::unordered_map<std::string, std::shared_ptr<Panel>> m_Panels;
    bool m_UseDockspace = true;
    bool m_FirstFrame = true;
    unsigned int m_DockspaceID = 0;
};

template<typename T, typename... Args>
std::shared_ptr<T> EditorLayout::AddPanel(const std::string& name, Args&&... args) {
    auto panel = std::make_shared<T>(name, std::forward<Args>(args)...);
    m_Panels[name] = panel;
    return panel;
}

template<typename T>
std::shared_ptr<T> EditorLayout::GetPanel(const std::string& name) {
    static_assert(std::is_base_of<Panel, T>::value, "T must derive from Panel");

    auto it = m_Panels.find(name);
    if (it != m_Panels.end()) {
        return std::dynamic_pointer_cast<T>(it->second);
    }
    return nullptr;
}