#include "GameState.h"
#include <fstream>
#include <sstream>

void GameState::loadHighScores(const std::string& filename) {
    highScores.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return;
    int score;
    while (file >> score) {
        highScores.push_back(score);
    }
    std::sort(highScores.rbegin(), highScores.rend());
    if (highScores.size() > 10) highScores.resize(10);
}

void GameState::saveHighScores(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    for (int score : highScores) {
        file << score << "\n";
    }
}
