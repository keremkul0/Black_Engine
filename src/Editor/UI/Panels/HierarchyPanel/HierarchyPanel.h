#pragma once
#include "../Panel.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Entity/GameObject.h"
#include <memory>

class HierarchyPanel : public Panel {
public:
    HierarchyPanel(const std::string& title, const std::shared_ptr<Scene> &scene);
    bool OnInputEvent(const InputEvent& event) override;
    void SetSelectedObject(const std::shared_ptr<GameObject>& gameObject);
    std::function<void(std::shared_ptr<GameObject>)> OnSelectionChanged;
    std::shared_ptr<GameObject> GetSelectedObject() const { return m_SelectedObject; }
    void SetScene(const std::shared_ptr<Scene>& scene);
    void DeleteObject(const std::shared_ptr<GameObject>& object);
    void DeleteSelectedObject();

protected:
    void DrawContent() override;
    void DrawNode(const std::shared_ptr<GameObject>& object);

private:
    std::shared_ptr<Scene> m_Scene;
    std::shared_ptr<GameObject> m_SelectedObject;
    char m_SearchBuffer[128] = "";

    // Helper method to draw a GameObject node and its children in the hierarchy
    void DrawGameObjectNode(const std::shared_ptr<GameObject>& object, 
                            const std::shared_ptr<GameObject>& selectedObject);
};