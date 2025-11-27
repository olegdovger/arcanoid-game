@echo off
set BUILD_DIR=build

echo Cleaning build directory...

if exist %BUILD_DIR% (
    echo Removing %BUILD_DIR%...
    rmdir /s /q %BUILD_DIR%
    echo Build directory cleaned.
) else (
    echo Build directory does not exist, nothing to clean.
)

