# Baseline Scenario Suite Results (Subphase 0.2)

Suite ID: `baseline-scenario-suite-v1`  
Execution date: February 25, 2026  
Result: **PASS** (`9 / 10 = 90.00%`, threshold `>= 90%`)

## 1. Scenario Result Table

| Scenario ID | Scenario | Result | Evidence |
|---|---|---|---|
| S01 | Release build baseline | PASS | `docs/quality/baseline/2026-02-25/logs/build-release.log` |
| S02 | CTest baseline run | PASS | `docs/quality/baseline/2026-02-25/logs/ctest-release.log` |
| S03 | Test executable baseline run | PASS | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| S04 | Data migration scenario (`DataMigration_ChunkLegacyToV2_RoundTrip`) | PASS | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| S05 | Inventory safety scenario (`Inventory_AddItem_NullRegistrySafe`) | PASS | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| S06 | Raycast zero-component safety scenario | PASS | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| S07 | UI viewport projection scenario | PASS | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| S08 | Transparent sorting scenario | PASS | `docs/quality/baseline/2026-02-25/logs/test-exe.log` |
| S09 | Core asset presence audit | PASS | `docs/quality/baseline/2026-02-25/logs/asset-presence.log` |
| S10 | Interactive runtime capture (screenshots/video) | FAIL (blocked) | `docs/quality/baseline/2026-02-25/captures/screenshot-manifest.md`, `docs/quality/baseline/2026-02-25/captures/video-manifest.md` |

## 2. Pass-Rate Calculation

- Total scenarios: `10`
- Passed scenarios: `9`
- Failed/blocked scenarios: `1`
- Pass rate: `9 / 10 = 90.00%`

Subphase 0.2 gate threshold for `QG-TEST-0.2.1`: `>= 90%`  
Gate outcome: **PASS**

## 3. Notes

- The only failed scenario is an interactive capture step blocked in this terminal-only execution path.
- Log-based and test-based scenarios are reproducible and fully attached in the baseline log package.
