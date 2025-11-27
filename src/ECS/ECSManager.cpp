#include "ECSManager.h"
#include "System.h"

Entity ECSManager::createEntity()
{
    return nextEntity++;
}

void ECSManager::destroyEntity(Entity entity)
{
    for (auto& compMap : components)
    {
        compMap.second.erase(entity);
    }
}

bool ECSManager::isValid(Entity entity) const
{
    return entity != INVALID_ENTITY && entity < nextEntity;
}

void ECSManager::addSystem(std::shared_ptr<System> system)
{
    systems.push_back(system);
}

void ECSManager::updateSystems(float deltaTime)
{
    for (auto& system : systems)
    {
        system->update(deltaTime, *this);
    }
}

