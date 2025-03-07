// src/Core/ImGui/ImGuiLayer.cpp
#include "ImGuiLayer.h"
#include "imgui.h"
#include "imgui_internal.h"  // Crucial for docking features
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

extern GLFWwindow *g_Window;

void ImGuiLayer::Init() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Enable docking and multi-viewport features
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Add these to enable native window features
    io.ConfigViewportsNoAutoMerge = false;
    io.ConfigViewportsNoTaskBarIcon = false;
    io.ConfigWindowsResizeFromEdges = true;
    io.ConfigWindowsMoveFromTitleBarOnly = false; // Allow moving from any part

    // Set up ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    // Adjust style for better appearance with viewports
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        // Add window padding for better appearance
        style.WindowPadding = ImVec2(8.0f, 8.0f);

        // Better window border
        style.WindowBorderSize = 1.0f;
    }

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::Begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End() {
    // Complete the Dear ImGui frame
    ImGui::Render();

    // Get current window size
    int display_w, display_h;
    glfwGetFramebufferSize(g_Window, &display_w, &display_h);

    // Render Dear ImGui draw data
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it)
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiLayer::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
