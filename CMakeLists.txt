cmake_minimum_required(VERSION 3.30)
project(Black_Engine)

set(CMAKE_CXX_STANDARD 20)

# Visual Studio için UTF-8 desteği
if(MSVC)
    add_compile_options(/utf-8)
endif()

# Testleri derle
option(BUILD_TESTS "Build the test suite" ON)

# ===========================
# Harici Kütüphaneler
# ===========================

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLAD
add_library(glad STATIC
        external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)

# ImGui
add_library(imgui STATIC
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
)

# spdlog
add_subdirectory(external/spdlog)

# JSON
add_subdirectory(external/json)

# ===========================
# Engine Kütüphanesi
# ===========================

# Core dosyaları
set(CORE_FILES
        src/Core/Camera/Camera.h
        src/Core/Camera/Camera.cpp
        src/Core/InputManager/InputManager.h
        src/Core/InputManager/InputManager.cpp
        src/Core/InputManager/InputEvent.h
        src/Core/InputManager/IInputEventReceiver.h
        src/Core/InputSystem/InputSystem.h
        src/Core/InputSystem/InputSystem.cpp
        src/Core/WindowManager/WindowManager.h
        src/Core/WindowManager/WindowManager.cpp
        src/Core/ImGui/ImGuiLayer.h
        src/Core/ImGui/ImGuiLayer.cpp        src/Core/ProjectManager/ProjectManager.h
        src/Core/ProjectManager/ProjectManager.cpp
        src/Core/FileSystem/FileSystem.h
        src/Core/FileSystem/FileSystem.cpp
        src/Core/SceneManager/SceneManager.h
        src/Core/SceneManager/SceneManager.cpp
        src/Core/AssetDatabase/AssetDatabase.h
        src/Core/AssetDatabase/AssetDatabase.cpp
        src/Core/AssetDatabase/Asset.h
          # Logger sistemi
        src/Core/Logger/LogMacros.h
        src/Core/Logger/MockLogger.h
        src/Core/Logger/LogTypes.h
        src/Core/Logger/LogManager.h
        src/Core/Logger/LogManager.cpp
        src/Core/Logger/CategoryInfo.h
        src/Core/Logger/CategoryInfo.cpp
        src/Core/Logger/ConsoleLoggerBackend.cpp
        
        # Utils
        src/Core/Utils/GuidUtils.h
        src/Core/Utils/GuidUtils.cpp
        src/Core/Utils/MetaFile.h
        src/Core/Utils/MetaFile.cpp
)

# Engine dosyaları
set(ENGINE_FILES
        src/Engine/Render/Shader/Shader.h
        src/Engine/Render/Shader/Shader.cpp
        src/Engine/Render/Mesh/Mesh.cpp
        src/Engine/Render/Mesh/Mesh.h
        src/Engine/Render/Primitives/Primitives.cpp
        src/Engine/Render/Primitives/Primitives.h
        src/Engine/Scene/Scene.cpp
        src/Engine/Scene/Scene.h
        src/Engine/Component/BaseComponent.h
        src/Engine/Component/TransformComponent.cpp
        src/Engine/Component/TransformComponent.h
        src/Engine/Component/MeshRendererComponent.cpp
        src/Engine/Component/MeshRendererComponent.h
        src/Engine/Component/MeshComponent.h
        src/Engine/Component/MeshComponent.cpp
        src/Engine/Entity/GameObject.h
        src/Engine/Entity/GameObject.cpp
        src/Engine/Assets/Texture.h
        src/Engine/Assets/Material.h
)

# Editor dosyaları
set(EDITOR_FILES
        src/Editor/UI/Panels/Panel.h
        src/Editor/UI/Panels/Panel.cpp
        src/Editor/UI/Layout/EditorLayout.h
        src/Editor/UI/Layout/EditorLayout.cpp
        src/Editor/UI/Panels/HierarchyPanel/HierarchyPanel.h
        src/Editor/UI/Panels/HierarchyPanel/HierarchyPanel.cpp
        src/Editor/UI/Panels/InspectorPanel/InspectorPanel.h
        src/Editor/UI/Panels/InspectorPanel/InspectorPanel.cpp
        src/Editor/UI/Panels/InspectorPanel/ComponentDrawers.h
        src/Editor/UI/Panels/InspectorPanel/ComponentDrawers.cpp
        src/Editor/UI/Panels/ScenePanel/ScenePanel.h
        src/Editor/UI/Panels/ScenePanel/ScenePanel.cpp
        src/Editor/UI/Panels/GamePanel/GamePanel.h
        src/Editor/UI/Panels/GamePanel/GamePanel.cpp
)

# Engine kütüphanesi
add_library(Engine STATIC
        ${CORE_FILES}
        ${ENGINE_FILES}
        ${EDITOR_FILES}
)

target_include_directories(Engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/json/include
)

target_link_libraries(Engine PUBLIC
        spdlog
        imgui
        glfw
        opengl32
        glad
        nlohmann_json::nlohmann_json
)

# ===========================
# Ana Uygulama
# ===========================

set(APP_FILES
        src/main.cpp
        src/Application.cpp
        src/Application.h
)

add_executable(Black_Engine
        ${APP_FILES}
)

target_link_libraries(Black_Engine PRIVATE Engine)

# ===========================
# Testler
# ===========================

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(external/googletest)
    add_subdirectory(tests)
endif()
