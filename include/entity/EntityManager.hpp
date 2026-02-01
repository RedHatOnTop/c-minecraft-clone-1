#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include "entity/Entity.hpp"

class EntityManager {
public:
    void addEntity(std::shared_ptr<Entity> entity) {
        m_entities.push_back(entity);
    }

    void update(float deltaTime, World& world) {
        for (auto& entity : m_entities) {
            entity->update(deltaTime, world);
        }
        m_entities.erase(
            std::remove_if(m_entities.begin(), m_entities.end(),
                [](const std::shared_ptr<Entity>& e) { return e->removed; }),
            m_entities.end());
    }

    const std::vector<std::shared_ptr<Entity>>& getEntities() const {
        return m_entities;
    }

    std::vector<std::shared_ptr<Entity>>& getEntitiesMutable() {
        return m_entities;
    }

    void clear() {
        m_entities.clear();
    }

private:
    std::vector<std::shared_ptr<Entity>> m_entities;
};
