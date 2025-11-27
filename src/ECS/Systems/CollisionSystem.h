#pragma once

#include "../System.h"
#include "../Entity.h"
#include <SFML/Graphics.hpp>

class ECSManager;

class CollisionSystem : public System
{
public:
    void update(float deltaTime, ECSManager& ecs) override;
    bool isBallOutOfBounds(Entity ballEntity, ECSManager& ecs) const;
};

