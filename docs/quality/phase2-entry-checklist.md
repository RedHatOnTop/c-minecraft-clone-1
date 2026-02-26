# Phase 2 Entry Checklist and Dependency Map

Checklist ID: `phase2-entry-checklist-v1`  
Prepared in: Subphase 1.4 (Phase 2 Entry Setup)  
Publication date: February 26, 2026  
Roadmap reference: `docs/roadmap.md` (Section 6.2, Subphase 1.4)

## 1. Purpose

Provide a single handoff package so Subphase 2.1 (Movement and Physics Alignment) can start without setup blockers.

## 2. Entry Checklist

| Item | Required | Status | Evidence |
|---|---|---|---|
| Required CTest topology remains stable (`tests_data`, `tests_runtime`, `tests_render`, `tests_gameplay_smoke`) | Yes | Complete | `docs/quality/logs/subphase-1.4-ctest-list.log` |
| Required CTest run passes at 100% | Yes | Complete | `docs/quality/logs/subphase-1.4-ctest.log` |
| Movement metric harness test is in required targets | Yes | Complete | `tests/test_movement.cpp`, `docs/quality/logs/subphase-1.4-tests-runtime.log` |
| Worldgen metric harness tests are in required targets | Yes | Complete | `tests/test_worldgen.cpp`, `docs/quality/logs/subphase-1.4-tests-data.log` |
| Phase 2 metric-baseline interpretation notes published | Yes | Complete | `docs/quality/phase2-metric-baselines.md` |
| Subphase gate verification is published | Yes | Complete | `docs/quality/subphase-1.4-qg-report.md` |

## 3. Dependency Map for Subphase 2.1

### 3.1 Runtime Dependencies

| Area | Required Inputs | Primary Files |
|---|---|---|
| Player motion and jump behavior | Player update loop, gravity/jump constants, on-ground transitions | `src/physics/Player.cpp`, `include/physics/Player.hpp`, `include/entity/Entity.hpp` |
| World collision constraints | AABB collision behavior against block solids | `include/entity/Entity.hpp`, `src/world/World.cpp` |
| Tick/update cadence assumptions | Fixed-step test cadence and frame update interactions | `tests/test_movement.cpp`, `src/main.cpp` |

### 3.2 Test and Verification Dependencies

| Area | Required Inputs | Primary Files |
|---|---|---|
| Runtime regression gate | Runtime test runner wiring | `tests/test_main_runtime.cpp`, `CMakeLists.txt` |
| Worldgen regression gate | Data test runner wiring | `tests/test_main_data.cpp`, `CMakeLists.txt` |
| Replay stability guard | Deterministic smoke scenarios | `tests/test_replay.cpp`, `docs/quality/replay-harness-v1.md` |
| PR quality discipline | Mandatory build/test and test-impact policy | `docs/quality/contribution-gate-policy.md` |

## 4. Ownership Handoff

| Stream | Scope for Phase 2.1 Start | Input Package |
|---|---|---|
| WS-GAME | Movement constants, collision response, jump behavior tuning | This checklist + `docs/quality/phase2-metric-baselines.md` |
| WS-WORLD | World collision interactions touched by movement changes | This checklist + `tests/test_worldgen.cpp` constraints |
| WS-QA | Regression updates and gate evidence capture | Subphase 1.4 logs under `docs/quality/logs/` |

## 5. Phase 2.1 Start Rules

1. Keep all required CTest targets green after each movement/collision tuning change.
2. Any movement constant changes must include updates to deterministic runtime tests.
3. If world collision behavior changes, update both runtime and data-test evidence before merge.
4. If thresholds in `phase2-metric-baselines.md` are revised, record rationale and old/new values in the same commit.

## 6. Sign-Off

Subphase 1.4 entry checklist is complete and approved for Phase 2.1 kickoff.
