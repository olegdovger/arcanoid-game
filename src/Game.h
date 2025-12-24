#pragma once

#include <SFML/Graphics.hpp>
#include "ECS/ECSManager.h"
#include "ECS/Systems/InputSystem.h"
#include "ECS/Systems/MovementSystem.h"
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/ResizeSystem.h"
#include "ECS/Systems/BallSpeedSystem.h"
#include <memory>

enum class GameMode
{
    Playing,
    Victory,
    MainMenu,
};

class Game
{
public:
    Game();
    ~Game();

    void run();
    
    
    Entity getPlatform() const { return platform; }
    Entity getBall() const { return ball; }
    const std::vector<Entity>& getBricks() const { return bricks; }
    std::vector<Entity>& getBricks() { return bricks; }
    ECSManager& getECS() { return ecs; }
    const ECSManager& getECS() const { return ecs; }

private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    void renderVictoryScreen();
    void renderMainMenuScreen();
    void renderScore();
    int countRemainingBricks();
    void resetGame();
    void exitToMenu();
    void initializeGameObjects();
    void recreateBricks();
    void updateBonuses(float deltaTime);
    void renderBonusTimers();

    sf::RenderWindow window;
    ECSManager ecs;

    std::shared_ptr<InputSystem> inputSystem;
    std::shared_ptr<MovementSystem> movementSystem;
    std::shared_ptr<CollisionSystem> collisionSystem;
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<ResizeSystem> resizeSystem;
    std::shared_ptr<BallSpeedSystem> ballSpeedSystem;

    Entity platform;
    Entity ball;
    std::vector<Entity> bricks;

    sf::Clock clock;
    sf::Clock restartTimer;
    bool isRestarting = false;
    GameMode gameMode = GameMode::MainMenu;
    bool victoryChoiceYes = true; 
    sf::Font font;
};
