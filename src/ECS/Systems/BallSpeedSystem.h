#pragma once

#include "../System.h"
#include "../Components.h"

class BallSpeedSystem : public System
{
public:
    BallSpeedSystem();
    
    void update(float deltaTime, ECSManager& ecs) override;
    
    void setGameTime(float time) { gameTime = time; }
    float getGameTime() const { return gameTime; }
    
    void reset();

private:
    float gameTime = 0.0f;
    float lastSpeedIncreaseTime = 0.0f;
    float speedMultiplier = 1.0f;
    bool initialized = false;
};
