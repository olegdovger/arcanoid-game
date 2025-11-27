#pragma once

#include <SFML/Graphics.hpp>

class Platform;

class Ball
{
public:
    Ball(float x, float y, float radius);

    void update(float deltaTime, const sf::RenderWindow& window, const Platform& platform);

    void draw(sf::RenderWindow& window) const;

    sf::Vector2f getPosition() const;
    float getRadius() const;

private:
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float speed;
};

