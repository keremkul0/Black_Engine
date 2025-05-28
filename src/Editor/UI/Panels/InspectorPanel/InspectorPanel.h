// src/Editor/UI/InspectorPanel.h
#pragma once
#include "../Panel.h"
#include <memory>

#include "Engine/Entity/GameObject.h"

class InspectorPanel : public Panel {
public:
    InspectorPanel();
    explicit InspectorPanel(const std::string& title);
    ~InspectorPanel() override;
    
    void SetSelectedObject(std::shared_ptr<GameObject> object);
    void SetTargetObject(const std::shared_ptr<GameObject>& gameObject) { m_TargetObject = gameObject; }
    
    [[nodiscard]] std::shared_ptr<GameObject> GetSelectedObject() const { return m_SelectedObject; }

protected:
    void DrawContent() override;

private:
    std::shared_ptr<GameObject> m_SelectedObject;
    std::shared_ptr<GameObject> m_TargetObject = nullptr;
    static void DrawComponentUI(BaseComponent* component);
};
