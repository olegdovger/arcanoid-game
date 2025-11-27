#include "ResizeSystem.h"
#include "../ECSManager.h"
#include <SFML/Graphics.hpp>

void ResizeSystem::setWindow(sf::RenderWindow* win)
{
    window = win;
    if (window)
    {
        lastWidth = window->getSize().x;
        lastHeight = window->getSize().y;
    }
}

void ResizeSystem::update(float deltaTime, ECSManager& ecs)
{
    if (!window) return;

    unsigned int currentWidth = window->getSize().x;
    unsigned int currentHeight = window->getSize().y;

    // Check if window size has changed
    // Note: Visual scaling is handled by RenderSystem automatically
    // Physics uses fixed dimensions (800x600) from GameState
    if (currentWidth != lastWidth || currentHeight != lastHeight)
    {
        // Update stored size
        lastWidth = currentWidth;
        lastHeight = currentHeight;
    }
}

