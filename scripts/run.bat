@echo off
set BUILD_DIR=build
set EXE=%BUILD_DIR%\bin\arcanoid-game.exe

if not exist %EXE% (
    echo Executable not found: %EXE%
    echo Building project first...
    call "%~dp0build.bat"
    if errorlevel 1 (
        echo Build failed, cannot run.
        exit /b 1
    )
)

echo Running arcanoid-game...
echo.
%EXE%

if errorlevel 1 (
    echo.
    echo Program exited with error code: %ERRORLEVEL%
    pause
)
