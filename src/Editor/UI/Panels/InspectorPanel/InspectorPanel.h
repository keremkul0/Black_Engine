// src/Editor/UI/InspectorPanel.h
#pragma once
#include "../Panel.h"
#include <memory>

#include "Engine/Entity/GameObject.h"

class InspectorPanel : public Panel {
public:
    InspectorPanel();
    explicit InspectorPanel(const std::string& title);
    void SetSelectedObject(std::shared_ptr<GameObject> object);
    void SetTargetObject(const std::shared_ptr<GameObject>& gameObject) { m_TargetObject = gameObject; }

protected:
    void DrawContent() override;

private:
    std::shared_ptr<GameObject> m_SelectedObject;
    std::shared_ptr<GameObject> m_TargetObject = nullptr;
    static void DrawComponentUI(BaseComponent* component);
};
