// src/Editor/UI/InspectorPanel.h
#pragma once
#include "Panel.h"
#include "../../../Engine/Entity/GameObject.h"
#include <memory>

class InspectorPanel : public Panel {
public:
    InspectorPanel();
    InspectorPanel(const std::string& title);
    void SetSelectedObject(std::shared_ptr<GameObject> object);

protected:
    void DrawContent() override;

private:
    std::shared_ptr<GameObject> m_SelectedObject;

    void DrawComponentUI(BaseComponent* component);
};