#include "MovementSystem.h"
#include "../ECSManager.h"
#include "../Components.h"
#include <SFML/Graphics.hpp>

void MovementSystem::update(float deltaTime, ECSManager& ecs)
{
    // Get all entities with both Position and Velocity components
    auto entities = ecs.getEntitiesWithComponent<PositionComponent>();

    for (Entity entity : entities)
    {
        auto position = ecs.getComponent<PositionComponent>(entity);
        auto velocity = ecs.getComponent<VelocityComponent>(entity);
        auto input = ecs.getComponent<InputComponent>(entity);

        if (!position) continue;

        // Handle input-based movement (for platform)
        if (input && velocity)
        {
            float moveDirection = 0.0f;
            if (input->leftPressed)
                moveDirection = -1.0f;
            else if (input->rightPressed)
                moveDirection = 1.0f;

            velocity->velocity.x = moveDirection * input->moveSpeed;
        }

        // Update position based on velocity
        if (velocity)
        {
            position->position += velocity->velocity * deltaTime;
        }
    }
}

