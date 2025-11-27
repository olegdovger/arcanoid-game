#pragma once

#include "../System.h"
#include <SFML/Graphics.hpp>

class RenderSystem : public System
{
public:
    void setWindow(sf::RenderWindow* window);
    void update(float deltaTime, ECSManager& ecs) override;

private:
    sf::RenderWindow* window = nullptr;
};

