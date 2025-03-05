
#include "InspectorPanel.h"
#include "Engine/Component/BaseComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "imgui.h"
#include <typeinfo>
#include <glm/glm.hpp>

InspectorPanel::InspectorPanel()
    : Panel("Inspector"), m_SelectedObject(nullptr) {}

InspectorPanel::InspectorPanel(const std::string& title)
    : Panel(title), m_SelectedObject(nullptr) {}


void InspectorPanel::SetSelectedObject(std::shared_ptr<GameObject> object) {
    m_SelectedObject = object;
}

void InspectorPanel::DrawContent() {
    if (!m_SelectedObject) {
        ImGui::TextWrapped("No object selected");
        return;
    }

    // Object name edit
    char nameBuf[256];
    strcpy(nameBuf, m_SelectedObject->name.c_str());
    if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
        m_SelectedObject->name = nameBuf;
    }

    // Transform section
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto transformComponent = m_SelectedObject->GetComponent<TransformComponent>();
        if (transformComponent) {
            // Position
            glm::vec3& position = transformComponent->position;
            ImGui::DragFloat3("Position", &position.x, 0.1f);

            // Rotation
            glm::vec3& rotation = transformComponent->rotation;
            ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);

            // Scale
            glm::vec3& scale = transformComponent->scale;
            ImGui::DragFloat3("Scale", &scale.x, 0.1f);
        }
        else {
            ImGui::Text("No transform component found");
        }
    }

    // Components
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Access components vector directly
        for (auto& component : m_SelectedObject->components) {
            DrawComponentUI(component.get());
        }

        // Add component button
        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("ComponentPopup");
        }

        if (ImGui::BeginPopup("ComponentPopup")) {
            // List of component types that can be added
            if (ImGui::MenuItem("Mesh Renderer")) {
                // m_SelectedObject->AddComponent<MeshRendererComponent>();
            }
            if (ImGui::MenuItem("Camera")) {
                // m_SelectedObject->AddComponent<CameraComponent>();
            }
            // Add more component types as needed
            ImGui::EndPopup();
        }
    }
}

void InspectorPanel::DrawComponentUI(BaseComponent* component) {
    if (!component) return;

    // Draw component header
    ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_DefaultOpen;
    bool opened = ImGui::CollapsingHeader(typeid(*component).name(), headerFlags);

    // Context menu for component
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Remove Component")) {
            // Mark component for removal
            // m_SelectedObject->RemoveComponent(component->GetID());
        }
        ImGui::EndPopup();
    }

    if (opened) {
        // Component-specific UI
        // This will need to be expanded based on your component types
        ImGui::Text("Component properties will appear here");
    }
}