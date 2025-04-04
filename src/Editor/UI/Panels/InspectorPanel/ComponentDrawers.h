#ifndef COMPONENT_DRAWERS_H
#define COMPONENT_DRAWERS_H

#include <functional>
#include <string>

class BaseComponent;

class ComponentDrawers {
public:
    // Register all component drawers at once
    static void RegisterAllDrawers();

    // Register a single drawer
    static void RegisterDrawer(const std::string& typeName,
                              std::function<void(BaseComponent*)> drawerFunction);

    // Draw a component using the appropriate drawer
    static void DrawComponent(BaseComponent* component);
};

#endif // COMPONENT_DRAWERS_H