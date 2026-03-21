#pragma once

#include "world/Block.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>

// Forward declarations
class World;
struct AABB;

struct PlacementResult {
    bool valid;
    std::string reason;
};

class PlacementValidator {
public:
    static PlacementResult validate(
        const glm::ivec3& placePos,
        const glm::ivec3& againstPos,
        BlockID blockToPlace,
        const World& world,
        const AABB& playerAABB,
        const std::vector<AABB>& entityAABBs
    );

    static uint8_t computeDirectionalMeta(BlockID blockId, float playerYaw);
};
