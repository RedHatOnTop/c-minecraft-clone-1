# Defect Ledger

This ledger tracks stabilization defects discovered during the large integration pass.
Each defect has a stable ID and category so implementation commits and tests can map 1:1.

## Build

### BUILD-001: Non-reproducible `gl3w` source revision
- Severity: High
- Symptom: Configure/build behavior changed depending on current `master` state.
- Reproduction:
1. Configure from a clean machine with network access.
2. Reconfigure on a different day.
3. Observe dependency revision drift.
- Fix Strategy: Pin `gl3w` to a commit SHA and separate dependency declarations into `cmake/Dependencies.cmake`.

### BUILD-002: Online dependency updates during normal configure
- Severity: High
- Symptom: Configure fails in restricted network environments.
- Reproduction:
1. Disable outbound network access.
2. Run `cmake -B build`.
3. Observe FetchContent update failures.
- Fix Strategy: Enable offline update mode by default and fail with actionable error if cache is missing.

## Data

### DATA-001: Ambiguous chunk format parsing
- Severity: Critical
- Symptom: Legacy and intermediate chunk files can be misparsed.
- Reproduction:
1. Save worlds with different historical formats.
2. Load with `in.peek() != EOF` based parser.
3. Observe data skew/corruption risk.
- Fix Strategy: Add explicit file header (`magic/version/flags/payloadLength`) and dedicated legacy readers.

### DATA-002: Block entity payload cannot skip unknown types
- Severity: High
- Symptom: Legacy loader cannot safely skip unknown block-entity types.
- Reproduction:
1. Write custom/unknown block-entity type payload.
2. Load with old stream parser.
3. Parser desynchronizes.
- Fix Strategy: Encode `type + payloadLength` per block-entity entry in the new format.

## Runtime

### RUNTIME-001: Entity health field shadowing
- Severity: High
- Symptom: `Mob` had a duplicate `health` member diverging from `Entity::health`.
- Reproduction:
1. Damage a mob.
2. Save/load entities.
3. Observe possible state mismatch in different call sites.
- Fix Strategy: Remove duplicate member and keep `Entity::health` as single source of truth.

### RUNTIME-002: Raycast division-by-zero risk
- Severity: High
- Symptom: Direction components near zero could produce unstable reciprocal behavior.
- Reproduction:
1. Cast rays aligned with axis planes.
2. Observe unstable/undefined stepping.
- Fix Strategy: Guard zero-length vectors and zero-axis reciprocal paths.

### RUNTIME-003: Inventory null item dereference
- Severity: Medium
- Symptom: Stacking logic could dereference null registry entries.
- Reproduction:
1. Inject unknown item ID into inventory.
2. Trigger `addItem`.
3. Observe crash risk when reading `maxStackSize`.
- Fix Strategy: Skip null item metadata in stack-merge pass.

## Rendering

### RENDER-001: Hardcoded viewport dimensions
- Severity: Medium
- Symptom: UI layout and projection use fixed 800x600 values.
- Reproduction:
1. Resize the window.
2. Observe stretched or misplaced UI.
- Fix Strategy: Propagate runtime viewport dimensions to GUI/HUD/Font render paths.

### RENDER-002: Unstable transparent draw order
- Severity: Medium
- Symptom: Transparent chunk meshes were rendered in map iteration order.
- Reproduction:
1. Place transparent blocks in overlapping depth regions.
2. Move camera.
3. Observe blending artifacts.
- Fix Strategy: Sort transparent chunks by camera distance (far-to-near).

## Architecture

### ARCH-001: Monolithic `main.cpp` control flow
- Severity: Medium
- Symptom: Input/update/render concerns were tightly coupled and used `goto`.
- Reproduction:
1. Trace gameplay input paths.
2. Observe hard-to-maintain branching and state coupling.
- Fix Strategy: Split into explicit functions and centralize runtime state in `GameContext`.

### ARCH-002: Container UI tracked by raw pointers
- Severity: High
- Symptom: Pointer validity depends on chunk lifetime and block mutations.
- Reproduction:
1. Open container UI.
2. Unload chunk or replace block.
3. Pointer can become invalid.
- Fix Strategy: Track opened container as block coordinates + block ID and resolve entity per frame.

## Tests

### TEST-001: No executable regression suite
- Severity: High
- Symptom: `ctest` reports zero tests, allowing regressions to ship silently.
- Reproduction:
1. Run `ctest --test-dir build -N`.
2. Observe `Total Tests: 0`.
- Fix Strategy: Add deterministic C++ regression tests and wire them into CTest.

## Defect-to-Test Mapping

- `DATA-001` -> `DataMigration_ChunkLegacyToV2_RoundTrip`
- `DATA-002` -> `DataMigration_ChunkLegacyToV2_RoundTrip`
- `RUNTIME-001` -> `Entity_MobHealthPersistence`
- `RUNTIME-002` -> `Raycast_ZeroDirectionComponent_NoCrash`
- `RUNTIME-003` -> `Inventory_AddItem_NullRegistrySafe`
- `RENDER-001` -> `UI_ViewportResize_ProjectionUpdate`
- `RENDER-002` -> `ChunkMesh_TransparentSort_Stable`
