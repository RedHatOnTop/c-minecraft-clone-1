#include "mechanics/PlacementValidator.hpp"

// Stub — implementation in task 6.1
PlacementResult PlacementValidator::validate(
    const glm::ivec3& /*placePos*/,
    const glm::ivec3& /*againstPos*/,
    BlockID /*blockToPlace*/,
    const World& /*world*/,
    const AABB& /*playerAABB*/,
    const std::vector<AABB>& /*entityAABBs*/
) {
    return {true, ""};
}

uint8_t PlacementValidator::computeDirectionalMeta(BlockID /*blockId*/, float /*playerYaw*/) {
    return 0;
}
