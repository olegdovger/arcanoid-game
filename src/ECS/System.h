#pragma once

#include "Entity.h"
#include <vector>

// Forward declaration
class ECSManager;

// Base system class - systems contain logic
class System
{
public:
    virtual ~System() = default;
    virtual void update(float deltaTime, ECSManager& ecs) = 0;
};

