#pragma once

#include <glm/glm.hpp>
#include "world/World.hpp"

struct RaycastResult {
    bool hit;
    glm::ivec3 blockPos;
    glm::ivec3 faceNormal;
    float distance;

    RaycastResult() : hit(false), blockPos(0), faceNormal(0), distance(0) {}
};

class Raycaster {
public:
    static RaycastResult raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, World& world);
};
