#include "InspectorPanel.h"
#include "ComponentDrawers.h"
#include "Engine/Component/BaseComponent.h"
#include "Editor/SelectionManager.h"
#include "imgui.h"
#include <utility>
#include <glm/gtc/type_ptr.hpp> 

// External global matrices from Application.cpp - will use if camera is not available
extern glm::mat4 gViewMatrix;
extern glm::mat4 gProjectionMatrix;

InspectorPanel::InspectorPanel()
    : Panel("Inspector"), m_SelectedObject(nullptr) {
    ComponentDrawers::RegisterAllDrawers();
    
    // Register with the SelectionManager to receive selection changes
    SelectionManager::GetInstance().AddSelectionChangedListener(
        [this](const std::shared_ptr<GameObject>& selectedObject) {
            SetSelectedObject(selectedObject);
        });
}

InspectorPanel::InspectorPanel(const std::string& title)
    : Panel(title), m_SelectedObject(nullptr) {
    ComponentDrawers::RegisterAllDrawers();
    
    // Register with the SelectionManager to receive selection changes
    SelectionManager::GetInstance().AddSelectionChangedListener(
        [this](const std::shared_ptr<GameObject>& selectedObject) {
            SetSelectedObject(selectedObject);
        });
}

InspectorPanel::~InspectorPanel() {
    // Clean up the listener when the panel is destroyed
    // Note: This is a simplified approach since we can't easily unregister a lambda
    // A more robust solution would involve storing a token or ID for the listener
}

void InspectorPanel::SetSelectedObject(std::shared_ptr<GameObject> object) {
    m_SelectedObject = std::move(object);
}

void InspectorPanel::DrawContent() {
    if (!m_SelectedObject) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No object selected");
        return;
    }

    // Draw object properties
    const std::string name = m_SelectedObject->GetName();
    char buffer[256];
    strcpy(buffer, name.c_str());
    if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
        m_SelectedObject->SetName(buffer);
    }

    // Active checkbox
    bool isActive = m_SelectedObject->IsActive();
    if (ImGui::Checkbox("Active", &isActive)) {
        m_SelectedObject->SetActive(isActive);
    }

    // Draw components
    ImGui::Separator();
    for (const auto& component : m_SelectedObject->GetComponents()) {
        // ComponentDrawers::DrawComponent(component.get());

        DrawComponentUI(component.get());
    }
}

void InspectorPanel::DrawComponentUI(BaseComponent* component) {
    if (!component) return;

    // Component type as header
    const std::string componentName = component->GetTypeName();
    constexpr ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_DefaultOpen;

    ImGui::PushID(component);
    const bool opened = ImGui::CollapsingHeader(componentName.c_str(), headerFlags);

    // Component context menu
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Remove Component")) {
            // TODO: Mark component for removal
            // m_SelectedObject->RemoveComponent(component);
        }
        ImGui::EndPopup();
    }

    if (opened) {
        ImGui::Indent();

        // Use specialized drawer for this component type
        ComponentDrawers::DrawComponent(component);

        ImGui::Unindent();
    }
    ImGui::PopID();
}
//TODO: Implement additional component types if needed
