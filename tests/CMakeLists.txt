# Test yürütülebilir dosyasını oluştur
add_executable(unit_tests
        test_main.cpp
        TestsComponents/TestComponents.cpp
        TestsComponents/TestTransform.cpp
        TestLogger/TestsLogger.cpp
        TestsFileSystem/TestFileSystem.cpp
)

# Testlerin Engine kütüphanesi ve Google Test ile bağlanması
target_link_libraries(unit_tests
        PRIVATE
        gtest
        gtest_main
        Engine
)

# Test dosyaları ek include dizinleri gerektirebilir
# Projenin geri kalanını (Engine'in header'ları vb.) parent CMake'ten alıyoruz
target_include_directories(unit_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/TestsComponents
        ${CMAKE_CURRENT_SOURCE_DIR}/TestLogger
        ${CMAKE_CURRENT_SOURCE_DIR}/TestsFileSystem
)

# GoogleTest test keşfi
include(GoogleTest)
gtest_discover_tests(unit_tests)
