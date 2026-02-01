#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include "physics/AABB.hpp"
#include "world/World.hpp"

class Entity {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 dimensions;
    bool onGround;
    float health;
    bool removed;

    Entity(glm::vec3 pos, glm::vec3 dim) 
        : position(pos), velocity(0.0f), dimensions(dim), onGround(false), health(20.0f), removed(false) {}
    
    virtual ~Entity() = default;

    virtual void update(float deltaTime, World& world) = 0;

    void markRemoved() { removed = true; }
    
    AABB getAABB() const {
        return AABB(position - glm::vec3(dimensions.x/2, 0, dimensions.z/2), 
                    position + glm::vec3(dimensions.x/2, dimensions.y, dimensions.z/2));
    }

protected:
    void applyPhysics(float deltaTime, World& world) {
        velocity.y -= 24.0f * deltaTime; // Gravity
        
        glm::vec3 nextPos = position + velocity * deltaTime;
        
        // Simplified collision (should be improved for entities)
        if (world.getBlock((int)std::floor(nextPos.x), (int)std::floor(nextPos.y), (int)std::floor(nextPos.z)).id != Blocks::AIR) {
            velocity.y = 0;
            onGround = true;
        } else {
            onGround = false;
            position = nextPos;
        }
    }
};
