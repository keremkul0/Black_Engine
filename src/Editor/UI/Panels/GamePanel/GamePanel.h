#pragma once
#include <memory>
#include "../Panel.h"
#include "Engine/Scene/Scene.h"


class GamePanel : public Panel {
public:
    explicit GamePanel(const std::string& title);
    ~GamePanel() override;

    // Get framebuffer ID to render into
    [[nodiscard]] unsigned int GetFramebufferID() const { return m_FramebufferID; }

    // Get current dimensions
    ImVec2 GetViewportSize() const { return ImVec2(m_ViewportWidth, m_ViewportHeight); }

    // Add scene reference
    void SetScene(const std::shared_ptr<Scene>& scene);

protected:
    void DrawContent() override;
    bool OnInputEvent(const InputEvent& event) override;

private:
    void SetupFramebuffer();
    void CleanupFramebuffer();

    unsigned int m_FramebufferID = 0;
    unsigned int m_TextureID = 0;
    unsigned int m_RenderbufferID = 0;

    // Track viewport size without ImVec2
    int m_ViewportWidth = 800;
    int m_ViewportHeight = 600;

    // Add scene reference
    std::shared_ptr<Scene> m_Scene;
};
