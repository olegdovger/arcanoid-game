#pragma once

#include <SFML/Graphics.hpp>

struct GameState
{
    // Window settings
    static constexpr unsigned int WINDOW_WIDTH = 800;
    static constexpr unsigned int WINDOW_HEIGHT = 600;
    static constexpr const char* WINDOW_TITLE = "Arcanoid Game";

    // FPS settings
    static constexpr unsigned int TARGET_FPS = 60;
    static constexpr float FRAME_TIME = 1.0f / TARGET_FPS;

    // Game constants
    static constexpr float RESTART_PAUSE_TIME_SECONDS = 1.0f;
    // Initial positions
    static constexpr float PLATFORM_START_X = 350.0f;
    static constexpr float PLATFORM_START_Y = 550.0f;
    static constexpr float BALL_START_X = 400.0f;
    static constexpr float BALL_START_Y = 400.0f;

    // Platform settings
    static constexpr float PLATFORM_WIDTH = 100.0f;
    static constexpr float PLATFORM_HEIGHT = 20.0f;

    // Ball settings
    static constexpr float BALL_RADIUS = 10.0f;
    static constexpr float BALL_INITIAL_VELOCITY_X = 200.0f;
    static constexpr float BALL_INITIAL_VELOCITY_Y = -200.0f;
};
