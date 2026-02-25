# Parity Matrix v1 (Draft)

Version: v1-draft  
Owner: WS-QA + WS-ARCH  
Prepared for: Subphase 0.1 (Measurement Model Setup)  
Last updated: February 25, 2026

## 1. Matrix Purpose

This matrix defines measurable parity items for Minecraft Java Edition 1.8.9 comparison.
It is the scoring source of truth referenced by the roadmap.

## 2. Measurement Schema

Each item must include:
- `ID`: stable identifier used in reports and test mapping.
- `Domain`: A-F (same weighting model as roadmap).
- `Must-Have`: `Y` or `N`.
- `Measurement Method`: how the item is evaluated.
- `Metric`: numeric or binary metric used for scoring.
- `Target Threshold`: pass-level expectation for near-match evidence.
- `Evidence Type`: screenshot, replay log, metric report, or test output.

## 3. Must-Have Coverage Summary (QG-DOC-0.1.1)

| Domain | Must-Have Target Count | Enumerated Count | Coverage |
|---|---:|---:|---:|
| A. Visual/Rendering | 8 | 8 | 100.00% |
| B. Core Mechanics | 10 | 9 | 90.00% |
| C. Content | 7 | 6 | 85.71% |
| D. World/Progression | 7 | 6 | 85.71% |
| E. UX/Audio | 5 | 5 | 100.00% |
| F. Quality/Stability | 5 | 5 | 100.00% |
| **Total** | **42** | **39** | **92.86%** |

Subphase 0.1 acceptance threshold: `>= 90%` enumerated must-have items.  
Current result: `39 / 42 = 92.86%` (PASS).

## 4. Enumerated Must-Have Items

## 4.1 Domain A - Visual/Rendering

| ID | Must-Have | Item | Measurement Method | Metric | Target Threshold | Evidence Type |
|---|---|---|---|---|---|---|
| A-01 | Y | Block texture fidelity coverage | Asset mapping audit + screenshot compare | % core blocks with matching textures | >= 95% | Asset report + screenshots |
| A-02 | Y | Block model geometry alignment | Model bounds and face-layout diff | Mean geometry deviation | <= 5% | Geometry diff report |
| A-03 | Y | Lighting step accuracy | Light level test scenes | Mean per-block brightness error | <= 1 light level | Scene captures |
| A-04 | Y | Transparent sorting stability | Camera sweep on transparent stack scenes | Sorting artifact count | 0 severe artifacts | Video capture |
| A-05 | Y | Fog curve behavior | Distance-based fog samples | Fog intensity error | <= 0.05 normalized error | Metric plot |
| A-06 | Y | Time-of-day sky transition | Time-lapse color sampling | Color delta (normalized) | <= 0.08 | Time-lapse report |
| A-07 | Y | Particle event coverage | Event-to-particle mapping checklist | % parity-critical events covered | >= 90% | Event checklist |
| A-08 | Y | First-person camera feel | Bobbing/FOV profile replay | Motion curve deviation | <= 10% | Replay metrics |

## 4.2 Domain B - Core Mechanics

| ID | Must-Have | Item | Measurement Method | Metric | Target Threshold | Evidence Type |
|---|---|---|---|---|---|---|
| B-01 | Y | Walk and sprint velocity profile | Deterministic movement replay | Speed curve deviation | <= 8% | Replay metrics |
| B-02 | Y | Jump apex and fall timing | Tick-based movement simulation | Apex height/time error | <= 8% | Simulation logs |
| B-03 | Y | Ground friction deceleration | Stop-distance replay test | Deceleration profile error | <= 10% | Replay metrics |
| B-04 | Y | AABB collision resolution | Collision scenario pack | Incorrect resolution rate | 0 critical failures | Test output |
| B-05 | Y | Block break duration and tool modifiers | Rule-table parameter tests | Break-time error | <= 10% | Parametric tests |
| B-06 | Y | Block placement validity rules | Placement constraint test matrix | Invalid placements accepted | 0 parity-critical cases | Test output |
| B-07 | Y | Inventory slot transfer semantics | Slot action replay suite | Behavior mismatch rate | <= 5% | Replay + assertions |
| B-08 | Y | Crafting recipe resolution | Recipe conflict tests | Wrong output rate | 0 parity-critical recipes | Test output |
| B-09 | Y | Smelting state transitions | Input/fuel/output state tests | Invalid transition rate | 0 | State transition tests |

## 4.3 Domain C - Content

| ID | Must-Have | Item | Measurement Method | Metric | Target Threshold | Evidence Type |
|---|---|---|---|---|---|---|
| C-01 | Y | Core block set availability | Registry audit | Core block coverage | >= 95% | Registry report |
| C-02 | Y | Core item and tool set availability | Registry audit | Core item/tool coverage | >= 90% | Registry report |
| C-03 | Y | Armor and durability baseline | Scenario tests | Baseline functionality pass rate | 100% | Scenario tests |
| C-04 | Y | Core hostile mob set availability | Spawn and behavior checks | Core hostile mob coverage | >= 85% | Coverage checklist |
| C-05 | Y | Utility block set availability | Interaction tests | Utility block coverage | >= 85% | Test output |
| C-06 | Y | Core status-effect set availability | Effect application tests | Core effect coverage | >= 80% | Effect tests |

## 4.4 Domain D - World/Progression

| ID | Must-Have | Item | Measurement Method | Metric | Target Threshold | Evidence Type |
|---|---|---|---|---|---|---|
| D-01 | Y | Biome distribution quality | Fixed-seed biome sampling | Biome spread deviation | <= 12% | Seed metrics |
| D-02 | Y | Terrain height profile | Height histogram comparison | Distribution divergence | <= 12% | Histogram report |
| D-03 | Y | Cave density profile | Subsurface sampling | Cave cell ratio error | <= 15% | Sampling report |
| D-04 | Y | Ore spawn frequency profile | Ore distribution sampling | Spawn frequency error | <= 15% | Ore metrics |
| D-05 | Y | Structure generation availability | Multi-seed structure checks | Required structures generated | >= 95% seeds | Seed validation logs |
| D-06 | Y | Save/load progression continuity | Scenario round-trip tests | Progression continuity failures | 0 parity-critical failures | Round-trip tests |

## 4.5 Domain E - UX/Audio

| ID | Must-Have | Item | Measurement Method | Metric | Target Threshold | Evidence Type |
|---|---|---|---|---|---|---|
| E-01 | Y | Main/pause/options flow completion | UI state navigation tests | Dead-end or missing flow count | 0 | UI flow tests |
| E-02 | Y | HUD element completeness | HUD checklist + screenshot compare | Missing parity-critical elements | 0 | HUD checklist |
| E-03 | Y | Keybinding persistence | Settings save/reload tests | Persistence failures | 0 | Settings tests |
| E-04 | Y | Mouse sensitivity and invert behavior | Input behavior tests | Behavior mismatch rate | <= 5% | Input tests |
| E-05 | Y | Audio category routing and volume | Audio event mapping tests | Misrouted events | 0 parity-critical events | Audio test logs |

## 4.6 Domain F - Quality/Stability

| ID | Must-Have | Item | Measurement Method | Metric | Target Threshold | Evidence Type |
|---|---|---|---|---|---|---|
| F-01 | Y | Required CTest target pass rate | CI and local CTest runs | Pass rate | 100% required targets | CTest output |
| F-02 | Y | Crash-free 30-minute session rate | Repeated survival scenario runs | Crash-free run ratio | >= 95% | Session logs |
| F-03 | Y | Save/load integrity loop | Repeated persistence loop tests | Corruption incidence | 0 | Integrity loop report |
| F-04 | Y | Frame-time budget (p95) | Performance profiling scenes | p95 frame time | <= target budget | Perf report |
| F-05 | Y | Memory growth in 2-hour session | Long-session memory trace | Relative memory growth | <= 15% | Memory profile |

## 5. Remaining Must-Have Items (Not Yet Enumerated in v1)

These items are intentionally deferred to Subphase 0.2 and must be added before baseline capture closes.

| Domain | Planned ID | Item |
|---|---|---|
| B | B-10 | Combat hit-confirm and knockback frame profile |
| C | C-07 | Enchantment and repair parity baseline |
| D | D-07 | Structure density and placement plausibility score |

## 6. Traceability Rules

- Every item ID must map to at least one evidence artifact.
- Every automated parity test must reference at least one matrix item ID.
- Any roadmap score update must cite changed item IDs and evidence links.

## 7. Initial Baseline Ratings (Subphase 0.2)

Scale for this baseline capture:
- `N/A`: insufficient evidence for a reliable baseline score yet.
- `Partial`: evidence exists, but parity confidence is limited.
- `Match`: baseline evidence indicates near-match behavior for the item scope tested.

High-impact items for Subphase 0.2 evidence closure:

| ID | Initial Rating | Baseline Note | Evidence |
|---|---|---|---|
| A-01 | Partial | Core visual assets are present; scene-level visual parity not yet measured. | `docs/quality/baseline/2026-02-25/logs/asset-presence.log` |
| A-04 | Match | Transparent sorting regression test is passing in current baseline suite. | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| A-08 | Partial | First-person camera code path exists; motion-curve capture is pending. | `docs/quality/baseline/2026-02-25/logs/source-signals.log` |
| B-01 | Partial | Movement behavior exists; deterministic curve comparison is pending. | `docs/quality/baseline/2026-02-25/logs/source-signals.log` |
| B-04 | Partial | Collision and raycast safety tests pass; full collision matrix remains pending. | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| B-07 | Partial | Inventory safety test passes; full slot action parity suite is pending. | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| B-08 | N/A | Crafting parity baseline is not yet captured in automated scenarios. | `docs/quality/baseline/2026-02-25/scenario-suite-results.md` |
| B-09 | N/A | Smelting transition parity baseline is pending dedicated scenario tests. | `docs/quality/baseline/2026-02-25/scenario-suite-results.md` |
| C-01 | Partial | Core block coverage baseline is inferred from current assets and block registry paths. | `docs/quality/baseline/2026-02-25/logs/asset-presence.log` |
| C-02 | Partial | Core item/tool baseline evidence exists through inventory/item test flows. | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| D-01 | N/A | Biome distribution baseline requires fixed-seed sampling not yet executed. | `docs/quality/baseline/2026-02-25/scenario-suite-results.md` |
| D-05 | N/A | Structure generation baseline capture is scheduled for world baseline phase. | `docs/quality/baseline/2026-02-25/scenario-suite-results.md` |
| D-06 | Match | Save/load progression continuity is validated by passing persistence tests. | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| E-01 | Partial | Menu flow baseline is documented; interactive capture run is pending. | `docs/quality/baseline/2026-02-25/captures/screenshot-manifest.md` |
| E-03 | N/A | Settings persistence baseline requires interactive runtime flow capture. | `docs/quality/baseline/2026-02-25/scenario-suite-results.md` |
| F-01 | Match | Required CTest target passes in baseline run. | `docs/quality/baseline/2026-02-25/logs/ctest-release.log` |
| F-03 | Match | Persistence loop safety is covered by passing persistence-related tests. | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
