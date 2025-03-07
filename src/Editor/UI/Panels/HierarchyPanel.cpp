#include "HierarchyPanel.h"

#include <iostream>

#include "imgui.h"

HierarchyPanel::HierarchyPanel(const std::string& title, std::shared_ptr<Scene> scene)
    : Panel(title), m_Scene(scene), m_SelectedObject(nullptr) {}

void HierarchyPanel::DrawContent() {
    ImGui::InputText("Search", m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer));
    ImGui::Separator();

    for (const auto& gameObject : m_Scene->GetGameObjects()) {
        DrawNode(gameObject);
    }

    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Create Empty Object")) {
            m_Scene->CreateGameObject("New Object");
        }
        ImGui::EndPopup();
    }
}

void HierarchyPanel::DrawNode(const std::shared_ptr<GameObject>& object) {
    if (!m_Scene) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Scene could not be loaded!");
        return;
    }

    if (m_SearchBuffer[0] != '\0' &&
        object->name.find(m_SearchBuffer) == std::string::npos) {
        return;
    }

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_SpanAvailWidth;

    bool hasChildren = !object->GetChildren().empty();
    if (!hasChildren) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (m_SelectedObject == object) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    bool opened = ImGui::TreeNodeEx(object->name.c_str(), flags);

    if (ImGui::IsItemClicked()) {
        m_SelectedObject = object;
    }

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

    if (opened) {
        for (const auto& child : object->GetChildren()) {
            DrawNode(child);
        }
        ImGui::TreePop();
    }
}

bool HierarchyPanel::OnInputEvent(const InputEvent& event) {
    return false;
}
