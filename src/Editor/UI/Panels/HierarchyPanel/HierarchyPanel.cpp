#include "HierarchyPanel.h"

#include <iostream>

#include "imgui.h"
#include "Editor/UI/Panels/Panel.h"

HierarchyPanel::HierarchyPanel(const std::string& title, const std::shared_ptr<Scene> &scene)
    : ::Panel(title), m_Scene(scene), m_SelectedObject(nullptr) {}

void HierarchyPanel::SetSelectedObject(const std::shared_ptr<GameObject>& gameObject) {
    if (gameObject != m_SelectedObject) {
        m_SelectedObject = gameObject;

        // Notify listeners when selection changes
        if (OnSelectionChanged) {
            OnSelectionChanged(m_SelectedObject);
        }
    }
}

void HierarchyPanel::DrawContent() {
    ImGui::InputText("Search", m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer));
    ImGui::Separator();

    // Show scene as root node with game objects as children
    if (m_Scene) {
        constexpr ImGuiTreeNodeFlags sceneFlags = ImGuiTreeNodeFlags_DefaultOpen |
                                                  ImGuiTreeNodeFlags_SpanAvailWidth;

        const std::string sceneName = m_Scene->GetName().empty() ? "Untitled Scene" : m_Scene->GetName();
        const bool sceneOpen = ImGui::TreeNodeEx(sceneName.c_str(), sceneFlags);

        // Scene context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Create Empty Object")) {
                m_Scene->CreateGameObject("New Object");
            }
            ImGui::EndPopup();
        }

        if (sceneOpen) {
            // Draw only top-level GameObjects (those without parents)
            for (const auto& gameObject : m_Scene->GetGameObjects()) {
                // Check if this is a top-level object (no parent)
                if (!gameObject->GetParent()) {
                    DrawNode(gameObject);
                }
            }
            ImGui::TreePop();
        }
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No scene loaded!");
    }

    // Allow creating objects via right-click on empty space
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

    // Skip objects that don't match the search criteria
    if (m_SearchBuffer[0] != '\0' &&
        object->GetName().find(m_SearchBuffer) == std::string::npos) {
        return;
        }

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_SpanAvailWidth;

    // Set leaf flag if the object has no children
    const bool hasChildren = !object->GetChildren().empty();
    if (!hasChildren) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    // Add selected flag if this is the currently selected object
    if (m_SelectedObject == object) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // Create the tree node for this object
    const bool opened = ImGui::TreeNodeEx(object->GetName().c_str(), flags);

    // Handle selection when clicked
    if (ImGui::IsItemClicked()) {
        SetSelectedObject(object);
    }

    // Context menu for right-click operations
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Add Child")) {
            const auto child = m_Scene->CreateGameObject("Child");
            object->AddChild(child);
        }
        if (ImGui::MenuItem("Delete")) {
            // TODO: Implement deletion
        }
        ImGui::EndPopup();
    }

    // Draw children if the node is open
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
