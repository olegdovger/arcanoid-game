#include "Ball.h"
#include "Platform.h"
#include <cmath>

Ball::Ball(float x, float y, float radius)
{
    shape.setRadius(radius);
    shape.setPosition({x, y});
    shape.setFillColor(sf::Color::Green);
    position = {x, y};
    velocity = {200.0f, -200.0f}; // Initial velocity
    speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
}

void Ball::update(float deltaTime, const sf::RenderWindow& window, const Platform& platform)
{
    // Update position
    position += velocity * deltaTime;

    // Get window bounds
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    float radius = shape.getRadius();

    // Collision with left wall
    if (position.x - radius < 0.0f)
    {
        position.x = radius;
        velocity.x = -velocity.x;
    }

    // Collision with right wall
    if (position.x + radius > windowWidth)
    {
        position.x = windowWidth - radius;
        velocity.x = -velocity.x;
    }

    // Collision with top wall
    if (position.y - radius < 0.0f)
    {
        position.y = radius;
        velocity.y = -velocity.y;
    }

    // Collision with platform (manual AABB collision detection)
    sf::Vector2f platformPos = platform.getPosition();
    sf::Vector2f platformSize = platform.getSize();
    
    // Check if ball intersects with platform
    bool colliding = (position.x + radius > platformPos.x &&
                     position.x - radius < platformPos.x + platformSize.x &&
                     position.y + radius > platformPos.y &&
                     position.y - radius < platformPos.y + platformSize.y);

    if (colliding)
    {
        // Calculate collision point relative to platform center
        float platformCenterX = platformPos.x + platformSize.x / 2.0f;
        float ballCenterX = position.x;
        float relativeIntersectX = (ballCenterX - platformCenterX) / (platformSize.x / 2.0f);

        // Reflect ball upward
        if (velocity.y > 0.0f) // Only reflect if ball is moving downward
        {
            velocity.y = -std::abs(velocity.y);
            
            // Add some angle based on where ball hits platform
            velocity.x = relativeIntersectX * speed * 0.5f;
            
            // Normalize velocity to maintain speed
            float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            if (currentSpeed > 0.0f)
            {
                velocity = velocity * (speed / currentSpeed);
            }

            // Move ball above platform to prevent sticking
            position.y = platformPos.y - radius;
        }
    }

    shape.setPosition({position.x - radius, position.y - radius});
}

void Ball::draw(sf::RenderWindow& window) const
{
    window.draw(shape);
}

sf::Vector2f Ball::getPosition() const
{
    return position;
}

float Ball::getRadius() const
{
    return shape.getRadius();
}

