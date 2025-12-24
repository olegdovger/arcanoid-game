#pragma once

#include "Entity.h"
#include "Component.h"
#include "System.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>

class ECSManager
{
public:
    
    Entity createEntity();
    void destroyEntity(Entity entity);
    bool isValid(Entity entity) const;

    
    template<typename T>
    void addComponent(Entity entity, std::shared_ptr<T> component)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
        components[std::type_index(typeid(T))][entity] = component;
    }

    template<typename T>
    void removeComponent(Entity entity)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
        components[std::type_index(typeid(T))].erase(entity);
    }

    template<typename T>
    std::shared_ptr<T> getComponent(Entity entity)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
        auto it = components[std::type_index(typeid(T))].find(entity);
        if (it != components[std::type_index(typeid(T))].end())
        {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template<typename T>
    std::shared_ptr<const T> getComponent(Entity entity) const
    {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end())
        {
            auto entityIt = it->second.find(entity);
            if (entityIt != it->second.end())
            {
                return std::static_pointer_cast<const T>(entityIt->second);
            }
        }
        return nullptr;
    }

    template<typename T>
    bool hasComponent(Entity entity)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
        return components[std::type_index(typeid(T))].find(entity) != 
               components[std::type_index(typeid(T))].end();
    }

    template<typename T>
    std::vector<Entity> getEntitiesWithComponent()
    {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
        std::vector<Entity> entities;
        auto& compMap = components[std::type_index(typeid(T))];
        for (const auto& pair : compMap)
        {
            entities.push_back(pair.first);
        }
        return entities;
    }

    
    void addSystem(std::shared_ptr<System> system);
    void updateSystems(float deltaTime);

private:
    std::unordered_map<std::type_index, std::unordered_map<Entity, std::shared_ptr<Component>>> components;
    std::vector<std::shared_ptr<System>> systems;
    Entity nextEntity = 1;
};

