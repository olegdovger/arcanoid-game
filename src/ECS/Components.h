#pragma once

#include "Component.h"
#include <SFML/Graphics.hpp>
#include <cmath>


struct PositionComponent : public Component
{
    sf::Vector2f position;

    PositionComponent(float x = 0.0f, float y = 0.0f) : position(x, y) {}
};


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
    float radius; 
    sf::Vector2f size; 

    ColliderComponent(Type t, float r = 0.0f)
        : type(t), radius(r), size(0.0f, 0.0f) {}

    ColliderComponent(Type t, float w, float h)
        : type(t), radius(0.0f), size(w, h) {}
};


struct InputComponent : public Component
{
    bool leftPressed = false;
    bool rightPressed = false;
    float moveSpeed = 400.0f;
};

struct DurableBrick : public Component
{
    int maxHits = 1;
    int currentHits = 0;

    DurableBrick(int hits = 1) : maxHits(hits), currentHits(0) {}

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


enum class BonusType
{
    SlowBall,      
    FastPlatform,  
    BigPlatform    
};


struct BonusComponent : public Component
{
    BonusType type;
    bool collected = false;
    
    BonusComponent(BonusType t = BonusType::SlowBall) : type(t) {}
};


struct ActiveBonusComponent : public Component
{
    BonusType type;
    float remainingTime;
    float originalValue; 
    
    ActiveBonusComponent(BonusType t, float time, float original = 0.0f)
        : type(t), remainingTime(time), originalValue(original) {}
};
