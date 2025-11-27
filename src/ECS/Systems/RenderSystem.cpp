#include "RenderSystem.h"
#include "../ECSManager.h"
#include "../Components.h"
#include "../../GameState.h"

#include <SFML/Graphics.hpp>

void RenderSystem::setWindow(sf::RenderWindow* win)
{
    window = win;
}

void RenderSystem::update(float deltaTime, ECSManager& ecs)
{
    if (!window) return;

    // Ensure view is set correctly (fixed size to prevent stretching)
    sf::View view;
    view.setSize({static_cast<float>(GameState::WINDOW_WIDTH), static_cast<float>(GameState::WINDOW_HEIGHT)});
    view.setCenter({static_cast<float>(GameState::WINDOW_WIDTH) / 2.0f, static_cast<float>(GameState::WINDOW_HEIGHT) / 2.0f});
    window->setView(view);

    // Get all entities with Position and Shape components
    auto entities = ecs.getEntitiesWithComponent<PositionComponent>();

    for (Entity entity : entities)
    {
        auto position = ecs.getComponent<PositionComponent>(entity);
        auto shape = ecs.getComponent<ShapeComponent>(entity);

        if (!position || !shape) continue;

        if (shape->type == ShapeComponent::Type::Rectangle)
        {
            sf::RectangleShape rect;
            // Use original size and position (no scaling)
            rect.setSize({shape->rectangle.width, shape->rectangle.height});
            rect.setPosition(position->position);
            rect.setFillColor(shape->color);
            window->draw(rect);
        }
        else if (shape->type == ShapeComponent::Type::Circle)
        {
            // Use original radius and position (no scaling)
            float radius = shape->circle.radius;
            sf::CircleShape circle(radius);
            circle.setPosition({position->position.x - radius,
                               position->position.y - radius});
            circle.setFillColor(shape->color);
            window->draw(circle);
        }
    }
}

