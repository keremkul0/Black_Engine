
#include "HierarchyPanel.h"
#include "imgui.h"

HierarchyPanel::HierarchyPanel(const std::string& title, std::shared_ptr<Scene> scene)
    : Panel(title), m_Scene(scene), m_SelectedObject(nullptr) {}

void HierarchyPanel::DrawContent() {
    // Search box
    ImGui::InputText("Search", m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer));
    ImGui::Separator();

    // Display game objects
    for (const auto& gameObject : m_Scene->GetGameObjects()) {
        DrawNode(gameObject);
    }

    // Right-click menu
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Create Empty Object")) {
            m_Scene->CreateGameObject("New Object");
        }
        ImGui::EndPopup();
    }
}

void HierarchyPanel::DrawNode(const std::shared_ptr<GameObject>& object) {
    if (!m_Scene) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Scene yüklenemedi!");
        return;
    }
    // Skip objects that don't match search
    if (m_SearchBuffer[0] != '\0' &&
        object->name.find(m_SearchBuffer) == std::string::npos) {
        return;
    }

    // Tree node flags
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_SpanAvailWidth;

    // Make leaf nodes if no children
    bool hasChildren = !object->GetChildren().empty();
    if (!hasChildren) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (m_SelectedObject == object) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // Display node
    bool opened = ImGui::TreeNodeEx(object->name.c_str(), flags);

    // Handle selection
    if (ImGui::IsItemClicked()) {
        m_SelectedObject = object;
    }

    // Context menu
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Add Child")) {
            auto child = m_Scene->CreateGameObject("Child");
            object->AddChild(child);
        }
        if (ImGui::MenuItem("Delete")) {
            // TODO: Implement deletion
        }
        ImGui::EndPopup();
    }

    // Draw children
    if (opened) {
        for (const auto& child : object->GetChildren()) {
            DrawNode(child);
        }
        ImGui::TreePop();
    }
}