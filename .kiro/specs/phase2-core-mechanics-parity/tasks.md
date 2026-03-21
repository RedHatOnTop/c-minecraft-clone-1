# Implementation Plan: Phase 2 Core Mechanics Parity

## Overview

Implement six parity-aligned subsystems (physics tuning, break duration, placement validation, slot actions, crafting recipes, smelting state machine) to raise the parity score from 45.07 to ≥58. Work is organized into three subphases matching the design: Movement & Physics (2.1), Block Interaction/Inventory/Crafting (2.2), and Smelting & Validation (2.3). Each subphase ends with a checkpoint. A final integration pass wires everything together and runs the survival scenario.

## Tasks

- [x] 1. CMake and dependency setup
  - [x] 1.1 Add rapidcheck as FetchContent dependency in `cmake/Dependencies.cmake`
    - Add rapidcheck FetchContent_Declare and FetchContent_MakeAvailable
    - Configure offline fetch if needed (consistent with existing dependency pattern)
    - _Requirements: 12.1_

  - [x] 1.2 Register new source files and test targets in `CMakeLists.txt`
    - Add `src/mechanics/BreakDurationCalculator.cpp`, `src/mechanics/PlacementValidator.cpp`, `src/mechanics/SlotActionProcessor.cpp`, `src/mechanics/RecipeRegistry.cpp`, `src/mechanics/SmeltingRecipeRegistry.cpp` to `TEST_SHARED_SOURCES`
    - Add new test files to appropriate CTest targets: `tests/test_break_duration.cpp`, `tests/test_placement.cpp`, `tests/test_slot_action.cpp`, `tests/test_recipe.cpp`, `tests/test_smelting.cpp` to `tests_runtime`; `tests/test_physics_parity.cpp`, `tests/test_survival_scenario.cpp` to `tests_gameplay_smoke`
    - Link rapidcheck to test targets that use property-based tests
    - _Requirements: 12.1, 12.2_

- [x] 2. Checkpoint - Verify build configuration
  - Ensure the project configures and builds in Release mode with the new dependencies and empty source stubs. Ask the user if questions arise.

- [x] 3. Subphase 2.1 – Physics constants and engine tuning
  - [x] 3.1 Create `include/physics/PhysicsConstants.hpp` with MC 1.8.9 constants
    - Define WALK_SPEED (4.317), SPRINT_SPEED (5.612), JUMP_VELOCITY (0.42), GRAVITY (0.08), TERMINAL_VEL (3.92), GROUND_FRICTION (0.6), AIR_DRAG (0.91), GROUND_ACCEL (0.1), AIR_ACCEL (0.02), ICE_SLIPPERINESS (0.98), TICK_RATE (20.0)
    - _Requirements: 1.1, 1.2, 2.1, 2.3, 2.4, 3.1, 3.3, 3.4_

  - [x] 3.2 Implement fixed-tick accumulator in game loop
    - Modify `src/main.cpp` game loop to accumulate real time and step physics in fixed 1/20s increments
    - Cap tick accumulator at 10 ticks to prevent physics explosion on long frame stalls
    - _Requirements: 1.1, 2.2, 10.1_

  - [x] 3.3 Tune `Entity::applyPhysics` with MC 1.8.9 friction model and gravity
    - Replace hardcoded gravity (24.0) with per-tick GRAVITY (0.08 blocks/tick²)
    - Implement per-tick friction: `velocity.xz *= slipperiness * 0.91` on ground, `velocity.xz *= 0.91` in air
    - Apply terminal velocity cap (3.92 blocks/tick downward)
    - Handle NaN/Inf velocity by clamping to zero with warning
    - _Requirements: 1.5, 2.3, 2.4, 3.1, 3.3, 3.4_

  - [x] 3.4 Change AABB collision resolution order to Y, X, Z
    - Reorder axis resolution in `Entity::applyPhysics` from current X,Z,Y to Y,X,Z matching MC 1.8.9
    - _Requirements: 4.1, 4.2, 4.3, 4.5_

  - [x] 3.5 Tune `Player::handleInput` with acceleration-based movement
    - Replace direct velocity assignment with acceleration model: apply GROUND_ACCEL or AIR_ACCEL scaled by mode speed
    - Implement diagonal normalization: normalize input direction vector when both forward and strafe are active
    - Set jump velocity to JUMP_VELOCITY (0.42 blocks/tick) replacing hardcoded 8.0f
    - Support ice slipperiness factor from block-specific lookup
    - _Requirements: 1.1, 1.2, 1.4, 1.5, 2.1, 3.4, 4.6_

  - [-] 3.6 Write physics property tests in `tests/test_physics_parity.cpp`
    - **Property 1: Movement mode speed convergence** — verify walk (4.317 m/s) and sprint (5.612 m/s) steady-state within 8%
    - **Validates: Requirements 1.1, 1.2**
    - **Property 2: Diagonal speed cap** — verify horizontal magnitude ≤ mode target for all forward+strafe combos
    - **Validates: Requirements 1.4**
    - **Property 3: Per-tick friction factor** — verify velocity *= surface friction factor per tick, not zeroed
    - **Validates: Requirements 1.5, 3.1, 3.3, 3.4**
    - **Property 4: Jump profile bounds** — verify apex in [1.15, 1.25] blocks, cycle in [0.55, 0.75]s
    - **Validates: Requirements 2.1, 2.2**
    - **Property 5: Terminal velocity cap** — verify downward velocity ≤ 3.92 blocks/tick
    - **Validates: Requirements 2.4**
    - **Property 6: Collision no-penetration invariant** — verify player AABB never overlaps solid block after resolution
    - **Validates: Requirements 4.2**
    - **Property 7: Corner sliding** — verify player slides along free axis at corners
    - **Validates: Requirements 4.6**
    - **Property 23: Movement replay determinism** — verify identical position traces across 3 runs
    - **Validates: Requirements 2.5, 10.1**
    - Include unit tests for: speed curve deviation ≤ 8% (1.3), deceleration profile error ≤ 10% (3.2), block edge collision (4.3), 1-block gap rejection (4.4), AABB Y,X,Z order scenario (4.1)

- [ ] 4. Checkpoint - Subphase 2.1 complete
  - Ensure all physics tests pass. Verify walk/sprint speeds, jump profile, friction, and collision resolution match MC 1.8.9 within tolerance. Ask the user if questions arise.

- [ ] 5. Subphase 2.2 – Block interaction
  - [ ] 5.1 Implement `BreakDurationCalculator` in `include/mechanics/BreakDurationCalculator.hpp` and `src/mechanics/BreakDurationCalculator.cpp`
    - Define BlockHardness struct, ToolType/ToolTier enums, ToolSpeed struct
    - Implement static hardness table (stone=1.5, dirt=0.5, cobblestone=2.0, planks=2.0, bedrock=-1.0, etc.)
    - Implement static tool speed table (hand=1.0, wood=2.0, stone=4.0, iron=6.0, diamond=8.0)
    - Implement `calculate()` using MC 1.8.9 formula: `damage = speedMul / hardness / (requiresTool && !correctTool ? 100 : 30)`, `ticks = ceil(1/damage)`, `breakTime = ticks/20`
    - Return -1.0f for unbreakable blocks (hardness < 0)
    - Default hardness 1.0 for unknown BlockIDs with warning
    - _Requirements: 5.1, 5.2, 5.3, 5.6, 5.7_

  - [ ]* 5.2 Write break duration property tests in `tests/test_break_duration.cpp`
    - **Property 8: Break duration formula correctness** — verify computed duration matches MC 1.8.9 formula for all block/tool combos
    - **Validates: Requirements 5.1, 5.2, 5.3, 5.7**
    - Include unit tests for: stone + iron pickaxe = 0.75s, dirt + hand = 0.75s, bedrock unbreakable, break-time error ≤ 10% for parity-critical combos (5.6)

  - [ ] 5.3 Integrate break duration into `InputController::handleBlockInteraction`
    - Replace instant-break logic with progressive breaking: track `breakingBlockPos`, `breakProgress`, compute duration via `BreakDurationCalculator::calculate()`
    - Increment progress each frame; break block when progress ≥ 1.0
    - Reset progress to zero on button release or target block change
    - Prevent breaking unbreakable blocks (bedrock)
    - Display progressive break animation during break duration
    - _Requirements: 5.1, 5.4, 5.5, 5.7_

  - [ ]* 5.4 Write break progress reset test
    - **Property 9: Break progress reset on interruption** — verify progress resets to zero on release or target change
    - **Validates: Requirements 5.5**

- [ ] 6. Subphase 2.2 – Block placement
  - [ ] 6.1 Implement `PlacementValidator` in `include/mechanics/PlacementValidator.hpp` and `src/mechanics/PlacementValidator.cpp`
    - Implement PlacementResult struct and validation rules in order: air check, adjacency check, player AABB intersection, entity AABB intersection
    - Implement `computeDirectionalMeta()` for furnace/chest facing based on player yaw
    - Handle unloaded chunk and invalid block type edge cases
    - _Requirements: 6.1, 6.2, 6.3, 6.4, 6.6_

  - [ ]* 6.2 Write placement property tests in `tests/test_placement.cpp`
    - **Property 10: Placement validity** — verify placement succeeds iff all four conditions met (air, adjacent, no player overlap, no entity overlap)
    - **Validates: Requirements 6.1, 6.2, 6.3, 6.4**
    - **Property 11: Directional block orientation** — verify yaw quadrant maps to correct cardinal direction
    - **Validates: Requirements 6.6**
    - Include unit tests for: placement constraint test matrix (6.5)

  - [ ] 6.3 Integrate placement validator into `InputController::handleBlockInteraction`
    - Replace direct block placement with `PlacementValidator::validate()` call on right-click
    - Apply directional metadata for furnace/chest placement
    - _Requirements: 6.1, 6.2, 6.3, 6.4, 6.5, 6.6_

- [ ] 7. Subphase 2.2 – Inventory slot actions
  - [ ] 7.1 Implement `SlotActionProcessor` in `include/mechanics/SlotActionProcessor.hpp` and `src/mechanics/SlotActionProcessor.cpp`
    - Define ClickAction enum, SlotActionResult struct
    - Implement `processClick()` with left-click semantics (pick up, merge, swap), right-click semantics (place 1, pick half), and swap on type mismatch
    - Implement `processShiftClick()` with hotbar↔main inventory transfer (merge first, then empty slots)
    - Handle out-of-range slot index, stack overflow, null item registry edge cases
    - _Requirements: 7.1, 7.2, 7.3, 7.4, 7.5, 7.6_

  - [ ] 7.2 Add cursor stack to `GameContext` or new `InventoryUIState`
    - Add `ItemStack cursorStack` field to track item held on cursor during inventory interaction
    - _Requirements: 7.1_

  - [ ]* 7.3 Write slot action property tests in `tests/test_slot_action.cpp`
    - **Property 12: Left-click item conservation** — verify total item count preserved across cursor+slot for all left-click scenarios
    - **Validates: Requirements 7.1, 7.2, 7.3**
    - **Property 13: Right-click semantics** — verify exactly 1 item placed or ceil(count/2) picked up, total conserved
    - **Validates: Requirements 7.4, 7.5**
    - **Property 14: Shift-click transfer** — verify stack transfers to opposite region, total conserved
    - **Validates: Requirements 7.6**
    - **Property 24: Inventory action replay determinism** — verify identical final state across 3 replays
    - **Validates: Requirements 7.8**
    - Include unit tests for: slot action replay suite mismatch rate ≤ 5% (7.7)

  - [ ] 7.4 Integrate slot actions into inventory UI click handlers
    - Replace direct `setStack`/`removeStack` calls with `SlotActionProcessor::processClick()` and `processShiftClick()`
    - Wire cursor stack through `GameContext`
    - _Requirements: 7.1, 7.2, 7.3, 7.4, 7.5, 7.6, 7.7_

- [ ] 8. Subphase 2.2 – Crafting recipe system
  - [ ] 8.1 Implement `RecipeRegistry` in `include/mechanics/RecipeRegistry.hpp` and `src/mechanics/RecipeRegistry.cpp`
    - Define RecipeSlot, ShapedRecipe, ShapelessRecipe, RecipeMatch structs
    - Implement `registerShaped()`, `registerShapeless()`, `resolve()`, `consumeInputs()`
    - Resolution algorithm: try shaped (with grid offset sliding), then shapeless (order-independent), select highest priority, ties broken by shaped > shapeless then first-registered
    - Implement `loadDefaultRecipes()` with all 9 parity-critical recipes: planks from logs, sticks from planks, crafting table, wooden pickaxe, stone pickaxe, wooden sword, furnace, chest, torches
    - Handle empty grid fast path, corrupted recipe data skip
    - _Requirements: 8.1, 8.2, 8.3, 8.4, 8.5, 8.6, 8.8_

  - [ ]* 8.2 Write recipe property tests in `tests/test_recipe.cpp`
    - **Property 15: Recipe resolution round-trip** — verify resolve produces registered output, re-resolve is idempotent
    - **Validates: Requirements 8.1, 8.2, 8.9**
    - **Property 16: Shapeless recipe order independence** — verify all permutations of shapeless ingredients produce same output
    - **Validates: Requirements 8.3**
    - **Property 17: Recipe priority resolution** — verify highest priority recipe selected on multi-match
    - **Validates: Requirements 8.4**
    - **Property 18: No-match produces no output** — verify non-matching grids return empty result
    - **Validates: Requirements 8.5**
    - **Property 19: Crafting input consumption conservation** — verify consumed inputs exactly equal recipe requirements
    - **Validates: Requirements 8.6**
    - Include unit tests for: all 9 parity-critical recipes resolve correctly (8.8), recipe conflict test suite zero wrong outputs (8.7)

  - [ ] 8.3 Integrate recipe resolver into `InputController`
    - Replace hardcoded C-key crafting shortcut with `RecipeRegistry::resolve()` call
    - Wire crafting grid (2×2 player inventory, 3×3 crafting table) to resolver
    - Implement `consumeInputs()` on craft output take
    - _Requirements: 8.1, 8.2, 8.3, 8.6_

- [ ] 9. Checkpoint - Subphase 2.2 complete
  - Ensure all block interaction, placement, inventory, and crafting tests pass. Verify break durations, placement rules, slot semantics, and recipe resolution match MC 1.8.9. Ask the user if questions arise.

- [ ] 10. Subphase 2.3 – Smelting state machine
  - [ ] 10.1 Implement `SmeltingRecipeRegistry` in `include/mechanics/SmeltingRecipeRegistry.hpp` and `src/mechanics/SmeltingRecipeRegistry.cpp`
    - Define SmeltingRecipe, FuelEntry structs
    - Implement `registerRecipe()`, `registerFuel()`, `findRecipe()`, `findFuel()`, `loadDefaults()`
    - Register parity-critical recipes: cobblestone→stone, iron ore→iron ingot, sand→glass, raw beef→cooked beef, raw porkchop→cooked porkchop (all 200 ticks)
    - Register fuel entries: coal=1600, planks=300, sticks=100, log=300
    - _Requirements: 9.9_

  - [ ] 10.2 Extend `FurnaceBlockEntity` with tick-based state machine
    - Add FurnaceState enum (IDLE, ACTIVE), burnTimeRemaining, burnTimeTotal, smeltProgress, smeltDuration fields
    - Implement `tick()` method with state machine logic: Idle→Active on valid input+fuel, Active decrements burn/increments progress, consume input/produce output at smeltDuration, fuel re-consumption, Idle transition on fuel exhaustion
    - Handle pause conditions: output full (64 items) pauses smelt progress, input empty pauses smelt progress but fuel continues burning
    - Handle output slot type mismatch, missing recipe, missing fuel edge cases
    - Update serialization for backward compatibility (default to Idle with zero timers if fields missing)
    - _Requirements: 9.1, 9.2, 9.3, 9.4, 9.5, 9.6, 9.7, 9.8_

  - [ ] 10.3 Integrate furnace tick into game loop
    - Call `furnaceBlockEntity->tick(registry)` once per game tick for every loaded furnace in `src/main.cpp`
    - Remove Y-key instant-smelt shortcut from `InputController`
    - _Requirements: 9.1_

  - [ ]* 10.4 Write smelting property tests in `tests/test_smelting.cpp`
    - **Property 20: Furnace state transitions** — verify Idle→Active on valid input+fuel, Active→Idle on fuel exhaustion
    - **Validates: Requirements 9.1, 9.4, 9.5**
    - **Property 21: Furnace tick progress** — verify burnTime decrements, smeltProgress increments, output produced at smeltDuration
    - **Validates: Requirements 9.2, 9.3**
    - **Property 22: Furnace smelt pause conditions** — verify smeltProgress pauses when output full or input empty, fuel still burns
    - **Validates: Requirements 9.6, 9.7**
    - Include unit tests for: all parity-critical smelting recipes (9.9), zero invalid state transitions (9.8), smelting recipe registry serialization (data integrity)

- [ ] 11. Checkpoint - Subphase 2.3 complete
  - Ensure all smelting tests pass. Verify furnace state machine transitions, fuel consumption, and output production match MC 1.8.9. Ask the user if questions arise.

- [ ] 12. Survival scenario and final validation
  - [ ] 12.1 Implement survival scenario runner in `tests/test_survival_scenario.cpp`
    - Script a 30-minute automated scenario covering: block breaking (with tool modifiers), block placement (with validation), crafting (2×2 and 3×3), smelting (full lifecycle), and inventory management (all click types)
    - Wrap each tick in try-catch for crash detection; log tick number, player state, last action on failure
    - Implement 35-minute wall-clock timeout abort
    - Verify 5 consecutive runs with zero crash-class defects
    - _Requirements: 11.1, 11.2, 11.3_

  - [ ] 12.2 Add movement replay tests to `tests/test_physics_parity.cpp`
    - Add deterministic replay scenarios for walk, sprint, jump, fall, friction stop
    - Verify position delta < 0.001 blocks between 3 consecutive runs
    - Detect trace divergence when constants change
    - _Requirements: 10.1, 10.2, 10.3, 12.3_

  - [ ]* 12.3 Write inventory and crafting replay regression tests
    - Add slot-action replay suite to `tests/test_slot_action.cpp`
    - Add crafting replay suite to `tests/test_recipe.cpp`
    - _Requirements: 12.4_

- [ ] 13. Final checkpoint - Contribution gate compliance
  - Ensure all four CTest targets pass at 100% (tests_data, tests_runtime, tests_render, tests_gameplay_smoke). Verify Release mode build with zero errors. Confirm parity score reaches ≥58. Ask the user if questions arise.
  - _Requirements: 12.1, 12.2, 11.4_

## Notes

- Tasks marked with `*` are optional and can be skipped for faster MVP
- Each task references specific requirements for traceability
- Checkpoints ensure incremental validation after each subphase
- Property tests validate universal correctness properties (24 total from design)
- Unit tests validate specific examples, edge cases, and parity-critical reference values
- All new components are pure-logic C++ classes testable without OpenGL context
- rapidcheck property tests use minimum 100 iterations per property
