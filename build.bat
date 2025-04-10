@echo off
setlocal EnableDelayedExpansion

set BUILD_TYPE=normal
set CLEAN_CACHE=0
set RUN_TESTS=0
set BUILD_CONFIG=Debug

rem Parse arguments
:arg_loop
if "%1"=="" goto end_args

if /i "%1"=="clean" (
    set BUILD_TYPE=clean
    shift
    goto arg_loop
)

if /i "%1"=="rebuild" (
    set BUILD_TYPE=rebuild
    shift
    goto arg_loop
)

if /i "%1"=="-cleancache" (
    set CLEAN_CACHE=1
    shift
    goto arg_loop
)

if /i "%1"=="-runtests" (
    set RUN_TESTS=1
    shift
    goto arg_loop
)

if /i "%1"=="-release" (
    set BUILD_CONFIG=Release
    shift
    goto arg_loop
)

if /i "%1"=="-buildconfig" (
    shift
    set BUILD_CONFIG=%1
    shift
    goto arg_loop
)

if /i "%1"=="-help" (
    call :show_usage
    exit /b 0
)

rem Unknown argument, show usage
echo Error: Unknown argument '%1'
call :show_usage
exit /b 1

:end_args

rem Absolute paths
set "PROJECT_ROOT=%~dp0"
set "BUILD_DIR=%PROJECT_ROOT%build"
set "BUILD_CACHE_FILE=%BUILD_DIR%\CMakeCache.txt"

echo Black Engine Build System
echo ------------------------
echo Build Type: %BUILD_TYPE%
echo Configuration: %BUILD_CONFIG%
echo Clean Cache: %CLEAN_CACHE%
echo Run Tests: %RUN_TESTS%
echo ------------------------

rem Handle build type flow
if /i "%BUILD_TYPE%"=="clean" (
    call :clean_build_directory
    call :configure_cmake
    call :build_project
) else if /i "%BUILD_TYPE%"=="rebuild" (
    call :ensure_build_directory
    call :clean_cmake_cache
    call :configure_cmake
    call :build_project
) else (
    rem Normal build
    call :ensure_build_directory
    
    if "%CLEAN_CACHE%"=="1" (
        call :clean_cmake_cache
    )
    
    call :configure_cmake
    call :build_project
)

rem Run tests if requested
if "%RUN_TESTS%"=="1" (
    call :run_project_tests
)

echo Build process completed!
exit /b 0

:show_usage
echo Build script for Black_Engine
echo Usage: build.bat [build-type] [options]
echo.
echo Build Types:
echo   normal               Normal build (default)
echo   clean                Clean build (deletes build directory)
echo   rebuild              Rebuild (cleans cache then builds)
echo.
echo Options:
echo   -cleancache          Cleans CMake cache before building
echo   -runtests            Run tests after building
echo   -release             Short for -buildconfig Release
echo   -buildconfig ^<type^>  Specify build configuration (Debug, Release, RelWithDebInfo, MinSizeRel)
echo   -help                Show this help message
echo.
echo Examples:
echo   build.bat                      # Normal debug build
echo   build.bat clean                # Clean everything and rebuild
echo   build.bat rebuild              # Clean cache and rebuild
echo   build.bat normal -runtests     # Build and run tests
echo   build.bat -release             # Build with Release configuration
goto :eof

:ensure_build_directory
if not exist "%BUILD_DIR%" (
    echo Creating build directory...
    mkdir "%BUILD_DIR%" || (
        echo ERROR: Failed to create build directory
        exit /b 1
    )
)
goto :eof

:clean_build_directory
echo Cleaning build directory...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%" || (
        echo ERROR: Failed to clean build directory
        exit /b 1
    )
)
call :ensure_build_directory
goto :eof

:clean_cmake_cache
echo Cleaning CMake cache...
if exist "%BUILD_CACHE_FILE%" (
    del /f /q "%BUILD_CACHE_FILE%" || (
        echo ERROR: Failed to clean CMake cache
        exit /b 1
    )
)
goto :eof

:configure_cmake
echo Configuring CMake...
pushd "%BUILD_DIR%"
cmake -G "Visual Studio 17 2022" "%PROJECT_ROOT%" || (
    echo ERROR: CMake configuration failed
    exit /b 1
)
popd
goto :eof

:build_project
echo Building project...
pushd "%BUILD_DIR%"
cmake --build . --config %BUILD_CONFIG% || (
    echo ERROR: Build failed
    exit /b 1
)
echo Build completed successfully!
popd
goto :eof

:run_project_tests
echo Running tests...
pushd "%BUILD_DIR%"
ctest --output-on-failure
if errorlevel 1 (
    echo Some tests failed.
) else (
    echo All tests passed!
)
popd
goto :eof
