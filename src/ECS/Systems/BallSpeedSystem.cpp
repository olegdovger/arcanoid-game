#include "BallSpeedSystem.h"
#include "../ECSManager.h"
#include "../../GameState.h"

BallSpeedSystem::BallSpeedSystem()
{
    reset();
}

void BallSpeedSystem::update(float deltaTime, ECSManager& ecs)
{
    
    gameTime += deltaTime;

    
    auto entities = ecs.getEntitiesWithComponent<ColliderComponent>();

    for (Entity entity : entities)
    {
        auto collider = ecs.getComponent<ColliderComponent>(entity);
        if (!collider || collider->type != ColliderComponent::Type::Ball)
            continue;

        auto velocity = ecs.getComponent<VelocityComponent>(entity);
        if (!velocity)
            continue;

        
        if (!initialized)
        {
            initialized = true;
            
            speedMultiplier = 1.0f;
            lastSpeedIncreaseTime = gameTime;
        }

        
        auto activeBonus = ecs.getComponent<ActiveBonusComponent>(entity);
        bool hasSlowBall = (activeBonus && activeBonus->type == BonusType::SlowBall);
        
        
        if (hasSlowBall)
        {
            lastSpeedIncreaseTime += deltaTime;
        }

        
        if (!hasSlowBall && gameTime - lastSpeedIncreaseTime >= GAME_STATE.BALL_SPEED_INCREASE_INTERVAL)
        {
            
            float newMultiplier = speedMultiplier * GAME_STATE.BALL_SPEED_MULTIPLIER;

            
            if (newMultiplier <= GAME_STATE.BALL_MAX_SPEED_MULTIPLIER)
            {
                speedMultiplier = newMultiplier;
                lastSpeedIncreaseTime = gameTime;

                
                float currentSpeed = std::sqrt(velocity->velocity.x * velocity->velocity.x +
                                             velocity->velocity.y * velocity->velocity.y);

                if (currentSpeed > 0)
                {
                    
                    float newSpeed = std::sqrt(GAME_STATE.BALL_INITIAL_VELOCITY_X * GAME_STATE.BALL_INITIAL_VELOCITY_X +
                                             GAME_STATE.BALL_INITIAL_VELOCITY_Y * GAME_STATE.BALL_INITIAL_VELOCITY_Y) * speedMultiplier;

                    velocity->velocity.x = (velocity->velocity.x / currentSpeed) * newSpeed;
                    velocity->velocity.y = (velocity->velocity.y / currentSpeed) * newSpeed;
                    velocity->speed = newSpeed;
                }
            }
        }
    }
}

void BallSpeedSystem::reset()
{
    gameTime = 0.0f;
    lastSpeedIncreaseTime = 0.0f;
    speedMultiplier = 1.0f;
    initialized = false;
}
