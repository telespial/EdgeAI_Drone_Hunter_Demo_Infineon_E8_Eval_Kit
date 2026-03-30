# PROJECT_STATE

- Project: EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Last Updated: 2026-03-30
- Primary runtime file:
  - `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Baseline Summary
- Phases `1-14` are complete.
- Phase `15` is active.
- Current runtime includes:
  - strategic stochastic attacker release composition (doctrine-weighted, run-varying spawn order),
  - splash-start entropy reseed + reset to prevent boot-time replay patterns,
  - top-center foreground debug stage banner (`DBG:*`) for freeze tracing,
  - phase-14 movement doctrine,
  - phase-15 settings controls,
  - CIWS stationary-structure + dynamic stream behavior,
  - inventory exhaustion end mode,
  - flame/render freeze-hardening pass (reduced fire render budget + sprite update throttle),
  - render stability safe mode active for city-fire path to reduce freeze risk during active gameplay,
  - centralized fire profile mapping with weighted selection (75% bright, 25% dark),
  - hunter icon anti-flicker stabilization for both in-flight sprites and deck icons,
  - bottom deck liquid-glass opacity increased for readability,
  - bright fire visibility pass with even bright-color profile selection,
  - naming normalization (`Stinger`, `Pelican`, `TYTAN`),
  - deterministic ALGO with EDGEAI layered behavior/fallback model,
  - audio scheduler now includes city ambience/emergency plus attacker drone flyby cues (fixed-wing + FPV cadence),
  - actual PCM sample playback pipeline is active on codec/TDM output (embedded clips sourced from `sounds/`),
  - audio runtime now includes:
    - CIWS fire mapped to dedicated gun-sound event path (separate from kill explosion path),
    - CIWS source clip start offset constrained to >=3 seconds into the source file,
    - delayed ambulance response 8-15 seconds after each successful attacker hit,
    - randomized 3-6 second city/gunfire background segment scheduling,
    - elevated drone bed levels for persistent audibility,
    - latest requested master gain uplift,
    - max-fit city ambient asset footprint for linker-safe runtime,
    - continuous city bed with random `40-55` second segment windows and crossfade-to-start at segment boundary.
    - explosion assets extended to ~2x prior duration with stronger fade-out tails,
    - explosion mapping now actively uses two source recordings,
    - asset-size rebalance to remove city micro-loop wobble while maintaining build fit.
    - CIWS fire audio emit now triggers on every real burst (no shared cooldown suppression).
  - UI/runtime hardening:
    - hunter deck icon hidden flags are force-cleared each refresh to prevent random icon disappearance.
  - targeting/runtime fixes from latest pass:
    - persistent hunter terminal engagement (retry instead of immediate turn-away egress) with bounded forced-kill attempt cap,
    - lock-box coordinate-space fix for x-wing/small attack drones (arena-local alignment),
    - long-range commit-gate relaxation so distant uncovered threats are targeted earlier,
    - per-tick hunter target-index sanitization guard.
  - freeze investigation update (strategy/launch transition focus):
    - added explicit launch-target bounds guard in `update_hunter()` to prevent invalid target-slot access,
    - clamped city-fire loop cardinality/head state each tick (`city_fire_count`/`city_fire_head`) to prevent array overrun if state drifts,
    - clamped nearest-fire scan loop bound to `CITY_FIRE_MAX`.
  - known active issue:
    - freeze severity reduced with safe-mode + launch/state guards; extended soak still required to confirm full resolution.

## Verified Hardware
- Kit: `KIT_PSE84_EVAL`
- Device: `PSE846GPS2DBZC4A`
- Probe: `KitProg3`

## Verified Recovery Program Order
1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

Observed success signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 3923968 bytes` / `verified 3920440 bytes`
- `** Resetting Target **`

## Active Runbook Script
- `/home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh`

## Next Execution Focus
1. Audio overhaul kickoff:
   - replace speaker-test/pong audio with mapped gameplay soundscape (city/drone/explosion/emergency timing model).
2. Improve attacker/defender `ALGO` strategic behavior for more engaging play.
3. Add project settings/help files and integrate with current workflow.
4. Prepare flame redraw work package (detailed visual scope to be defined at start of that step).
5. Run extended soak validation to confirm freeze resolution under current stability guard settings.
