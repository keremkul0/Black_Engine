#pragma once

#include "Engine/Component/BaseComponent.h"
#include "Engine/Component/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
// Include other component headers as needed

class ComponentDrawers {
public:
    // Register all drawer functions
    static void RegisterAllDrawers();

    // Generic component drawer function
    static void DrawComponent(BaseComponent *component);

private:
    // Specialized drawer functions for each component type
    static void DrawMeshComponent(const MeshComponent *component);

    static void DrawTransformComponent(TransformComponent *component);

    // Add more component drawer methods as needed
};
