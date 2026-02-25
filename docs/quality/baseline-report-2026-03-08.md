# Baseline Report (Published Ahead of Schedule)

Report ID: `baseline-report-2026-03-08`  
Publication date: February 26, 2026  
Roadmap reference: `docs/roadmap.md`, Subphase 0.3

## 1. Objective

Publish a reproducible baseline parity score and lock top parity gaps for the next phases.

## 2. Input Data and Reproducibility

Primary input dataset:
- `docs/quality/baseline/2026-02-25/data/baseline-ratings-2026-03-08.csv`

Computed baseline output:
- `docs/quality/baseline/2026-02-25/logs/qg-0.3-score.log`

Score model:
- Item score scale: `0-5` from parity matrix baseline ratings.
- Domain weighted score: `(domain_avg / 5) * domain_weight`.
- Total baseline score: sum of all domain weighted scores.

## 3. Published Baseline Score

| Domain | Weight | Domain Avg (0-5) | Weighted Score |
|---|---:|---:|---:|
| A. Visual/Rendering | 22 | 1.0000 | 4.40 |
| B. Core Mechanics | 28 | 0.6667 | 3.73 |
| C. Content | 18 | 0.6667 | 2.40 |
| D. World/Progression | 16 | 0.6667 | 2.13 |
| E. UX/Audio | 8 | 0.4000 | 0.64 |
| F. Quality/Stability | 8 | 1.6000 | 2.56 |

Published baseline total score: **15.87 / 100**

## 4. Baseline Interpretation

- The baseline confirms strong gap concentration in core mechanics parity and visual parity.
- Stability checks already show partial strength (`F` domain) due passing regression suite and persistence tests.
- World/content/UX parity has sparse baseline evidence and remains primarily `N/A` or `Partial`.

## 5. Top-20 Gap Lock

Locked backlog:
- `docs/quality/top-20-gap-backlog-2026-03-08.md`

Lock criteria:
- Severity-ranked by `gap_points = domain_weight * (5 - rating_score) / 5`.
- Traceable mapping to target roadmap phase/subphase.
- Owner stream and owner role assigned for each item.

## 6. Phase Transition Outcome

Subphase 0.3 deliverables are completed:
- Baseline score published.
- Top-20 gaps locked and traceable.

Phase 0 can transition to Phase 1 planning with fixed parity priorities.
