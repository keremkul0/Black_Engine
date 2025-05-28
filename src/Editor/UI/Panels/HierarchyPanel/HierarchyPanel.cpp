
#include "HierarchyPanel.h"
#include "imgui.h"
#include "Editor/SelectionManager.h"
#include <iostream>
#include <GLFW/glfw3.h>

HierarchyPanel::HierarchyPanel(const std::string &title, const std::shared_ptr<Scene> &scene) 
    : Panel(title), m_Scene(scene), m_SelectedObject(nullptr) {
    memset(m_SearchBuffer, 0, sizeof(m_SearchBuffer));
}

void HierarchyPanel::SetScene(const std::shared_ptr<Scene>& scene) {
    m_Scene = scene;
}

void HierarchyPanel::SetSelectedObject(const std::shared_ptr<GameObject>& gameObject) {
    m_SelectedObject = gameObject;
    std::cout << "HierarchyPanel::SetSelectedObject cagrildi: " <<
        (gameObject ? gameObject->GetName() : "nullptr") << std::endl;
}

void HierarchyPanel::DrawContent() {
    if (!m_Scene) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No scene loaded");
        return;
    }

    // Sağ tık menüsü işlemleri
    bool openPopup = false;
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
        openPopup = true;
        std::cout << "Sağ tık algılandı, popup açılmalı" << std::endl;
    }

    if (openPopup) {
        ImGui::OpenPopup("HierarchyCreateMenu");
    }

    if (ImGui::BeginPopup("HierarchyCreateMenu")) {
        std::cout << "Popup açıldı" << std::endl;

        if (ImGui::MenuItem("Empty GameObject")) {
            std::cout << "Empty GameObject seçildi" << std::endl;
            m_Scene->CreatePrimitive("Empty");
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::BeginMenu("3D Primitives")) {
            std::cout << "3D Primitives menüsü açıldı" << std::endl;

            if (ImGui::MenuItem("Cube")) {
                std::cout << "Cube seçildi" << std::endl;
                m_Scene->CreatePrimitive("Cube");
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }

    // Nesne oluşturma bölümü - katlanabilir başlık içinde
    if (ImGui::CollapsingHeader("Nesne Oluştur", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent(10.0f);
        
        if (ImGui::Button("Empty GameObject", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            m_Scene->CreatePrimitive("Empty");
        }
        
        ImGui::Text("3D Primitives:");
        ImGui::Separator();
        
        float buttonWidth = (ImGui::GetContentRegionAvail().x - 10.0f) / 2.0f;
        
        if (ImGui::Button("Cube", ImVec2(buttonWidth, 0))) {
            std::cout << "Tıklama alındı" << std::endl;
            m_Scene->CreatePrimitive("Cube");
        }
        ImGui::SameLine();
        if (ImGui::Button("Sphere", ImVec2(buttonWidth, 0))) {
            m_Scene->CreatePrimitive("Sphere");
        }
        
        if (ImGui::Button("Plane", ImVec2(buttonWidth, 0))) {
            m_Scene->CreatePrimitive("Plane");
        }
        ImGui::SameLine();
        if (ImGui::Button("Cylinder", ImVec2(buttonWidth, 0))) {
            m_Scene->CreatePrimitive("Cylinder");
        }
        
        if (ImGui::Button("Capsule", ImVec2(buttonWidth, 0))) {
            m_Scene->CreatePrimitive("Capsule");
        }
        
        ImGui::Unindent(10.0f);
    }

    // Sahne hiyerarşisi bölümü - katlanabilir başlık içinde
    if (ImGui::CollapsingHeader("Sahne Hiyerarşisi", ImGuiTreeNodeFlags_DefaultOpen)) {
        std::shared_ptr<GameObject> selectedObject = SelectionManager::GetInstance().GetSelectedObject();
        if (selectedObject) {
            ImGui::BeginGroup();
            ImGui::Text("Seçili: %s", selectedObject->GetName().c_str());
            
            ImGui::SameLine(ImGui::GetWindowWidth() - 100);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("Sil##SelectedObject", ImVec2(80, 0))) {
                std::cout << "Sil butonu tıklandı" << std::endl;
                DeleteSelectedObject();
            }
            ImGui::PopStyleColor(2);
            ImGui::EndGroup();
            
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Silmek için DELETE tuşunu kullanabilirsiniz");
            ImGui::Separator();
        }

        const auto& objects = m_Scene->GetGameObjects();
        
        if (objects.empty()) {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Sahnede nesne yok");
        } else {
            for (const auto& object : objects) {
                DrawGameObjectNode(object, selectedObject);
            }
        }
    }
}

void HierarchyPanel::DrawGameObjectNode(
    const std::shared_ptr<GameObject>& object,
    const std::shared_ptr<GameObject>& selectedObject)
{
    if (!object) return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (selectedObject == object)
        flags |= ImGuiTreeNodeFlags_Selected;

    const auto& children = object->GetChildren();
    if (children.empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    void* nodeId = reinterpret_cast<void*>(reinterpret_cast<intptr_t>(object.get()));

    bool isOpen = ImGui::TreeNodeEx(nodeId, flags, "%s", object->GetName().c_str());

    if (ImGui::IsItemClicked()) {
        SelectionManager::GetInstance().SetSelectedObject(object);
        m_SelectedObject = object;
        std::cout << "Nesne ağaçta seçildi: " << object->GetName() << std::endl;
    }

    // Sağ tıklama popup menüsü
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Sil")) {
            DeleteObject(object);
            ImGui::EndPopup();
            if (isOpen) ImGui::TreePop(); // TreeNode kapat
            return; // Fonksiyondan çık
        }
        ImGui::EndPopup();
    }

    if (isOpen) {
        for (const auto& child : children) {
            DrawGameObjectNode(child, selectedObject); // recursive çağrı
        }
        ImGui::TreePop(); // En sonda kapat
    }
}

void HierarchyPanel::DeleteObject(const std::shared_ptr<GameObject>& object) {
    if (!object || !m_Scene) {
        std::cout << "Silme işlemi başarısız: Nesne veya sahne yok" << std::endl;
        return;
    }

    try {
        std::string objName = object->GetName();

        std::cout << "Nesne siliniyor: " << objName << std::endl;

        SelectionManager::GetInstance().ClearSelection();

        m_SelectedObject = nullptr;

        m_Scene->RemoveGameObject(object);

        std::cout << "Nesne silindi, render güncellemesi gerekiyor" << std::endl;

        std::cout << "Silme işlemi tamamlandı: " << objName << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Silme sırasında hata: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Silme sırasında bilinmeyen hata!" << std::endl;
    }
}
        
        void HierarchyPanel::DeleteSelectedObject() {
            auto selectedObject = SelectionManager::GetInstance().GetSelectedObject();
            
            if (selectedObject) {
                std::cout << "----------------------------------------" << std::endl;
                std::cout << "Seçili nesne siliniyor: " << selectedObject->GetName() << std::endl;
                
                SelectionManager::GetInstance().ClearSelection();
                
                selectedObject->isSelected = false;
                
                m_SelectedObject = nullptr;
                
                m_Scene->RemoveGameObject(selectedObject);
                
                std::cout << "Seçili nesne başarıyla silindi" << std::endl;
                std::cout << "----------------------------------------" << std::endl;
                
                ImGui::SetWindowFocus(nullptr); // Mevcut odağı kaldır
                
            } else {
                std::cout << "----------------------------------------" << std::endl;
        std::cout << "Silinecek seçili nesne yok!" << std::endl;
    }
}

void HierarchyPanel::DrawNode(const std::shared_ptr<GameObject>& object) {
    DrawGameObjectNode(object, m_SelectedObject);
}

bool HierarchyPanel::OnInputEvent(const InputEvent& event) {
    if (event.type == InputEventType::KeyDown) {
        if (event.key == GLFW_KEY_DELETE) {
            std::cout << "HierarchyPanel: DELETE tuşu algılandı" << std::endl;
            
            // SelectionManager'dan seçili nesneyi al
            auto selectedObject = SelectionManager::GetInstance().GetSelectedObject();
            
            if (selectedObject) {
                std::cout << "HierarchyPanel: DELETE tuşu ile seçili nesne siliniyor: " 
                          << selectedObject->GetName() << std::endl;
                
                SelectionManager::GetInstance().ClearSelection();
                
                selectedObject->isSelected = false;
                
                m_SelectedObject = nullptr;
                
                m_Scene->RemoveGameObject(selectedObject);
                
                std::cout << "DELETE tuşu ile nesne başarıyla silindi" << std::endl;
                
                ImGui::SetWindowFocus(nullptr);
                
                return true;
            } else {
                std::cout << "HierarchyPanel: Silinecek seçili nesne yok (SelectionManager)" << std::endl;
                
                if (m_SelectedObject) {
                    std::cout << "HierarchyPanel: Lokal seçili nesne bulundu, siliniyor: " 
                              << m_SelectedObject->GetName() << std::endl;
                    
                    auto objToDelete = m_SelectedObject;
                    m_SelectedObject = nullptr;
                    
                    objToDelete->isSelected = false;
                    
                    SelectionManager::GetInstance().ClearSelection();
                    
                    m_Scene->RemoveGameObject(objToDelete);
                    
                    ImGui::SetWindowFocus(nullptr);
                    
                    return true;
                } else {
                    std::cout << "HierarchyPanel: Lokal olarak da seçili nesne yok" << std::endl;
                }
            }
        }
    }
    return false;
}