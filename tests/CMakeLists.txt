# tests/CMakeLists.txt

# Create test executable
add_executable(unit_tests
        test_main.cpp
        TestsComponents/TestComponents.cpp
        TestsComponents/TestTransform.cpp
        TestLogger/TestsLogger.cpp
        TestsFileSystem/TestFileSystem.cpp
)

# We need to create a library from your engine code to link against
add_library(engine_lib STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/Engine/Entity/GameObject.cpp
        ../src/Engine/Component/TransformComponent.cpp
        ../src/Core/FileSystem/FileSystem.cpp
        # Added Logger implementation
        ../src/Core/Logger/LoggerManager.cpp
        ../src/Core/Logger/SpdlogLogger.cpp
        ../src/Core/Logger/MockLogger.cpp
)

target_include_directories(engine_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/spdlog/include
)

include_directories(${CMAKE_SOURCE_DIR}/external/glm)

# Link Google Test, ImGui and your engine code
target_link_libraries(unit_tests
        PRIVATE
        gtest
        engine_lib
        imgui
        spdlog
)

target_include_directories(unit_tests PRIVATE
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/spdlog/include
)

include(GoogleTest)
gtest_discover_tests(unit_tests)