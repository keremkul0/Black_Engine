// src/Core/ImGui/ImGuiLayer.cpp
            #include "ImGuiLayer.h"
            #include "imgui.h"
            #include "imgui_internal.h"  // Crucial for docking features
            #include "imgui_impl_glfw.h"
            #include "imgui_impl_opengl3.h"
            #include <GLFW/glfw3.h>

            extern GLFWwindow* g_Window;

            void ImGuiLayer::Init() {
                IMGUI_CHECKVERSION();
                ImGui::CreateContext();
                ImGuiIO& io = ImGui::GetIO();
                io.Fonts->AddFontDefault();
                // Enable docking
                io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
                // Enable multi-viewport / platform windows
                io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

                // Setup Dear ImGui style
                ImGui::StyleColorsDark();

                // When viewports are enabled we tweak WindowRounding/WindowBg
                ImGuiStyle& style = ImGui::GetStyle();
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                    style.WindowRounding = 0.0f;
                    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
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
                ImGuiIO& io = ImGui::GetIO();

                // Get actual window size
                int width, height;
                glfwGetFramebufferSize(g_Window, &width, &height);
                io.DisplaySize = ImVec2((float)width, (float)height);

                // Rendering
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                // Update and Render additional Platform Windows
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