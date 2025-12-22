#pragma once

#include "ECS/Entity.h"
#include "ECS/ECSManager.h"
#include "ECS/Components.h"
#include <SFML/Graphics.hpp>

class EntityFactory
{
public:
    static Entity createPlatform(ECSManager& ecs, float x, float y, float width, float height);
    static Entity createBall(ECSManager& ecs, float x, float y, float radius);
    static Entity createBrick(ECSManager& ecs, float x, float y, float width, float height, sf::Color color = sf::Color::Red, int hitPoints = 1, bool hasBonus = false, BonusType bonusType = BonusType::SlowBall);
};

