#include "CollisionSystem.h"
#include "../ECSManager.h"
#include "../Components.h"
#include "../Entity.h"
#include "../../GameState.h"
#include <cstdint>
#include <cmath>
#include <cstdio>

namespace {
void applyBonus(ECSManager& ecs, Entity targetEntity, BonusType type, float duration) {
    
    auto existing = ecs.getComponent<ActiveBonusComponent>(targetEntity);
    if (existing) {
        if (existing->type == type) {
            
            existing->remainingTime = duration;
            return;
        } else {
            
            switch (existing->type) {
                case BonusType::SlowBall: {
                    auto velocity = ecs.getComponent<VelocityComponent>(targetEntity);
                    if (velocity) {
                        velocity->speed = existing->originalValue;
                        
                        float currentSpeed = std::sqrt(velocity->velocity.x * velocity->velocity.x + velocity->velocity.y * velocity->velocity.y);
                        if (currentSpeed > 0.0f) {
                            velocity->velocity = velocity->velocity * (velocity->speed / currentSpeed);
                        }
                    }
                    break;
                }
                case BonusType::FastPlatform: {
                    auto input = ecs.getComponent<InputComponent>(targetEntity);
                    if (input) input->moveSpeed = existing->originalValue;
                    break;
                }
                case BonusType::BigPlatform: {
                    auto shape = ecs.getComponent<ShapeComponent>(targetEntity);
                    auto position = ecs.getComponent<PositionComponent>(targetEntity);
                    if (shape && shape->type == ShapeComponent::Type::Rectangle) {
                        shape->rectangle.width = existing->originalValue;
                        if (auto collider = ecs.getComponent<ColliderComponent>(targetEntity)) {
                            collider->size.x = existing->originalValue;
                        }
                        
                        if (position) {
                            float newWidth = existing->originalValue;
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
            ecs.removeComponent<ActiveBonusComponent>(targetEntity);
        }
    }
    
    
    float originalValue = 0.0f;
    switch (type) {
        case BonusType::SlowBall: {
            auto velocity = ecs.getComponent<VelocityComponent>(targetEntity);
            if (velocity) {
                originalValue = velocity->speed;
                
                velocity->speed *= 0.5f;
                
                float currentSpeed = std::sqrt(velocity->velocity.x * velocity->velocity.x + velocity->velocity.y * velocity->velocity.y);
                if (currentSpeed > 0.0f) {
                    velocity->velocity = velocity->velocity * (velocity->speed / currentSpeed);
                }
            }
            break;
        }
        case BonusType::FastPlatform: {
            auto input = ecs.getComponent<InputComponent>(targetEntity);
            if (input) {
                originalValue = input->moveSpeed;
                
                input->moveSpeed *= 1.5f;
            }
            break;
        }
        case BonusType::BigPlatform: {
            auto shape = ecs.getComponent<ShapeComponent>(targetEntity);
            auto position = ecs.getComponent<PositionComponent>(targetEntity);
            if (shape && shape->type == ShapeComponent::Type::Rectangle) {
                originalValue = shape->rectangle.width;
                printf("[DEBUG] Applying BigPlatform bonus. Original width: %.1f\n", originalValue);
                
                shape->rectangle.width *= 1.5f;
                printf("[DEBUG] New width: %.1f\n", shape->rectangle.width);
                if (auto collider = ecs.getComponent<ColliderComponent>(targetEntity)) {
                    collider->size.x = shape->rectangle.width;
                }
                
                if (position) {
                    float newWidth = shape->rectangle.width;
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
    
    
    ecs.addComponent<ActiveBonusComponent>(targetEntity,
        std::make_shared<ActiveBonusComponent>(type, duration, originalValue));
}
}

void CollisionSystem::update(float deltaTime, ECSManager& ecs)
{
    
    auto entities = ecs.getEntitiesWithComponent<ColliderComponent>();

    for (Entity entity : entities)
    {
        auto collider = ecs.getComponent<ColliderComponent>(entity);
        auto position = ecs.getComponent<PositionComponent>(entity);
        auto velocity = ecs.getComponent<VelocityComponent>(entity);

        if (!collider || !position) continue;

        
        if (collider->type == ColliderComponent::Type::Ball && velocity)
        {
            float radius = collider->radius;

            
            if (position->position.x - radius < 0.0f)
            {
                position->position.x = radius;
                velocity->velocity.x = -velocity->velocity.x;
            }

            
            if (position->position.x + radius > static_cast<float>(GAME_STATE.WINDOW_WIDTH))
            {
                position->position.x = static_cast<float>(GAME_STATE.WINDOW_WIDTH) - radius;
                velocity->velocity.x = -velocity->velocity.x;
            }

            
            if (position->position.y - radius < 0.0f)
            {
                position->position.y = radius;
                velocity->velocity.y = -velocity->velocity.y;
            }
        }

        
        if (collider->type == ColliderComponent::Type::Platform)
        {
            float platformWidth = collider->size.x;
            if (position->position.x < 0.0f)
                position->position.x = 0.0f;
            if (position->position.x + platformWidth > static_cast<float>(GAME_STATE.WINDOW_WIDTH))
                position->position.x = static_cast<float>(GAME_STATE.WINDOW_WIDTH) - platformWidth;
        }
    }

    
    Entity platformEntity = INVALID_ENTITY;
    Entity ballEntity = INVALID_ENTITY;

    for (Entity entity : entities)
    {
        auto collider = ecs.getComponent<ColliderComponent>(entity);
        if (collider)
        {
            if (collider->type == ColliderComponent::Type::Platform)
                platformEntity = entity;
            else if (collider->type == ColliderComponent::Type::Ball)
                ballEntity = entity;
        }
    }

    if (platformEntity != INVALID_ENTITY && ballEntity != INVALID_ENTITY)
    {
        auto platformPos = ecs.getComponent<PositionComponent>(platformEntity);
        auto platformCollider = ecs.getComponent<ColliderComponent>(platformEntity);
        auto ballPos = ecs.getComponent<PositionComponent>(ballEntity);
        auto ballCollider = ecs.getComponent<ColliderComponent>(ballEntity);
        auto ballVelocity = ecs.getComponent<VelocityComponent>(ballEntity);

        if (platformPos && platformCollider && ballPos && ballCollider && ballVelocity)
        {
            float ballRadius = ballCollider->radius;
            sf::Vector2f platformSize = platformCollider->size;

            
            bool colliding = (ballPos->position.x + ballRadius > platformPos->position.x &&
                             ballPos->position.x - ballRadius < platformPos->position.x + platformSize.x &&
                             ballPos->position.y + ballRadius > platformPos->position.y &&
                             ballPos->position.y - ballRadius < platformPos->position.y + platformSize.y);

            if (colliding && ballVelocity->velocity.y > 0.0f) 
            {
                
                ballVelocity->velocity.y = -std::abs(ballVelocity->velocity.y);

                
                
                if (platformSize.x > 0.0f)
                {
                    float platformCenterX = platformPos->position.x + platformSize.x / 2.0f;
                    float ballCenterX = ballPos->position.x;
                    float relativeIntersectX = (ballCenterX - platformCenterX) / (platformSize.x / 2.0f);

                    
                    ballVelocity->velocity.x = relativeIntersectX * ballVelocity->speed * 0.5f;
                }
                else
                {
                    
                    ballVelocity->velocity.x = 0.0f;
                }

                
                float currentSpeed = std::sqrt(ballVelocity->velocity.x * ballVelocity->velocity.x +
                                              ballVelocity->velocity.y * ballVelocity->velocity.y);
                if (currentSpeed > 0.0f)
                {
                    ballVelocity->velocity = ballVelocity->velocity * (ballVelocity->speed / currentSpeed);
                }

                
                ballPos->position.y = platformPos->position.y - ballRadius;
            }
        }
    }

    if (ballEntity != INVALID_ENTITY)
    {
        auto ballPos = ecs.getComponent<PositionComponent>(ballEntity);
        auto ballCollider = ecs.getComponent<ColliderComponent>(ballEntity);
        auto ballVelocity = ecs.getComponent<VelocityComponent>(ballEntity);

        if (ballPos && ballCollider && ballVelocity)
        {
            float ballRadius = ballCollider->radius;
            std::vector<Entity> bricksToDestroy;
            bool ballCollisionHandled = false;

            for (Entity entity : entities)
            {
                auto collider = ecs.getComponent<ColliderComponent>(entity);
                if (collider && collider->type == ColliderComponent::Type::Brick)
                {
                    auto brickPos = ecs.getComponent<PositionComponent>(entity);
                    if (brickPos)
                    {
                        sf::Vector2f brickSize = collider->size;

                        bool colliding = (ballPos->position.x + ballRadius > brickPos->position.x &&
                                         ballPos->position.x - ballRadius < brickPos->position.x + brickSize.x &&
                                         ballPos->position.y + ballRadius > brickPos->position.y &&
                                         ballPos->position.y - ballRadius < brickPos->position.y + brickSize.y);

                        if (colliding)
                        {

                            if (!ballCollisionHandled)
                            {
                                float ballCenterX = ballPos->position.x;
                                float ballCenterY = ballPos->position.y;
                                float brickLeft = brickPos->position.x;
                                float brickRight = brickPos->position.x + brickSize.x;
                                float brickTop = brickPos->position.y;
                                float brickBottom = brickPos->position.y + brickSize.y;

                                float distLeft = std::abs(ballCenterX - brickLeft);
                                float distRight = std::abs(ballCenterX - brickRight);
                                float distTop = std::abs(ballCenterY - brickTop);
                                float distBottom = std::abs(ballCenterY - brickBottom);

                                float minDist = std::min({distLeft, distRight, distTop, distBottom});

                                if (minDist == distLeft)
                                {
                                    ballVelocity->velocity.x = -ballVelocity->velocity.x;
                                    ballPos->position.x = brickLeft - ballRadius;
                                }
                                else if (minDist == distRight)
                                {
                                    ballVelocity->velocity.x = -ballVelocity->velocity.x;
                                    ballPos->position.x = brickRight + ballRadius;
                                }
                                else if (minDist == distTop)
                                {
                                    ballVelocity->velocity.y = -ballVelocity->velocity.y;
                                    ballPos->position.y = brickTop - ballRadius;
                                }
                                else
                                {
                                    ballVelocity->velocity.y = -ballVelocity->velocity.y;
                                    ballPos->position.y = brickBottom + ballRadius;
                                }

                                ballCollisionHandled = true;
                            }


                            auto durableBrick = ecs.getComponent<DurableBrick>(entity);
                            if (durableBrick) {
                                durableBrick->takeHit();

                                auto shape = ecs.getComponent<ShapeComponent>(entity);
                                if (shape) {
                                    float healthPercentage = durableBrick->getHealthPercentage();
                                    sf::Color originalColor = shape->color;

                                    shape->color = sf::Color(
                                        static_cast<std::uint8_t>(originalColor.r * healthPercentage),
                                        static_cast<std::uint8_t>(originalColor.g * healthPercentage),
                                        static_cast<std::uint8_t>(originalColor.b * healthPercentage)
                                    );
                                }

                                if (durableBrick->isDestroyed()) {
                                    GAME_STATE.addScore(durableBrick->maxHits * 10);
                                    
                                    auto bonus = ecs.getComponent<BonusComponent>(entity);
                                    if (bonus && !bonus->collected) {
                                        bonus->collected = true;
                                        
                                        Entity targetEntity = INVALID_ENTITY;
                                        float duration = 0.0f;
                                        switch (bonus->type) {
                                            case BonusType::SlowBall:
                                                targetEntity = ballEntity;
                                                duration = GAME_STATE.SLOW_BALL_DURATION;
                                                break;
                                            case BonusType::FastPlatform:
                                                targetEntity = platformEntity;
                                                duration = GAME_STATE.FAST_PLATFORM_DURATION;
                                                break;
                                            case BonusType::BigPlatform:
                                                targetEntity = platformEntity;
                                                duration = GAME_STATE.BIG_PLATFORM_DURATION;
                                                break;
                                        }
                                        if (targetEntity != INVALID_ENTITY) {
                                            applyBonus(ecs, targetEntity, bonus->type, duration);
                                        }
                                    }
                                    bricksToDestroy.push_back(entity);
                                }
                            } else {
                                GAME_STATE.addScore(10);
                                
                                auto bonus = ecs.getComponent<BonusComponent>(entity);
                                if (bonus && !bonus->collected) {
                                    bonus->collected = true;
                                    
                                    Entity targetEntity = INVALID_ENTITY;
                                    float duration = 0.0f;
                                    switch (bonus->type) {
                                        case BonusType::SlowBall:
                                            targetEntity = ballEntity;
                                            duration = GAME_STATE.SLOW_BALL_DURATION;
                                            break;
                                        case BonusType::FastPlatform:
                                            targetEntity = platformEntity;
                                            duration = GAME_STATE.FAST_PLATFORM_DURATION;
                                            break;
                                        case BonusType::BigPlatform:
                                            targetEntity = platformEntity;
                                            duration = GAME_STATE.BIG_PLATFORM_DURATION;
                                            break;
                                    }
                                    if (targetEntity != INVALID_ENTITY) {
                                        applyBonus(ecs, targetEntity, bonus->type, duration);
                                    }
                                }
                                bricksToDestroy.push_back(entity);
                            }
                        }
                    }
                }
            }

            for (Entity brick : bricksToDestroy)
            {
                ecs.destroyEntity(brick);
            }
        }
    }
}

bool CollisionSystem::isBallOutOfBounds(Entity ballEntity, ECSManager& ecs) const
{
    auto ballPos = ecs.getComponent<PositionComponent>(ballEntity);
    auto ballCollider = ecs.getComponent<ColliderComponent>(ballEntity);

    if (!ballPos || !ballCollider) return false;

    float radius = ballCollider->radius;

    return (ballPos->position.y - radius > static_cast<float>(GAME_STATE.WINDOW_HEIGHT));
}
