#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <SFML/System/Vector2.hpp>
#include "ECS/Entity.h"


class Game;
class ECSManager;

struct BrickSaveData {
    float x, y;
    float width, height;
    std::uint8_t r, g, b, a;
    int hitPoints;
    int maxHits;
    bool hasBonus;
    int bonusType; 
};

struct GameSaveData {
    
    float platformX, platformY;
    
    
    float ballX, ballY;
    float ballVelX, ballVelY;
    
    
    std::vector<BrickSaveData> bricks;
    
    
    int currentScore;
    
    
    std::uint64_t saveTime;
};

class SaveSystem {
public:
    
    static bool saveGame(const Game& game, const std::string& filename);
    
    
    static bool loadGame(Game& game, const std::string& filename);
    
    
    static bool saveExists(const std::string& filename);
    
private:
    
    static bool saveToFile(const GameSaveData& data, const std::string& filename);
    static bool loadFromFile(GameSaveData& data, const std::string& filename);
    
    
    static GameSaveData createSaveData(const Game& game);
    static bool applySaveData(Game& game, const GameSaveData& data);
};