#include "EntityFactory.h"
#include "ECS/Components.h"

#include "GameState.h"

Entity EntityFactory::createPlatform(ECSManager& ecs, float x, float y, float width, float height)
{
    Entity entity = ecs.createEntity();

    // Add components
    ecs.addComponent<PositionComponent>(entity, std::make_shared<PositionComponent>(x, y));
    ecs.addComponent<VelocityComponent>(entity, std::make_shared<VelocityComponent>(0.0f, 0.0f, 0.0f));
    ecs.addComponent<ShapeComponent>(entity, std::make_shared<ShapeComponent>(
        ShapeComponent::Type::Rectangle, sf::Color::White));
    ecs.getComponent<ShapeComponent>(entity)->rectangle.width = width;
    ecs.getComponent<ShapeComponent>(entity)->rectangle.height = height;
    ecs.addComponent<ColliderComponent>(entity, std::make_shared<ColliderComponent>(
        ColliderComponent::Type::Platform, width, height));
    ecs.addComponent<InputComponent>(entity, std::make_shared<InputComponent>());

    return entity;
}

Entity EntityFactory::createBall(ECSManager& ecs, float x, float y, float radius)
{
    Entity entity = ecs.createEntity();

    // Add components
    ecs.addComponent<PositionComponent>(entity, std::make_shared<PositionComponent>(x, y));
    ecs.addComponent<VelocityComponent>(entity, std::make_shared<VelocityComponent>(GAME_STATE.BALL_INITIAL_VELOCITY_X, GAME_STATE.BALL_INITIAL_VELOCITY_Y, 0.0f));
    ecs.addComponent<ShapeComponent>(entity, std::make_shared<ShapeComponent>(
        ShapeComponent::Type::Circle, sf::Color::Green));
    ecs.getComponent<ShapeComponent>(entity)->circle.radius = radius;
    ecs.addComponent<ColliderComponent>(entity, std::make_shared<ColliderComponent>(
        ColliderComponent::Type::Ball, radius));

    return entity;
}

Entity EntityFactory::createBrick(ECSManager& ecs, float x, float y, float width, float height, sf::Color color, int hitPoints)
{
    Entity entity = ecs.createEntity();

    // Add components
    ecs.addComponent<PositionComponent>(entity, std::make_shared<PositionComponent>(x, y));
    ecs.addComponent<VelocityComponent>(entity, std::make_shared<VelocityComponent>(0.0f, 0.0f, 0.0f));
    ecs.addComponent<ShapeComponent>(entity, std::make_shared<ShapeComponent>(
        ShapeComponent::Type::Rectangle, color));
    ecs.getComponent<ShapeComponent>(entity)->rectangle.width = width;
    ecs.getComponent<ShapeComponent>(entity)->rectangle.height = height;
    ecs.addComponent<ColliderComponent>(entity, std::make_shared<ColliderComponent>(
        ColliderComponent::Type::Brick, width, height));
    ecs.addComponent<DurableBrick>(entity, std::make_shared<DurableBrick>(hitPoints));

    return entity;
}
