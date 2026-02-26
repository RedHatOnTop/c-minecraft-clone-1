# Phase 2 Metric Baselines (Movement and Worldgen)

Baseline ID: `phase2-metric-baseline-v1`  
Prepared in: Subphase 1.4 (Phase 2 Entry Setup)  
Publication date: February 26, 2026  
Roadmap reference: `docs/roadmap.md` (Subphase 1.4 deliverable package)

## 1. Purpose

Freeze the initial metric expectations used to start Subphase 2.1 and prevent untracked threshold drift.

## 2. Movement Metrics

Source test:
- `test_Physics_JumpApex_FixedTickProfile` in `tests/test_movement.cpp`

Execution evidence:
- `docs/quality/logs/subphase-1.4-tests-runtime.log`

| Metric ID | Metric | Baseline Rule | Intent |
|---|---|---|---|
| `M-MOVE-001` | Jump apex gain from start Y | Must stay in `1.15` to `1.55` | Keep jump vertical feel inside bounded profile while tuning |
| `M-MOVE-002` | Jump cycle duration (liftoff to landing) | Must stay in `0.55s` to `0.95s` | Protect major timing regressions in gravity/jump tuning |
| `M-MOVE-003` | Liftoff and landing observability | Must observe both `leftGround=true` and final `onGround=true` | Ensure jump path is physically complete and non-stuck |

## 3. Worldgen Metrics

Source tests:
- `test_WorldGen_BiomeDistribution_FunctionalBaseline` in `tests/test_worldgen.cpp`
- `test_WorldGen_HeightAndDecoration_Profile` in `tests/test_worldgen.cpp`

Execution evidence:
- `docs/quality/logs/subphase-1.4-tests-data.log`

| Metric ID | Metric | Baseline Rule | Intent |
|---|---|---|---|
| `M-WORLD-001` | Terrain-band diversity in sampled chunks | All three bands (plains/mid/mountain) must appear at least once | Avoid degenerate biome/height profile collapse |
| `M-WORLD-002` | Sampled height minimum | `minHeight >= 45` | Prevent severe low-terrain regressions |
| `M-WORLD-003` | Sampled height maximum | `maxHeight <= 140` | Prevent extreme height spikes outside expected envelope |
| `M-WORLD-004` | Height spread | `(maxHeight - minHeight) >= 25` | Preserve macro terrain variation |
| `M-WORLD-005` | Mean terrain height | `60.0 <= meanHeight <= 95.0` | Keep broad terrain centerline stable during iteration |
| `M-WORLD-006` | Decoration presence | `logBlocks > 0` and `leafBlocks > 0` | Keep decoration pass active in baseline samples |

## 4. Update Policy for Phase 2+

1. Baselines above are quality guards, not final parity targets.
2. Threshold changes require a rationale note and updated evidence logs in the same commit.
3. If a baseline is widened temporarily, add a follow-up item in the active subphase tracker to tighten it back.
4. Any baseline change that weakens regression sensitivity must be approved in subphase QG notes.
