#pragma once

#include <SFML/Graphics.hpp>

class GameState
{
private:
    GameState() = default;

public:
    static GameState& Instance() {
      static GameState instance;
      return instance;
    }

    // Window settings
    const unsigned int WINDOW_WIDTH = 800;
    const unsigned int WINDOW_HEIGHT = 600;
    const char* WINDOW_TITLE = "Arcanoid Game";

    // FPS settings
    const unsigned int TARGET_FPS = 60;
    const float FRAME_TIME = 1.0f / TARGET_FPS;

    // Game constants
    const float RESTART_PAUSE_TIME_SECONDS = 1.0f;
    // Initial positions
    const float PLATFORM_START_X = 350.0f;
    const float PLATFORM_START_Y = 550.0f;
    const float BALL_START_X = 400.0f;
    const float BALL_START_Y = 400.0f;

    // Platform settings
    const float PLATFORM_WIDTH = 100.0f;
    const float PLATFORM_HEIGHT = 20.0f;

    // Ball settings
    const float BALL_RADIUS = 10.0f;
    const float BALL_INITIAL_VELOCITY_X = 200.0f;
    const float BALL_INITIAL_VELOCITY_Y = -200.0f;

    // Ball speed increase settings
    const float BALL_SPEED_INCREASE_INTERVAL = 10.0f; // Increase speed every 10 seconds
    const float BALL_SPEED_MULTIPLIER = 1.2f; // Multiply speed by this factor
    const float BALL_MAX_SPEED_MULTIPLIER = 3.0f; // Maximum speed multiplier
};

#define GAME_STATE GameState::Instance()
