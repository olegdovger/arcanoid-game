#include "Platform.h"

Platform::Platform(float x, float y, float width, float height)
{
    shape.setSize({width, height});
    shape.setPosition({x, y});
    shape.setFillColor(sf::Color::White);
    position = {x, y};
    speed = 400.0f; // pixels per second
}

void Platform::update(float deltaTime, const sf::RenderWindow& window)
{
    float moveDirection = 0.0f;

    // Check for left/right movement (A/D or Left/Right arrows)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        moveDirection = -1.0f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        moveDirection = 1.0f;
    }

    // Update position smoothly
    position.x += moveDirection * speed * deltaTime;

    // Keep platform within window bounds
    float windowWidth = static_cast<float>(window.getSize().x);
    float platformWidth = shape.getSize().x;
    if (position.x < 0.0f)
        position.x = 0.0f;
    if (position.x + platformWidth > windowWidth)
        position.x = windowWidth - platformWidth;

    shape.setPosition(position);
}

sf::FloatRect Platform::getBounds() const
{
    return shape.getGlobalBounds();
}

void Platform::draw(sf::RenderWindow& window) const
{
    window.draw(shape);
}

sf::Vector2f Platform::getPosition() const
{
    return position;
}

sf::Vector2f Platform::getSize() const
{
    return shape.getSize();
}

