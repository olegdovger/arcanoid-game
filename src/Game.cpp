#include <cmath>
#include <random>
#include <stdexcept>
#include <iostream>

#include "GameState.h"
#include "ECS/Components.h"
#include "ECS/Systems/BallSpeedSystem.h"
#include "EntityFactory.h"
#include "Game.h"
#include "SaveSystem.h"
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
    : window(sf::VideoMode({GAME_STATE.WINDOW_WIDTH, GAME_STATE.WINDOW_HEIGHT}),
             GAME_STATE.WINDOW_TITLE) {
  if (!window.isOpen()) {
    throw std::runtime_error("Failed to create window");
  }

  window.setFramerateLimit(GAME_STATE.TARGET_FPS);

  
  sf::View view;
  view.setSize({static_cast<float>(GAME_STATE.WINDOW_WIDTH),
                static_cast<float>(GAME_STATE.WINDOW_HEIGHT)});
  view.setCenter({static_cast<float>(GAME_STATE.WINDOW_WIDTH) / 2.0f,
                  static_cast<float>(GAME_STATE.WINDOW_HEIGHT) / 2.0f});
  window.setView(view);

  
  inputSystem = std::make_shared<InputSystem>();
  movementSystem = std::make_shared<MovementSystem>();
  collisionSystem = std::make_shared<CollisionSystem>();
  renderSystem = std::make_shared<RenderSystem>();
  resizeSystem = std::make_shared<ResizeSystem>();
  ballSpeedSystem = std::make_shared<BallSpeedSystem>();

  renderSystem->setWindow(&window);
  resizeSystem->setWindow(&window);

  
  ecs.addSystem(inputSystem);
  ecs.addSystem(movementSystem);
  ecs.addSystem(collisionSystem);
  ecs.addSystem(resizeSystem);
  ecs.addSystem(renderSystem);
  ecs.addSystem(ballSpeedSystem);

  
  initializeGameObjects();

   
   if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
     
     if (!font.openFromFile("C:/Windows/Fonts/calibri.ttf")) {
       
     }
   }

   
   GAME_STATE.loadHighScores("highscores.txt");
}

void Game::run() {
  while (window.isOpen()) {
    
    float deltaTime = clock.restart().asSeconds();

    handleEvents();

    
    if (gameMode == GameMode::Playing && countRemainingBricks() == 0) {
      GAME_STATE.submitCurrentScore();
      gameMode = GameMode::Victory;
      victoryChoiceYes = true;
    }

    
    if (gameMode == GameMode::Playing && !isRestarting &&
        ball != INVALID_ENTITY &&
        (collisionSystem->isBallOutOfBounds(ball, ecs))) {
      isRestarting = true;
      restartTimer.restart();
    }

    
    if (isRestarting) {
      if (restartTimer.getElapsedTime().asSeconds() >=
          GAME_STATE.RESTART_PAUSE_TIME_SECONDS) {
        
        auto platformPos = ecs.getComponent<PositionComponent>(platform);
        auto ballPos = ecs.getComponent<PositionComponent>(ball);
        auto ballVelocity = ecs.getComponent<VelocityComponent>(ball);

        if (platformPos) {
          platformPos->position.x = GAME_STATE.PLATFORM_START_X;
          platformPos->position.y = GAME_STATE.PLATFORM_START_Y;
        }

        if (ballPos && ballVelocity) {
          ballPos->position.x = GAME_STATE.BALL_START_X;
          ballPos->position.y = GAME_STATE.BALL_START_Y;
          ballVelocity->velocity = {GAME_STATE.BALL_INITIAL_VELOCITY_X,
                                    GAME_STATE.BALL_INITIAL_VELOCITY_Y};
          
          ballVelocity->speed =
              std::sqrt(ballVelocity->velocity.x * ballVelocity->velocity.x +
                        ballVelocity->velocity.y * ballVelocity->velocity.y);

          
          ballSpeedSystem->reset();
        }

        recreateBricks();

        isRestarting = false;
      } else {

        deltaTime = 0.0f;
      }
    }

    
    
    if (gameMode == GameMode::Playing && !isRestarting) {
      update(deltaTime);
    }

    render();

    
    float elapsed = clock.getElapsedTime().asSeconds();
    if (elapsed < GAME_STATE.FRAME_TIME) {
      sf::sleep(sf::seconds(GAME_STATE.FRAME_TIME - elapsed));
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
      case GameMode::Playing: {
        const auto *keyPressed = event->getIf<sf::Event::KeyPressed>();
        if (keyPressed) {
          if (keyPressed->code == sf::Keyboard::Key::F5) {
            
            if (SaveSystem::saveGame(*this, "savegame.dat")) {
              
              std::cout << "Game saved!" << std::endl;
            } else {
              std::cerr << "Failed to save game!" << std::endl;
            }
          } else if (keyPressed->code == sf::Keyboard::Key::F9) {
            
            if (SaveSystem::saveExists("savegame.dat")) {
              if (SaveSystem::loadGame(*this, "savegame.dat")) {
                std::cout << "Game loaded!" << std::endl;
              } else {
                std::cerr << "Failed to load game!" << std::endl;
              }
            } else {
              std::cerr << "No save file found!" << std::endl;
            }
          }
        }
        break;
      }
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
  updateBonuses(deltaTime);
}

void Game::render() {
  window.clear(sf::Color::Black);

  if (gameMode == GameMode::Playing) {
    renderSystem->update(0.0f, ecs);
    
    if (font.getInfo().family != "") {
        sf::Text scoreText(font);
        scoreText.setString("Score: " + std::to_string(GAME_STATE.getCurrentScore()));
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition({GAME_STATE.WINDOW_WIDTH - 150.0f, 10.0f});
        window.draw(scoreText);
    }
    renderBonusTimers();
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
  
  for (Entity brick : bricks) {
    ecs.destroyEntity(brick);
  }
  bricks.clear();

  
  ecs.destroyEntity(platform);
  ecs.destroyEntity(ball);

  
  initializeGameObjects();

  
  ballSpeedSystem->reset();

  
  GAME_STATE.resetCurrentScore();

  gameMode = GameMode::Playing;
  isRestarting = false;
}

void Game::initializeGameObjects() {
  
  platform = EntityFactory::createPlatform(
      ecs, GAME_STATE.PLATFORM_START_X, GAME_STATE.PLATFORM_START_Y,
      GAME_STATE.PLATFORM_WIDTH, GAME_STATE.PLATFORM_HEIGHT);

  
  ball = EntityFactory::createBall(ecs, GAME_STATE.BALL_START_X,
                                   GAME_STATE.BALL_START_Y,
                                   GAME_STATE.BALL_RADIUS);

  recreateBricks();
}

void Game::recreateBricks() {
  for (Entity brick : bricks) {
    ecs.destroyEntity(brick);
  }
  bricks.clear();

  const float brickWidth = 80.0f;
  const float brickHeight = 30.0f;
  const float brickSpacing = 5.0f;
  const float startX = 50.0f;
  const float startY = 50.0f;
  const int rows = 5;
  const int cols = 8;

  sf::Color brickColors[] = {sf::Color::Red, sf::Color(255, 165, 0), 
                             sf::Color::Yellow, sf::Color::Green,
                             sf::Color::Cyan};

  int bonusesAdded = 0;
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      float x = startX + col * (brickWidth + brickSpacing);
      float y = startY + row * (brickHeight + brickSpacing);

      static std::random_device rd;
      static std::mt19937 gen(rd());
      static std::uniform_int_distribution<int> hitPointsDis(1, 3);
      static std::uniform_real_distribution<float> bonusChanceDis(0.0f, 1.0f);
      static std::uniform_int_distribution<int> bonusTypeDis(0, 2);
      
      int hitPoints = hitPointsDis(gen);
      bool hasBonus = false;
      BonusType bonusType = BonusType::SlowBall;
      
      if (bonusesAdded < GAME_STATE.MAX_VISIBLE_BONUSES && bonusChanceDis(gen) < GAME_STATE.BONUS_CHANCE) {
        hasBonus = true;
        bonusType = static_cast<BonusType>(bonusTypeDis(gen));
        bonusesAdded++;
      }
      
      Entity brick = EntityFactory::createBrick(
          ecs, x, y, brickWidth, brickHeight, brickColors[row % 5], hitPoints, hasBonus, bonusType);
      bricks.push_back(brick);
    }
  }
}

void Game::exitToMenu() {
  
  for (Entity brick : bricks) {
    ecs.destroyEntity(brick);
  }
  bricks.clear();

  
  ecs.destroyEntity(platform);
  ecs.destroyEntity(ball);

  
  initializeGameObjects();

  
  ballSpeedSystem->reset();

  renderMainMenuScreen();
  gameMode = GameMode::MainMenu;
}

void Game::renderVictoryScreen() {
  sf::RectangleShape overlay;
  overlay.setSize(
      sf::Vector2f(GAME_STATE.WINDOW_WIDTH, GAME_STATE.WINDOW_HEIGHT));
  overlay.setFillColor(sf::Color(0, 0, 0, 200));
  window.draw(overlay);

  
  try {
    (void)font.getInfo();

    {
      
      sf::Text congratsText(font);
      congratsText.setString(L"Поздравляем! Вы выиграли!");
      congratsText.setCharacterSize(48);
      congratsText.setFillColor(sf::Color::Yellow);
      congratsText.setStyle(sf::Text::Bold);
      sf::FloatRect congratsBounds = congratsText.getLocalBounds();
      congratsText.setPosition(
          {(GAME_STATE.WINDOW_WIDTH - congratsBounds.position.x -
            congratsBounds.size.x) /
               2.0f,
           150.0f});
      window.draw(congratsText);
    }
    
    {
        sf::Text highScoreTitle(font);
        highScoreTitle.setString(L"High Scores:");
        highScoreTitle.setCharacterSize(24);
        highScoreTitle.setFillColor(sf::Color::Cyan);
        sf::FloatRect titleBounds = highScoreTitle.getLocalBounds();
        highScoreTitle.setPosition(
            {(GAME_STATE.WINDOW_WIDTH - titleBounds.position.x - titleBounds.size.x) / 2.0f,
             200.0f});
        window.draw(highScoreTitle);

        const auto& scores = GAME_STATE.getHighScores();
        float y = 230.0f;
        for (size_t i = 0; i < scores.size() && i < 5; ++i) {
            sf::Text scoreText(font);
            scoreText.setString(std::to_string(i + 1) + ". " + std::to_string(scores[i]));
            scoreText.setCharacterSize(20);
            scoreText.setFillColor(sf::Color::White);
            sf::FloatRect scoreBounds = scoreText.getLocalBounds();
            scoreText.setPosition(
                {(GAME_STATE.WINDOW_WIDTH - scoreBounds.position.x - scoreBounds.size.x) / 2.0f,
                 y});
            window.draw(scoreText);
            y += 25.0f;
        }
    }
    {
      
      sf::Text questionText(font);
      questionText.setString(L"Хотите сыграть еще раз?");
      questionText.setCharacterSize(32);
      questionText.setFillColor(sf::Color::White);
      sf::FloatRect questionBounds = questionText.getLocalBounds();
      questionText.setPosition(
          {(GAME_STATE.WINDOW_WIDTH - questionBounds.position.x -
            questionBounds.size.x) /
               2.0f,
            350.0f});
      window.draw(questionText);

      sf::Text yesText(font);
      yesText.setString(L"Да");
      yesText.setCharacterSize(36);
      yesText.setFillColor(victoryChoiceYes ? sf::Color::Green
                                            : sf::Color::White);
      yesText.setStyle(victoryChoiceYes ? sf::Text::Bold : sf::Text::Regular);
      sf::FloatRect yesBounds = yesText.getLocalBounds();
      yesText.setPosition(
          {(GAME_STATE.WINDOW_WIDTH - yesBounds.position.x - yesBounds.size.x) /
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
          {(GAME_STATE.WINDOW_WIDTH - noBounds.position.x - noBounds.size.x) /
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
          {(GAME_STATE.WINDOW_WIDTH - instBounds.position.x -
            instBounds.size.x) /
               2.0f,
           450.0f});
   window.draw(instructionsText);
}





  } catch (...) {
    
  }
}

void Game::renderMainMenuScreen() {
  sf::RectangleShape overlay;

  overlay.setSize(
      sf::Vector2f(GAME_STATE.WINDOW_WIDTH, GAME_STATE.WINDOW_HEIGHT));
  overlay.setFillColor(sf::Color(0, 0, 0, 200));
  window.draw(overlay);

  sf::Text gameTitleText(font);
  gameTitleText.setString(L"Игра \"Арканоид\"");
  gameTitleText.setCharacterSize(64);
  gameTitleText.setFillColor(sf::Color(255, 255, 255));
  sf::FloatRect titleBounds = gameTitleText.getLocalBounds();
  gameTitleText.setPosition(
      {(GAME_STATE.WINDOW_WIDTH - titleBounds.position.x - titleBounds.size.x) /
           2.0f,
       200.0f});
  window.draw(gameTitleText);

  sf::Text instructionsText(font);
  instructionsText.setString(L"Нажмите Enter для начала игры");
  instructionsText.setCharacterSize(20);
  instructionsText.setFillColor(sf::Color(200, 200, 200));
  sf::FloatRect instBounds = instructionsText.getLocalBounds();
  instructionsText.setPosition(
      {(GAME_STATE.WINDOW_WIDTH - instBounds.position.x - instBounds.size.x) /
           2.0f,
       450.0f});
  window.draw(instructionsText);
}

void Game::updateBonuses(float deltaTime) {
    auto entities = ecs.getEntitiesWithComponent<ActiveBonusComponent>();
    static size_t lastCount = 0;
    if (entities.size() != lastCount) {
        std::cout << "[DEBUG] updateBonuses: " << entities.size() << " active bonuses" << std::endl;
        lastCount = entities.size();
    }
    for (Entity entity : entities) {
        auto bonus = ecs.getComponent<ActiveBonusComponent>(entity);
        if (!bonus) continue;
        
        bonus->remainingTime -= deltaTime;
        if (bonus->remainingTime <= 0.0f) {
            std::cout << "[DEBUG] Bonus expired: type=" << static_cast<int>(bonus->type) 
                      << ", remainingTime=" << bonus->remainingTime << std::endl;
            
            switch (bonus->type) {
                case BonusType::SlowBall: {
                    auto velocity = ecs.getComponent<VelocityComponent>(entity);
                    if (velocity) {
                        velocity->speed = bonus->originalValue;
                        
                        float currentSpeed = std::sqrt(velocity->velocity.x * velocity->velocity.x + velocity->velocity.y * velocity->velocity.y);
                        if (currentSpeed > 0.0f) {
                            velocity->velocity = velocity->velocity * (velocity->speed / currentSpeed);
                        }
                    }
                    break;
                }
                case BonusType::FastPlatform: {
                    auto input = ecs.getComponent<InputComponent>(entity);
                    if (input) {
                        input->moveSpeed = bonus->originalValue;
                    }
                    break;
                }
                case BonusType::BigPlatform: {
                    auto shape = ecs.getComponent<ShapeComponent>(entity);
                    auto collider = ecs.getComponent<ColliderComponent>(entity);
                    auto position = ecs.getComponent<PositionComponent>(entity);
                    if (shape && shape->type == ShapeComponent::Type::Rectangle) {
                        std::cout << "[DEBUG] BigPlatform bonus expired. Restoring width from " 
                                  << shape->rectangle.width << " to " << bonus->originalValue << std::endl;
                        shape->rectangle.width = bonus->originalValue;
                        if (collider) {
                            collider->size.x = bonus->originalValue;
                        }
                        
                        if (position) {
                            float newWidth = bonus->originalValue;
                            if (position->position.x < 0.0f) {
                                position->position.x = 0.0f;
                            }
                            if (position->position.x + newWidth > static_cast<float>(GAME_STATE.WINDOW_WIDTH)) {
                                position->position.x = static_cast<float>(GAME_STATE.WINDOW_WIDTH) - newWidth;
                            }
                        }
                    }
                    break;
                }
            }
            ecs.removeComponent<ActiveBonusComponent>(entity);
        }
    }
}

void Game::renderBonusTimers() {
    auto entities = ecs.getEntitiesWithComponent<ActiveBonusComponent>();
    float startX = 30.0f;
    float startY = 30.0f;
    float spacing = 40.0f;
    int index = 0;
    for (Entity entity : entities) {
        auto bonus = ecs.getComponent<ActiveBonusComponent>(entity);
        if (!bonus) continue;
        
        float radius = 15.0f;
        float thickness = 5.0f;
        
        sf::Color color;
        switch (bonus->type) {
            case BonusType::SlowBall: color = sf::Color::Red; break;
            case BonusType::FastPlatform: color = sf::Color::Green; break;
            case BonusType::BigPlatform: color = sf::Color::Blue; break;
        }
        
        
        sf::CircleShape outerCircle(radius);
        outerCircle.setPosition({startX + index * spacing - radius, startY - radius});
        outerCircle.setFillColor(sf::Color::Transparent);
        outerCircle.setOutlineColor(color);
        outerCircle.setOutlineThickness(thickness);
        window.draw(outerCircle);
        
        
        if (font.getInfo().family != "") {
            sf::Text timeText(font);
            timeText.setString(std::to_string(static_cast<int>(bonus->remainingTime) + 1));
            timeText.setCharacterSize(12);
            timeText.setFillColor(sf::Color::White);
            timeText.setPosition({startX + index * spacing - 5, startY - 6});
            window.draw(timeText);
        }
        
        index++;
    }
}

Game::~Game() {
    GAME_STATE.saveHighScores("highscores.txt");
}
