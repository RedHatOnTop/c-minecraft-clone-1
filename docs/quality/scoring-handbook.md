# Parity Scoring Handbook (v1)

Version: v1  
Prepared for: Subphase 0.1  
Last updated: February 25, 2026

## 1. Purpose

This handbook defines how parity items are scored and how evidence is interpreted.
It is used together with `docs/quality/parity-matrix-v1.md`.

## 2. Item Scoring Scale (0-5)

| Score | Label | Definition |
|---:|---|---|
| 0 | Absent | Behavior/system is not implemented or not usable for comparison. |
| 1 | Stub | Minimal placeholder exists but is non-representative of 1.8.9 behavior. |
| 2 | Partial | Core behavior exists but major parity gaps remain in common scenarios. |
| 3 | Functional Baseline | Behavior is broadly correct for normal scenarios with visible edge-case gaps. |
| 4 | Near Match | Behavior is close to 1.8.9 in most target scenarios; only minor differences remain. |
| 5 | Parity Match | Behavior is functionally and visually equivalent in defined acceptance scenarios. |

## 3. Evidence Requirements by Score Band

| Score Band | Minimum Evidence Requirement |
|---|---|
| 0-1 | Design note or implementation note explaining missing/parital state. |
| 2-3 | At least one reproducible scenario and one objective metric/test output. |
| 4 | Objective metric + side-by-side comparison evidence with documented tolerance. |
| 5 | Multi-scenario evidence across representative cases + regression protection. |

## 4. Item-Level Interpretation Examples

The examples below are normative references for consistent scoring decisions.

## 4.1 Example A-04 (Transparent Sorting Stability)

Reference item: `A-04` in parity matrix.

| Score | Interpretation Example |
|---:|---|
| 0 | Transparent meshes render in arbitrary order with frequent severe artifacts. |
| 2 | Distance sorting exists, but artifacts still occur in common camera sweeps. |
| 3 | Most scenes are stable; artifacts appear in edge-case overlap patterns. |
| 4 | No severe artifacts in standard validation scenes; only minor edge flicker remains. |
| 5 | Stable in all defined validation scenes and protected by regression tests. |

## 4.2 Example B-07 (Inventory Slot Transfer Semantics)

Reference item: `B-07` in parity matrix.

| Score | Interpretation Example |
|---:|---|
| 0 | Core slot transfer actions are missing. |
| 2 | Basic move works, but split/merge/shift behavior often diverges. |
| 3 | Common slot behaviors match, with some edge mismatches under mixed stacks. |
| 4 | Slot behavior matches expected semantics in almost all tested scenarios. |
| 5 | Full scenario matrix passes with no parity-critical mismatch. |

## 4.3 Example D-05 (Structure Generation Availability)

Reference item: `D-05` in parity matrix.

| Score | Interpretation Example |
|---:|---|
| 0 | Target structure class is not generated. |
| 2 | Structure appears occasionally but missing in many valid seeds. |
| 3 | Structures generate reliably but with known distribution inconsistency. |
| 4 | Required structures appear in target seed coverage with minor density differences. |
| 5 | Required structures meet target availability and plausibility profile consistently. |

## 4.4 Example F-04 (Frame-Time Budget p95)

Reference item: `F-04` in parity matrix.

| Score | Interpretation Example |
|---:|---|
| 0 | p95 frame time far exceeds target budget and blocks playable quality. |
| 2 | p95 improves but frequently exceeds budget in normal gameplay scenes. |
| 3 | p95 is near budget in common scenes, with periodic spikes in heavy scenes. |
| 4 | p95 meets budget in reference scenes with limited outliers. |
| 5 | p95 consistently meets budget and regressions are automatically detected. |

## 5. Scoring Workflow

1. Select item ID from matrix and target scenario set.  
2. Collect evidence artifacts (tests, logs, screenshots, metrics).  
3. Assign provisional score based on handbook examples.  
4. Peer-review score and evidence links.  
5. Record final score and rationale in sprint parity update.

## 6. Tie-Break and Disagreement Policy

- If evidence quality is insufficient, the score cannot exceed 2.
- If visual and behavioral evidence conflict, prefer objective test/metric evidence.
- If reviewer disagreement remains, assign lower score and mark for re-evaluation in next sprint.
