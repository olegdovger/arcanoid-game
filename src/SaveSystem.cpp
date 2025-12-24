#include "SaveSystem.h"
#include "Game.h"
#include "EntityFactory.h"
#include "ECS/ECSManager.h"
#include "ECS/Components.h"
#include "GameState.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <chrono>

bool SaveSystem::saveGame(const Game& game, const std::string& filename) {
    GameSaveData data = createSaveData(game);
    return saveToFile(data, filename);
}

bool SaveSystem::loadGame(Game& game, const std::string& filename) {
    GameSaveData data;
    if (!loadFromFile(data, filename)) {
        return false;
    }
    return applySaveData(game, data);
}

bool SaveSystem::saveExists(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    return file.good();
}

bool SaveSystem::saveToFile(const GameSaveData& data, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open save file: " << filename << std::endl;
        return false;
    }
    
    
    const char header[] = "ARCSAVE";
    file.write(header, 8);
    
    
    file.write(reinterpret_cast<const char*>(&data.platformX), sizeof(float));
    file.write(reinterpret_cast<const char*>(&data.platformY), sizeof(float));
    
    
    file.write(reinterpret_cast<const char*>(&data.ballX), sizeof(float));
    file.write(reinterpret_cast<const char*>(&data.ballY), sizeof(float));
    file.write(reinterpret_cast<const char*>(&data.ballVelX), sizeof(float));
    file.write(reinterpret_cast<const char*>(&data.ballVelY), sizeof(float));
    
    
    file.write(reinterpret_cast<const char*>(&data.currentScore), sizeof(int));
    
    
    uint32_t brickCount = static_cast<uint32_t>(data.bricks.size());
    file.write(reinterpret_cast<const char*>(&brickCount), sizeof(uint32_t));
    
    
    for (const auto& brick : data.bricks) {
        file.write(reinterpret_cast<const char*>(&brick.x), sizeof(float));
        file.write(reinterpret_cast<const char*>(&brick.y), sizeof(float));
        file.write(reinterpret_cast<const char*>(&brick.width), sizeof(float));
        file.write(reinterpret_cast<const char*>(&brick.height), sizeof(float));
        file.write(reinterpret_cast<const char*>(&brick.r), sizeof(std::uint8_t));
        file.write(reinterpret_cast<const char*>(&brick.g), sizeof(std::uint8_t));
        file.write(reinterpret_cast<const char*>(&brick.b), sizeof(std::uint8_t));
        file.write(reinterpret_cast<const char*>(&brick.a), sizeof(std::uint8_t));
        file.write(reinterpret_cast<const char*>(&brick.hitPoints), sizeof(int));
        file.write(reinterpret_cast<const char*>(&brick.maxHits), sizeof(int));
        file.write(reinterpret_cast<const char*>(&brick.hasBonus), sizeof(bool));
        file.write(reinterpret_cast<const char*>(&brick.bonusType), sizeof(int));
    }
    
    
    file.write(reinterpret_cast<const char*>(&data.saveTime), sizeof(std::uint64_t));
    
    return file.good();
}

bool SaveSystem::loadFromFile(GameSaveData& data, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open save file: " << filename << std::endl;
        return false;
    }
    
    
    char header[9] = {0};
    file.read(header, 8);
    if (std::strcmp(header, "ARCSAVE") != 0) {
        std::cerr << "Invalid save file format" << std::endl;
        return false;
    }
    
    
    file.read(reinterpret_cast<char*>(&data.platformX), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.platformY), sizeof(float));
    
    
    file.read(reinterpret_cast<char*>(&data.ballX), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.ballY), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.ballVelX), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.ballVelY), sizeof(float));
    
    
    file.read(reinterpret_cast<char*>(&data.currentScore), sizeof(int));
    
    
    uint32_t brickCount = 0;
    file.read(reinterpret_cast<char*>(&brickCount), sizeof(uint32_t));
    
    
    data.bricks.resize(brickCount);
    for (uint32_t i = 0; i < brickCount; ++i) {
        auto& brick = data.bricks[i];
        file.read(reinterpret_cast<char*>(&brick.x), sizeof(float));
        file.read(reinterpret_cast<char*>(&brick.y), sizeof(float));
        file.read(reinterpret_cast<char*>(&brick.width), sizeof(float));
        file.read(reinterpret_cast<char*>(&brick.height), sizeof(float));
        file.read(reinterpret_cast<char*>(&brick.r), sizeof(std::uint8_t));
        file.read(reinterpret_cast<char*>(&brick.g), sizeof(std::uint8_t));
        file.read(reinterpret_cast<char*>(&brick.b), sizeof(std::uint8_t));
        file.read(reinterpret_cast<char*>(&brick.a), sizeof(std::uint8_t));
        file.read(reinterpret_cast<char*>(&brick.hitPoints), sizeof(int));
        file.read(reinterpret_cast<char*>(&brick.maxHits), sizeof(int));
        file.read(reinterpret_cast<char*>(&brick.hasBonus), sizeof(bool));
        file.read(reinterpret_cast<char*>(&brick.bonusType), sizeof(int));
    }
    
    
    file.read(reinterpret_cast<char*>(&data.saveTime), sizeof(std::uint64_t));
    
    return file.good();
}

GameSaveData SaveSystem::createSaveData(const Game& game) {
    GameSaveData data;
    const auto& ecs = game.getECS();
    Entity platform = game.getPlatform();
    Entity ball = game.getBall();
    const auto& bricks = game.getBricks();
    
    
    if (auto pos = ecs.getComponent<PositionComponent>(platform)) {
        data.platformX = pos->position.x;
        data.platformY = pos->position.y;
    }
    
    
    if (auto pos = ecs.getComponent<PositionComponent>(ball)) {
        data.ballX = pos->position.x;
        data.ballY = pos->position.y;
    }
    if (auto vel = ecs.getComponent<VelocityComponent>(ball)) {
        data.ballVelX = vel->velocity.x;
        data.ballVelY = vel->velocity.y;
    }
    
    
    data.currentScore = GAME_STATE.getCurrentScore();
    
    
    for (Entity brickEntity : bricks) {
        auto pos = ecs.getComponent<PositionComponent>(brickEntity);
        auto shape = ecs.getComponent<ShapeComponent>(brickEntity);
        auto collider = ecs.getComponent<ColliderComponent>(brickEntity);
        auto durable = ecs.getComponent<DurableBrick>(brickEntity);
        
        if (pos && shape && collider) {
            BrickSaveData brickData;
            brickData.x = pos->position.x;
            brickData.y = pos->position.y;
            brickData.width = shape->rectangle.width;
            brickData.height = shape->rectangle.height;
            brickData.r = shape->color.r;
            brickData.g = shape->color.g;
            brickData.b = shape->color.b;
            brickData.a = shape->color.a;
            
            if (durable) {
                brickData.hitPoints = durable->currentHits;
                brickData.maxHits = durable->maxHits;
            } else {
                brickData.hitPoints = 0;
                brickData.maxHits = 1;
            }
            
            
            auto bonus = ecs.getComponent<BonusComponent>(brickEntity);
            brickData.hasBonus = (bonus != nullptr);
            if (bonus) {
                brickData.bonusType = static_cast<int>(bonus->type);
            } else {
                brickData.bonusType = 0;
            }
            
            data.bricks.push_back(brickData);
        }
    }
    
    
    data.saveTime = std::chrono::system_clock::now().time_since_epoch().count();
    
    return data;
}

bool SaveSystem::applySaveData(Game& game, const GameSaveData& data) {
    auto& ecs = game.getECS();
    Entity platform = game.getPlatform();
    Entity ball = game.getBall();
    auto& bricks = game.getBricks();
    
    
    for (Entity brick : bricks) {
        ecs.destroyEntity(brick);
    }
    bricks.clear();
    
    
    if (auto pos = ecs.getComponent<PositionComponent>(platform)) {
        pos->position.x = data.platformX;
        pos->position.y = data.platformY;
    }
    
    
    if (auto pos = ecs.getComponent<PositionComponent>(ball)) {
        pos->position.x = data.ballX;
        pos->position.y = data.ballY;
    }
    if (auto vel = ecs.getComponent<VelocityComponent>(ball)) {
        vel->velocity.x = data.ballVelX;
        vel->velocity.y = data.ballVelY;
        
        vel->speed = std::sqrt(vel->velocity.x * vel->velocity.x + vel->velocity.y * vel->velocity.y);
    }
    
    
    GAME_STATE.resetCurrentScore();
    GAME_STATE.addScore(data.currentScore);
    
    
    for (const auto& brickData : data.bricks) {
        sf::Color color(brickData.r, brickData.g, brickData.b, brickData.a);
        
        Entity brick = EntityFactory::createBrick(ecs, brickData.x, brickData.y, brickData.width, brickData.height, color, brickData.maxHits);
        
        if (auto durable = ecs.getComponent<DurableBrick>(brick)) {
            durable->currentHits = brickData.hitPoints;
            
        }
        
        if (brickData.hasBonus) {
            BonusType type = static_cast<BonusType>(brickData.bonusType);
            ecs.addComponent<BonusComponent>(brick, std::make_shared<BonusComponent>(type));
        }
        bricks.push_back(brick);
    }
    
    return true;
}