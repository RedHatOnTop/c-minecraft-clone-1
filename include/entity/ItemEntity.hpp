#pragma once

#include "entity/Entity.hpp"
#include "item/ItemStack.hpp"

class ItemEntity : public Entity {
public:
    ItemStack stack;
    float ageSeconds;

    ItemEntity(glm::vec3 pos, const ItemStack& stack)
        : Entity(pos, glm::vec3(0.25f, 0.25f, 0.25f)), stack(stack), ageSeconds(0.0f) {
        velocity = glm::vec3(0.0f, 2.0f, 0.0f);
    }

    void update(float deltaTime, World& world) override {
        ageSeconds += deltaTime;
        applyPhysics(deltaTime, world);
    }
};

