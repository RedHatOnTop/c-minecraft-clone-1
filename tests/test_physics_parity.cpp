#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include <rapidcheck.h>

#include "physics/PhysicsConstants.hpp"
#include "physics/Player.hpp"
#include "test_common.hpp"
#include "world/World.hpp"

// ============================================================================
// Helpers
// ============================================================================

static void buildFlatGround(World& world, int radius, int groundY) {
    world.loadChunk(0, 0);
    if (radius > 8) {
        world.loadChunk(-1, 0);
        world.loadChunk(0, -1);
        world.loadChunk(-1, -1);
    }
    for (int x = -radius; x <= radius; ++x)
        for (int z = -radius; z <= radius; ++z)
            world.setBlock(x, groundY, z, Block(Blocks::STONE));
}

// Simulate one physics tick: apply acceleration (mimicking handleInput), then
// call player.update which runs applyPhysics.
static void simulateTick(Player& player, World& world,
                         const glm::vec3& inputDir, bool sprint, bool jump) {
    float modeSpeed = sprint
        ? (PhysicsConstants::SPRINT_SPEED / PhysicsConstants::TICK_RATE)
        : (PhysicsConstants::WALK_SPEED / PhysicsConstants::TICK_RATE);

    glm::vec3 dir = inputDir;
    dir.y = 0.0f;
    if (glm::length(dir) > 0.0f) {
        dir = glm::normalize(dir);
        float accel = player.onGround ? PhysicsConstants::GROUND_ACCEL
                                      : PhysicsConstants::AIR_ACCEL;
        player.velocity.x += dir.x * accel * modeSpeed;
        player.velocity.z += dir.z * accel * modeSpeed;
    }

    if (jump && player.onGround) {
        player.velocity.y = PhysicsConstants::JUMP_VELOCITY;
    }

    // One tick = TICK_DURATION seconds
    player.update(PhysicsConstants::TICK_DURATION, world);
}

static float horizontalSpeed(const glm::vec3& v) {
    return std::sqrt(v.x * v.x + v.z * v.z);
}

// Convert blocks/tick to m/s
static float tickSpeedToMs(float blocksPerTick) {
    return blocksPerTick * PhysicsConstants::TICK_RATE;
}

// ============================================================================
// Property 1: Movement mode speed convergence
// Feature: phase2-core-mechanics-parity, Property 1: Movement mode speed convergence
// Validates: Requirements 1.1, 1.2
// ============================================================================

void test_PhysicsParity_WalkSpeedConvergence() {
    rc::check("Walk speed converges to 4.317 m/s within 8%", []() {
        World world;
        buildFlatGround(world, 10, 0);

        Player player(glm::vec3(0.5f, 1.0f, 0.5f));
        player.onGround = true;

        glm::vec3 forward(0.0f, 0.0f, 1.0f);

        // Run 200 ticks (10 seconds) to reach steady state
        for (int t = 0; t < 200; ++t)
            simulateTick(player, world, forward, false, false);

        float speedMs = tickSpeedToMs(horizontalSpeed(player.velocity));
        float deviation = std::abs(speedMs - PhysicsConstants::WALK_SPEED) / PhysicsConstants::WALK_SPEED;
        RC_ASSERT(deviation <= 0.08f);
    });
}

void test_PhysicsParity_SprintSpeedConvergence() {
    rc::check("Sprint speed converges to 5.612 m/s within 8%", []() {
        World world;
        buildFlatGround(world, 10, 0);

        Player player(glm::vec3(0.5f, 1.0f, 0.5f));
        player.onGround = true;

        glm::vec3 forward(0.0f, 0.0f, 1.0f);

        for (int t = 0; t < 200; ++t)
            simulateTick(player, world, forward, true, false);

        float speedMs = tickSpeedToMs(horizontalSpeed(player.velocity));
        float deviation = std::abs(speedMs - PhysicsConstants::SPRINT_SPEED) / PhysicsConstants::SPRINT_SPEED;
        RC_ASSERT(deviation <= 0.08f);
    });
}
