# Deterministic Replay Harness v1

Subphase: 1.2 - Test Topology Split  
Roadmap reference: `docs/roadmap.md` (Section 6.2, Subphase 1.2)  
Published: February 25, 2026

## 1. Purpose

Replay harness v1 verifies deterministic behavior for high-risk gameplay paths by replaying fixed scenario scripts and asserting exact trace equality across repeated runs.

## 2. Implementation Summary

- Harness implementation: `tests/test_replay.cpp`
- Smoke runner target: `tests_gameplay_smoke`
- Determinism method:
  - Each scenario emits a deterministic trace of state snapshots.
  - The same scenario is executed `3` times.
  - Every run must match the fixed expected trace exactly (step-by-step).

## 3. Sample Scenarios (v1)

| Scenario ID | Test Function | Scope | Deterministic Trace Basis |
|---|---|---|---|
| RH-INV-001 | `test_ReplayHarness_InventoryFlow_Deterministic` | Inventory/crafting-like slot transitions | Slot itemID/count/meta signature sequence |
| RH-WORLD-001 | `test_ReplayHarness_BlockRaycastFlow_Deterministic` | Block placement/removal + raycast hit order | Raycast hit block ID and coordinates per step |
| RH-PERSIST-001 | `test_ReplayHarness_PersistenceFlow_Deterministic` | Save/load round-trip state consistency | Player slot/health/inventory + entity class/health aggregate |

## 4. Execution Commands

```bash
cmake -S . -B build -DMC_OFFLINE_FETCHCONTENT=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
build/bin/Release/tests_gameplay_smoke.exe
```

## 5. Evidence

- CTest target listing: `docs/quality/logs/subphase-1.2-ctest-list.log`
- Full CTest pass: `docs/quality/logs/subphase-1.2-ctest.log`
- Replay scenario execution output: `docs/quality/logs/subphase-1.2-replay.log`
