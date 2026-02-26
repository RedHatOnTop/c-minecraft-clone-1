# Subphase 1.4 Kickoff

Subphase: 1.4 - Phase 2 Entry Setup  
Roadmap reference: `docs/roadmap.md` (Section 6.2, Subphase 1.4)  
Kickoff date: February 26, 2026

## 1. Objective

Prepare execution-ready inputs for Phase 2.1 (Movement and Physics Alignment) without blocking on setup work during the first implementation sprint.

## 2. Initial Scope

- Freeze baseline metric harnesses now available in required tests:
  - `test_Physics_JumpApex_FixedTickProfile`
  - `test_WorldGen_BiomeDistribution_FunctionalBaseline`
  - `test_WorldGen_HeightAndDecoration_Profile`
- Align Phase 2 entry documents (handoff checklist, dependencies, gate references).
- Keep all required CTest targets green while integrating Phase 2 prep assets.

## 3. Initial Work Items

1. Publish Phase 2 entry checklist and ownership handoff.
2. Add metric-baseline interpretation notes for movement/worldgen tests.
3. Validate CI and local parity of required test targets after each integration commit.

## 4. Current Status

- Subphase started.
- Gate tracker published at `docs/quality/subphase-1.4-qg-report.md`.
