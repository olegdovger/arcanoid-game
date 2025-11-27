#pragma once

#include "../System.h"
#include <SFML/Window/Keyboard.hpp>

class InputSystem : public System
{
public:
    void update(float deltaTime, ECSManager& ecs) override;
};

