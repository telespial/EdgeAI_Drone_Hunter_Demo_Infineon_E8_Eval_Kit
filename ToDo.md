# EdgeAI Drone Hunter - Roadmap and Phase Tracker

Date: 2026-03-28
Canonical rules source: `rules.md`
Primary runtime target: `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Program Status Snapshot
- Completed in code: Phases `1` to `14`
- In progress: Phase `15`
- Planned next: Phase `16`
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
| 14 | 360 Movement Doctrine + Dynamic Intercept Decisions | Complete (built and flashed on board) |
| 15 | Cleanup + Settings + HUD Simplification + Flash Safety | In Progress |

---

## Phase 14 - Implemented Scope
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

### Implementation notes (2026-03-28)
- Added attacker ALGO baseline movement arbitration using goal vector + hunter-avoidance vector.
- Added attacker EDGEAI override path with predictive hunter avoidance and deterministic ALGO fallback.
- Added Phase-14 telemetry counters:
  - `attacker_algo_ticks`, `attacker_edgeai_overrides`, `attacker_edgeai_fallbacks`,
  - `attacker_evasion_events`, `attacker_goal_detonations`,
  - `h_opportunistic_switch_events`.
- Added hunter lock-persistence and switch-cooldown timers with opportunistic target switching gates:
  - switch only when path/easy-target conditions satisfy abandon-risk constraints.
- Attack drones still detonate only at assigned goal coordinates on arrival.

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

### First Steps Started (2026-03-28)
- Completed:
  - Repository inventory scan to identify cleanup/leak-risk docs and mirrors.
  - TODO single-source check confirmed:
    - canonical roadmap remains `ToDo.md`,
    - `docs/TODO.md` and `firmware_kit_epc2/docs/TODO.md` are pointer-only mirrors.
  - Initial leak-risk file inventory captured in `docs/PHASE15_KICKOFF.md`.
  - Lower icon/HUD deck bounce hardening completed:
    - added shared combat floor boundary,
    - applied floor clamp to attacker/hunter update + render paths,
    - validated with fresh full 3-image flash pass.
  - Settings popup touch controls integrated and validated:
    - `ATTACKER`: `ALGO/EDGEAI`,
    - `DEFENDER`: `ALGO/EDGEAI/HUMAN`,
    - `NPU`: `ON/OFF`,
    - `DIFFICULTY`: `EASY/MED/HARD`,
    - `SPEED++`: `NORMAL/FAST+/FAST++`.
  - Runtime behavior wiring added for settings:
    - control mode application for attacker/defender,
    - attacker EDGEAI path gated by `NPU`,
    - difficulty/speed++ movement multipliers.
- Next queued:
  - Remove or quarantine non-gameplay-facing `start_here`/framework guidance from gameplay docs.
  - Implement simplified top HUD scoreboard (`ATTACKER` left, `DEFENDER` right) replacing dense telemetry text.
  - Begin first pass on improved fire rendering variants/sizing.

---

## Immediate Execution Order
1. Execute Phase 15 settings/HUD cleanup and final UX polish.
2. Re-validate flash safety after Phase 15 and publish next golden/failsafe update.
3. Start Phase 16 audio layer implementation.

---

## Phase 16 - Audio Layer (City + Combat Soundscape)
### Goal
Add an immersive and readable audio layer for gameplay awareness and atmosphere.

### Required outcomes
- City ambience/noise bed (wind/urban background).
- Air-raid and warning sirens tied to threat pressure and wave intensity.
- Drone audio cues:
  - inbound attacker drone presence and proximity scaling,
  - hunter drone launch/flight cues.
- Combat audio cues:
  - explosion variants by class/scale,
  - CIWS/gun burst audio with burst-size differentiation.
- Mix behavior:
  - prevent clipping during heavy action,
  - prioritize critical cues (sirens, near impacts, active gunfire) over ambience.

### Milestones
1. M16.1 audio event model and trigger mapping from gameplay states.
2. M16.2 core SFX hooks (guns, explosions, drone movement, launches).
3. M16.3 adaptive siren/ambience layer tied to mission pressure.
4. M16.4 volume ducking/priority mix and on-device tuning.
5. M16.5 settings integration (master/effects/ambience) and validation.
