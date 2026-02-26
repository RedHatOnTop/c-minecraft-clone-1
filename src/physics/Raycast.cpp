#include "physics/Raycast.hpp"
#include <cmath>

RaycastResult Raycaster::raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, World& world) {
    RaycastResult result;

    const float eps = 1e-6f;
    float len = glm::length(direction);
    if (len <= eps) return result;

    glm::vec3 dir = direction / len;
    glm::ivec3 currentBlock = glm::floor(origin);
    
    glm::vec3 step = glm::sign(dir);
    glm::vec3 deltaDist(
        std::abs(dir.x) > eps ? std::abs(1.0f / dir.x) : 1e30f,
        std::abs(dir.y) > eps ? std::abs(1.0f / dir.y) : 1e30f,
        std::abs(dir.z) > eps ? std::abs(1.0f / dir.z) : 1e30f
    );
    
    glm::vec3 sideDist;
    sideDist.x = (dir.x > eps) ? (currentBlock.x + 1.0f - origin.x) * deltaDist.x
                               : (dir.x < -eps ? (origin.x - currentBlock.x) * deltaDist.x : 1e30f);
    sideDist.y = (dir.y > eps) ? (currentBlock.y + 1.0f - origin.y) * deltaDist.y
                               : (dir.y < -eps ? (origin.y - currentBlock.y) * deltaDist.y : 1e30f);
    sideDist.z = (dir.z > eps) ? (currentBlock.z + 1.0f - origin.z) * deltaDist.z
                               : (dir.z < -eps ? (origin.z - currentBlock.z) * deltaDist.z : 1e30f);

    float dist = 0;
    glm::ivec3 normal(0);

    while (dist < maxDistance) {
        Block b = world.getBlock(currentBlock.x, currentBlock.y, currentBlock.z);
        if (b.id != Blocks::AIR) {
            result.hit = true;
            result.blockPos = currentBlock;
            result.faceNormal = normal;
            result.distance = dist;
            return result;
        }

        if (sideDist.x < sideDist.y) {
            if (sideDist.x < sideDist.z) {
                dist = sideDist.x;
                sideDist.x += deltaDist.x;
                currentBlock.x += step.x;
                normal = glm::ivec3(-step.x, 0, 0);
            } else {
                dist = sideDist.z;
                sideDist.z += deltaDist.z;
                currentBlock.z += step.z;
                normal = glm::ivec3(0, 0, -step.z);
            }
        } else {
            if (sideDist.y < sideDist.z) {
                dist = sideDist.y;
                sideDist.y += deltaDist.y;
                currentBlock.y += step.y;
                normal = glm::ivec3(0, -step.y, 0);
            } else {
                dist = sideDist.z;
                sideDist.z += deltaDist.z;
                currentBlock.z += step.z;
                normal = glm::ivec3(0, 0, -step.z);
            }
        }
    }

    return result;
}
