#pragma once

// Minecraft 1.8.9 physics constants (per-tick values unless noted)
namespace PhysicsConstants {
    constexpr float WALK_SPEED       = 4.317f;   // m/s horizontal steady-state
    constexpr float SPRINT_SPEED     = 5.612f;   // m/s horizontal steady-state
    constexpr float JUMP_VELOCITY    = 0.42f;    // blocks/tick initial upward velocity
    constexpr float GRAVITY          = 0.08f;    // blocks/tick² downward acceleration
    constexpr float TERMINAL_VEL     = 3.92f;    // blocks/tick max downward speed
    constexpr float GROUND_FRICTION  = 0.6f;     // ground slipperiness factor
    constexpr float AIR_DRAG         = 0.91f;    // horizontal drag while airborne
    constexpr float GROUND_ACCEL     = 0.1f;     // acceleration factor on ground
    constexpr float AIR_ACCEL        = 0.02f;    // acceleration factor in air
    constexpr float ICE_SLIPPERINESS = 0.98f;    // ice block slipperiness
    constexpr float TICK_RATE        = 20.0f;    // ticks per second
    constexpr float TICK_DURATION    = 1.0f / TICK_RATE; // seconds per tick (0.05)
    constexpr int   MAX_TICKS_PER_FRAME = 10;    // cap to prevent physics explosion
}
