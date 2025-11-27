@echo off
setlocal enabledelayedexpansion

set BUILD_DIR=build
set GENERATOR=Ninja

echo ========================================
echo Building arcanoid-game with Ninja
echo ========================================
echo.

REM Create build directory if it doesn't exist
if not exist %BUILD_DIR% (
    echo Creating build directory...
    mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

REM Configure with CMake if build.ninja doesn't exist (Ninja-specific check)
if not exist build.ninja (
    echo Configuring CMake with Ninja generator...
    cmake -G %GENERATOR% ..
    if errorlevel 1 (
        echo ERROR: CMake configuration failed!
        cd ..
        exit /b 1
    )
) else (
    echo CMake already configured for Ninja, skipping...
)

echo.
echo Building project with Ninja...
ninja
if errorlevel 1 (
    echo ERROR: Build failed!
    cd ..
    exit /b 1
)

cd ..

echo.
echo ========================================
echo Build completed successfully!
echo Executable: %BUILD_DIR%\bin\arcanoid-game.exe
echo ========================================

