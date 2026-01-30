#include "physics/Raycast.hpp"
#include <cmath>

RaycastResult Raycaster::raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, World& world) {
    RaycastResult result;
    
    glm::vec3 dir = glm::normalize(direction);
    glm::ivec3 currentBlock = glm::floor(origin);
    
    glm::vec3 step = glm::sign(dir);
    glm::vec3 deltaDist = glm::abs(1.0f / dir);
    
    glm::vec3 sideDist;
    sideDist.x = (dir.x > 0) ? (currentBlock.x + 1.0f - origin.x) * deltaDist.x : (origin.x - currentBlock.x) * deltaDist.x;
    sideDist.y = (dir.y > 0) ? (currentBlock.y + 1.0f - origin.y) * deltaDist.y : (origin.y - currentBlock.y) * deltaDist.y;
    sideDist.z = (dir.z > 0) ? (currentBlock.z + 1.0f - origin.z) * deltaDist.z : (origin.z - currentBlock.z) * deltaDist.z;

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
