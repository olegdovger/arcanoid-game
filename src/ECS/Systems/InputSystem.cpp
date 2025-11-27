#include "InputSystem.h"
#include "../ECSManager.h"
#include "../Components.h"
#include <SFML/Window/Keyboard.hpp>

void InputSystem::update(float deltaTime, ECSManager& ecs)
{
    // Get all entities with InputComponent
    auto entities = ecs.getEntitiesWithComponent<InputComponent>();

    for (Entity entity : entities)
    {
        auto input = ecs.getComponent<InputComponent>(entity);
        if (!input) continue;

        // Check keyboard state
        input->leftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left));
        input->rightPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
                              sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right));
    }
}

