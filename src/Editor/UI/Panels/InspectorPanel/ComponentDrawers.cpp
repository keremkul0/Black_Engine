#include "Engine/Component/TransformComponent.h"
#include "Engine/Component/MeshComponent.h"
#include "ComponentDrawers.h"
#include "imgui.h"
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <functional>
#include <iostream>

// Type-erased drawer function map
using DrawerFunction = std::function<void(BaseComponent *)>;
static std::unordered_map<std::type_index, DrawerFunction> s_DrawerFunctions;

void ComponentDrawers::RegisterAllDrawers() {
    // Register all drawer functions by component type
    s_DrawerFunctions[std::type_index(typeid(MeshComponent))] = [](BaseComponent *comp) {
        DrawMeshComponent(dynamic_cast<MeshComponent *>(comp));
    };

    s_DrawerFunctions[std::type_index(typeid(TransformComponent))] = [](BaseComponent *comp) {
        DrawTransformComponent(dynamic_cast<TransformComponent *>(comp));
    };

    // Register more component drawers here
}

void ComponentDrawers::DrawComponent(BaseComponent *component) {
    if (!component) return;

    // Find and call appropriate drawer function
    const auto typeIndex = std::type_index(typeid(*component));
    const std::string typeName = component->GetTypeName();

    if (const auto it = s_DrawerFunctions.find(typeIndex); it != s_DrawerFunctions.end()) {
        // Use registered drawer function
        it->second(component);
    } else {
        // Improved fallback for components without specialized drawers
        ImGui::Text("%s", typeName.c_str());
        ImGui::Text("No specialized editor for this component type");
    }
}

void ComponentDrawers::DrawMeshComponent(const MeshComponent *component) {
    if (!component) return;

    ImGui::Text("Mesh Component");

    // Mesh information display
    if (component->IsLoaded() && component->GetMesh()) {
        ImGui::Text("Mesh: %s", component->GetMeshPath().c_str());

        // Mesh properties
        // TODO: Display vertex/face counts
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No mesh assigned");
    }

    // Mesh selection button
    if (ImGui::Button("Select Mesh")) {
        // TODO: Open file dialog
    }
}

void ComponentDrawers::DrawTransformComponent(TransformComponent *component) {
    if (!component) return;

    // Position
    glm::vec3 &position = component->position;
    if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
        // Optional: Handle position change
    }

    // Rotation
    glm::vec3 &rotation = component->rotation;
    if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f)) {
        // Optional: Handle rotation change
    }

    // Scale
    glm::vec3 &scale = component->scale;
    if (ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.01f, 100.0f)) {
        // Optional: Handle scale change
    }
}
