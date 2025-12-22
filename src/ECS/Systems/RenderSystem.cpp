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

    
    sf::View view;
    view.setSize({static_cast<float>(GAME_STATE.WINDOW_WIDTH), static_cast<float>(GAME_STATE.WINDOW_HEIGHT)});
    view.setCenter({static_cast<float>(GAME_STATE.WINDOW_WIDTH) / 2.0f, static_cast<float>(GAME_STATE.WINDOW_HEIGHT) / 2.0f});
    window->setView(view);

    
    auto entities = ecs.getEntitiesWithComponent<PositionComponent>();

    for (Entity entity : entities)
    {
        auto position = ecs.getComponent<PositionComponent>(entity);
        auto shape = ecs.getComponent<ShapeComponent>(entity);

        if (!position || !shape) continue;

        if (shape->type == ShapeComponent::Type::Rectangle)
        {
            sf::RectangleShape rect;
            
            rect.setSize({shape->rectangle.width, shape->rectangle.height});
            rect.setPosition(position->position);
            rect.setFillColor(shape->color);

            auto durableBrick = ecs.getComponent<DurableBrick>(entity);

            if (durableBrick && durableBrick->maxHits > 1) {

                rect.setOutlineColor(sf::Color::White);
                rect.setOutlineThickness(1.0f * durableBrick->getHealthPercentage());

                if (!durableBrick->isDestroyed() && durableBrick->currentHits > 0) {
                    float damageRatio = static_cast<float>(durableBrick->currentHits) / static_cast<float>(durableBrick->maxHits);
                    rect.setOutlineColor(sf::Color(255, 255, 255, 255 * damageRatio));
                    rect.setOutlineThickness(2.0f);
                }
            }

            window->draw(rect);

            auto bonus = ecs.getComponent<BonusComponent>(entity);
            if (bonus && !bonus->collected) {
                
                float centerX = position->position.x + shape->rectangle.width / 2.0f;
                float centerY = position->position.y + shape->rectangle.height / 2.0f;
                float radius = 5.0f;
                sf::CircleShape indicator(radius);
                indicator.setPosition({centerX - radius, centerY - radius});
                
                switch (bonus->type) {
                    case BonusType::SlowBall: indicator.setFillColor(sf::Color::Red); break;
                    case BonusType::FastPlatform: indicator.setFillColor(sf::Color::Green); break;
                    case BonusType::BigPlatform: indicator.setFillColor(sf::Color::Blue); break;
                }
                indicator.setOutlineColor(sf::Color::Black);
                indicator.setOutlineThickness(1.0f);
                window->draw(indicator);
            }
        }
        else if (shape->type == ShapeComponent::Type::Circle)
        {
            
            float radius = shape->circle.radius;
            sf::CircleShape circle(radius);
            circle.setPosition({position->position.x - radius,
                               position->position.y - radius});
            circle.setFillColor(shape->color);
            window->draw(circle);
        }
    }
}
