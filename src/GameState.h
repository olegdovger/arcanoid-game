#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <string>

class GameState
{
private:
    GameState() = default;

public:
    static GameState& Instance() {
      static GameState instance;
      return instance;
    }


    const unsigned int WINDOW_WIDTH = 800;
    const unsigned int WINDOW_HEIGHT = 600;
    const char* WINDOW_TITLE = "Arcanoid Game";


    const unsigned int TARGET_FPS = 60;
    const float FRAME_TIME = 1.0f / TARGET_FPS;


    const float RESTART_PAUSE_TIME_SECONDS = 1.0f;

    const float PLATFORM_START_X = 350.0f;
    const float PLATFORM_START_Y = 550.0f;
    const float BALL_START_X = 400.0f;
    const float BALL_START_Y = 400.0f;


    const float PLATFORM_WIDTH = 100.0f;
    const float PLATFORM_HEIGHT = 20.0f;


    const float BALL_RADIUS = 10.0f;
    const float BALL_INITIAL_VELOCITY_X = 200.0f;
    const float BALL_INITIAL_VELOCITY_Y = -200.0f;


    const float BALL_SPEED_INCREASE_INTERVAL = 10.0f;
    const float BALL_SPEED_MULTIPLIER = 1.2f;
    const float BALL_MAX_SPEED_MULTIPLIER = 3.0f;


    const float BONUS_CHANCE = 0.8f;
    const int MAX_VISIBLE_BONUSES = 20;
    const float SLOW_BALL_DURATION = 5.0f;
    const float FAST_PLATFORM_DURATION = 10.0f;
    const float BIG_PLATFORM_DURATION = 5.0f;

    int currentScore = 0;
    std::vector<int> highScores;

    void resetCurrentScore() { currentScore = 0; }
    void addScore(int points) { currentScore += points; }
    int getCurrentScore() const { return currentScore; }


    void addHighScore(int score) {
        highScores.push_back(score);
        std::sort(highScores.rbegin(), highScores.rend());
        if (highScores.size() > 10) highScores.resize(10);
    }
    const std::vector<int>& getHighScores() const { return highScores; }
    void submitCurrentScore() {
        addHighScore(currentScore);
        resetCurrentScore();
    }
    void loadHighScores(const std::string& filename);
    void saveHighScores(const std::string& filename);
};

#define GAME_STATE GameState::Instance()
