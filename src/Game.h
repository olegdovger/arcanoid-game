#pragma once

#include <SFML/Graphics.hpp>
#include "ECS/ECSManager.h"
#include "ECS/Systems/InputSystem.h"
#include "ECS/Systems/MovementSystem.h"
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/ResizeSystem.h"
#include "ECS/Components.h"
#include "EntityFactory.h"
#include "GameState.h"
#include <memory>

class Game
{
public:
    Game();
    ~Game() = default;

    void run();
    bool isWindowOpen() const { return window.isOpen(); }

private:
    void handleEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow window;
    ECSManager ecs;

    std::shared_ptr<InputSystem> inputSystem;
    std::shared_ptr<MovementSystem> movementSystem;
    std::shared_ptr<CollisionSystem> collisionSystem;
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<ResizeSystem> resizeSystem;

    Entity platform;
    Entity ball;

    sf::Clock clock;
    sf::Clock restartTimer;
    bool isRestarting = false;
};

