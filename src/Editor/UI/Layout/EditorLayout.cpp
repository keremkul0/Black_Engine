#include "EditorLayout.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Editor/UI/Panels/GamePanel/GamePanel.h"
#include "Editor/UI/Panels/HierarchyPanel/HierarchyPanel.h"
#include "Editor/UI/Panels/InspectorPanel/InspectorPanel.h"
#include "Editor/UI/Panels/ScenePanel/ScenePanel.h"
#include "iostream"


EditorLayout::EditorLayout() : m_FirstFrame(true), m_DockspaceID(0), m_DockspaceInitialized(false) {
}

void EditorLayout::SetupDefaultLayout(const std::shared_ptr<Scene> &scene) {
    // Add standard panels
    auto hierarchyPanel = AddPanel<HierarchyPanel>("Hierarchy", scene);
    auto inspectorPanel = AddPanel<InspectorPanel>("Inspector");
    auto scenePanel = AddPanel<ScenePanel>("Scene");
    scenePanel->SetScene(scene);
    auto gamePanel = AddPanel<GamePanel>("Game");
    gamePanel->SetScene(scene);

    // Connect the hierarchy and inspector panels
    hierarchyPanel->OnSelectionChanged = [inspectorPanel](std::shared_ptr<GameObject> selectedObject) {
        inspectorPanel->SetSelectedObject(selectedObject);
    };
    
    // SelectionManager olaylarını dinle ve HierarchyPanel'e bildir
    SelectionManager::GetInstance().AddSelectionChangedListener(
        [hierarchyPanel](std::shared_ptr<GameObject> selectedObject) {
            // HierarchyPanel'i güncelle
            hierarchyPanel->SetSelectedObject(selectedObject);
            
            // Seçim değiştiğinde log
            if (!selectedObject) {
                std::cout << "SelectionManager: Seçim temizlendi" << std::endl;
            }
        }
    );
    
    std::cout << "EditorLayout: SelectionManager ve HierarchyPanel arasında bağlantı kuruldu" << std::endl;
}

void EditorLayout::SetupCustomLayout(const std::string &layoutName, const std::shared_ptr<Scene> &scene) {
    // Add custom panels based on layout name
}

void EditorLayout::RenderLayout() {
    if (m_UseDockspace) {
        SetupDockspace();
    }
    // Render all panels
    for (auto &[name, panel]: m_Panels) {
        panel->Render();
    }
}


void EditorLayout::UpdateAllPanels(float deltaTime) {
    for (auto &panelPair: m_Panels) {
        panelPair.second->OnUpdate(deltaTime);
    }
}

void EditorLayout::SetupDockspace() {
    // Configure dockspace window flags
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // Set window position and size to match viewport
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // Style adjustments for dockspace window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    // Begin the dockspace window
    static bool dockspaceOpen = true;
    ImGui::Begin("BlackEngineDockspace", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar(3);

    // Create a unique, persistent ID for our dockspace
    m_DockspaceID = ImGui::GetID("BlackEngineDockspaceID");

    // Initialize the docking layout if needed
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // Create dockspace with our ID and initialize layout if needed
    if (!m_DockspaceInitialized || ImGui::DockBuilderGetNode(m_DockspaceID) == nullptr) {
        m_DockspaceInitialized = true;

        // Reset and create new dockspace
        ImGui::DockBuilderRemoveNode(m_DockspaceID);
        ImGui::DockBuilderAddNode(m_DockspaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(m_DockspaceID, viewport->Size);

        // Define default layout
        ImGuiID dockMainID = m_DockspaceID;
        ImGuiID dockLeftID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.2f, nullptr, &dockMainID);
        ImGuiID dockRightID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, 0.25f, nullptr, &dockMainID);
        ImGuiID dockBottomID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.25f, nullptr, &dockMainID);

        // Set dock locations for standard panels
        ImGui::DockBuilderDockWindow("Hierarchy", dockLeftID);
        ImGui::DockBuilderDockWindow("Inspector", dockRightID);
        ImGui::DockBuilderDockWindow("Scene", dockMainID);
        ImGui::DockBuilderDockWindow("Game", dockBottomID);

        ImGui::DockBuilderFinish(m_DockspaceID);
    }

    // Create the actual dockspace
    ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags);
    ImGui::End(); // End the dockspace window
}

std::shared_ptr<Panel> EditorLayout::GetPanelUnderMouse() {
    ImVec2 mousePos = ImGui::GetMousePos();

    for (const auto &[name, panel]: m_Panels) {
        // Check if window exists in ImGui context
        if (!ImGui::FindWindowByName(name.c_str()))
            continue;

        // Get window position and size
        ImGuiWindow *window = ImGui::FindWindowByName(name.c_str());
        if (!window || !window->Active)
            continue;

        ImVec2 panelMin = window->Pos;
        ImVec2 panelMax = ImVec2(
            panelMin.x + window->Size.x,
            panelMin.y + window->Size.y
        );

        // Check if mouse is over this panel
        if (mousePos.x >= panelMin.x && mousePos.y >= panelMin.y &&
            mousePos.x < panelMax.x && mousePos.y < panelMax.y) {
            return panel;
        }
    }

    return nullptr;
}

void EditorLayout::ProcessInput(const InputEvent &event) {
    // For ImGui multi-viewport (floating windows) to work correctly,
    // we need to ensure input is properly routed

    const ImGuiIO &io = ImGui::GetIO();
    const bool isMouseEvent = (event.type == InputEventType::MouseMove ||
                               event.type == InputEventType::MouseDown ||
                               event.type == InputEventType::MouseUp ||
                               event.type == InputEventType::MouseScroll);

    const bool isKeyEvent = (event.type == InputEventType::KeyDown ||
                             event.type == InputEventType::KeyUp ||
                             event.type == InputEventType::KeyHeld);

    // DELETE tuşu için özel işlem
    if (event.type == InputEventType::KeyDown && event.key == GLFW_KEY_DELETE) {
        // Hierarchy panelini bul ve seçili nesneyi sil
        auto hierarchyIt = m_Panels.find("Hierarchy");
        if (hierarchyIt != m_Panels.end()) {
            auto hierarchyPanel = std::dynamic_pointer_cast<HierarchyPanel>(hierarchyIt->second);
            if (hierarchyPanel) {
                std::cout << "DELETE tuşu algılandı, nesne silme deneniyor..." << std::endl;
                hierarchyPanel->DeleteSelectedObject();
                return; // Tuş işlendi
            }
        }
    }

    // Process panel-specific input even when ImGui wants capture
    // This allows game panels to receive input when focused
    if (isMouseEvent) {
        // Find panel under mouse regardless of io.WantCaptureMouse
        if (const auto panelUnderMouse = GetPanelUnderMouse()) {
            // Always send mouse events to the panel under mouse
            // The panel itself can decide whether to use it
            panelUnderMouse->OnInputEvent(event);

            // If it's a mouse down event, focus this panel
            if (event.type == InputEventType::MouseDown) {
                for (auto &[name, panel]: m_Panels) {
                    if (panel == panelUnderMouse) {
                        ImGui::SetWindowFocus(name.c_str());
                        break;
                    }
                }
            }

            // Store as hovered panel
            m_HoveredPanel = panelUnderMouse;
        }
    }

    // For keyboard events, route to the active panel for input
    if (isKeyEvent) {
        // Skip if ImGui wants keyboard focus (e.g., text input fields)
        if (io.WantCaptureKeyboard)
            return;

        for (auto &[name, panel]: m_Panels) {
            if (panel->IsActiveForInput()) {
                panel->OnInputEvent(event);
                break;
            }
        }
    }
}

void EditorLayout::ShowPanel(const std::string &name, bool show) {
    if (const auto it = m_Panels.find(name); it != m_Panels.end()) {
        it->second->SetActive(show);
    }
}

void EditorLayout::EnableDockspace(const bool enable) {
    m_UseDockspace = enable;
}

void EditorLayout::SaveLayoutConfig(const std::string &filename) {
    //TODO: Implement saving layout configuration
}

void EditorLayout::LoadLayoutConfig(const std::string &filename) {
    //TODO: Implement loading layout configuration
}

void EditorLayout::DeleteSelectedObject() {
    // Hierarchy panelini bul ve seçili nesneyi sil
    auto hierarchyIt = m_Panels.find("Hierarchy");
    if (hierarchyIt != m_Panels.end()) {
        auto hierarchyPanel = std::dynamic_pointer_cast<HierarchyPanel>(hierarchyIt->second);
        if (hierarchyPanel) {
            std::cout << "EditorLayout: DeleteSelectedObject çağrıldı" << std::endl;
            hierarchyPanel->DeleteSelectedObject();
        } else {
            std::cout << "EditorLayout: Hierarchy paneli bulunamadı" << std::endl;
        }
    } else {
        std::cout << "EditorLayout: 'Hierarchy' isimli panel bulunamadı" << std::endl;
    }
}
