# EdgeAI Drone Hunter - Roadmap and Phase Tracker

Date: 2026-03-28
Canonical rules source: `rules.md`
Primary runtime target: `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Program Status Snapshot
- Completed: Phases `1` to `13`
- Planned: Phases `14` and `15`
- Hardware baseline: restored and validated with full 3-image programming sequence (`cm33_s_signed`, `cm33_ns_shifted`, `cm55`).

## Phase Matrix
| Phase | Name | Status |
|---|---|---|
| 1 | Data Model Completion | Complete |
| 2 | Detection / Classification / Commit Pipeline | Complete |
| 3 | Threat Score + Prioritization Engine | Complete |
| 4 | Hunter Assignment + Matchup Matrix | Complete |
| 5 | Attacker Strategy Layer (16 Launch Sites) | Complete |
| 6 | CIWS Doctrine Finalization | Complete |
| 7 | HUD/UX Completion | Complete |
| 8 | Wrong-Choice Consequence System | Complete |
| 9 | Friendly-Fire / IFF Advanced Mode | Complete |
| 10 | Mission/Wave Structure and Difficulty | Complete |
| 11 | Win/Loss and Collateral Rules | Complete |
| 12 | Verification + Calibration + Restore Governance | Complete |
| 13 | Hunter Guidance + Intercept Geometry Hardening | Complete |
| 14 | 360 Movement Doctrine + Dynamic Intercept Decisions | Planned |
| 15 | Cleanup + Settings + HUD Simplification + Flash Safety | Planned |

---

## Phase 14 - Planned Scope
### Goal
Implement true all-direction tactical movement and decision arbitration between baseline ALGO and adaptive EDGEAI behavior.

### Required outcomes
- Attack drones:
  - can maneuver in all directions,
  - evade hunters,
  - detonate only at assigned launch target coordinates.
- Hunter drones:
  - can maneuver in all directions,
  - preserve lock persistence,
  - opportunistically intercept easier/path-conflict threats under deterministic gate rules.
- ALGO/EDGEAI runtime architecture:
  - ALGO and EDGEAI run on same control core,
  - EDGEAI may offload inference work to NPU/U55,
  - strict ALGO fallback on timeout/error/invalid EDGEAI result.

### Milestones
1. M14.1 data handoff and arbitration schema with telemetry.
2. M14.2 attacker evasion model under ALGO.
3. M14.3 hunter opportunistic switch gate + lock persistence policy.
4. M14.4 EDGEAI path with NPU acceleration + deterministic fallback.
5. M14.5 on-device profiling/tuning and closure criteria.

---

## Phase 15 - Planned Scope
### Goal
Finish UX/settings cleanup and repository hygiene, while preserving reliable flash safety.

### Required outcomes
- Repo cleanup:
  - scan and remove unrelated artifacts/docs where appropriate,
  - prevent framework-doc leakage into gameplay-facing docs.
- HUD simplification:
  - replace busy top HUD text with simple left/right `ATTACKER` vs `DEFENDER` score presentation.
- Settings popup controls:
  - `ATTACKER`: `ALGO/EDGEAI`
  - `DEFENDER`: `ALGO/EDGEAI/HUMAN`
  - `NPU`: `ON/OFF`
  - `DIFFICULTY`: `EASY/MED/HARD`
  - `SPEED++`: global simulation speed multiplier
- Visual polish:
  - improve fire rendering variety and realism.
- Safety:
  - enforce latest-artifact flashing path,
  - keep hunter lower-boundary bounce fix hardened.

---

## Immediate Execution Order
1. Implement Phase 14 runtime behavior and telemetry gates.
2. Validate Phase 14 on board and document closure criteria/results.
3. Execute Phase 15 settings/HUD cleanup and final UX polish.
4. Re-validate flash safety and publish next golden restore point.
