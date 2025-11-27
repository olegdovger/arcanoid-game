#pragma once

#include "ECS/Entity.h"
#include "ECS/ECSManager.h"

class EntityFactory
{
public:
    static Entity createPlatform(ECSManager& ecs, float x, float y, float width, float height);
    static Entity createBall(ECSManager& ecs, float x, float y, float radius);
};

