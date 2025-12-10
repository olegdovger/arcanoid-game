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
    view.setSize({static_cast<float>(GAME_STATE.WINDOW_WIDTH), static_cast<float>(GAME_STATE.WINDOW_HEIGHT)});
    view.setCenter({static_cast<float>(GAME_STATE.WINDOW_WIDTH) / 2.0f, static_cast<float>(GAME_STATE.WINDOW_HEIGHT) / 2.0f});
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

            auto destructible = ecs.getComponent<DestructibleComponent>(entity);
            if (destructible && destructible->maxHits > 1) {

                rect.setOutlineColor(sf::Color::White);
                rect.setOutlineThickness(1.0f * destructible->getHealthPercentage());

                if (!destructible->isDestroyed() && destructible->currentHits > 0) {
                    float damageRatio = static_cast<float>(destructible->currentHits) / static_cast<float>(destructible->maxHits);
                    rect.setOutlineColor(sf::Color(255, 255, 255, 255 * damageRatio));
                    rect.setOutlineThickness(2.0f);
                }
            }

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
