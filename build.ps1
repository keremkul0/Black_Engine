#!/usr/bin/env pwsh

param (
    [Parameter(Position = 0)]
    [string]$BuildType = "normal",
    
    [Parameter()]
    [switch]$CleanCache,
    
    [Parameter()]
    [switch]$RunTests,
    
    [Parameter()]
    [switch]$Release,
    
    [Parameter()]
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$BuildConfig = "Debug",
    
    [Parameter()]
    [switch]$Run,
    
    [Parameter()]
    [switch]$Help
)

# Absolute paths
$ProjectRoot = $PSScriptRoot
$BuildDir = Join-Path -Path $ProjectRoot -ChildPath "build"
$BuildCacheFile = Join-Path -Path $BuildDir -ChildPath "CMakeCache.txt"

# Help function to display usage
function Show-Usage {
    Write-Host "Build script for Black_Engine"
    Write-Host "Usage: ./build.ps1 [build-type] [options]"
    Write-Host ""
    Write-Host "Build Types:"
    Write-Host "  normal               Normal build (default)"
    Write-Host "  clean                Clean build (deletes build directory)"
    Write-Host "  rebuild              Rebuild (cleans cache then builds)"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -CleanCache          Cleans CMake cache before building"
    Write-Host "  -RunTests            Run tests after building"
    Write-Host "  -Release             Short for -BuildConfig Release"
    Write-Host "  -BuildConfig <type>  Specify build configuration (Debug, Release, RelWithDebInfo, MinSizeRel)"
    Write-Host "  -Help                Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  ./build.ps1                      # Normal debug build"
    Write-Host "  ./build.ps1 clean                # Clean everything and rebuild"
    Write-Host "  ./build.ps1 rebuild              # Clean cache and rebuild"
    Write-Host "  ./build.ps1 normal -RunTests     # Build and run tests"
    Write-Host "  ./build.ps1 -Release             # Build with Release configuration"
    Write-Host "  ./build.ps1 -CleanCache -Release # Clean cache and build Release"
}

# Show help if requested
if ($Help) {
    Show-Usage
    exit 0
}

# Convert Release switch to BuildConfig if specified
if ($Release) {
    $BuildConfig = "Release"
}

Write-Host "Black Engine Build System" -ForegroundColor Cyan
Write-Host "------------------------" -ForegroundColor Cyan
Write-Host "Build Type: $BuildType" -ForegroundColor Yellow
Write-Host "Configuration: $BuildConfig" -ForegroundColor Yellow
Write-Host "Clean Cache: $CleanCache" -ForegroundColor Yellow
Write-Host "Run Tests: $RunTests" -ForegroundColor Yellow
Write-Host "Run Executable: $Run" -ForegroundColor Yellow
Write-Host "------------------------" -ForegroundColor Cyan

# Function to handle failure
function Handle-Error {
    param($ErrorMessage)
    Write-Host "ERROR: $ErrorMessage" -ForegroundColor Red
    exit 1
}

# Create build directory if it doesn't exist
function Ensure-BuildDirectory {
    if (-not (Test-Path $BuildDir)) {
        Write-Host "Creating build directory..." -ForegroundColor Green
        New-Item -ItemType Directory -Path $BuildDir | Out-Null
        if (-not $?) {
            Handle-Error "Failed to create build directory"
        }
    }
}

# Clean build directory
function Clean-BuildDirectory {
    if (Test-Path $BuildDir) {
        Write-Host "Cleaning build directory..." -ForegroundColor Green
        Remove-Item -Recurse -Force $BuildDir | Out-Null
        if (-not $?) {
            Handle-Error "Failed to clean build directory"
        }
    }
    Ensure-BuildDirectory
}

# Clean CMake cache
function Clean-CMakeCache {
    if (Test-Path $BuildCacheFile) {
        Write-Host "Cleaning CMake cache..." -ForegroundColor Green
        Remove-Item -Force $BuildCacheFile | Out-Null
        if (-not $?) {
            Handle-Error "Failed to clean CMake cache"
        }
    }
}

# Configure CMake
function Configure-CMake {
    Write-Host "Configuring CMake..." -ForegroundColor Green
    Set-Location $BuildDir
    
    # Using Visual Studio generator since it's what was used before
    $configureCmd = "cmake -G 'Visual Studio 17 2022' $ProjectRoot"
    Invoke-Expression $configureCmd
    
    if (-not $?) {
        Handle-Error "CMake configuration failed"
    }
}

# Build the project
function Build-Project {
    Write-Host "Building project..." -ForegroundColor Green
    Set-Location $BuildDir
      # Use cmake --build which works across all generators
    cmake --build . --config $BuildConfig
    
    if (-not $?) {
        Handle-Error "Build failed"
    }
    
    Write-Host "Build completed successfully!" -ForegroundColor Green
}

# Run tests
function Run-ProjectTests {
    Write-Host "Running tests..." -ForegroundColor Green
    Set-Location $BuildDir
    
    # Using CTest to run the tests
    ctest --output-on-failure
    
    if (-not $?) {
        Write-Host "Some tests failed." -ForegroundColor Red
        # We don't exit here to allow continuing the workflow
    } else {
        Write-Host "All tests passed!" -ForegroundColor Green
    }
}

# Main execution flow based on build type
switch ($BuildType.ToLower()) {
    "clean" {
        Clean-BuildDirectory
        Configure-CMake
        Build-Project
    }
    "rebuild" {
        Ensure-BuildDirectory
        Clean-CMakeCache
        Configure-CMake
        Build-Project
    }
    default { # normal build
        Ensure-BuildDirectory
        
        if ($CleanCache) {
            Clean-CMakeCache
        }
        
        Configure-CMake
        Build-Project
    }
}

# Run tests if requested
if ($RunTests) {
    Run-ProjectTests
}

# Run the executable if requested
function Run-Executable {
    Write-Host "Running Black_Engine executable..." -ForegroundColor Green
    
    # Executable path depends on build configuration
    $exePath = Join-Path -Path $BuildDir -ChildPath "$BuildConfig\Black_Engine.exe"
    
    # Check if executable exists
    if (-not (Test-Path $exePath)) {
        # Try alternative path structure (Visual Studio might put it in different locations)
        $exePath = Join-Path -Path $BuildDir -ChildPath "Debug\Black_Engine.exe"
        if (-not (Test-Path $exePath)) {
            Handle-Error "Executable not found at $exePath"
        }
    }
    
    # Run the executable
    Write-Host "Starting: $exePath" -ForegroundColor Green
    Start-Process -FilePath $exePath -Wait
    
    if (-not $?) {
        Write-Host "Program exited with an error." -ForegroundColor Red
    } else {
        Write-Host "Program completed successfully!" -ForegroundColor Green
    }
}

# Run the executable if requested
if ($Run) {
    Run-Executable
}

# Return to the project root
Set-Location $ProjectRoot

Write-Host "Build process completed!" -ForegroundColor Cyan
