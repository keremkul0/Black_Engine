#ifndef COMPONENT_DRAWERS_H
#define COMPONENT_DRAWERS_H

#include <functional>
#include <string>

class BaseComponent;

namespace Black {
    class RigidBodyComponent;
}

class ComponentDrawers {
public:
    // Register all component drawers at once
    static void RegisterAllDrawers();

    // Register a single drawer
    static void RegisterDrawer(const std::string& typeName,
                              std::function<void(BaseComponent*)> drawerFunction);

    // Draw a component using the appropriate drawer
    static void DrawComponent(BaseComponent* component);
      // RigidBody drawer
    static void DrawRigidBodyComponent(Black::RigidBodyComponent* component);
};

#endif // COMPONENT_DRAWERS_H