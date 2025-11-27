#include "Game.h"
#include <cmath>
#include <stdexcept>

Game::Game()
    : window(sf::VideoMode({GameState::WINDOW_WIDTH, GameState::WINDOW_HEIGHT}), GameState::WINDOW_TITLE)
{
    if (!window.isOpen())
    {
        throw std::runtime_error("Failed to create window");
    }
    
    window.setFramerateLimit(GameState::TARGET_FPS);
    
    // Set fixed view to prevent stretching when window is resized
    sf::View view;
    view.setSize({static_cast<float>(GameState::WINDOW_WIDTH), static_cast<float>(GameState::WINDOW_HEIGHT)});
    view.setCenter({static_cast<float>(GameState::WINDOW_WIDTH) / 2.0f, static_cast<float>(GameState::WINDOW_HEIGHT) / 2.0f});
    window.setView(view);

    // Create systems
    inputSystem = std::make_shared<InputSystem>();
    movementSystem = std::make_shared<MovementSystem>();
    collisionSystem = std::make_shared<CollisionSystem>();
    renderSystem = std::make_shared<RenderSystem>();
    resizeSystem = std::make_shared<ResizeSystem>();

    renderSystem->setWindow(&window);
    resizeSystem->setWindow(&window);

    // Register systems
    ecs.addSystem(inputSystem);
    ecs.addSystem(movementSystem);
    ecs.addSystem(collisionSystem);
    ecs.addSystem(resizeSystem);
    ecs.addSystem(renderSystem);

    // Create entities
    platform = EntityFactory::createPlatform(ecs, GameState::PLATFORM_START_X, GameState::PLATFORM_START_Y,
                                              GameState::PLATFORM_WIDTH, GameState::PLATFORM_HEIGHT);
    ball = EntityFactory::createBall(ecs, GameState::BALL_START_X, GameState::BALL_START_Y,
                                     GameState::BALL_RADIUS);
}

void Game::run()
{
    while (window.isOpen())
    {
        // Calculate delta time
        float deltaTime = clock.restart().asSeconds();

        handleEvents();

        // Check if ball is out of bounds
        if (!isRestarting && collisionSystem->isBallOutOfBounds(ball, ecs))
        {
            isRestarting = true;
            restartTimer.restart();
        }

        // Handle restart pause
        if (isRestarting)
        {
            if (restartTimer.getElapsedTime().asSeconds() >= GameState::RESTART_PAUSE_TIME_SECONDS)
            {
                // Reset positions
                auto platformPos = ecs.getComponent<PositionComponent>(platform);
                auto ballPos = ecs.getComponent<PositionComponent>(ball);
                auto ballVelocity = ecs.getComponent<VelocityComponent>(ball);

                if (platformPos)
                {
                    platformPos->position.x = GameState::PLATFORM_START_X;
                    platformPos->position.y = GameState::PLATFORM_START_Y;
                }

                if (ballPos && ballVelocity)
                {
                    ballPos->position.x = GameState::BALL_START_X;
                    ballPos->position.y = GameState::BALL_START_Y;
                    ballVelocity->velocity = {GameState::BALL_INITIAL_VELOCITY_X, GameState::BALL_INITIAL_VELOCITY_Y};
                    // Recalculate speed
                    ballVelocity->speed = std::sqrt(ballVelocity->velocity.x * ballVelocity->velocity.x +
                                                   ballVelocity->velocity.y * ballVelocity->velocity.y);
                }

                isRestarting = false;
            }
            else
            {
                // During pause, skip game updates but still render
                deltaTime = 0.0f;
            }
        }

        // Update game systems (input, movement, collision) - only if not restarting
        if (!isRestarting)
        {
            update(deltaTime);
        }

        render();

        // Optional: Sleep to maintain consistent frame rate if needed
        float elapsed = clock.getElapsedTime().asSeconds();
        if (elapsed < GameState::FRAME_TIME)
        {
            sf::sleep(sf::seconds(GameState::FRAME_TIME - elapsed));
        }
    }
}

void Game::handleEvents()
{
    // Handle events - SFML 3.0 API
    while (const auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        // Handle window resize events
        else if (const auto* resized = event->getIf<sf::Event::Resized>())
        {
            // View will be updated by ResizeSystem
        }
    }
}

void Game::update(float deltaTime)
{
    inputSystem->update(deltaTime, ecs);
    movementSystem->update(deltaTime, ecs);
    resizeSystem->update(deltaTime, ecs);
    collisionSystem->update(deltaTime, ecs);
}

void Game::render()
{
    window.clear(sf::Color::Black);
    // Render all entities
    renderSystem->update(0.0f, ecs);
    window.display();
}

