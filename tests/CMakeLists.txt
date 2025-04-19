add_executable(unit_tests
        test_main.cpp
        TestsComponents/TestComponents.cpp
        TestsComponents/TestTransform.cpp
        TestsFileSystem/TestFileSystem.cpp
        TestLogger/CategoryInfoTests.cpp
        TestLogger/LogManagerTests.cpp
        TestLogger/BackendTests.cpp
        TestLogger/ConfigLoadTests.cpp
)

target_link_libraries(unit_tests
        PRIVATE
        gtest
        gtest_main
        Engine
)

target_include_directories(unit_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/TestsComponents
        ${CMAKE_CURRENT_SOURCE_DIR}/TestLogger
        ${CMAKE_CURRENT_SOURCE_DIR}/TestsFileSystem
)

include(GoogleTest)
gtest_discover_tests(unit_tests)