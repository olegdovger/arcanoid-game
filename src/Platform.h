#pragma once

#include <SFML/Graphics.hpp>

class Platform
{
public:
    Platform(float x, float y, float width, float height);

    void update(float deltaTime, const sf::RenderWindow& window);

    sf::FloatRect getBounds() const;

    void draw(sf::RenderWindow& window) const;

    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;

private:
    sf::RectangleShape shape;
    sf::Vector2f position;
    float speed;
};

