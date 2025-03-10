#pragma once
#include <imgui.h>
#include <memory>
#include <unordered_map>
#include <string>
#include "Editor/UI/Panels/Panel.h"
#include "Core/InputManager/InputEvent.h"
#include "Core/InputManager/IInputEventReceiver.h"
#include "Engine/Scene/Scene.h"

class EditorLayout final : public IInputEventReceiver {
public:
    EditorLayout();
    ~EditorLayout() override = default;

    void SetupDefaultLayout(const std::shared_ptr<Scene> &scene);
    static void SetupCustomLayout(const std::string &layoutName, const std::shared_ptr<Scene> &scene);

    // Render all active panels
    void RenderLayout();
    void UpdateAllPanels(float deltaTime);

    // Add a panel to the layout
    template<typename T, typename... Args>
    std::shared_ptr<T> AddPanel(const std::string &name, Args &&... args);

    // Get a panel by name
    template<typename T>
    std::shared_ptr<T> GetPanel(const std::string &name);

    // Show/hide specific panel
    void ShowPanel(const std::string &name, bool show = true);

    // Toggle dockspace mode
    void EnableDockspace(bool enable);

    // Process input events for UI
    void ProcessInput(const InputEvent& event) override;

    // Save/load layout configuration
    void SaveLayoutConfig(const std::string &filename);
    void LoadLayoutConfig(const std::string &filename);

private:
    void SetupDockspace();
    std::shared_ptr<Panel> GetPanelUnderMouse();

    std::shared_ptr<Panel> m_HoveredPanel = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Panel>> m_Panels;
    bool m_UseDockspace = true;
    bool m_FirstFrame = true;
    ImGuiID m_DockspaceID = 0;
    bool m_DockspaceInitialized = false;
};

template<typename T, typename... Args>
std::shared_ptr<T> EditorLayout::AddPanel(const std::string &name, Args &&... args) {
    auto panel = std::make_shared<T>(name, std::forward<Args>(args)...);
    m_Panels[name] = panel;
    return panel;
}

template<typename T>
std::shared_ptr<T> EditorLayout::GetPanel(const std::string &name) {
    static_assert(std::is_base_of_v<Panel, T>, "T must derive from Panel");

    if (const auto it = m_Panels.find(name); it != m_Panels.end()) {
        return std::dynamic_pointer_cast<T>(it->second);
    }
    return nullptr;
}