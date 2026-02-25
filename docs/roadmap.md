# MinecraftClone189 Master Roadmap (v3)

Baseline Date: February 25, 2026  
Target: Reach at least 90% visual and gameplay-mechanics parity with Minecraft Java Edition 1.8.9  
Scope: Single-player only (world generation, progression, save/load, combat/survival/crafting, UI/audio, rendering)

---

## 1. Document Purpose and Completion Definition

### 1.1 Purpose of This Document
- Define a concrete execution plan from the current codebase state to the 90%+ parity target.
- Cover the full system: parity measurement, development, verification, release readiness, and certification.
- Keep priorities stable under schedule pressure through gate-driven milestone control.

### 1.2 Definition of 100% Completion (Roadmap Complete)
The roadmap is complete only when all conditions below are met.

1. Total parity score is 90 or higher (official model in Section 4).  
2. Zero Blocker and zero Critical defects on the release branch.  
3. Save-data integrity passes including long-session and migration scenarios.  
4. Performance and stability targets are met (Phase 9 release gates).  
5. Release checklist is 100% complete and two consecutive RC passes succeed.

### 1.3 Out of Scope (for this 90% target)
- Full multiplayer/server protocol compatibility.
- Modding API and plugin ecosystem support.
- Features outside 1.8.9 gameplay semantics (for example, 1.9+ combat model).

---

## 2. Current State Snapshot (Starting Point)

### 2.1 Confirmed Foundation
- CMake offline dependency mode and CI build/test flow exist.
- Executable regression tests are wired through `ctest`.
- Save/load migration, base rendering/input/inventory/raycast/container flows are present.
- A first stabilization barrier exists for key data/runtime/rendering defect classes.

### 2.2 Main Gaps Relative to 90% Parity
- Feature coverage: many 1.8.9 systems are still missing or partial.
- Visual fidelity: block states, particles, animation details, and lighting behavior still diverge.
- Architecture: significant responsibility remains concentrated in `main.cpp`.
- Test density: deterministic replay and long-session coverage are still insufficient.

---

## 3. Development Strategy (Principles)

1. Measure first: progress is scored against a parity matrix, not subjective judgment.  
2. Vertical slices: each slice includes implementation, tests, and docs updates.  
3. Stability first: any persistence-related change requires backward-compatibility verification.  
4. Gate-driven flow: phase transitions require explicit quantitative exits.  
5. Freeze discipline: final phases are bugfix/tuning only, no net-new feature expansion.

---

## 4. Parity Measurement Framework (Official 90% Criteria)

## 4.1 Total Score Formula

Total score = 100 points across weighted domains.  
Release target = 90 points or higher.

| Domain | Weight | Description |
|---|---:|---|
| A. Visual/Rendering Parity | 22 | Block/model/lighting/transparent effects/particles/camera look-and-feel |
| B. Core Mechanics Parity | 28 | Movement, collisions, block interactions, inventory, crafting, combat |
| C. Content Parity | 18 | Coverage of blocks/items/entities/status systems |
| D. World/Progression Parity | 16 | World generation, biomes, structures, progression loop |
| E. UX/Audio Parity | 8 | Menus, HUD, options, input experience, audio feedback |
| F. Quality/Stability | 8 | Crash rate, save integrity, performance, automation quality |

## 4.2 Scoring Rules
- Each sub-item is scored 0 to 5 (0 = absent, 3 = partial, 5 = near-match).
- Domain score = (average sub-item score / 5) * domain weight.
- Scores are updated every sprint and logged under `docs/quality/`.

## 4.3 Hard Thresholds for 90% Qualification
Even if total score is 90+, release is rejected if any threshold below is missed.

- A (Visual): at least 18/22  
- B (Core Mechanics): at least 24/28  
- D (World/Progression): at least 13/16  
- F (Quality/Stability): at least 7/8

---

## 5. Workstream Structure

Development is organized into six parallel tracks.

- WS-ARCH: architecture, modularization, game-loop separation
- WS-RENDER: rendering, assets, visual effects
- WS-GAME: player/combat/crafting/inventory/block interactions
- WS-WORLD: world generation/chunks/save/migration
- WS-UXAUDIO: menus/HUD/input settings/audio
- WS-QA: tests/CI/performance/long-session stability

Each milestone must produce outcomes in at least one item from each active track.

---

## 6. Master Schedule with Subphases and Quality Gates

Full timeline: 2026-02-26 to 2027-02-20

### 6.0 Gate Taxonomy (used in all subphases)
- `QG-BUILD`: Build/configure must pass on target environments.
- `QG-TEST`: Required automated tests must pass at 100%.
- `QG-PARITY`: Parity score or subdomain threshold must be met.
- `QG-PERF`: Performance budget thresholds must be met.
- `QG-STAB`: Stability/integrity thresholds must be met.
- `QG-DOC`: Required docs and traceability artifacts must be updated.

### 6.1 Phase 0 - Parity Baseline Definition
Window: 2026-02-26 to 2026-03-08  
Phase score target: Baseline established (no fixed score yet)

#### Subphase 0.1 - Measurement Model Setup (2026-02-26 to 2026-03-01)
Status: Completed on February 25, 2026 (executed ahead of schedule)

Focus:
- Define parity rubric and item hierarchy.
- Freeze scoring rules and evidence format.

Deliverables:
- Draft `docs/quality/parity-matrix-v1.md`.
- Scoring handbook section with item-level interpretation examples.

Quality gates:
- `QG-DOC-0.1.1`: At least 90% of must-have parity items are enumerated.
- `QG-DOC-0.1.2`: All six weighted domains map to explicit measurable items.

#### Subphase 0.2 - Baseline Capture Execution (2026-03-02 to 2026-03-05)
Status: Completed on February 25, 2026 (executed ahead of schedule)

Focus:
- Run baseline gameplay scenarios and collect evidence.

Deliverables:
- Baseline screenshot/video/log package.
- Initial item-level ratings (`N/A`, `Partial`, `Match`).

Quality gates:
- `QG-TEST-0.2.1`: Baseline scenario suite run success >= 90%.
- `QG-DOC-0.2.2`: Evidence attached for all high-impact scoring items.

Completion note:
- Scenario suite pass rate reached `90.00%` (`9/10`).
- Interactive screenshot/video capture remains pending and is tracked in capture manifests.

#### Subphase 0.3 - Gap Ranking and Lock (2026-03-06 to 2026-03-08)
Status: Completed on February 26, 2026 (executed ahead of schedule)

Focus:
- Rank top parity gaps and lock the next-phase priorities.

Deliverables:
- `docs/quality/baseline-report-2026-03-08.md`.
- Top-20 gap backlog with owner and target phase mapping.

Quality gates:
- `QG-DOC-0.3.1`: Top-20 gaps are severity-ranked and traceable to roadmap phases.
- `QG-PARITY-0.3.2`: Baseline score is published and reproducible.

Completion note:
- Baseline score published as `15.87 / 100` with reproducible scoring input and output logs.
- Top-20 gap backlog is locked with target phase/subphase and owner mapping.

Phase exit criteria:
- Baseline matrix and report approved.
- Next two phases have prioritized, sequenced work packages.

### 6.2 Phase 1 - Test Hardening and Architecture Foundation
Window: 2026-03-09 to 2026-04-05  
Phase score target: 45+

#### Subphase 1.1 - Architecture Boundary Cut (2026-03-09 to 2026-03-15)
Focus:
- Define and implement first modular boundaries from `main.cpp`.

Deliverables:
- `AppBootstrap`, `InputController` modules.
- Updated dependency and ownership diagram.

Quality gates:
- `QG-BUILD-1.1.1`: Clean configure/build on local and CI.
- `QG-TEST-1.1.2`: Existing regression suite remains green.

#### Subphase 1.2 - Test Topology Split (2026-03-16 to 2026-03-25)
Focus:
- Split monolithic test execution into domain targets.

Deliverables:
- CTest targets: `tests_data`, `tests_runtime`, `tests_render`, `tests_gameplay_smoke`.
- Deterministic replay harness v1.

Quality gates:
- `QG-TEST-1.2.1`: At least 4 independent CTest targets are runnable.
- `QG-TEST-1.2.2`: Replay harness can deterministically reproduce at least 3 sample scenarios.

#### Subphase 1.3 - Integration Hardening (2026-03-26 to 2026-04-05)
Focus:
- Stabilize architecture/test changes and close regressions.

Deliverables:
- Integration report with defect deltas and resolved regressions.
- Updated contribution gate guidance (test impact requirements).

Quality gates:
- `QG-TEST-1.3.1`: 100% pass on all required test targets.
- `QG-DOC-1.3.2`: PR gate policy updated and linked in docs.
- `QG-PARITY-1.3.3`: Score reaches at least 45.

Phase exit criteria:
- Architecture split is safe to iterate on.
- Test topology supports domain-level ownership.

### 6.3 Phase 2 - Core Mechanics Parity Pass 1
Window: 2026-04-06 to 2026-05-17  
Phase score target: 58+

#### Subphase 2.1 - Movement and Physics Alignment (2026-04-06 to 2026-04-19)
Focus:
- Tune movement constants and collision behavior.

Deliverables:
- Updated movement/physics parameter table.
- Deterministic movement replay tests.

Quality gates:
- `QG-TEST-2.1.1`: Physics replay variance within configured tolerance.
- `QG-PARITY-2.1.2`: Core movement sub-score improves by planned delta.

#### Subphase 2.2 - Block, Inventory, and Crafting Semantics (2026-04-20 to 2026-05-03)
Focus:
- Align block interactions and inventory/crafting behaviors.

Deliverables:
- Slot action matrix implementation.
- 2x2 and 3x3 recipe path support with conflict rules.

Quality gates:
- `QG-TEST-2.2.1`: Slot-action regression suite passes.
- `QG-TEST-2.2.2`: Recipe path tests cover priority and edge cases.

#### Subphase 2.3 - Smelting Loop and Scenario Validation (2026-05-04 to 2026-05-17)
Focus:
- Complete smelting lifecycle and validate core survival loop.

Deliverables:
- Smelting state machine and tests.
- 30-minute survival scenario report (5 consecutive runs).

Quality gates:
- `QG-STAB-2.3.1`: Zero crash-class defects in core mechanics scenarios.
- `QG-TEST-2.3.2`: 5/5 survival scenario pass streak.
- `QG-PARITY-2.3.3`: Score reaches at least 58.

Phase exit criteria:
- Core loop is stable and testable end-to-end.

### 6.4 Phase 3 - Combat, Entity, and AI Parity
Window: 2026-05-18 to 2026-06-28  
Phase score target: 68+

#### Subphase 3.1 - Combat Rule Core (2026-05-18 to 2026-05-31)
Focus:
- Align combat hit logic, timing, damage semantics.

Deliverables:
- Combat rule table and deterministic hit-sequence tests.

Quality gates:
- `QG-TEST-3.1.1`: Combat sequence tests pass with deterministic outcomes.
- `QG-PARITY-3.1.2`: Combat sub-score increase meets target.

#### Subphase 3.2 - Mob AI and Spawn/Despawn Rules (2026-06-01 to 2026-06-14)
Focus:
- Implement and tune detection, pursuit, attack cadence, and spawn lifecycle.

Deliverables:
- AI behavior benchmark scenarios and telemetry traces.

Quality gates:
- `QG-TEST-3.2.1`: AI scenario tests pass across fixed seeds.
- `QG-STAB-3.2.2`: Abnormal behavior rate < 2% in 20-mob simulation.

#### Subphase 3.3 - Persistence and Stress Validation (2026-06-15 to 2026-06-28)
Focus:
- Verify entity/combat state persistence across save/load cycles.

Deliverables:
- Entity persistence regression suite expansion.
- Combat stress test report.

Quality gates:
- `QG-STAB-3.3.1`: Zero critical combat defects remain open.
- `QG-TEST-3.3.2`: Persistence round-trip tests all green.
- `QG-PARITY-3.3.3`: Score reaches at least 68.

Phase exit criteria:
- Combat and entity systems are stable under repeated simulation and persistence.

### 6.5 Phase 4 - World Generation, Biome, and Structure Parity
Window: 2026-06-29 to 2026-08-16  
Phase score target: 76+

#### Subphase 4.1 - Terrain and Biome Calibration (2026-06-29 to 2026-07-12)
Focus:
- Tune terrain profile and biome distribution against target expectations.

Deliverables:
- Seed comparison metric report (height variance, biome spread, cave density).

Quality gates:
- `QG-TEST-4.1.1`: Fixed-seed metric suite passes tolerance thresholds.
- `QG-PARITY-4.1.2`: World-generation sub-score increase meets target.

#### Subphase 4.2 - Structure Rollout (2026-07-13 to 2026-07-29)
Focus:
- Add parity-critical structures in incremental priority waves.

Deliverables:
- Structure generation matrix (implemented vs planned).
- Structure discovery scenario logs.

Quality gates:
- `QG-TEST-4.2.1`: Structure generation minimums pass on 5 validation seeds.
- `QG-STAB-4.2.2`: No crash/data-loss defects from structure generation.

#### Subphase 4.3 - Chunk and Persistence Stability (2026-07-30 to 2026-08-16)
Focus:
- Stabilize chunk lifecycle and large-world save/load behavior.

Deliverables:
- 100-cycle save/load integrity report.
- Chunk boundary regression pack.

Quality gates:
- `QG-STAB-4.3.1`: Zero reproducible world corruption defects.
- `QG-TEST-4.3.2`: 100-cycle integrity loop passes.
- `QG-PARITY-4.3.3`: Score reaches at least 76.

Phase exit criteria:
- World exploration and progression infrastructure is reliable.

### 6.6 Phase 5 - Visual Parity Focus
Window: 2026-08-17 to 2026-09-27  
Phase score target: 83+

#### Subphase 5.1 - Lighting and Material Response (2026-08-17 to 2026-08-30)
Focus:
- Improve lighting propagation, brightness stepping, and shading consistency.

Deliverables:
- Lighting regression scenes and tuning record.

Quality gates:
- `QG-TEST-5.1.1`: Lighting scene suite passes visual tolerance checks.
- `QG-PARITY-5.1.2`: Visual domain score trend is positive and on plan.

#### Subphase 5.2 - Transparency, Atmosphere, and Camera Feel (2026-08-31 to 2026-09-13)
Focus:
- Stabilize transparent draw order and time-based atmosphere effects.

Deliverables:
- Transparent ordering validation pack.
- Sky/fog/time transition comparison captures.

Quality gates:
- `QG-TEST-5.2.1`: No transparent ordering regressions in validation scenes.
- `QG-STAB-5.2.2`: Zero severe visual artifacts (depth/flicker/ordering).

#### Subphase 5.3 - Particles and First-Person Polish (2026-09-14 to 2026-09-27)
Focus:
- Add and tune particles, break feedback, pickup feedback, and first-person polish.

Deliverables:
- Particle behavior matrix.
- 20-scene side-by-side similarity review.

Quality gates:
- `QG-PARITY-5.3.1`: At least 18 of 20 representative scenes judged close-match.
- `QG-PARITY-5.3.2`: Score reaches at least 83.
- `QG-TEST-5.3.3`: Visual regression suite remains green.

Phase exit criteria:
- Visual presentation is consistently near target in representative scenes.

### 6.7 Phase 6 - UI, UX, and Audio Parity
Window: 2026-09-28 to 2026-11-02  
Phase score target: 87+

#### Subphase 6.1 - Menu and Settings Backbone (2026-09-28 to 2026-10-10)
Focus:
- Complete menu navigation and settings persistence architecture.

Deliverables:
- Menu state-flow specification and implementation.
- Persisted settings round-trip tests.

Quality gates:
- `QG-TEST-6.1.1`: Menu flow test suite passes.
- `QG-STAB-6.1.2`: No settings persistence failures across restart cycles.

#### Subphase 6.2 - HUD and Input Ergonomics (2026-10-11 to 2026-10-21)
Focus:
- Align HUD readability and control behavior with parity targets.

Deliverables:
- HUD layout comparison set.
- Input option behavior verification matrix.

Quality gates:
- `QG-TEST-6.2.1`: HUD and input regression tests pass.
- `QG-PARITY-6.2.2`: UX sub-score progress meets milestone plan.

#### Subphase 6.3 - Audio Routing and UX Closure (2026-10-22 to 2026-11-02)
Focus:
- Finalize audio categories, volume paths, and interaction feedback.

Deliverables:
- Audio event routing map.
- 50-item UX checklist execution report.

Quality gates:
- `QG-STAB-6.3.1`: Zero UX blockers remain open.
- `QG-TEST-6.3.2`: Audio and UX checklist pass rate >= 95%.
- `QG-PARITY-6.3.3`: Score reaches at least 87.

Phase exit criteria:
- UI, controls, and audio feedback are production-stable for parity validation.

### 6.8 Phase 7 - Content Parity Expansion
Window: 2026-11-03 to 2026-12-14  
Phase score target: 90+ (first achievement)

#### Subphase 7.1 - Content Wave A (2026-11-03 to 2026-11-16)
Focus:
- Implement parity-critical progression content first.

Deliverables:
- Content priority wave A implementation report.
- Updated parity matrix coverage for core progression items.

Quality gates:
- `QG-PARITY-7.1.1`: Core progression coverage reaches planned threshold.
- `QG-TEST-7.1.2`: New content regression tests pass.

#### Subphase 7.2 - Content Wave B (2026-11-17 to 2026-11-30)
Focus:
- Extend combat, utility, and status-related content.

Deliverables:
- Content wave B matrix and interaction tests.

Quality gates:
- `QG-TEST-7.2.1`: Item/block/status interaction matrix passes.
- `QG-STAB-7.2.2`: No progression blocker defects introduced.

#### Subphase 7.3 - Cross-System Consistency Pass (2026-12-01 to 2026-12-14)
Focus:
- Resolve cross-system parity mismatches before hardening phases.

Deliverables:
- Cross-system consistency defect burn-down report.
- 2-hour long-session smoke report.

Quality gates:
- `QG-STAB-7.3.1`: Zero progression blockers remain.
- `QG-PARITY-7.3.2`: Total parity score reaches at least 90.
- `QG-TEST-7.3.3`: Long-session smoke passes on required runs.

Phase exit criteria:
- 90+ score is reached and reproducible with test evidence.

### 6.9 Phase 8 - Performance, Stability, and Long-Session Hardening
Window: 2026-12-15 to 2027-01-25  
Phase score target: 92+

#### Subphase 8.1 - Performance Budget Closure (2026-12-15 to 2026-12-28)
Focus:
- Profile and reduce frame-time spikes and heavy-path costs.

Deliverables:
- Performance budget dashboard and before/after traces.

Quality gates:
- `QG-PERF-8.1.1`: Frame-time budget targets met on reference scenes.
- `QG-TEST-8.1.2`: Performance regression checks integrated in CI/nightly.

#### Subphase 8.2 - Memory and Persistence Robustness (2026-12-29 to 2027-01-11)
Focus:
- Remove leak/fragmentation classes and harden persistence failure handling.

Deliverables:
- Memory stability report.
- Crash-recovery and large-save integrity report.

Quality gates:
- `QG-STAB-8.2.1`: No confirmed long-session leak class remains.
- `QG-STAB-8.2.2`: Zero confirmed save corruption in fault-injection scenarios.

#### Subphase 8.3 - Soak and Reliability Certification (2027-01-12 to 2027-01-25)
Focus:
- Validate full reliability under extended soak and repeated scenario replay.

Deliverables:
- Nightly soak summary (minimum 6-hour runs).
- Reliability trend report and residual-risk list.

Quality gates:
- `QG-STAB-8.3.1`: 5 of 5 soak runs pass.
- `QG-STAB-8.3.2`: Crash rate <= 0.5% in internal sessions.
- `QG-PARITY-8.3.3`: Score reaches at least 92.

Phase exit criteria:
- System is stable enough for feature freeze and RC cycle.

### 6.10 Phase 9 - Release Candidate and Final Certification
Window: 2027-01-26 to 2027-02-20  
Phase score target: Sustain 90+ and certify release readiness

#### Subphase 9.1 - Feature Freeze and Defect Burn-Down (2027-01-26 to 2027-02-02)
Focus:
- Freeze scope and remove release-blocking defects.

Deliverables:
- Freeze announcement and accepted-change policy.
- Prioritized release defect board.

Quality gates:
- `QG-STAB-9.1.1`: Zero open scope creep exceptions without approval.
- `QG-STAB-9.1.2`: Blocker/Critical trend is strictly downward to zero.

#### Subphase 9.2 - RC1 Validation and Correction (2027-02-03 to 2027-02-10)
Focus:
- Execute RC1, triage regressions, and apply only release-safe fixes.

Deliverables:
- RC1 test and parity certification report.
- RC1-to-RC2 correction plan.

Quality gates:
- `QG-TEST-9.2.1`: Full release regression suite pass.
- `QG-PARITY-9.2.2`: RC1 score >= 90 and hard thresholds hold.

#### Subphase 9.3 - RC2 Certification and Sign-Off (2027-02-11 to 2027-02-20)
Focus:
- Final certification and release sign-off package.

Deliverables:
- RC2 final report.
- Known issues, change log, build/run guide.
- Signed release checklist.

Quality gates:
- `QG-PARITY-9.3.1`: RC2 score >= 90 and A/B/D/F hard thresholds pass.
- `QG-STAB-9.3.2`: Zero Blocker and zero Critical defects.
- `QG-DOC-9.3.3`: Release documentation package complete.

Phase exit criteria:
- Release candidate is approved for public milestone tag.

---

## 7. Domain Checklists (Summary)

## 7.1 A. Visual/Rendering (22)
- Block/item texture and model fidelity
- Transparent object ordering stability
- Sky/fog/time-of-day transitions
- Lighting and shading feel
- Particle and break animation feedback
- Camera/FOV/view bobbing/hand rendering

## 7.2 B. Core Mechanics (28)
- Movement and physics constant alignment
- Block placement/break/collision behavior
- Inventory/hotbar/slot behavior
- Crafting/smelting/item flow integrity
- Combat hit/damage/reaction alignment
- Save/load state consistency

## 7.3 C. Content (18)
- Core block set
- Core item/tool/armor set
- Core entity/mob set
- Required status/progression systems

## 7.4 D. World/Progression (16)
- Biome distribution quality
- Cave/ore/terrain density profile
- Structure generation and discovery flow
- End-to-end progression loop quality

## 7.5 E. UX/Audio (8)
- Complete menu flow
- HUD information density/readability
- Input settings persistence
- Correct feedback from sound and ambience

## 7.6 F. Quality/Stability (8)
- Automation coverage
- Long-session stability
- Save integrity
- Performance target attainment

---

## 8. Quality Gates and Operational Cadence

### 8.1 Weekly Cadence
- Monday: commit top 3 priorities for the week.
- Wednesday: integration check (build, tests, risk review).
- Friday: merge, record regression outcomes, update parity score.

### 8.2 PR Gates (Mandatory)
- Build and required CTest targets must pass.
- Include tests or explicit test-impact documentation.
- Include migration/integrity tests when world/save format is affected.

### 8.3 Defect SLA
- Blocker: mitigation plan within 24 hours.
- Critical: fix work starts within 72 hours.
- High: resolved within active milestone window.

---

## 9. Risk Register and Mitigation

| Risk | Impact | Mitigation |
|---|---|---|
| Residual `main.cpp` responsibility concentration | High refactor cost and regression risk | Enforce module split in Phase 1 |
| Save format evolution risk | Data corruption risk | Keep version headers and migration tests mandatory |
| Visual regressions from render changes | Major quality drop | Add scene-level screenshot diff checks |
| Slow content expansion | Delay in reaching 90% score | Prioritize highest score-contribution items first |
| Long-session instability | Late RC defect spikes | Start soak tests early in Phase 8 |

---

## 10. Immediate Backlog (Next 3 Weeks)

Window: 2026-02-26 to 2026-03-20

1. Create `docs/quality/parity-matrix-v1.md`.  
   Status: Completed (Subphase 0.1)
2. Implement domain-split CTest targets.  
3. Execute first `main.cpp` split (`AppBootstrap`, `InputController`).  
4. Add 5 architecture regression tests (container lifetime/state transitions).  
5. Add scripts for baseline scenario recording/log capture.

Completion criteria:
- 100% of Phase 0 gates passed.
- At least 30% of Phase 1 scope started and tracked.

---

## 11. Expected Score by Milestone End

| Milestone | Score Target |
|---|---:|
| End of Phase 0 (2026-03-08) | Baseline established |
| End of Phase 1 (2026-04-05) | 45+ |
| End of Phase 2 (2026-05-17) | 58+ |
| End of Phase 3 (2026-06-28) | 68+ |
| End of Phase 4 (2026-08-16) | 76+ |
| End of Phase 5 (2026-09-27) | 83+ |
| End of Phase 6 (2026-11-02) | 87+ |
| End of Phase 7 (2026-12-14) | 90+ |
| End of Phase 8 (2027-01-25) | 92+ |
| End of Phase 9 (2027-02-20) | Sustain 90+ and complete RC |

---

## 12. Conclusion

This roadmap is an execution and certification framework, not only a feature list.

Three operating rules define success:

1. Manage parity with measurable sprint-level scoring.  
2. Enforce phase transitions through quantitative gates.  
3. Reserve the final phases for release hardening and certification only.

If these rules are maintained, achieving and certifying 90%+ visual and mechanics parity by 2027-02-20 is realistic.
