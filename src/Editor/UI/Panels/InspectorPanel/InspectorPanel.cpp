#include "InspectorPanel.h"
#include "ComponentDrawers.h"
#include "Engine/Component/BaseComponent.h"
#include "imgui.h"
#include <utility>

InspectorPanel::InspectorPanel()
    : Panel("Inspector"), m_SelectedObject(nullptr) {
    ComponentDrawers::RegisterAllDrawers();
}

InspectorPanel::InspectorPanel(const std::string& title)
    : Panel(title), m_SelectedObject(nullptr) {}

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
