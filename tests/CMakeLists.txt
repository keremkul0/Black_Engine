# tests/CMakeLists.txt

# Create test executable
add_executable(unit_tests
        TestsComponents/TestComponents.cpp
        TestsComponents/TestTransform.cpp
)

# We need to create a library from your engine code to link against
add_library(engine_lib STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/Engine/Entity/GameObject.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/Engine/Component/BaseComponent.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/Engine/Component/TransformComponent.cpp
)

target_include_directories(engine_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui
)

include_directories(${CMAKE_SOURCE_DIR}/external/glm)

# Link Google Test, ImGui and your engine code
target_link_libraries(unit_tests
        PRIVATE
        gtest
        gtest_main
        engine_lib
        imgui
)

target_include_directories(unit_tests PRIVATE
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui
)

include(GoogleTest)
gtest_discover_tests(unit_tests)