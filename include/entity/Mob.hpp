#pragma once

#include "entity/Entity.hpp"
#include <string>

class Mob : public Entity {
public:
    std::string type;
    float health;

    Mob(glm::vec3 pos, const std::string& type) 
        : Entity(pos, glm::vec3(0.6f, 1.8f, 0.6f)), type(type), health(20.0f) {}

    void update(float deltaTime, World& world) override {
        applyPhysics(deltaTime, world);
        
        // Simple AI: Wander around
        static float wanderTimer = 0;
        wanderTimer += deltaTime;
        if (wanderTimer > 2.0f) {
            velocity.x = (float)(rand() % 100 - 50) / 50.0f * 2.0f;
            velocity.z = (float)(rand() % 100 - 50) / 50.0f * 2.0f;
            wanderTimer = 0;
        }
    }
};
