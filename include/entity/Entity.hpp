#pragma once

#include <algorithm>
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
        auto isSolid = [](BlockID id) {
            return id != Blocks::AIR && id != Blocks::WATER && id != Blocks::LAVA;
        };

        auto aabbAt = [this](const glm::vec3& pos) -> AABB {
            return AABB(pos - glm::vec3(dimensions.x / 2.0f, 0.0f, dimensions.z / 2.0f),
                        pos + glm::vec3(dimensions.x / 2.0f, dimensions.y, dimensions.z / 2.0f));
        };

        auto collides = [&](const AABB& box) -> bool {
            const float eps = 0.001f;
            int minX = (int)std::floor(box.min.x + eps);
            int minY = (int)std::floor(box.min.y + eps);
            int minZ = (int)std::floor(box.min.z + eps);
            int maxX = (int)std::floor(box.max.x - eps);
            int maxY = (int)std::floor(box.max.y - eps);
            int maxZ = (int)std::floor(box.max.z - eps);

            for (int x = minX; x <= maxX; ++x) {
                for (int y = minY; y <= maxY; ++y) {
                    for (int z = minZ; z <= maxZ; ++z) {
                        if (isSolid(world.getBlock(x, y, z).id)) return true;
                    }
                }
            }
            return false;
        };

        auto moveAxis = [&](int axis, float amount) {
            if (amount == 0.0f) return;
            int steps = std::max(1, (int)std::ceil(std::abs(amount) / 0.05f));
            float step = amount / (float)steps;
            for (int i = 0; i < steps; ++i) {
                glm::vec3 next = position;
                next[axis] += step;
                if (collides(aabbAt(next))) {
                    velocity[axis] = 0.0f;
                    break;
                }
                position = next;
            }
        };

        velocity.y -= 24.0f * deltaTime;
        onGround = false;

        glm::vec3 delta = velocity * deltaTime;
        moveAxis(0, delta.x);
        moveAxis(2, delta.z);

        float beforeY = position.y;
        moveAxis(1, delta.y);
        if (delta.y < 0.0f && position.y == beforeY) {
            onGround = true;
        }
    }
};
