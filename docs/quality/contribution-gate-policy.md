# Contribution Gate Policy (Phase 1.3 Update)

Last updated: February 26, 2026  
Roadmap reference: `docs/roadmap.md` (Subphase 1.3, `QG-DOC-1.3.2`)

## 1. Purpose

This policy defines mandatory quality gates for pull requests so architecture and test-topology changes from Phase 1 remain stable.

## 2. Required Checks Per PR

Every PR must pass:

1. Configure and build in Release mode.
2. CTest pass at 100% for all required targets.
3. Scope-specific validation required by the changed subsystem.

Minimum command set:

```bash
cmake -S . -B build -DMC_OFFLINE_FETCHCONTENT=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## 3. Required Test Targets

The required CTest targets are:

- `tests_data`
- `tests_runtime`
- `tests_render`
- `tests_gameplay_smoke`

PRs that break any required target cannot be merged.

## 4. Test Impact Requirements

When code changes touch the following areas, additional expectations apply:

- Persistence/chunk/entity serialization paths:
  - Update or add data round-trip tests under `tests_data`.
  - Document backward-compatibility impact in PR notes.
- Input, interaction, gameplay rules:
  - Update or add deterministic replay tests under `tests_gameplay_smoke`.
- Render/UI/HUD behavior:
  - Update or add regression coverage under `tests_render`.
- Shared utility/runtime behavior:
  - Ensure `tests_runtime` still provides direct regression coverage.

If no new automated tests are added, the PR description must explain why existing tests are sufficient.

## 5. Documentation Requirements

PRs affecting roadmap-tracked quality gates must update related docs:

- Gate evidence logs under `docs/quality/logs/`
- Subphase QG report under `docs/quality/`
- Roadmap status in `docs/roadmap.md` when a gate milestone changes

## 6. Merge Blocking Conditions

A PR is blocked from merge when any condition below is true:

- Configure/build failure.
- Any required CTest target failure.
- Missing test-impact note for changed subsystem.
- Missing roadmap/QG documentation updates for gate-impacting changes.
