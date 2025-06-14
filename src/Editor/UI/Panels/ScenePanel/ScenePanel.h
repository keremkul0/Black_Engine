#pragma once

#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "imgui.h"  // Include ImGui first
#include "ImGuizmo.h" // Then include ImGuizmo

#include "Editor/UI/Panels/Panel.h"
#include "Engine/Scene/Scene.h"
#include "Core/Camera/Camera.h"
#include "Core/InputManager/InputEvent.h"
#include "Core/InputManager/InputManager.h"
#include "Core/Math/Ray.h" // Include the Ray class
#include "Editor/SelectionManager.h"

class ScenePanel final : public Panel {
public:
    explicit ScenePanel(const std::string &title);
    ~ScenePanel() override;

    void SetScene(const std::shared_ptr<Scene> &scene);

    bool OnInputEvent(const InputEvent &event) override;
    void OnUpdate(float deltaTime) override;

    // Mouse pozisyonundan ışın oluştur
    Math::Ray ScreenToWorldRay(const glm::vec2& screenPos);
    
    // Selection functionality
    void SelectObjectAtMousePos();
    void ClearSelection();

protected:
    void DrawContent() override;

private:
    // Kamera özellikleri
    std::unique_ptr<Camera> m_Camera;
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

    // Mouse picking özellikleri
    glm::vec2 m_MousePosInPanel = glm::vec2(0.0f);
    glm::vec2 m_PanelMin = glm::vec2(0.0f);
    glm::vec2 m_PanelSize = glm::vec2(0.0f);
    bool m_MouseInPanel = false;

    // Kamera kontrol ayarları
    float m_CameraSpeed = 5.0f;
    float m_CameraRotationSpeed = 0.3f; // Updated rotation speed
    glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    float m_CameraYaw = -90.0f;
    float m_CameraPitch = 0.0f;
    glm::vec3 m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Sahne referansı
    std::shared_ptr<Scene> m_Scene;

    // Kamera manipülasyon durumları
    bool m_IsRotating = false;
    bool m_IsPanning = false;

    // Framebuffer özellikleri
    unsigned int m_FramebufferID = 0;
    unsigned int m_TextureID = 0;
    unsigned int m_DepthRenderBuffer = 0;

    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    //shadowMap properties
    unsigned int m_ShadowMapFBO = 0;
    unsigned int m_ShadowMapTexture = 0; // Bu bizim shadow map'imiz olacak
    unsigned int m_ShadowMapWidth = 2048; // Yüksek çözünürlük önerilir, test için 1024, 2048 de olabilir
    unsigned int m_ShadowMapHeight = 2048;

    glm::mat4 m_LightProjection; // Işığın bakış açısındaki projeksiyon matrisi
    glm::mat4 m_LightView;       // Işığın bakış açısındaki view matrisi
    glm::mat4 m_LightSpaceMatrix; // LightProjection * LightView
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//

    // Cursor durumu
    InputManager::CursorType m_CurrentCursor = InputManager::DEFAULT_CURSOR;

    // Selection properties
    std::shared_ptr<GameObject> m_SelectedObject = nullptr;

    // ImGuizmo implementation
    void DrawGuizmo();
    static void CustomizeImGuizmoStyle();
    ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE m_CurrentGizmoMode = ImGuizmo::LOCAL;
    
    // Yardımcı fonksiyonlar
    void SetupFramebuffer();
    void ResizeFramebuffer(int width, int height);
    void CleanupResources();
    void SetupCamera();
    void UpdateCamera(float deltaTime);
    void UpdateCursor();
    std::shared_ptr<GameObject> FindObjectUnderMouse(const Math::Ray& ray);
    void HighlightSelectedObject();

    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    void InitializeShadowMap();
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
};
