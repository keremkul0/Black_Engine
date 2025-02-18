cmake_minimum_required(VERSION 3.30)
project(Black_Engine)

set(CMAKE_CXX_STANDARD 20)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/external/glfw/include)
include_directories(${CMAKE_SOURCE_DIR}/external/glm)

# Set build directory
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/../build)

# GLFW'de gereksiz örnekler, testler, dokümantasyon kapatılıyor.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# GLFW'yi alt proje olarak ekle. (external/glfw içinde kendi CMakeLists'i var)
add_subdirectory(external/glfw)

# GLAD'i static kütüphane olarak ekle.
add_library(glad STATIC
        external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)

# Ana proje (executable)
add_executable(Black_Engine main.cpp)

# GLM header-only. Sadece include yolunu ekliyoruz.
target_include_directories(Black_Engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
)

# Windows için OpenGL fonksiyonları: opengl32 eklenir.
# (Linux'ta -lGL, macOS'ta farklı ayarlar gerekir.)
target_link_libraries(Black_Engine PRIVATE
        glad
        glfw
        opengl32
)
