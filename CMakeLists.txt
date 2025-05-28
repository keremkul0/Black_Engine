cmake_minimum_required(VERSION 3.30)
project(Black_Engine)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build the test suite" ON)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif()

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)

add_library(glad STATIC
        external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)

# First define imgui library so ImGuizmo can find it
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

# Now define ImGuizmo library after imgui is set up
add_library(ImGuizmo STATIC
        external/ImGuizmo/ImGuizmo.cpp
)

target_include_directories(ImGuizmo PUBLIC
        external/ImGuizmo
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
)

add_executable(Black_Engine
        src/main.cpp
        src/Application.cpp
        src/Application.h
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
        src/Engine/Entity/GameObject.h
        src/Engine/Entity/GameObject.cpp
        src/Core/Camera/Camera.h
        src/Core/Camera/Camera.cpp
        src/Core/InputManager/InputManager.h
        src/Core/InputManager/InputManager.cpp
        src/Core/WindowManager/WindowManager.h
        src/Core/WindowManager/WindowManager.cpp
        src/Editor/UI/Panels/Panel.h
        src/Editor/UI/Panels/Panel.cpp
        src/Editor/UI/Layout/EditorLayout.h
        src/Editor/UI/Layout/EditorLayout.cpp
        src/Editor/UI/Panels/HierarchyPanel/HierarchyPanel.h
        src/Editor/UI/Panels/InspectorPanel/InspectorPanel.h
        src/Editor/UI/Panels/ScenePanel/ScenePanel.h
        src/Editor/UI/Panels/GamePanel/GamePanel.h
        src/Core/ImGui/ImGuiLayer.h
        src/Core/ImGui/ImGuiLayer.cpp
        src/Editor/UI/Panels/HierarchyPanel/HierarchyPanel.cpp
        src/Editor/UI/Panels/GamePanel/GamePanel.cpp
        src/Editor/UI/Panels/ScenePanel/ScenePanel.cpp
        src/Editor/UI/Panels/InspectorPanel/InspectorPanel.cpp
        src/Core/InputManager/InputEvent.h
        src/Core/InputSystem/InputSystem.h
        src/Core/InputSystem/InputSystem.cpp
        src/Core/InputManager/IInputEventReceiver.h
        src/Engine/Component/MeshComponent.h
        src/Engine/Component/MeshComponent.cpp
        src/Editor/UI/Panels/InspectorPanel/ComponentDrawers.h
        src/Editor/UI/Panels/InspectorPanel/ComponentDrawers.cpp
        src/Editor/SelectionManager.h
        src/Editor/SelectionManager.cpp

)

target_include_directories(Black_Engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/ImGuizmo
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
)

target_link_libraries(Black_Engine PRIVATE
        glad
        imgui
        glfw
        opengl32
        ImGuizmo
)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(external/googletest)
    add_subdirectory(tests)
endif()