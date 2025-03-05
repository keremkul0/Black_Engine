#pragma once
#include "Panel.h"
#include <string>
#include <glad/glad.h>
#include "imgui.h"

class GamePanel : public Panel {
public:
    GamePanel(const std::string& title);
    ~GamePanel(); // Add destructor declaration

protected:
    void DrawContent() override;

private:
    void SetupFramebuffer();
    void ResizeFramebuffer(float width, float height);

    unsigned int m_FramebufferID = 0;  // Use unsigned int instead of GLuint
    unsigned int m_TextureID = 0;      // Use unsigned int instead of GLuint
    ImVec2 m_LastSize = ImVec2(0, 0);
};