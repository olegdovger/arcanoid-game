#pragma once

#include "Entity.h"
#include <vector>


class ECSManager;


class System
{
public:
    virtual ~System() = default;
    virtual void update(float deltaTime, ECSManager& ecs) = 0;
};

