#include <algorithm>

#include "physics/Player.hpp"
#include "test_common.hpp"
#include "world/World.hpp"

static void buildFlatGround(World& world, int radius, int y) {
    world.loadChunk(0, 0);
    for (int x = -radius; x <= radius; ++x) {
        for (int z = -radius; z <= radius; ++z) {
            world.setBlock(x, y, z, Block(Blocks::STONE));
        }
    }
}

void test_Physics_JumpApex_FixedTickProfile() {
    World world;
    buildFlatGround(world, 8, 0);

    Player player(glm::vec3(0.5f, 1.0f, 0.5f));
    player.onGround = true;
    player.velocity = glm::vec3(0.0f, 8.0f, 0.0f);

    const float dt = 1.0f / 120.0f;
    float elapsed = 0.0f;
    float apex = player.position.y;
    bool leftGround = false;

    while (elapsed < 2.0f) {
        player.update(dt, world);
        elapsed += dt;
        apex = std::max(apex, player.position.y);
        if (!player.onGround) leftGround = true;
        if (leftGround && player.onGround) break;
    }

    const float apexGain = apex - 1.0f;
    TEST_ASSERT(leftGround);
    TEST_ASSERT(player.onGround);
    TEST_ASSERT(apexGain > 1.15f);
    TEST_ASSERT(apexGain < 1.55f);
    TEST_ASSERT(elapsed > 0.55f);
    TEST_ASSERT(elapsed < 0.95f);
}
