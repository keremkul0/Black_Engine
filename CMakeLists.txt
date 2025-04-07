cmake_minimum_required(VERSION 3.30)
project(Black_Engine)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build the test suite" ON)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif()

# =========================================
# External Libraries Configuration
# =========================================

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

# JSON for Modern C++
add_subdirectory(external/json)

# =========================================
# Engine Source Files
# =========================================

# Core Files
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
    src/Core/ImGui/ImGuiLayer.cpp
    src/Core/ProjectManager/ProjectManager.h
    src/Core/ProjectManager/ProjectManager.cpp
    src/Core/FileSystem/FileSystem.h
    src/Core/FileSystem/FileSystem.cpp
    src/Core/Logger/Logger.h
    src/Core/Logger/Logger.cpp
)

# Engine Files
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
)

# Editor Files
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

# Main Application
set(APP_FILES
    src/main.cpp
    src/Application.cpp
    src/Application.h
        src/Core/SceneManager/SceneManager.h
        src/Core/SceneManager/SceneManager.cpp
        src/Engine/Assets/Texture.h
        src/Engine/Assets/Material.h
        src/Core/AssetDatabase/AssetDatabase.h
        src/Core/AssetDatabase/AssetDatabase.cpp
        src/Core/AssetDatabase/Asset.h
        src/Core/Logger/LogMacros.h
)

# =========================================
# Main Target Configuration
# =========================================

add_executable(Black_Engine
    ${APP_FILES}
    ${CORE_FILES}
    ${ENGINE_FILES}
    ${EDITOR_FILES}
)

target_include_directories(Black_Engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/json/include
)

target_link_libraries(Black_Engine PRIVATE
    glad
    imgui
    glfw
    opengl32
    spdlog
    nlohmann_json::nlohmann_json
)

# =========================================
# Tests Configuration
# =========================================

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(external/googletest)
    add_subdirectory(tests)
endif()