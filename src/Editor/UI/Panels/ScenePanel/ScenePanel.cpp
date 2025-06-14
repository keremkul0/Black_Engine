#include <glad/glad.h>
#include "imgui.h"  // Include ImGui first, before ImGuizmo
#include "ImGuizmo.h"  // Now ImGuizmo can see ImGui types

#include "ScenePanel.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/Component/TransformComponent.h"
#include "Core/Math/TransformUtils.h"
#include "Core/Camera/Camera.h"
#include "Core/InputManager/InputManager.h"
#include "Core/InputManager/InputEvent.h"

extern glm::mat4 gViewMatrix;
extern glm::mat4 gProjectionMatrix;

ScenePanel::ScenePanel(const std::string &title)
    : Panel(title), m_CurrentCursor(InputManager::DEFAULT_CURSOR) {
    m_Camera = std::make_unique<Camera>(m_CameraPosition);
    SetupFramebuffer();
    SetupCamera();
    CustomizeImGuizmoStyle();


}

ScenePanel::~ScenePanel() {
    CleanupResources();
}

void ScenePanel::SetScene(const std::shared_ptr<Scene> &scene) {
    m_Scene = scene;
    if (m_Scene && m_Camera)
    {
        m_Scene->SetCamera(m_Camera.get());

    }
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    InitializeShadowMap();
    //set default shader for all objects in the scene
    if (m_Scene) {
        const std::string shaderPath = "../src/shaders/";
        auto defaultShader = std::make_shared<Shader>(
            (shaderPath + "default.vert").c_str(),
            (shaderPath + "default.frag").c_str()
        );
        m_Scene->SetDefaultShader(defaultShader);
    }
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//

}

void ScenePanel::OnUpdate(float deltaTime) {
    // Eğer panel aktif değilse veya açık değilse güncelleme yapılmasın.
    if (!m_IsActive || !m_IsOpen)
        return;

    // Panelin focus/hover durumunu, Panel::Render() içinde ImGui üzerinden güncellenen bayraklardan alıyoruz.

    if (m_IsFocused || m_IsHovered) {
        // Mouse buton durumlarını InputManager üzerinden oku.
        const bool rightMouseDown = InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        const bool middleMouseDown = InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE);

        // Kamera kontrol modlarını belirle.
        m_IsRotating = rightMouseDown;
        m_IsPanning = middleMouseDown;

        // Kamera moduna göre cursor'u güncelle (durum değişiminde).
        UpdateCursor();        // Yalnızca rotasyon modunda WASD hareketleri ile kamera hareketini işle.
        if (m_IsRotating) {

            const float speedMultiplier = (InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ||
                                           InputManager::IsKeyPressed(GLFW_KEY_RIGHT_SHIFT))
                                              ? 3.0f
                                              : 1.0f;
            const float adjustedSpeed = m_CameraSpeed * speedMultiplier * deltaTime;

            if (InputManager::IsKeyPressed(GLFW_KEY_W))
                m_CameraPosition += adjustedSpeed * m_CameraFront;
            if (InputManager::IsKeyPressed(GLFW_KEY_S))
                m_CameraPosition -= adjustedSpeed * m_CameraFront;
            if (InputManager::IsKeyPressed(GLFW_KEY_A))
                m_CameraPosition -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * adjustedSpeed;
            if (InputManager::IsKeyPressed(GLFW_KEY_D))
                m_CameraPosition += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * adjustedSpeed;
            if (InputManager::IsKeyPressed(GLFW_KEY_Q))
                m_CameraPosition -= m_CameraUp * adjustedSpeed;
            if (InputManager::IsKeyPressed(GLFW_KEY_E))
                m_CameraPosition += m_CameraUp * adjustedSpeed;

            UpdateCamera(deltaTime);
        }
    } else {        // Panel focus veya hover değilse, kamera kontrol modlarını sıfırla ve cursor'u default yap.
        m_IsRotating = false;
        m_IsPanning = false;
        if (m_CurrentCursor != InputManager::DEFAULT_CURSOR) {
            m_CurrentCursor = InputManager::DEFAULT_CURSOR;
            InputManager::SetCursor(m_CurrentCursor);
        }
    }
} // End of OnUpdate

bool ScenePanel::OnInputEvent(const InputEvent& event) {
    // Input olaylarını yalnızca panel aktif ve focus/hover durumdaysa işleyelim.
    if (!m_IsActive || !m_IsOpen || (!m_IsFocused && !m_IsHovered))
        return false;

    switch (event.type) {
        case InputEventType::KeyDown: {
            // R tuşu ile kamera resetleme
            if (event.key == GLFW_KEY_R) {
                m_CameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
                m_CameraYaw = -90.0f;
                m_CameraPitch = 0.0f;
                m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
                UpdateCamera(0.0f);
                return true;
            }

            // ImGuizmo operation mode switching
            if (event.key == GLFW_KEY_1) {
                m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
                std::cout << "Gizmo Mode: TRANSLATE" << std::endl;
                return true;
            }
            if (event.key == GLFW_KEY_2) {
                m_CurrentGizmoOperation = ImGuizmo::ROTATE;
                std::cout << "Gizmo Mode: ROTATE" << std::endl;
                return true;
            }
            if (event.key == GLFW_KEY_3) {
                m_CurrentGizmoOperation = ImGuizmo::SCALE;
                std::cout << "Gizmo Mode: SCALE" << std::endl;
                return true;
            }

            // Toggle between local and world space
            if (event.key == GLFW_KEY_4) {
                m_CurrentGizmoMode = (m_CurrentGizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
                std::cout << "Gizmo Space: " << ((m_CurrentGizmoMode == ImGuizmo::LOCAL) ? "LOCAL" : "WORLD") <<
                        std::endl;
                return true;
            }

            return false;
        }
        case InputEventType::KeyHeld: {
            // R tuşu ile kamera resetleme
            if (event.key == GLFW_KEY_R) {
                m_CameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
                m_CameraYaw = -90.0f;
                m_CameraPitch = 0.0f;
                m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
                UpdateCamera(0.0f);
                return true;
            }
            return false;
        }
        case InputEventType::MouseDown: {
            // Left mouse button for object selection
            if (event.button == GLFW_MOUSE_BUTTON_LEFT && m_MouseInPanel) {
                // Gizmo üzerinde tıklandıysa nesne seçimini tetikleme
                if (m_SelectedObject && ImGuizmo::IsOver()) {
                    std::cout << "Mouse is over Gizmo, skipping object selection" << std::endl;
                    return true;
                }

                // Handle object selection
                SelectObjectAtMousePos();
                return true;
            }
            return false;
        }
        case InputEventType::MouseMove: {
            if (m_IsRotating) {
                m_CameraYaw += event.mouseDelta.x * m_CameraRotationSpeed;
                m_CameraPitch -= event.mouseDelta.y * m_CameraRotationSpeed;
                m_CameraPitch = glm::clamp(m_CameraPitch, -89.0f, 89.0f);

                glm::vec3 front;
                front.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
                front.y = sin(glm::radians(m_CameraPitch));
                front.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
                m_CameraFront = glm::normalize(front);

                UpdateCamera(0.0f);
                return true;
            }
            if (m_IsPanning) {
                constexpr float panSpeed = 0.01f;
                const glm::vec3 right = glm::normalize(glm::cross(m_CameraFront, m_CameraUp));
                const glm::vec3 cameraRelativeUp = glm::normalize(glm::cross(right, m_CameraFront));

                m_CameraPosition -= right * event.mouseDelta.x * panSpeed;
                m_CameraPosition += cameraRelativeUp * event.mouseDelta.y * panSpeed;

                UpdateCamera(0.0f);
                return true;
            }
            break;
        }
        case InputEventType::MouseScroll: {
            constexpr float zoomSpeed = 0.5f;
            m_CameraPosition += m_CameraFront * event.scrollDelta * zoomSpeed;
            UpdateCamera(0.0f);
            return true;
        }
        default:
            break;
    }
    return false;
}

// ForceRenderUpdate metodu kaldırıldı - dosya derlenme hatasını gidermek için

void ScenePanel::UpdateCursor() {
    InputManager::CursorType newCursor;

    if (m_IsRotating) {
        newCursor = InputManager::EYE_CURSOR;
    } else if (m_IsPanning) {
        newCursor = InputManager::HAND_CURSOR;
    } else {
        newCursor = InputManager::DEFAULT_CURSOR;
    }

    // Only set cursor if it needs to change
    if (m_CurrentCursor != newCursor) {
        m_CurrentCursor = newCursor;
        InputManager::SetCursor(m_CurrentCursor);
    }
}

void ScenePanel::SetupCamera() {
    if (!m_Camera)
        m_Camera = std::make_unique<Camera>(m_CameraPosition);

    m_Camera->SetPosition(m_CameraPosition);

    glm::vec3 front;
    front.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
    front.y = sin(glm::radians(m_CameraPitch));
    front.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
    m_CameraFront = glm::normalize(front);

    m_Camera->SetFront(m_CameraFront);
}

void ScenePanel::UpdateCamera(float deltaTime) {
    m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraPosition + m_CameraFront, m_CameraUp);
    gViewMatrix = m_ViewMatrix;
    if (m_Camera) {
        m_Camera->SetPosition(m_CameraPosition);
        m_Camera->SetFront(m_CameraFront);
    }
}

void ScenePanel::SetupFramebuffer() {
    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);

    glGenRenderbuffers(1, &m_DepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ScenePanel::DrawContent() {
    const ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
    if (contentRegionAvail.x <= 0 || contentRegionAvail.y <= 0)
        return;

    // Panel dimensions and mouse position tracking
    m_PanelMin = glm::vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
    m_PanelSize = glm::vec2(contentRegionAvail.x, contentRegionAvail.y);

    // Get global mouse position and convert to panel-local coordinates
    ImVec2 mousePos = ImGui::GetMousePos();
    m_MousePosInPanel.x = mousePos.x - m_PanelMin.x;
    m_MousePosInPanel.y = mousePos.y - m_PanelMin.y;

    // Check if mouse is inside panel
    m_MouseInPanel = (m_MousePosInPanel.x >= 0 && m_MousePosInPanel.x < m_PanelSize.x &&
                      m_MousePosInPanel.y >= 0 && m_MousePosInPanel.y < m_PanelSize.y);

    static int lastWidth = 0, lastHeight = 0;
    if (lastWidth != static_cast<int>(contentRegionAvail.x) || lastHeight != static_cast<int>(contentRegionAvail.y)) {
        ResizeFramebuffer(static_cast<int>(contentRegionAvail.x), static_cast<int>(contentRegionAvail.y));
        lastWidth = static_cast<int>(contentRegionAvail.x);
        lastHeight = static_cast<int>(contentRegionAvail.y);
    }

    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    // Işık pozisyonunuzu ve view matrisini güncelleyin (büyük projenizde ışık yönetimi nerede yapılıyorsa oradan alın)
    // Örnek: Sahneye ışık eklediyseniz, ışık objesinin pozisyonundan almalısınız.
    /*glm::vec3 currentLightPos = glm::vec3(-1.0f, 1.0f, -1.0f);//m_Scene->GetLightSourcePosition(); // Örnek fonksiyon

    m_LightView = glm::lookAt(currentLightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Hedef noktayı sahnenin merkezi olarak varsaydım
    m_LightSpaceMatrix = m_LightProjection * m_LightView;*/

    // --- Gölge Haritasını Render Etme Adımı ---
    if (m_ShadowMapFBO > 0) {
        glEnable(GL_DEPTH_TEST); // Derinlik testini aç
        glViewport(0, 0, m_ShadowMapWidth, m_ShadowMapHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT); // Sadece derinlik buffer'ını temizle


        // Shadow Map Shader'ı aktif et
        // Buradaki m_Scene->DrawAll() çağrısı muhtemelen Scene sınıfınızda objelerin shader'ını set ediyordur.
        // Eğer Scene sınıfınız bir DrawShadowMap fonksiyonu içeriyorsa onu kullanmak daha iyi olur.
        // Aksi takdirde, her objeyi tek tek ışığın bakış açısından doğru shader ile çizmelisiniz.
        // Geçici olarak:
        if (m_Scene) {
            // Scene sınıfınızın bir ShadowMapShader'ı set edebilmesi ve objeleri bu shader ile çizebilmesi gerekir.
            // Örnek:
            // m_Scene->SetCurrentShader(m_ShadowMapShader); // ShadowMapShader'ı aktif et
            // m_Scene->SetLightSpaceMatrix(m_LightSpaceMatrix); // Shader'a lightSpaceMatrix'i gönder
            // m_Scene->DrawAllForShadowMap(); // Sadece objeleri çiz

            // Ya da daha kaba bir yaklaşım (Mesh'leriniz draw fonksiyonunda kendi shader'larını aktif ediyorsa):
            // ** DİKKAT: Küçük projenizdeki Draw(shaderProgram, camera) çağrısı burada kamerayı kullanmamalı.**
            // Gölge haritasını çizerken kamera değil, ışığın bakış açısı kullanılmalıdır.
            // Bu nedenle, m_Scene->DrawAll() fonksiyonunuzun içeride nasıl çalıştığı kritik.
            // Eğer m_Scene->DrawAll() her objeyi iterate edip kendi shader'ı ile çiziyorsa,
            // Scene sınıfında bir 'ShadowMapPass()' fonksiyonu oluşturmanız en temizi olacaktır.
            // Bu fonksiyon, tüm objeleri gölge haritası shader'ı ile çizerken sadece lightSpaceMatrix'i kullanır.

            // Geçici ve test amaçlı (eğer objeleri doğrudan burada yönetebiliyorsanız):
            // m_ShadowMapShader.Activate(); // Bu shader'ı bir ScenePanel üyesi olarak tanımlamanız gerekir
            // glUniformMatrix4fv(glGetUniformLocation(m_ShadowMapShader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(m_LightSpaceMatrix));
            // m_Scene->DrawObjectsWithSpecificShader(m_ShadowMapShader); // Bu Scene'deki tüm objeleri lightSpaceMatrix ile çizsin

            m_Scene->DrawAll2ShadowMap(); // Bu fonksiyonun Scene sınıfınızda gölge haritası için objeleri çizen bir versiyon olması gerekir.
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    // Varsayılan framebuffer'a geri dön
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, static_cast<int>(contentRegionAvail.x), static_cast<int>(contentRegionAvail.y)); // Viewport'u panel boyutuna geri getir
    // --- Gölge Haritası Render Etme Adımı Sonu ---
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
    //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//


    if (m_FramebufferID > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
        glViewport(0, 0, static_cast<int>(contentRegionAvail.x), static_cast<int>(contentRegionAvail.y));
        glClearColor(0.17f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_Scene) {
            //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
            //default shader'ı aktif et ve lightSpaceMatrix uniformunu ayarla
            m_Scene->GetDefaultShader()->use();
            glUniformMatrix4fv(glGetUniformLocation(m_Scene->GetDefaultShader()->ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(m_LightSpaceMatrix));
            glActiveTexture(GL_TEXTURE0 + 2);
            glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture); // Gölge haritasını bağla
            glUniform1i(glGetUniformLocation(m_Scene->GetDefaultShader()->ID, "shadowMap"), 2); // ShadowMap uniformunu ayarla
            glActiveTexture(GL_TEXTURE0);
            //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//

            // Update camera matrices consistently for both rendering and ImGuizmo
            m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraPosition + m_CameraFront, m_CameraUp);
            gViewMatrix = m_ViewMatrix; // Use the exact same matrix for global reference

            m_Scene->SetViewMatrix(m_ViewMatrix);
            m_Scene->SetProjectionMatrix(m_ProjectionMatrix);
            m_Scene->DrawAll();

            // Highlight selected object if any
            HighlightSelectedObject();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ImGui::Image(reinterpret_cast<ImTextureID>(reinterpret_cast<void *>(static_cast<intptr_t>(m_TextureID))),
                     contentRegionAvail, ImVec2(0, 1), ImVec2(1, 0));


        //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
        //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
        // --- Gölge Haritasını Debug Etme Amacıyla Göster ---
        // Yeni bir ImGui penceresinde veya mevcut pencerede farklı bir yerde
        ImGui::Begin("Shadow Map Debug"); // Yeni bir pencere aç
        // Pencerenin boyutunu ayarlayabilirsiniz, örneğin 256x256
        ImVec2 shadowMapDisplaySize(1024, 1024);
        ImGui::Image(static_cast<ImTextureID>(m_ShadowMapTexture), // Sadece static_cast yeterli!
             shadowMapDisplaySize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        // --- Gölge Haritasını Debug Etme Amacıyla Gösterme Sonu ---
        //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
        //**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//

        // Draw ImGuizmo if object is selected
        DrawGuizmo();
    }

    // Debug information
    ImGui::SetCursorPos(ImVec2(10, 10));
    // Opsiyonel: Debug bilgileri çizilebilir.
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::Text("Camera: %.1f, %.1f, %.1f", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);

    // Display mouse coordinates and ray information for debugging
    if (m_MouseInPanel) {
        // Panel coordinates
        ImGui::SetCursorPos(ImVec2(10, 30));
        ImGui::Text("Mouse: %.1f, %.1f (in panel)", m_MousePosInPanel.x, m_MousePosInPanel.y);

        // Ray information
        Math::Ray ray = ScreenToWorldRay(m_MousePosInPanel);
        ImGui::SetCursorPos(ImVec2(10, 50));
        ImGui::Text("Ray Dir: %.2f, %.2f, %.2f",
                    ray.GetDirection().x, ray.GetDirection().y, ray.GetDirection().z);

        // Display NDC coordinates
        float ndcX = (2.0f * m_MousePosInPanel.x) / m_PanelSize.x - 1.0f;
        float ndcY = 1.0f - (2.0f * m_MousePosInPanel.y) / m_PanelSize.y;
        ImGui::SetCursorPos(ImVec2(10, 70));
        ImGui::Text("NDC: %.2f, %.2f", ndcX, ndcY);
    }
}

void ScenePanel::ResizeFramebuffer(int width, int height) {
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
    gProjectionMatrix = m_ProjectionMatrix;
}

void ScenePanel::CleanupResources() {
    if (m_FramebufferID != 0) {
        glDeleteFramebuffers(1, &m_FramebufferID);
        m_FramebufferID = 0;
    }
    if (m_TextureID != 0) {
        glDeleteTextures(1, &m_TextureID);
        m_TextureID = 0;
    }
    if (m_DepthRenderBuffer != 0) {
        glDeleteRenderbuffers(1, &m_DepthRenderBuffer);
        m_DepthRenderBuffer = 0;
    }
}

Math::Ray ScenePanel::ScreenToWorldRay(const glm::vec2& mousePos) {
    // 1. NDC (Normalized Device Coordinates)
    float x = (2.0f * mousePos.x) / m_PanelSize.x - 1.0f;
    float y = 1.0f - (2.0f * mousePos.y) / m_PanelSize.y;
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

    // 2. Eye space
    // Use m_ProjectionMatrix instead of calling a non-existent method
    glm::vec4 rayEye = glm::inverse(m_ProjectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Z = -1 yönünde

    // 3. World space
    glm::mat4 view = glm::lookAt(m_CameraPosition, m_CameraPosition + m_CameraFront, m_CameraUp);
    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

    return Math::Ray(m_CameraPosition, rayWorld);
}

std::shared_ptr<GameObject> ScenePanel::FindObjectUnderMouse(const Math::Ray& ray) {
    if (!m_Scene) {
        return nullptr;
    }

    // Use the Scene's PickObjectWithRay method for complete object picking logic
    return m_Scene->PickObjectWithRay(ray);
}

void ScenePanel::SelectObjectAtMousePos() {
    if (!m_MouseInPanel || !m_Scene) {
        std::cout << "[SelectObjectAtMousePos] Mouse not in panel or scene not set." << std::endl;
        return;
    }

    // Step 1: Create ray from mouse position
    Math::Ray ray = ScreenToWorldRay(m_MousePosInPanel);

    // Log ray information for debugging
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Ray Casting Debug:" << std::endl;
    std::cout << "Mouse position: " << m_MousePosInPanel.x << ", " << m_MousePosInPanel.y << std::endl;

    // Calculate NDC coordinates (log these for debugging)
    float ndcX = (2.0f * m_MousePosInPanel.x) / m_PanelSize.x - 1.0f;
    float ndcY = 1.0f - (2.0f * m_MousePosInPanel.y) / m_PanelSize.y;
    std::cout << "NDC coordinates: " << ndcX << ", " << ndcY << std::endl;

    // Log ray origin and direction
    const glm::vec3 &rayOrigin = ray.GetOrigin();
    const glm::vec3 &rayDir = ray.GetDirection();
    std::cout << "Ray origin: " << rayOrigin.x << ", " << rayOrigin.y << ", " << rayOrigin.z << std::endl;
    std::cout << "Ray direction: " << rayDir.x << ", " << rayDir.y << ", " << rayDir.z << std::endl;

    // Step 2: Find object under mouse
    try {
        std::shared_ptr<GameObject> hitObject = FindObjectUnderMouse(ray);

        // Log the result of object picking
        if (hitObject) {
            std::cout << "Selected object: " << hitObject->GetName() << std::endl;
        } else {
            std::cout << "No object selected" << std::endl;
        }

        // Step 3: Update selection state
        SelectionManager::GetInstance().SetSelectedObject(hitObject);

        // Update our local reference to the selected object
        // m_SelectedObject = hitObject; // Artık gerek yok, kaldırıldı
    } catch (const std::exception &e) {
        std::cerr << "Error during object selection: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error during object selection" << std::endl;
    }

    std::cout << "----------------------------------------" << std::endl;
}

void ScenePanel::ClearSelection() {
    if (m_SelectedObject) {
        m_SelectedObject->isSelected = false;
        m_SelectedObject = nullptr;
    }
}


void ScenePanel::HighlightSelectedObject() {
    auto selected = SelectionManager::GetInstance().GetSelectedObject();
    if (!selected || !selected->IsActive() || !m_Scene || !m_Scene->HasGameObject(selected)) {
        SelectionManager::GetInstance().ClearSelection();
        return;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    glDisable(GL_DEPTH_TEST);

    for (const auto& comp: selected->GetComponents()) {
        comp->DrawWireframe();
    }

    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
}


void ScenePanel::DrawGuizmo() {
    auto selected = SelectionManager::GetInstance().GetSelectedObject();
    if (!selected || !m_Scene || !m_Scene->HasGameObject(selected)) {
        SelectionManager::GetInstance().ClearSelection();
        return;
    }

    auto transformComp = selected->GetComponent<TransformComponent>();
    if (!transformComp) return;

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(m_PanelMin.x, m_PanelMin.y, m_PanelSize.x, m_PanelSize.y);
    ImGuizmo::SetGizmoSizeClipSpace(0.15f);
    ImGuizmo::AllowAxisFlip(false);

    glm::mat4 modelMatrix = transformComp->GetModelMatrix();
    float *view = glm::value_ptr(m_ViewMatrix);
    float *proj = glm::value_ptr(m_ProjectionMatrix);
    float *model = glm::value_ptr(modelMatrix);
    const ImGuizmo::OPERATION operation = m_CurrentGizmoOperation;
    const ImGuizmo::MODE mode = m_CurrentGizmoMode;
    float snapValues[3] = {0.5f, 0.5f, 0.5f};
    bool useSnap = ImGui::IsKeyPressed(ImGuiKey_LeftShift) || ImGui::IsKeyPressed(ImGuiKey_RightShift);
    if (useSnap) {
        if (operation == ImGuizmo::TRANSLATE) {
            snapValues[0] = snapValues[1] = snapValues[2] = 0.5f;
        } else if (operation == ImGuizmo::ROTATE) {
            snapValues[0] = snapValues[1] = snapValues[2] = 45.0f;
        } else if (operation == ImGuizmo::SCALE) {
            snapValues[0] = snapValues[1] = snapValues[2] = 0.1f;
        }
    }
    ImGuizmo::Manipulate(
        view,
        proj,
        operation,
        mode,
        model,
        nullptr,
        useSnap ? snapValues : nullptr
    );
    ImGui::SetCursorPos(ImVec2(10, 90));
    if (ImGuizmo::IsOver()) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Gizmo uzerinde!");
    } else if (ImGuizmo::IsUsing()) {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Gizmo kullaniliyor!");
    } else {
        const char *opName = (operation == ImGuizmo::TRANSLATE)
                                 ? "Translate"
                                 : (operation == ImGuizmo::ROTATE)
                                       ? "Rotate"
                                       : "Scale";
        const char *modeName = (mode == ImGuizmo::LOCAL) ? "Local" : "World";
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Gizmo: %s (%s)", opName, modeName);
    }
    if (ImGuizmo::IsUsing()) {
        glm::vec3 translation, rotation, scale;
        bool success = Math::DecomposeTransform(modelMatrix, translation, rotation, scale);
        if (success) {
            rotation = glm::degrees(rotation);
            transformComp->SetPosition(translation);
            transformComp->SetRotation(rotation);
            transformComp->SetScale(scale);
            transformComp->MarkDirty();
        }
    }
}

void ScenePanel::CustomizeImGuizmoStyle() {
    ImGuizmo::Style &style = ImGuizmo::GetStyle();

    // Customize colors for better visibility
    style.Colors[ImGuizmo::DIRECTION_X] = ImVec4(0.9f, 0.2f, 0.2f, 1.0f); // X axis - red
    style.Colors[ImGuizmo::DIRECTION_Y] = ImVec4(0.2f, 0.9f, 0.2f, 1.0f); // Y axis - green
    style.Colors[ImGuizmo::DIRECTION_Z] = ImVec4(0.2f, 0.2f, 0.9f, 1.0f); // Z axis - blue

    // Highlight colors when selected or hovered
    style.Colors[ImGuizmo::PLANE_X] = ImVec4(0.9f, 0.2f, 0.2f, 0.5f); // YZ plane
    style.Colors[ImGuizmo::PLANE_Y] = ImVec4(0.2f, 0.9f, 0.2f, 0.5f); // XZ plane
    style.Colors[ImGuizmo::PLANE_Z] = ImVec4(0.2f, 0.2f, 0.9f, 0.5f); // XY plane

    style.Colors[ImGuizmo::SELECTION] = ImVec4(1.0f, 0.8f, 0.0f, 1.0f); // Yellow selection
    style.Colors[ImGuizmo::INACTIVE] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Gray inactive

    // Adjust sizing
    style.TranslationLineThickness = 4.0f;
    style.TranslationLineArrowSize = 8.0f;
    style.RotationLineThickness = 3.0f;
    style.RotationOuterLineThickness = 4.0f;    style.ScaleLineThickness = 3.0f;
    style.ScaleLineCircleSize = 7.0f;
    style.CenterCircleSize = 7.0f;
}

//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//
// ScenePanel::Initialize() veya constructor içinde
void ScenePanel::InitializeShadowMap() {
    // Gölge haritası framebuffer'ı oluştur
    glGenFramebuffers(1, &m_ShadowMapFBO);

    // Gölge haritası dokusu oluştur
    glGenTextures(1, &m_ShadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_ShadowMapWidth, m_ShadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Frustum dışını beyaz yapar
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Framebuffer'a derinlik dokusunu ata
    glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMapTexture, 0);

    // Sadece derinlik yazacağımız için renk buffer'larına yazmayı devre dışı bırak
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    /*
    // Framebuffer'ın tamamlanıp tamamlanmadığını kontrol et
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout<<"Shadow Map Framebuffer incomplete! Status:" << status << std::endl;
    }*/

    // Varsayılan framebuffer'a geri dön
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Işık projeksiyon matrisini hesapla (ışık kaynağınızın türüne göre)
    // Bu değerleri sahnenizdeki objelerin kapsadığı alanı düşünerek ayarlayın.
    // Küçük projenizdeki lightPos'u kullanarak bir örnek:
    // Bu kısım ışığınızın konumuna ve sahnenizin boyutuna göre dinamik olarak güncellenmelidir.
    m_LightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
    // m_LightView ve m_LightSpaceMatrix her karede ışık hareket ettikçe güncellenecek.

    // Işık pozisyonunuzu ve view matrisini güncelleyin (büyük projenizde ışık yönetimi nerede yapılıyorsa oradan alın)
    // Örnek: Sahneye ışık eklediyseniz, ışık objesinin pozisyonundan almalısınız.
    glm::vec3 currentLightPos = glm::vec3(-1.0f, 1.0f, 1.0f);//m_Scene->GetLightSourcePosition(); // Örnek fonksiyon

    m_LightView = glm::lookAt(20.0f*currentLightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Hedef noktayı sahnenin merkezi olarak varsaydım
    m_LightSpaceMatrix = m_LightProjection * m_LightView;


    const std::string shaderPath = "../src/shaders/";
    const auto shadowMapProgram = std::make_shared<Shader>(
    (shaderPath + "shadowMap.vert").c_str(),
    (shaderPath + "shadowMap.frag").c_str()
    );
    m_Scene->SetShadowMapShader(shadowMapProgram);

    // 3. Hesaplanan lightSpaceMatrix'i shadowMapProgram'a uniform olarak gönder
    if (m_Scene && m_Scene->GetShadowMapShader()) { // m_Scene ve shader'ın varlığını kontrol et
        m_Scene->GetShadowMapShader()->use(); // Shader'ı aktif et
        glUniformMatrix4fv(glGetUniformLocation(m_Scene->GetShadowMapShader()->ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(m_LightSpaceMatrix));
    }
}
//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//