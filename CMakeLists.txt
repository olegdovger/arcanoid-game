cmake_minimum_required(VERSION 3.20)
project(arcanoid-game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Try to find SFML 3.0.0 manually (SFML 3.0 CMake config has issues, so we use manual setup)
set(SFML_FOUND FALSE)

# Prioritize SFML-3.0.0 installation
set(SFML_ROOT_SEARCH_PATHS
        "C:/SFML-3.0.0"
        "C:/msys64/ucrt64"
        "C:/msys64/mingw64"
        "C:/msys64/usr"
    )

    foreach(SEARCH_PATH ${SFML_ROOT_SEARCH_PATHS})
        if(EXISTS "${SEARCH_PATH}/include/SFML")
            set(SFML_ROOT ${SEARCH_PATH})
            set(SFML_INCLUDE_DIR "${SEARCH_PATH}/include")
            set(SFML_LIB_DIR "${SEARCH_PATH}/lib")
            message(STATUS "Found SFML at: ${SFML_ROOT}")
            break()
        endif()
    endforeach()

    if(SFML_ROOT)
        # Detect compiler type to choose correct library format
        if(MSVC)
            # For MSVC, prefer .lib files
            set(SFML_LIB_NAMES_SYSTEM "sfml-system" "sfml-system.lib")
            set(SFML_LIB_NAMES_WINDOW "sfml-window" "sfml-window.lib")
            set(SFML_LIB_NAMES_GRAPHICS "sfml-graphics" "sfml-graphics.lib")
        else()
            # For MinGW/GCC, use .a files (static libraries)
            set(SFML_LIB_NAMES_SYSTEM "libsfml-system.a" "sfml-system.a")
            set(SFML_LIB_NAMES_WINDOW "libsfml-window.a" "sfml-window.a")
            set(SFML_LIB_NAMES_GRAPHICS "libsfml-graphics.a" "sfml-graphics.a")
            # Additional dependencies for static linking
            set(SFML_STATIC_DEPS
                "C:/SFML-3.0.0/lib/libfreetype.a"
                "C:/SFML-3.0.0/lib/libogg.a"
                "C:/SFML-3.0.0/lib/libvorbis.a"
                "C:/SFML-3.0.0/lib/libvorbisfile.a"
                "C:/SFML-3.0.0/lib/libFLAC.a"
                "-lopengl32"
                "-lwinmm"
                "-lgdi32"
                "-lkernel32"
                "-luser32"
                "-lgdi32"
                "-lwinspool"
                "-lshell32"
                "-lole32"
                "-loleaut32"
                "-luuid"
                "-lcomdlg32"
                "-ladvapi32"
                "-static-libgcc"
                "-static-libstdc++"
                # "-Wl,--subsystem,windows"  # Commented out for debugging - shows console window
            )
        endif()

        # Find libraries manually
        find_library(SFML_SYSTEM_LIB
            NAMES ${SFML_LIB_NAMES_SYSTEM}
            PATHS ${SFML_LIB_DIR}
            NO_DEFAULT_PATH
        )
        find_library(SFML_WINDOW_LIB
            NAMES ${SFML_LIB_NAMES_WINDOW}
            PATHS ${SFML_LIB_DIR}
            NO_DEFAULT_PATH
        )
        find_library(SFML_GRAPHICS_LIB
            NAMES ${SFML_LIB_NAMES_GRAPHICS}
            PATHS ${SFML_LIB_DIR}
            NO_DEFAULT_PATH
        )

        if(SFML_SYSTEM_LIB AND SFML_WINDOW_LIB AND SFML_GRAPHICS_LIB)
            set(SFML_FOUND TRUE)
            message(STATUS "SFML libraries found:")
            message(STATUS "  System: ${SFML_SYSTEM_LIB}")
            message(STATUS "  Window: ${SFML_WINDOW_LIB}")
            message(STATUS "  Graphics: ${SFML_GRAPHICS_LIB}")
        else()
            message(WARNING "Could not find all SFML libraries")
        endif()
    endif()

if(NOT SFML_FOUND)
    if(MSVC)
        message(FATAL_ERROR "SFML 3.0 not found! Please install SFML or set SFML_ROOT environment variable.")
    else()
        message(FATAL_ERROR "SFML 3.0 not found for MinGW compiler!")
        message(FATAL_ERROR "Please check that SFML 3.0 is installed at C:/SFML-3.0.0")
    endif()
endif()

# Add executable
add_executable(${PROJECT_NAME}
    game.cpp
    src/Game.cpp
    src/GameState.cpp
    src/ECS/ECSManager.cpp
    src/ECS/Systems/InputSystem.cpp
    src/ECS/Systems/MovementSystem.cpp
    src/ECS/Systems/CollisionSystem.cpp
    src/ECS/Systems/RenderSystem.cpp
    src/ECS/Systems/ResizeSystem.cpp
    src/EntityFactory.cpp
)

# Link SFML libraries
if(TARGET SFML::System)
    # Modern CMake targets
    target_link_libraries(${PROJECT_NAME}
        SFML::System
        SFML::Window
        SFML::Graphics
    )
else()
    # Manual linking
    target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_INCLUDE_DIR})

    # Link libraries
    target_link_libraries(${PROJECT_NAME}
        ${SFML_SYSTEM_LIB}
        ${SFML_WINDOW_LIB}
        ${SFML_GRAPHICS_LIB}
        ${SFML_STATIC_DEPS}
    )
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy SFML DLLs to output directory (Windows only)
if(WIN32 AND NOT MSVC)
    # For MinGW, check if we're using static libraries (.a files)
    if(SFML_SYSTEM_LIB AND EXISTS "${SFML_SYSTEM_LIB}")
        get_filename_component(SFML_SYSTEM_LIB_NAME ${SFML_SYSTEM_LIB} NAME)
        # If using static libraries (.a), still copy dependency DLLs if they exist
        if(SFML_SYSTEM_LIB_NAME MATCHES "\\.a$")
            message(STATUS "Using static SFML libraries")
            # Check for dependency DLLs that might still be needed
            set(SFML_BIN_DIR "")
            if(SFML_ROOT)
                set(SFML_BIN_DIR "${SFML_ROOT}/bin")
            elseif(EXISTS "C:/SFML-3.0.0/bin")
                set(SFML_BIN_DIR "C:/SFML-3.0.0/bin")
            endif()

            # For static linking, copy DLLs to bin directory for runtime
            # Even with static linking, some DLLs might be needed at runtime
            if(SFML_BIN_DIR AND EXISTS "${SFML_BIN_DIR}")
                # Copy all DLLs from SFML bin directory
                file(GLOB SFML_DLLS "${SFML_BIN_DIR}/*.dll")
                foreach(DLL ${SFML_DLLS})
                    get_filename_component(DLL_NAME ${DLL} NAME)
                    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${DLL}"
                            $<TARGET_FILE_DIR:${PROJECT_NAME}>
                        COMMENT "Copying ${DLL_NAME}..."
                    )
                endforeach()
            endif()
        else()
            # Find SFML bin directory for dynamic linking
            set(SFML_BIN_DIR "")
            if(SFML_ROOT)
                set(SFML_BIN_DIR "${SFML_ROOT}/bin")
            elseif(EXISTS "C:/SFML-3.0.0/bin")
                set(SFML_BIN_DIR "C:/SFML-3.0.0/bin")
            endif()

            if(SFML_BIN_DIR AND EXISTS "${SFML_BIN_DIR}")
                # Copy main SFML DLLs
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${SFML_BIN_DIR}/sfml-system-3.dll"
                        "${SFML_BIN_DIR}/sfml-window-3.dll"
                        "${SFML_BIN_DIR}/sfml-graphics-3.dll"
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>
                    COMMENT "Copying SFML DLLs..."
                )

                # Copy all DLLs from SFML bin directory (for dependencies)
                file(GLOB SFML_DLLS "${SFML_BIN_DIR}/*.dll")
                foreach(DLL ${SFML_DLLS})
                    get_filename_component(DLL_NAME ${DLL} NAME)
                    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${DLL}"
                            $<TARGET_FILE_DIR:${PROJECT_NAME}>
                        COMMENT "Copying ${DLL_NAME}..."
                    )
                endforeach()
            endif()
        endif()
    endif()
elseif(WIN32 AND MSVC)
    # For MSVC, always copy DLLs (MSVC typically uses dynamic linking)
    set(SFML_BIN_DIR "")
    if(SFML_ROOT)
        set(SFML_BIN_DIR "${SFML_ROOT}/bin")
    elseif(EXISTS "C:/SFML-3.0.0/bin")
        set(SFML_BIN_DIR "C:/SFML-3.0.0/bin")
    endif()

    if(SFML_BIN_DIR AND EXISTS "${SFML_BIN_DIR}")
        # Copy main SFML DLLs
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SFML_BIN_DIR}/sfml-system-3.dll"
                "${SFML_BIN_DIR}/sfml-window-3.dll"
                "${SFML_BIN_DIR}/sfml-graphics-3.dll"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying SFML DLLs..."
        )

        # Copy all DLLs from SFML bin directory (for dependencies)
        file(GLOB SFML_DLLS "${SFML_BIN_DIR}/*.dll")
        foreach(DLL ${SFML_DLLS})
            get_filename_component(DLL_NAME ${DLL} NAME)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DLL}"
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "Copying ${DLL_NAME}..."
            )
        endforeach()
    endif()
endif()
