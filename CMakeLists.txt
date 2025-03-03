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

add_executable(Black_Engine
        src/main.cpp
        src/Application.cpp
        src/Application.h
        src/Engine/Render/Shader.h
        src/Engine/Render/Shader.cpp
        src/Engine/Render/Mesh.cpp
        src/Engine/Render/Mesh.h
        src/Engine/Render/Primitives.cpp
        src/Engine/Render/Primitives.h
        src/Engine/Scene/Scene.cpp
        src/Engine/Scene/Scene.h
        src/Engine/Component/BaseComponent.h
        src/Engine/Component/BaseComponent.cpp
        src/Engine/Component/TransformComponent.cpp
        src/Engine/Component/TransformComponent.h
        src/Engine/Component/MeshRendererComponent.cpp
        src/Engine/Component/MeshRendererComponent.h
        src/Engine/Entity/GameObject.h
        src/Engine/Entity/GameObject.cpp
)

target_include_directories(Black_Engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
)

target_link_libraries(Black_Engine PRIVATE
        glad
        imgui
        glfw
        opengl32
)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(external/googletest)
    add_subdirectory(tests)
endif()