#include <cmath>
#include <stdexcept>

#include "ECS/Components.h"
#include "ECS/Systems/BallSpeedSystem.h"
#include "EntityFactory.h"
#include "Game.h"
#include "GameState.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/System/Vector2.hpp"

enum class EventType { Closed, Resized, KeyPressed, Unknown };

EventType getEventType(const sf::Event &event) {
  if (event.is<sf::Event::Closed>())
    return EventType::Closed;
  if (event.is<sf::Event::Resized>())
    return EventType::Resized;
  if (event.is<sf::Event::KeyPressed>())
    return EventType::KeyPressed;
  return EventType::Unknown;
}

Game::Game()
    : window(sf::VideoMode({GameState::WINDOW_WIDTH, GameState::WINDOW_HEIGHT}),
             GameState::WINDOW_TITLE) {
  if (!window.isOpen()) {
    throw std::runtime_error("Failed to create window");
  }

  window.setFramerateLimit(GameState::TARGET_FPS);

  // Set fixed view to prevent stretching when window is resized
  sf::View view;
  view.setSize({static_cast<float>(GameState::WINDOW_WIDTH),
                static_cast<float>(GameState::WINDOW_HEIGHT)});
  view.setCenter({static_cast<float>(GameState::WINDOW_WIDTH) / 2.0f,
                  static_cast<float>(GameState::WINDOW_HEIGHT) / 2.0f});
  window.setView(view);

  // Create systems
  inputSystem = std::make_shared<InputSystem>();
  movementSystem = std::make_shared<MovementSystem>();
  collisionSystem = std::make_shared<CollisionSystem>();
  renderSystem = std::make_shared<RenderSystem>();
  resizeSystem = std::make_shared<ResizeSystem>();
  ballSpeedSystem = std::make_shared<BallSpeedSystem>();

  renderSystem->setWindow(&window);
  resizeSystem->setWindow(&window);

  // Register systems
  ecs.addSystem(inputSystem);
  ecs.addSystem(movementSystem);
  ecs.addSystem(collisionSystem);
  ecs.addSystem(resizeSystem);
  ecs.addSystem(renderSystem);
  ecs.addSystem(ballSpeedSystem);

  // Initialize game objects (platform, ball, bricks)
  initializeGameObjects();

  // Load font (using default system font)
  if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
    // Try alternative font path
    if (!font.openFromFile("C:/Windows/Fonts/calibri.ttf")) {
      // If font loading fails, we'll use default rendering
    }
  }
}

void Game::run() {
  while (window.isOpen()) {
    // Calculate delta time
    float deltaTime = clock.restart().asSeconds();

    handleEvents();

    // Check for victory condition
    if (gameMode == GameMode::Playing && countRemainingBricks() == 0) {
      gameMode = GameMode::Victory;
      victoryChoiceYes = true;
    }

    // Check if ball is out of bounds
    if (gameMode == GameMode::Playing && !isRestarting &&
        ball != INVALID_ENTITY &&
        (collisionSystem->isBallOutOfBounds(ball, ecs))) {
      isRestarting = true;
      restartTimer.restart();
    }

    // Handle restart pause
    if (isRestarting) {
      if (restartTimer.getElapsedTime().asSeconds() >=
          GameState::RESTART_PAUSE_TIME_SECONDS) {
        // Reset positions
        auto platformPos = ecs.getComponent<PositionComponent>(platform);
        auto ballPos = ecs.getComponent<PositionComponent>(ball);
        auto ballVelocity = ecs.getComponent<VelocityComponent>(ball);

        if (platformPos) {
          platformPos->position.x = GameState::PLATFORM_START_X;
          platformPos->position.y = GameState::PLATFORM_START_Y;
        }

        if (ballPos && ballVelocity) {
          ballPos->position.x = GameState::BALL_START_X;
          ballPos->position.y = GameState::BALL_START_Y;
          ballVelocity->velocity = {GameState::BALL_INITIAL_VELOCITY_X,
                                    GameState::BALL_INITIAL_VELOCITY_Y};
          // Recalculate speed
          ballVelocity->speed =
              std::sqrt(ballVelocity->velocity.x * ballVelocity->velocity.x +
                        ballVelocity->velocity.y * ballVelocity->velocity.y);

          // Reset ball speed system
          ballSpeedSystem->reset();
        }

        isRestarting = false;
      } else {
        // During pause, skip game updates but still render
        deltaTime = 0.0f;
      }
    }

    // Update game systems (input, movement, collision) - only if playing and
    // not restarting
    if (gameMode == GameMode::Playing && !isRestarting) {
      update(deltaTime);
    }

    render();

    // Optional: Sleep to maintain consistent frame rate if needed
    float elapsed = clock.getElapsedTime().asSeconds();
    if (elapsed < GameState::FRAME_TIME) {
      sf::sleep(sf::seconds(GameState::FRAME_TIME - elapsed));
    }
  }
}

void Game::handleEvents() {
  while (const auto event = window.pollEvent()) {
    switch (getEventType(*event)) {
    case EventType::Closed:
      window.close();
      break;
    case EventType::Resized:
      break;
    case EventType::KeyPressed:
      switch (gameMode) {
      case GameMode::Victory: {
        const auto *keyPressed = event->getIf<sf::Event::KeyPressed>();

        if (keyPressed->code == sf::Keyboard::Key::Left ||
            keyPressed->code == sf::Keyboard::Key::Right) {
          victoryChoiceYes = !victoryChoiceYes;
        } else if (keyPressed->code == sf::Keyboard::Key::Enter ||
                   keyPressed->code == sf::Keyboard::Key::Space) {
          if (victoryChoiceYes) {
            resetGame();
          } else {
            exitToMenu();
          }
        }
        break;
      }
      case GameMode::MainMenu: {
        const auto *menuKeyPressed = event->getIf<sf::Event::KeyPressed>();

        if (menuKeyPressed->code == sf::Keyboard::Key::Enter ||
            menuKeyPressed->code == sf::Keyboard::Key::Space) {
          resetGame();
        }
        break;
      }
      case GameMode::Playing:
        break;
      }
      break;
    case EventType::Unknown:
    default:
      break;
    }
  }
}

void Game::update(float deltaTime) {
  inputSystem->update(deltaTime, ecs);
  movementSystem->update(deltaTime, ecs);
  resizeSystem->update(deltaTime, ecs);
  collisionSystem->update(deltaTime, ecs);
  ballSpeedSystem->update(deltaTime, ecs);
}

void Game::render() {
  window.clear(sf::Color::Black);

  if (gameMode == GameMode::Playing) {
    renderSystem->update(0.0f, ecs);
  } else if (gameMode == GameMode::Victory) {
    renderSystem->update(0.0f, ecs);
    renderVictoryScreen();
  } else if (gameMode == GameMode::MainMenu) {
    renderSystem->update(0.0f, ecs);
    renderMainMenuScreen();
  }

  window.display();
}

int Game::countRemainingBricks() {
  int count = 0;
  for (Entity brick : bricks) {
    auto collider = ecs.getComponent<ColliderComponent>(brick);
    if (collider && collider->type == ColliderComponent::Type::Brick) {
      count++;
    }
  }
  return count;
}

void Game::resetGame() {
  // Clear all bricks
  for (Entity brick : bricks) {
    ecs.destroyEntity(brick);
  }
  bricks.clear();

  // Destroy platform and ball to recreate them
  ecs.destroyEntity(platform);
  ecs.destroyEntity(ball);

  // Reinitialize all game objects
  initializeGameObjects();

  // Reset ball speed system
  ballSpeedSystem->reset();

  gameMode = GameMode::Playing;
  isRestarting = false;
}

void Game::initializeGameObjects() {
  // Create platform
  platform = EntityFactory::createPlatform(
      ecs, GameState::PLATFORM_START_X, GameState::PLATFORM_START_Y,
      GameState::PLATFORM_WIDTH, GameState::PLATFORM_HEIGHT);

  // Create ball
  ball = EntityFactory::createBall(ecs, GameState::BALL_START_X,
                                   GameState::BALL_START_Y,
                                   GameState::BALL_RADIUS);

  // Create bricks
  const float brickWidth = 80.0f;
  const float brickHeight = 30.0f;
  const float brickSpacing = 5.0f;
  const float startX = 50.0f;
  const float startY = 50.0f;
  const int rows = 5;
  const int cols = 8;

  sf::Color brickColors[] = {sf::Color::Red, sf::Color(255, 165, 0), // Orange
                             sf::Color::Yellow, sf::Color::Green,
                             sf::Color::Cyan};

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      float x = startX + col * (brickWidth + brickSpacing);
      float y = startY + row * (brickHeight + brickSpacing);
      Entity brick = EntityFactory::createBrick(
          ecs, x, y, brickWidth, brickHeight, brickColors[row % 5]);
      bricks.push_back(brick);
    }
  }
}

void Game::exitToMenu() {
  // Clear all bricks
  for (Entity brick : bricks) {
    ecs.destroyEntity(brick);
  }
  bricks.clear();

  // Destroy platform and ball to recreate them
  ecs.destroyEntity(platform);
  ecs.destroyEntity(ball);

  // Reinitialize all game objects
  initializeGameObjects();

  // Reset ball speed system
  ballSpeedSystem->reset();

  renderMainMenuScreen();
  gameMode = GameMode::MainMenu;
}

void Game::renderVictoryScreen() {
  sf::RectangleShape overlay;
  overlay.setSize(
      sf::Vector2f(GameState::WINDOW_WIDTH, GameState::WINDOW_HEIGHT));
  overlay.setFillColor(sf::Color(0, 0, 0, 200));
  window.draw(overlay);

  // Render text if font is loaded
  try {
    (void)font.getInfo();

    {
      // Congratulations text
      sf::Text congratsText(font);
      congratsText.setString(L"Поздравляем! Вы выиграли!");
      congratsText.setCharacterSize(48);
      congratsText.setFillColor(sf::Color::Yellow);
      congratsText.setStyle(sf::Text::Bold);
      sf::FloatRect congratsBounds = congratsText.getLocalBounds();
      congratsText.setPosition(
          {(GameState::WINDOW_WIDTH - congratsBounds.position.x -
            congratsBounds.size.x) /
               2.0f,
           150.0f});
      window.draw(congratsText);
    }
    {
      // Question text
      sf::Text questionText(font);
      questionText.setString(L"Хотите сыграть еще раз?");
      questionText.setCharacterSize(32);
      questionText.setFillColor(sf::Color::White);
      sf::FloatRect questionBounds = questionText.getLocalBounds();
      questionText.setPosition(
          {(GameState::WINDOW_WIDTH - questionBounds.position.x -
            questionBounds.size.x) /
               2.0f,
           250.0f});
      window.draw(questionText);

      sf::Text yesText(font);
      yesText.setString(L"Да");
      yesText.setCharacterSize(36);
      yesText.setFillColor(victoryChoiceYes ? sf::Color::Green
                                            : sf::Color::White);
      yesText.setStyle(victoryChoiceYes ? sf::Text::Bold : sf::Text::Regular);
      sf::FloatRect yesBounds = yesText.getLocalBounds();
      yesText.setPosition(
          {(GameState::WINDOW_WIDTH - yesBounds.position.x - yesBounds.size.x) /
                   2.0f -
               80.0f,
           350.0f});
      window.draw(yesText);

      sf::Text noText(font);
      noText.setString(L"Нет");
      noText.setCharacterSize(36);
      noText.setFillColor(!victoryChoiceYes ? sf::Color::Red
                                            : sf::Color::White);
      noText.setStyle(!victoryChoiceYes ? sf::Text::Bold : sf::Text::Regular);
      sf::FloatRect noBounds = noText.getLocalBounds();
      noText.setPosition(
          {(GameState::WINDOW_WIDTH - noBounds.position.x - noBounds.size.x) /
                   2.0f +
               80.0f,
           350.0f});
      window.draw(noText);

      sf::Text instructionsText(font);
      instructionsText.setString(
          L"Стрелки влево/вправо - выбор, Enter - подтвердить");
      instructionsText.setCharacterSize(20);
      instructionsText.setFillColor(sf::Color(200, 200, 200));
      sf::FloatRect instBounds = instructionsText.getLocalBounds();
      instructionsText.setPosition(
          {(GameState::WINDOW_WIDTH - instBounds.position.x -
            instBounds.size.x) /
               2.0f,
           450.0f});
      window.draw(instructionsText);
    }
  } catch (...) {
    // Font not loaded, skip text rendering
  }
}

void Game::renderMainMenuScreen() {
  sf::RectangleShape overlay;

  overlay.setSize(
      sf::Vector2f(GameState::WINDOW_WIDTH, GameState::WINDOW_HEIGHT));
  overlay.setFillColor(sf::Color(0, 0, 0, 200));
  window.draw(overlay);

  sf::Text gameTitleText(font);
  gameTitleText.setString(L"Игра \"Арканоид\"");
  gameTitleText.setCharacterSize(64);
  gameTitleText.setFillColor(sf::Color(255, 255, 255));
  sf::FloatRect titleBounds = gameTitleText.getLocalBounds();
  gameTitleText.setPosition(
      {(GameState::WINDOW_WIDTH - titleBounds.position.x - titleBounds.size.x) /
           2.0f,
       200.0f});
  window.draw(gameTitleText);

  sf::Text instructionsText(font);
  instructionsText.setString(L"Нажмите Enter для начала игры");
  instructionsText.setCharacterSize(20);
  instructionsText.setFillColor(sf::Color(200, 200, 200));
  sf::FloatRect instBounds = instructionsText.getLocalBounds();
  instructionsText.setPosition(
      {(GameState::WINDOW_WIDTH - instBounds.position.x - instBounds.size.x) /
           2.0f,
       450.0f});
  window.draw(instructionsText);
}
