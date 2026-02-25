# Top-20 Gap Backlog (Locked)

Backlog ID: `top-20-gap-backlog-2026-03-08`  
Lock date: February 26, 2026  
Source: `docs/quality/baseline/2026-02-25/logs/qg-0.3-score.log`

## 1. Ranking Method

Severity ranking metric:
- `gap_points = domain_weight * (5 - rating_score) / 5`
- Higher `gap_points` indicates higher parity risk.

Tie-break rule:
- If gap points tie, prioritize earlier roadmap phase and broader gameplay impact.

## 2. Locked Top-20 Gaps

| Rank | Item ID | Item | Gap Points | Severity Tier | Target Phase | Target Subphase | Owner Stream | Owner Role |
|---:|---|---|---:|---|---:|---|---|---|
| 1 | B-02 | Jump apex and fall timing | 28.0 | Critical | 2 | 2.1 | WS-GAME | Physics Owner |
| 2 | B-03 | Ground friction deceleration | 28.0 | Critical | 2 | 2.1 | WS-GAME | Physics Owner |
| 3 | B-05 | Block break duration and tool modifiers | 28.0 | Critical | 2 | 2.2 | WS-GAME | Interaction Owner |
| 4 | B-06 | Block placement validity rules | 28.0 | Critical | 2 | 2.2 | WS-GAME | Interaction Owner |
| 5 | B-08 | Crafting recipe resolution | 28.0 | Critical | 2 | 2.2 | WS-GAME | Crafting Owner |
| 6 | B-09 | Smelting state transitions | 28.0 | Critical | 2 | 2.3 | WS-GAME | Smelting Owner |
| 7 | A-02 | Block model geometry alignment | 22.0 | High | 5 | 5.1 | WS-RENDER | Geometry Owner |
| 8 | A-03 | Lighting step accuracy | 22.0 | High | 5 | 5.1 | WS-RENDER | Lighting Owner |
| 9 | A-05 | Fog curve behavior | 22.0 | High | 5 | 5.2 | WS-RENDER | Atmosphere Owner |
| 10 | A-06 | Time-of-day sky transition | 22.0 | High | 5 | 5.2 | WS-RENDER | Atmosphere Owner |
| 11 | A-07 | Particle event coverage | 22.0 | High | 5 | 5.3 | WS-RENDER | Effects Owner |
| 12 | C-03 | Armor and durability baseline | 18.0 | High | 7 | 7.1 | WS-GAME | Item Systems Owner |
| 13 | C-04 | Core hostile mob set availability | 18.0 | High | 3 | 3.2 | WS-GAME | AI/Entity Owner |
| 14 | C-05 | Utility block set availability | 18.0 | High | 7 | 7.2 | WS-GAME | Block Systems Owner |
| 15 | C-06 | Core status-effect set availability | 18.0 | High | 7 | 7.2 | WS-GAME | Effects Owner |
| 16 | B-01 | Walk and sprint velocity profile | 16.8 | Medium | 2 | 2.1 | WS-GAME | Physics Owner |
| 17 | B-04 | AABB collision resolution | 16.8 | Medium | 2 | 2.1 | WS-GAME | Physics Owner |
| 18 | B-07 | Inventory slot transfer semantics | 16.8 | Medium | 2 | 2.2 | WS-GAME | Inventory Owner |
| 19 | D-01 | Biome distribution quality | 16.0 | Medium | 4 | 4.1 | WS-WORLD | WorldGen Owner |
| 20 | D-02 | Terrain height profile | 16.0 | Medium | 4 | 4.1 | WS-WORLD | WorldGen Owner |

## 3. Traceability Notes

- All ranked items are mapped to explicit roadmap phase/subphase targets.
- All ranked items are present in baseline scoring input CSV.
- Ranking can be reproduced from:
  - `docs/quality/baseline/2026-02-25/data/baseline-ratings-2026-03-08.csv`
  - `docs/quality/baseline/2026-02-25/logs/qg-0.3-score.log`
