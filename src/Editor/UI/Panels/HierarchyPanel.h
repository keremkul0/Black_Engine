#pragma once
#include "Panel.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Entity/GameObject.h"
#include <memory>

class HierarchyPanel : public Panel {
public:
    HierarchyPanel(const std::string& title, std::shared_ptr<Scene> scene);

protected:
    void DrawContent() override;
    void DrawNode(const std::shared_ptr<GameObject>& object);

private:
    std::shared_ptr<Scene> m_Scene;
    std::shared_ptr<GameObject> m_SelectedObject;
    char m_SearchBuffer[128] = "";
};