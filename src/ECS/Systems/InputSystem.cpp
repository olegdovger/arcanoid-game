#include "InputSystem.h"
#include "../ECSManager.h"
#include "../Components.h"
#include <SFML/Window/Keyboard.hpp>

void InputSystem::update(float deltaTime, ECSManager& ecs)
{
    
    auto entities = ecs.getEntitiesWithComponent<InputComponent>();

    for (Entity entity : entities)
    {
        auto input = ecs.getComponent<InputComponent>(entity);
        if (!input) continue;

        
        input->leftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left));
        input->rightPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
                              sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right));
    }
}

