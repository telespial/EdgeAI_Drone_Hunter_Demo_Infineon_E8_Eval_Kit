# EdgeAI Drone Hunter - Roadmap and Phase Tracker

Date: 2026-04-07
Canonical rules source: `rules.md`
Primary runtime target: `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Program Status Snapshot
- Completed in code: Phases `1` to `14`
- In progress: Phase `15`
- Planned next: Phase `16`

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
| 14 | 360 Movement Doctrine + Dynamic Intercept Decisions | Complete |
| 15 | Cleanup + Settings + HUD Simplification + Flash Safety | In Progress |
| 16 | Audio Layer (City/Siren/Drone/Explosion/Gun) | Planned |

## Phase 15 Focus
1. HUD simplification and cleanup.
2. Visual-fire realism stabilization and freeze-proofing.
3. Repository and docs hygiene.
4. Flash/recovery safety hardening.
5. Strategic non-repeating attacker release sequencing + top debug-stage instrumentation.

## Phase 16 Preview
- Ambient city layer.
- Sirens tied to threat pressure.
- Drone flight audio cues.
- Explosion and CIWS burst audio cues.
- Mix priority/ducking.

## Priority Fix List (2026-04-07)
1. Investigate and fix intermittent gameplay freeze (top blocker):
   - freeze remains reproducible during gameplay despite prior hardening passes,
   - latest isolation mode disables hunters + CIWS so only attacker/leak/effects paths run,
   - action focus: isolate state/path transition that still produces lockup.
2. Replace pong speaker-test audio with full gameplay soundscape:
   - explosions, city ambience, firetrucks, ambulances, drone sounds, emergency escalation,
   - event-driven mapping and timing aligned with gameplay states.
3. Improve `ALGO` gameplay logic on attacker and defender for stronger strategy and more fun.
4. Add settings file and help file with user-facing guidance and controls reference.
5. Redraw flame visuals (detailed redraw scope to be defined when this step starts).
6. Add subtle city liveliness lighting:
   - tiny white/tungsten ground-level flickers across city footprint,
   - flicker sprite size target: `1px` to `3px` squares,
   - low-intensity, randomized timing/placement to avoid visual noise.
7. Exit attack-only isolation mode after freeze root-cause is confirmed:
   - re-enable hunter drones and CIWS in staged steps,
   - preserve validated impact-fireball-before-destroy behavior,
   - run long soak stability pass with mixed attacker roster (`x-wing` + fixed-wing).

## Plan For #1 (Audio Overhaul)
1. Audio asset inventory pass:
   - scan current `sounds/` library,
   - map self-descriptive file names to event categories (`ambient_city`, `drone_flight`, `explosion`, `ciws`, `siren`, `firetruck`, `ambulance`, `ui`).
2. Event-to-sound mapping table:
   - create a single source-of-truth table in code for trigger event, preferred clip set, volume, cooldown, and loop/one-shot behavior.
3. Timing and layering model:
   - define per-channel priorities and max concurrency,
   - set ducking rules (for example: explosion/CIWS briefly duck ambience, sustained emergency can ride over ambience),
   - set anti-spam cooldowns for rapid repeated events.
4. Game-state audio director:
   - idle/normal combat/high-threat/breach/emergency states,
   - escalate siren + emergency vehicle layers with city damage/threat pressure.
5. Runtime integration:
   - hook triggers into attacker launch, intercept, leak, city-hit, CIWS burst, and phase transition points.
6. Tuning and validation:
   - run long gameplay soaks,
   - verify no clipping, no starvation, and no event/audio desync,
   - freeze current audio baseline as the next restore-point candidate.
