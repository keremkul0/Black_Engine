// src/Core/ImGui/ImGuiLayer.h
#pragma once
#include <string>

class ImGuiLayer {
public:
    static void Init();
    static void Begin();
    static void End();
    static void Shutdown();
};