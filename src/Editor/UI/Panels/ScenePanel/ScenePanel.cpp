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
}

void ScenePanel::OnUpdate(const float deltaTime) {
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
        UpdateCursor();

        // Yalnızca rotasyon modunda WASD hareketleri ile kamera hareketini işle.
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
    } else {
        // Panel focus veya hover değilse, kamera kontrol modlarını sıfırla ve cursor'u default yap.
        m_IsRotating = false;
        m_IsPanning = false;
        if (m_CurrentCursor != InputManager::DEFAULT_CURSOR) {
            m_CurrentCursor = InputManager::DEFAULT_CURSOR;
            InputManager::SetCursor(m_CurrentCursor);
        }
    }
}

bool ScenePanel::OnInputEvent(const InputEvent &event) {
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

    if (m_FramebufferID > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
        glViewport(0, 0, static_cast<int>(contentRegionAvail.x), static_cast<int>(contentRegionAvail.y));
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_Scene) {
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

        // Draw ImGuizmo if object is selected
        DrawGuizmo();
    }

    // Debug information
    ImGui::SetCursorPos(ImVec2(10, 10));
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

void ScenePanel::ResizeFramebuffer(const int width, const int height) {
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
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

Math::Ray ScenePanel::ScreenToWorldRay(const glm::vec2 &mousePos) {
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

std::shared_ptr<GameObject> ScenePanel::FindObjectUnderMouse(const Math::Ray &ray) {
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
        m_SelectedObject = hitObject;
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
    if (!m_SelectedObject || !m_SelectedObject->IsActive() || !m_Scene || !m_Scene->HasGameObject(m_SelectedObject)) {
        // Silinmiş veya sahnede olmayan obje varsa temizle
        SelectionManager::GetInstance().ClearSelection();
        m_SelectedObject = nullptr;
        return;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    glDisable(GL_DEPTH_TEST);

    for (const auto& comp: m_SelectedObject->GetComponents()) {
        comp->DrawWireframe();
    }

    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
}


void ScenePanel::DrawGuizmo() {
    // Seçili obje yoksa gizmo'yu çizme
    if (!m_SelectedObject || !m_Scene || !m_Scene->HasGameObject(m_SelectedObject)) {
        SelectionManager::GetInstance().ClearSelection();
        m_SelectedObject = nullptr;
        return;
    }

    auto transformComp = m_SelectedObject->GetComponent<TransformComponent>();
    if (!transformComp) return;

    // ImGuizmo ayarları
    ImGuizmo::SetOrthographic(false); // Perspektif kamera kullanılıyor
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(m_PanelMin.x, m_PanelMin.y, m_PanelSize.x, m_PanelSize.y);

    // ImGuizmo'nun ekran içinde daha belirgin görünmesi için boyutunu ayarla
    ImGuizmo::SetGizmoSizeClipSpace(0.15f);

    // Eksen çizgilerinin sabit kalması için ekstra ayarlar
    ImGuizmo::AllowAxisFlip(false); // Eksenlerin ters dönmesini engelle

    // Güncel model matrisini al - Her çerçevede yeniden hesaplanmalı
    glm::mat4 modelMatrix = transformComp->GetModelMatrix();

    // View ve projection matrisleri
    float *view = glm::value_ptr(m_ViewMatrix);
    float *proj = glm::value_ptr(m_ProjectionMatrix);
    float *model = glm::value_ptr(modelMatrix);

    // Gizmo operasyon ve mod ayarları
    const ImGuizmo::OPERATION operation = m_CurrentGizmoOperation;
    const ImGuizmo::MODE mode = m_CurrentGizmoMode;

    // Snap ayarları
    float snapValues[3] = {0.5f, 0.5f, 0.5f}; // Varsayılan snap değerleri
    bool useSnap = ImGui::IsKeyPressed(ImGuiKey_LeftShift) || ImGui::IsKeyPressed(ImGuiKey_RightShift);

    // Operasyon türüne göre snap değerlerini ayarla
    if (useSnap) {
        if (operation == ImGuizmo::TRANSLATE) {
            snapValues[0] = snapValues[1] = snapValues[2] = 0.5f; // 0.5 birim hareket
        } else if (operation == ImGuizmo::ROTATE) {
            snapValues[0] = snapValues[1] = snapValues[2] = 45.0f; // 45 derece rotasyon
        } else if (operation == ImGuizmo::SCALE) {
            snapValues[0] = snapValues[1] = snapValues[2] = 0.1f; // 0.1 ölçek adımı
        }
    }

    // MANIPULATE ÇAĞRISI - Burada ImGuizmo model matrisini manipüle ediyor
    ImGuizmo::Manipulate(
        view,
        proj,
        operation,
        mode,
        model,
        nullptr, // delta matris
        useSnap ? snapValues : nullptr
    );

    // Gizmo durum göstergeleri
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

    // ImGuizmo kullanılıyorsa ve değişiklik olduysa transform bileşenini güncelle
    if (ImGuizmo::IsUsing()) {
        // Manipüle edilmiş model matrisi -> transform değerlerine dönüştür
        glm::vec3 translation, rotation, scale;
        bool success = Math::DecomposeTransform(modelMatrix, translation, rotation, scale);

        if (success) {
            // Rotasyon değerlerini derece cinsine çevir
            rotation = glm::degrees(rotation);

            // Transform bileşenini yeni değerlerle güncelle
            transformComp->SetPosition(translation);
            transformComp->SetRotation(rotation);
            transformComp->SetScale(scale);

            // Model matrisinin güncellendiğinden emin ol
            transformComp->UpdateModelMatrix();
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
    style.RotationOuterLineThickness = 4.0f;
    style.ScaleLineThickness = 3.0f;
    style.ScaleLineCircleSize = 7.0f;
    style.CenterCircleSize = 7.0f;
}
