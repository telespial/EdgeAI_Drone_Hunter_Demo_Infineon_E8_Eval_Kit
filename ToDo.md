# EdgeAI Drone Hunter - Build Roadmap (ToDo)

## Current Status Snapshot
- Hardware target: Infineon E8 Eval Kit (`PSE846GPS2DBZC4A`).
- Scope: this project only (`EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit`).
- Latest visual update: title -> image-based 8-drone 3D carousel -> `START ARENA`.
- Carousel issue tracked/fixed: clipping at large zoom mitigated via direct `lv_image` orbit objects and smaller base sprite dimensions.
- Gameplay direction locked:
  - 8 Ukrainian hunter drone types are shown as deployable inventory icons at bottom of arena.
  - Attack swarms are finite per wave and escalate from hundreds toward thousands.
  - One hunter deployment can neutralize only one attack drone (both are one-time-use).
  - Enemy wave faction alternates by wave (`RUS ATTACK SWARM` / `USA ATTACK SWARM` label model).

## Phase 0 - Platform Bring-Up and Identity [Done]
- Boot firmware, initialize LCD/peripherals.
- Preserve startup splash: `EdgeAI Drone Hunter`.
- Verify build + flash flow on hardware.
- Establish golden restore baseline for startup scene.

## Phase 1 - Core Arena Foundation [Done]
- Build autonomous arena loop and spawn/update timers.
- Add hunter/target entities and core asset objective.
- Add HUD baseline and round timer/core health.
- Add round reset/restart handling.

## Phase 2 - Comparison Modes and Target Classes [Partially Done]
### Done
- Mode set implemented: `ALGO vs ALGO`, `EdgeAI vs EdgeAI`, `ALGO vs EdgeAI`.
- Removed over-drone text labels for cleaner visuals.
- Added fixed-wing class behavior and larger silhouette.
- Added heading-locked orientation for fixed-wing targets.
- Added triangular/delta wing styling pass.
### Remaining
- Increase visual distinction between hunter teams (shape/trail language).
- Tighten spawn cadence so each mode difference is obvious in <10 seconds.

## Phase 3 - Fleet Inventory + Wave Economy [In Progress]
### Done
- Added bottom 8-drone inventory deck UI in arena with per-type remaining counts.
- Added finite hunter stock per type and depletion on intercept (one hunter consumed per kill).
- Added finite attack pool per wave and automatic wave progression.
- Added wave/faction HUD strip and destroy/leak counters.
### Remaining
- Add touch selection for manual hunter type deployment priority from bottom deck.
- Add "out of stock" warning pulse and optional auto-pause when no hunter inventory remains.
- Balance per-type stock values against their profile speed/range behavior.

## Phase 4 - Behavior + Clarity Pass [Next]
- Make role readability instant (hunters vs threats) without labels.
- Tune ALGO behavior to show reactive chase and more overshoot.
- Tune EdgeAI behavior to show predictive lead and smoother intercept arcs.
- Add lightweight event FX: intercept pop, core-hit pulse, threat-spawn cue.
- Add short demo-ready round pacing and clearer win conditions.

## Phase 5 - Scoring, Metrics, and Explainability [Planned]
- Track per-team intercept count, misses, core damage prevented.
- Add time-to-intercept and miss-distance metrics.
- Show per-round comparison summary screen.
- Add deterministic scenario seeds for fair A/B runs.

## Phase 6 - Mission Structure and Difficulty [Planned]
- Introduce wave compositions (FPV-heavy, fixed-wing-heavy, mixed).
- Add escalating difficulty via speed/turn-rate/spawn pressure.
- Add mission states: warmup, peak threat, end-of-round review.
- Add explicit "hundreds to thousands" wave ladder with caps and pacing controls.

## Phase 7 - Demo UX and Packaging [Planned]
- Add automated mode-cycle for unattended booth/demo runs.
- Finalize docs for clone/build/flash self-contained workflow.
- Cut golden restore tags for stable demo milestones (failsafe only on explicitly approved freeze).

## Documentation and Repo Hygiene [In Progress]
- Keep docs self-contained in this repo for pull/build/flash reproducibility.
- Keep `start_here.md` and project docs synchronized with actual build commands.
- Capture final architecture notes for ALGO vs EdgeAI behavior differences.

## Immediate Next Task
- Complete Phase 3 remaining items (manual bottom-deck selection + no-stock handling), then tune wave scaling on hardware.
