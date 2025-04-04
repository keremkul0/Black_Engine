#include "ComponentDrawers.h"
#include "Engine/Component/BaseComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Component/MeshComponent.h"
#include "Engine/Component/MeshRendererComponent.h"
#include <imgui.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>

using DrawerFunction = std::function<void(BaseComponent*)>;

std::unordered_map<std::string, DrawerFunction>& GetDrawerMap() {
    static std::unordered_map<std::string, DrawerFunction> instance;
    return instance;
}

void ComponentDrawers::RegisterDrawer(const std::string& typeName, DrawerFunction drawerFunction) {
    auto& drawers = GetDrawerMap();
    drawers[typeName] = drawerFunction;
}

void ComponentDrawers::DrawComponent(BaseComponent* component) {
    if (!component) return;

    auto& drawers = GetDrawerMap();
    std::string typeName = component->GetTypeName();

    auto it = drawers.find(typeName);
    if (it != drawers.end()) {
        it->second(component);
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No drawer for %s", typeName.c_str());
    }
}

// Transform component drawer
void DrawTransformComponent(BaseComponent* component) {
    auto* transform = static_cast<TransformComponent*>(component);

    glm::vec3 position = transform->position;
    if (ImGui::DragFloat3("Position", &position[0], 0.1f)) {
        transform->SetPosition(position);
    }

    glm::vec3 rotation = transform->rotation;
    if (ImGui::DragFloat3("Rotation", &rotation[0], 0.5f)) {
        transform->SetRotation(rotation);
    }

    glm::vec3 scale = transform->scale;
    if (ImGui::DragFloat3("Scale", &scale[0], 0.1f, 0.01f)) {
        transform->SetScale(scale);
    }
}

// Mesh component drawer
void DrawMeshComponent(BaseComponent* component) {
    ImGui::Text("Mesh Component Properties");

    char buffer[128] = "default_mesh";
    if (ImGui::InputText("Mesh Path", buffer, sizeof(buffer))) {
        // component->SetMeshPath(buffer);
    }
}

// Mesh renderer component drawer
void DrawMeshRendererComponent(BaseComponent* component) {
    ImGui::Text("Mesh Renderer Properties");

    bool castShadows = true;
    ImGui::Checkbox("Cast Shadows", &castShadows);

    bool receiveShadows = true;
    ImGui::Checkbox("Receive Shadows", &receiveShadows);

    const char* materials[] = { "Default", "Metal", "Wood", "Glass", "Plastic" };
    int currentMaterial = 0;
    ImGui::Combo("Material", &currentMaterial, materials, IM_ARRAYSIZE(materials));
}

// Register all component drawers
void ComponentDrawers::RegisterAllDrawers() {
    auto& drawers = GetDrawerMap();

    RegisterDrawer("TransformComponent", DrawTransformComponent);
    RegisterDrawer("MeshComponent", DrawMeshComponent);
    RegisterDrawer("MeshRendererComponent", DrawMeshRendererComponent);
}
