#pragma once

#include "../System.h"

class MovementSystem : public System
{
public:
    void update(float deltaTime, ECSManager& ecs) override;
};

