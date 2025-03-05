// src/Editor/UI/EditorLayout.cpp
    #include "EditorLayout.h"
    #include "imgui.h"
    #include "imgui_internal.h" // Required for docking API

    EditorLayout::EditorLayout() : m_UseDockspace(true), m_FirstFrame(true), m_DockspaceID(0) {}

void EditorLayout::RenderLayout() {
        if (m_UseDockspace) {
            SetupDockspace();
        }

   // // Test için basit bir ImGui penceresi
   // ImGui::Begin("Test Penceresi");
   // ImGui::Text("Merhaba ImGui!");
   // if (ImGui::Button("Tıkla Beni")) {
   //     // Buton tıklama işlemi
   // }
   // ImGui::End();

        // Mevcut panelleri çiz
        for (auto& [name, panel] : m_Panels) {
            panel->Render();
        }
    }

    void EditorLayout::SetupDockspace() {
        // Dockspace flags
        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        // Make dockspace window fullscreen
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        // Begin dockspace
        ImGui::Begin("DockSpace", nullptr, windowFlags);
        ImGui::PopStyleVar(3);

        // Submit the dockspace
        m_DockspaceID = ImGui::GetID("MainDockspace");

        if (ImGui::DockBuilderGetNode(m_DockspaceID) == nullptr || m_FirstFrame)
        {
            m_FirstFrame = false;
            ImGui::DockBuilderRemoveNode(m_DockspaceID);
            ImGui::DockBuilderAddNode(m_DockspaceID, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(m_DockspaceID, viewport->Size);

            // Define default layout
            ImGuiID dockMainID = m_DockspaceID;
            ImGuiID dockLeftID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.2f, nullptr, &dockMainID);
            ImGuiID dockRightID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, 0.25f, nullptr, &dockMainID);
            ImGuiID dockBottomID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.25f, nullptr, &dockMainID);

            // Set dock locations for standard panels
            ImGui::DockBuilderDockWindow("Hierarchy", dockLeftID);
            ImGui::DockBuilderDockWindow("Inspector", dockRightID);
            ImGui::DockBuilderDockWindow("Scene", dockMainID);
            ImGui::DockBuilderDockWindow("Game", dockBottomID);

            ImGui::DockBuilderFinish(m_DockspaceID);
        }

        // In SetupDockspace() function:
        ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        // Missing this line:
        ImGui::End(); // You need to end the dockspace window
    }