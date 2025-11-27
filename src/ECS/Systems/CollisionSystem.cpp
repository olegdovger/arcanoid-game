#include "CollisionSystem.h"
#include "../ECSManager.h"
#include "../Components.h"
#include "../Entity.h"
#include "../../GameState.h"
#include <cmath>

void CollisionSystem::update(float deltaTime, ECSManager& ecs)
{
    // Get all entities with ColliderComponent
    auto entities = ecs.getEntitiesWithComponent<ColliderComponent>();

    for (Entity entity : entities)
    {
        auto collider = ecs.getComponent<ColliderComponent>(entity);
        auto position = ecs.getComponent<PositionComponent>(entity);
        auto velocity = ecs.getComponent<VelocityComponent>(entity);

        if (!collider || !position) continue;

        // Handle wall collisions for balls
        if (collider->type == ColliderComponent::Type::Ball && velocity)
        {
            float radius = collider->radius;

            // Left wall
            if (position->position.x - radius < 0.0f)
            {
                position->position.x = radius;
                velocity->velocity.x = -velocity->velocity.x;
            }

            // Right wall
            if (position->position.x + radius > static_cast<float>(GameState::WINDOW_WIDTH))
            {
                position->position.x = static_cast<float>(GameState::WINDOW_WIDTH) - radius;
                velocity->velocity.x = -velocity->velocity.x;
            }

            // Top wall
            if (position->position.y - radius < 0.0f)
            {
                position->position.y = radius;
                velocity->velocity.y = -velocity->velocity.y;
            }
        }

        // Handle platform bounds (keep platform within window)
        if (collider->type == ColliderComponent::Type::Platform)
        {
            float platformWidth = collider->size.x;
            if (position->position.x < 0.0f)
                position->position.x = 0.0f;
            if (position->position.x + platformWidth > static_cast<float>(GameState::WINDOW_WIDTH))
                position->position.x = static_cast<float>(GameState::WINDOW_WIDTH) - platformWidth;
        }
    }

    // Ball-Platform collision detection
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

            // AABB collision detection
            bool colliding = (ballPos->position.x + ballRadius > platformPos->position.x &&
                             ballPos->position.x - ballRadius < platformPos->position.x + platformSize.x &&
                             ballPos->position.y + ballRadius > platformPos->position.y &&
                             ballPos->position.y - ballRadius < platformPos->position.y + platformSize.y);

            if (colliding && ballVelocity->velocity.y > 0.0f) // Only if ball moving downward
            {
                // Reflect ball upward
                ballVelocity->velocity.y = -std::abs(ballVelocity->velocity.y);

                // Calculate collision point relative to platform center
                // Add defensive check to prevent division by zero
                if (platformSize.x > 0.0f)
                {
                    float platformCenterX = platformPos->position.x + platformSize.x / 2.0f;
                    float ballCenterX = ballPos->position.x;
                    float relativeIntersectX = (ballCenterX - platformCenterX) / (platformSize.x / 2.0f);

                    // Add angle based on hit position
                    ballVelocity->velocity.x = relativeIntersectX * ballVelocity->speed * 0.5f;
                }
                else
                {
                    // If platform has zero width, just reflect vertically
                    ballVelocity->velocity.x = 0.0f;
                }

                // Normalize velocity to maintain speed
                float currentSpeed = std::sqrt(ballVelocity->velocity.x * ballVelocity->velocity.x +
                                              ballVelocity->velocity.y * ballVelocity->velocity.y);
                if (currentSpeed > 0.0f)
                {
                    ballVelocity->velocity = ballVelocity->velocity * (ballVelocity->speed / currentSpeed);
                }

                // Move ball above platform
                ballPos->position.y = platformPos->position.y - ballRadius;
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
    // Ball is out if it goes below the window
    return (ballPos->position.y - radius > static_cast<float>(GameState::WINDOW_HEIGHT));
}

