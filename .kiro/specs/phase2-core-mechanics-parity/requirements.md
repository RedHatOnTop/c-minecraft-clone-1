# Requirements Document

## Introduction

Phase 2 of the MinecraftClone189 project targets Core Mechanics Parity Pass 1, raising the total parity score from 45.07 to at least 58 out of 100. This phase addresses the top-ranked Domain B (Core Mechanics) gaps identified in the locked gap backlog: movement and physics alignment (Subphase 2.1), block interaction, inventory, and crafting semantics (Subphase 2.2), and smelting lifecycle with end-to-end scenario validation (Subphase 2.3).

The current codebase has basic movement with hardcoded constants, instant block break with no tool modifiers, a hardcoded crafting shortcut (log-to-planks via C key), a hardcoded smelting shortcut (cobblestone-to-stone via Y key), and basic inventory slot operations without full transfer semantics. This phase replaces those stubs with parity-aligned systems backed by deterministic tests.

All nine Domain B parity matrix items (B-01 through B-09) are in scope. Six are Critical severity (28.0 gap points each) and three are Medium severity (16.8 gap points each).

## Glossary

- **Physics_Engine**: The movement and collision subsystem in `Entity::applyPhysics` and `Player::handleInput` responsible for velocity, gravity, friction, and AABB resolution.
- **Player**: The player entity defined in `Player.hpp`/`Player.cpp` that handles input, movement, camera, and inventory.
- **AABB_Resolver**: The axis-aligned bounding box collision detection and resolution logic in `Entity::applyPhysics` that prevents entities from passing through solid blocks.
- **Block_Interaction_System**: The subsystem in `InputController.cpp` (`handleBlockInteraction`) responsible for block breaking and placement.
- **Break_Duration_Calculator**: A component that computes block break time based on block hardness, tool type, and tool tier, matching Minecraft 1.8.9 formulas.
- **Placement_Validator**: A component that enforces block placement validity rules (support requirements, collision checks, adjacency rules).
- **Inventory_System**: The inventory slot management subsystem in `Inventory.hpp`/`Inventory.cpp` handling slot reads, writes, transfers, and stack merging.
- **Slot_Action_Processor**: A component that processes inventory slot interactions (left-click pick/place, right-click split, shift-click transfer) matching 1.8.9 semantics.
- **Recipe_Resolver**: A component that resolves crafting grid contents to output items using a recipe registry, supporting both 2x2 and 3x3 grids with shaped and shapeless recipes.
- **Recipe_Registry**: A data structure holding all crafting recipes with priority and conflict resolution rules.
- **Smelting_State_Machine**: A component that manages furnace smelting lifecycle: fuel consumption, input processing, progress tracking, and output production with tick-accurate state transitions.
- **Survival_Scenario_Runner**: The test harness that executes a 30-minute automated survival scenario covering the core gameplay loop (mine, craft, smelt, build).
- **Replay_Harness**: The deterministic replay test framework in `test_replay.cpp` that validates reproducible game state across multiple runs.
- **Parity_Matrix**: The scoring rubric in `docs/quality/parity-matrix-v1.md` defining measurable items and thresholds for each domain.
- **Tick**: A fixed-timestep simulation update (1/20th of a second in Minecraft 1.8.9, though the clone may use a different internal rate).

## Requirements

### Requirement 1: Walk and Sprint Velocity Profile (B-01, Subphase 2.1)

**User Story:** As a player, I want walking and sprinting speeds to match Minecraft 1.8.9 velocity profiles, so that movement feels authentic.

#### Acceptance Criteria

1. WHEN the Player moves forward without sprinting, THE Physics_Engine SHALL apply a base walking speed of 4.317 m/s (horizontal magnitude).
2. WHEN the Player activates sprint while moving forward, THE Physics_Engine SHALL apply a sprint speed of 5.612 m/s (horizontal magnitude).
3. THE Physics_Engine SHALL produce a speed curve deviation of 8% or less compared to the Minecraft 1.8.9 reference profile across a 5-second deterministic replay.
4. WHEN the Player moves diagonally (forward plus strafe), THE Physics_Engine SHALL normalize the horizontal velocity vector so the resulting speed does not exceed the active movement mode speed.
5. WHEN the Player stops providing movement input, THE Physics_Engine SHALL decelerate the Player using ground friction (see Requirement 3) rather than setting velocity to zero instantly.

### Requirement 2: Jump Apex and Fall Timing (B-02, Subphase 2.1)

**User Story:** As a player, I want jump height and fall timing to match Minecraft 1.8.9, so that platforming and vertical movement feel correct.

#### Acceptance Criteria

1. WHEN the Player initiates a jump from flat ground, THE Physics_Engine SHALL produce an apex height gain between 1.15 and 1.25 blocks above the launch position.
2. WHEN the Player initiates a jump from flat ground, THE Physics_Engine SHALL complete the full jump cycle (liftoff to landing) within 0.55 to 0.75 seconds.
3. THE Physics_Engine SHALL apply a gravity acceleration value that produces an apex height error of 8% or less compared to the Minecraft 1.8.9 reference value of 1.2522 blocks.
4. WHEN the Player is airborne and falling, THE Physics_Engine SHALL apply a terminal velocity cap consistent with Minecraft 1.8.9 (approximately 78.4 m/s downward).
5. FOR ALL jump sequences executed with identical initial conditions, THE Replay_Harness SHALL produce identical apex height and cycle duration values across 3 consecutive runs (deterministic replay).

### Requirement 3: Ground Friction Deceleration (B-03, Subphase 2.1)

**User Story:** As a player, I want ground friction to slow me down at the same rate as Minecraft 1.8.9, so that stopping distance and slide feel are correct.

#### Acceptance Criteria

1. WHEN the Player releases all movement input while on the ground, THE Physics_Engine SHALL apply a per-tick friction multiplier of 0.6 to horizontal velocity (matching Minecraft 1.8.9 ground slipperiness of 0.6).
2. THE Physics_Engine SHALL produce a deceleration profile error of 10% or less compared to the Minecraft 1.8.9 reference friction curve over a 2-second stop-distance replay.
3. WHEN the Player is airborne, THE Physics_Engine SHALL apply air drag of 0.91 per tick to horizontal velocity instead of ground friction.
4. WHEN the Player is on a slippery block (ice), THE Physics_Engine SHALL apply the block-specific slipperiness factor instead of the default 0.6 ground friction.

### Requirement 4: AABB Collision Resolution (B-04, Subphase 2.1)

**User Story:** As a player, I want collision detection to resolve correctly against all block geometries, so that I do not clip through blocks or get stuck.

#### Acceptance Criteria

1. THE AABB_Resolver SHALL resolve collisions axis-by-axis (Y, then X, then Z) to match Minecraft 1.8.9 resolution order.
2. WHEN the Player moves into a solid block, THE AABB_Resolver SHALL stop the Player at the block boundary with zero penetration on the colliding axis.
3. WHEN the Player stands on a block edge overlapping two adjacent blocks, THE AABB_Resolver SHALL maintain ground contact without falling through.
4. WHEN the Player walks into a 1-block-high gap while standing (1.8 blocks tall), THE AABB_Resolver SHALL prevent entry and not teleport or clip the Player.
5. THE AABB_Resolver SHALL produce zero critical collision failures across the collision scenario test pack.
6. WHEN the Player is pushed against a corner formed by two solid blocks, THE AABB_Resolver SHALL allow sliding along the unblocked axis.

### Requirement 5: Block Break Duration and Tool Modifiers (B-05, Subphase 2.2)

**User Story:** As a player, I want block breaking to take the correct amount of time based on block hardness and my held tool, so that mining progression matches Minecraft 1.8.9.

#### Acceptance Criteria

1. WHEN the Player begins breaking a block, THE Break_Duration_Calculator SHALL compute the break time using the formula: `breakTime = blockHardness * hardnessMultiplier / toolSpeedModifier`, matching Minecraft 1.8.9 break-time tables.
2. WHEN the Player holds the correct tool type for a block (pickaxe for stone, axe for wood), THE Break_Duration_Calculator SHALL apply the tool speed modifier for that tool tier.
3. WHEN the Player holds no tool or an incorrect tool type, THE Break_Duration_Calculator SHALL apply the base hand mining speed (1.0).
4. THE Block_Interaction_System SHALL display a progressive break animation during the break duration rather than breaking blocks instantly.
5. WHEN the Player interrupts block breaking (releases the button or looks away), THE Block_Interaction_System SHALL reset the break progress to zero.
6. THE Break_Duration_Calculator SHALL produce break-time values with 10% or less error compared to Minecraft 1.8.9 reference values for all parity-critical block and tool combinations.
7. IF the Player attempts to break a block flagged as unbreakable (bedrock), THEN THE Block_Interaction_System SHALL prevent the block from being destroyed.

### Requirement 6: Block Placement Validity Rules (B-06, Subphase 2.2)

**User Story:** As a player, I want block placement to enforce the same validity rules as Minecraft 1.8.9, so that I cannot place blocks in invalid positions.

#### Acceptance Criteria

1. WHEN the Player attempts to place a block, THE Placement_Validator SHALL verify that the target position is adjacent to an existing solid block face.
2. WHEN the Player attempts to place a block at a position that intersects the Player AABB, THE Placement_Validator SHALL reject the placement.
3. WHEN the Player attempts to place a block at a position occupied by another solid block, THE Placement_Validator SHALL reject the placement.
4. WHEN the Player attempts to place a block at a position that intersects any entity AABB, THE Placement_Validator SHALL reject the placement.
5. THE Placement_Validator SHALL produce zero parity-critical invalid placements across the placement constraint test matrix.
6. WHEN the Player places a directional block (furnace, chest), THE Placement_Validator SHALL orient the block based on the Player facing direction.

### Requirement 7: Inventory Slot Transfer Semantics (B-07, Subphase 2.2)

**User Story:** As a player, I want inventory interactions (click, shift-click, split) to behave like Minecraft 1.8.9, so that item management is intuitive and correct.

#### Acceptance Criteria

1. WHEN the Player left-clicks an inventory slot while holding no cursor stack, THE Slot_Action_Processor SHALL pick up the full stack from that slot onto the cursor.
2. WHEN the Player left-clicks an inventory slot while holding a cursor stack of the same item type, THE Slot_Action_Processor SHALL merge the cursor stack into the slot up to the maximum stack size.
3. WHEN the Player left-clicks an inventory slot while holding a cursor stack of a different item type, THE Slot_Action_Processor SHALL swap the cursor stack with the slot stack.
4. WHEN the Player right-clicks an inventory slot while holding a cursor stack, THE Slot_Action_Processor SHALL place exactly one item from the cursor stack into the slot.
5. WHEN the Player right-clicks a non-empty inventory slot while holding no cursor stack, THE Slot_Action_Processor SHALL pick up half the stack (rounded up) onto the cursor.
6. WHEN the Player shift-clicks an inventory slot, THE Slot_Action_Processor SHALL transfer the stack to the first available slot in the opposite inventory region (main inventory to hotbar, or hotbar to main inventory).
7. THE Slot_Action_Processor SHALL produce a behavior mismatch rate of 5% or less compared to Minecraft 1.8.9 across the slot action replay suite.
8. FOR ALL slot action sequences, THE Replay_Harness SHALL produce identical inventory state across 3 consecutive runs (deterministic replay).

### Requirement 8: Crafting Recipe Resolution (B-08, Subphase 2.2)

**User Story:** As a player, I want crafting to resolve recipes from grid contents using a proper recipe system, so that I can craft all parity-critical items correctly.

#### Acceptance Criteria

1. WHEN the Player places items in a 2x2 crafting grid matching a registered shaped recipe pattern, THE Recipe_Resolver SHALL produce the correct output item and quantity.
2. WHEN the Player places items in a 3x3 crafting grid (crafting table) matching a registered shaped recipe pattern, THE Recipe_Resolver SHALL produce the correct output item and quantity.
3. WHEN the Player places items matching a registered shapeless recipe in any arrangement, THE Recipe_Resolver SHALL produce the correct output item and quantity.
4. WHEN multiple recipes match the current grid contents, THE Recipe_Resolver SHALL select the recipe with the highest priority (most specific pattern first).
5. WHEN no registered recipe matches the current grid contents, THE Recipe_Resolver SHALL produce no output.
6. WHEN the Player takes the crafting output, THE Recipe_Resolver SHALL consume exactly the required input items from the grid slots.
7. THE Recipe_Resolver SHALL produce zero wrong outputs for all parity-critical recipes in the recipe conflict test suite.
8. THE Recipe_Registry SHALL support at minimum the following parity-critical recipes: planks from logs, sticks from planks, crafting table from planks, wooden pickaxe, stone pickaxe, wooden sword, furnace from cobblestone, chest from planks, torches from sticks and coal.
9. FOR ALL valid recipe inputs, resolving the recipe and then verifying the output against the registry entry SHALL produce a consistent result (round-trip property).

### Requirement 9: Smelting State Transitions (B-09, Subphase 2.3)

**User Story:** As a player, I want furnace smelting to follow the correct state machine with fuel consumption and progress tracking, so that smelting behaves like Minecraft 1.8.9.

#### Acceptance Criteria

1. WHEN a valid smeltable item is placed in the input slot and fuel is placed in the fuel slot, THE Smelting_State_Machine SHALL transition from Idle to Active and begin consuming fuel.
2. WHILE the Smelting_State_Machine is in Active state, THE Smelting_State_Machine SHALL decrement the fuel burn timer each tick and increment the smelt progress timer each tick.
3. WHEN the smelt progress timer reaches the required smelt duration (200 ticks for standard items), THE Smelting_State_Machine SHALL consume one input item, produce one output item, and reset the smelt progress timer.
4. WHEN the fuel burn timer reaches zero and smeltable input remains, THE Smelting_State_Machine SHALL consume the next fuel item and reset the fuel burn timer to the fuel item burn duration.
5. WHEN the fuel burn timer reaches zero and no fuel remains, THE Smelting_State_Machine SHALL transition to Idle and reset the smelt progress timer.
6. WHEN the output slot is full (64 items), THE Smelting_State_Machine SHALL pause smelt progress until output space is available.
7. IF the input slot becomes empty while fuel is still burning, THEN THE Smelting_State_Machine SHALL continue the fuel burn timer but pause smelt progress.
8. THE Smelting_State_Machine SHALL produce zero invalid state transitions across the smelting state transition test suite.
9. THE Smelting_State_Machine SHALL support at minimum the following parity-critical smelting recipes: cobblestone to stone, iron ore to iron ingot, sand to glass, raw food items to cooked food items.

### Requirement 10: Movement Deterministic Replay (Subphase 2.1, Cross-cutting)

**User Story:** As a developer, I want movement physics to produce deterministic results across runs, so that physics tuning regressions are caught automatically.

#### Acceptance Criteria

1. FOR ALL movement replay scenarios (walk, sprint, jump, fall, friction stop), THE Replay_Harness SHALL produce identical position traces across 3 consecutive runs.
2. THE Replay_Harness SHALL detect physics replay variance exceeding the configured tolerance (position delta > 0.001 blocks between runs) and report it as a test failure.
3. WHEN movement constants are changed, THE Replay_Harness SHALL detect the trace divergence and require updated reference traces.

### Requirement 11: Survival Scenario Validation (Subphase 2.3, Cross-cutting)

**User Story:** As a developer, I want an automated 30-minute survival scenario that exercises the core gameplay loop, so that end-to-end stability is validated.

#### Acceptance Criteria

1. THE Survival_Scenario_Runner SHALL execute a scripted 30-minute scenario covering: block breaking, block placement, crafting (2x2 and 3x3), smelting, and inventory management.
2. THE Survival_Scenario_Runner SHALL complete 5 consecutive runs with zero crash-class defects.
3. WHEN a crash-class defect occurs during a survival scenario run, THE Survival_Scenario_Runner SHALL log the defect context (tick number, player state, last action) and mark the run as failed.
4. THE Survival_Scenario_Runner SHALL verify that the final parity score reaches at least 58 after all Phase 2 changes are integrated.

### Requirement 12: Contribution Gate Compliance (Cross-cutting)

**User Story:** As a developer, I want all Phase 2 changes to pass the contribution gate policy, so that existing functionality is not regressed.

#### Acceptance Criteria

1. THE Physics_Engine, Block_Interaction_System, Inventory_System, Recipe_Resolver, and Smelting_State_Machine SHALL configure and build in Release mode without errors.
2. THE Physics_Engine, Block_Interaction_System, Inventory_System, Recipe_Resolver, and Smelting_State_Machine SHALL pass all four required CTest targets (tests_data, tests_runtime, tests_render, tests_gameplay_smoke) at 100%.
3. WHEN a Phase 2 change modifies movement or physics code, THE change SHALL include updated deterministic replay tests under tests_gameplay_smoke.
4. WHEN a Phase 2 change modifies inventory, crafting, or smelting code, THE change SHALL include slot-action or recipe regression tests under tests_gameplay_smoke.
