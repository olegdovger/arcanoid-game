#pragma once

#include "Component.h"
#include <SFML/Graphics.hpp>
#include <cmath>

// Position component
struct PositionComponent : public Component
{
    sf::Vector2f position;

    PositionComponent(float x = 0.0f, float y = 0.0f) : position(x, y) {}
};

// Velocity component
struct VelocityComponent : public Component
{
    sf::Vector2f velocity;
    float speed;

    VelocityComponent(float vx = 0.0f, float vy = 0.0f, float s = 0.0f)
        : velocity(vx, vy), speed(s)
    {
        if (speed == 0.0f && (vx != 0.0f || vy != 0.0f))
        {
            speed = std::sqrt(vx * vx + vy * vy);
        }
    }
};

// Shape component for rendering
struct ShapeComponent : public Component
{
    enum class Type
    {
        Rectangle,
        Circle
    };

    Type type;
    sf::Color color;
    union
    {
        struct { float width, height; } rectangle;
        struct { float radius; } circle;
    };

    ShapeComponent(Type t, sf::Color c = sf::Color::White)
        : type(t), color(c)
    {
        if (t == Type::Rectangle)
        {
            rectangle = {0.0f, 0.0f};
        }
        else
        {
            circle = {0.0f};
        }
    }
};

// Collider component
struct ColliderComponent : public Component
{
    enum class Type
    {
        Platform,
        Ball,
        Wall,
        Brick
    };

    Type type;
    float radius; // For circle colliders
    sf::Vector2f size; // For rectangle colliders

    ColliderComponent(Type t, float r = 0.0f)
        : type(t), radius(r), size(0.0f, 0.0f) {}

    ColliderComponent(Type t, float w, float h)
        : type(t), radius(0.0f), size(w, h) {}
};

// Input component (for entities that respond to input)
struct InputComponent : public Component
{
    bool leftPressed = false;
    bool rightPressed = false;
    float moveSpeed = 400.0f;
};

struct DestructibleComponent : public Component
{
    int maxHits = 1;
    int currentHits = 0;
    
    DestructibleComponent(int hits = 1) : maxHits(hits), currentHits(0) {}
    
    bool isDestroyed() const {
        return currentHits >= maxHits;
    }
    
    void takeHit() {
        if (!isDestroyed()) {
            currentHits++;
        }
    }
    
    float getHealthPercentage() const {
        return 1.0f - (static_cast<float>(currentHits) / static_cast<float>(maxHits));
    }
};

