#pragma once

#include "../System.h"
#include <SFML/Graphics.hpp>

class ResizeSystem : public System
{
public:
    void setWindow(sf::RenderWindow* window);
    void update(float deltaTime, ECSManager& ecs) override;

private:
    sf::RenderWindow* window = nullptr;
    unsigned int lastWidth = 0;
    unsigned int lastHeight = 0;
};

