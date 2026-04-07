# PROJECT_STATE

- Project: EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Last Updated: 2026-04-07
- Primary runtime file:
  - `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Baseline Summary
- Phases `1-14` are complete.
- Phase `15` is active.
- Current runtime includes:
  - 2026-04-07 attack-only isolation mode:
    - `ENABLE_RUNTIME_AUDIO=1`,
    - `DISABLE_ATTACK_SUCCESS_FIREBALLS=0`,
    - `DISABLE_HUNTER_DRONES=1`,
    - `DISABLE_CIWS_GUNS=1`,
    - round-end defender-exhaustion bypass when both defender layers are disabled,
    - attackers leak to city uninterrupted for freeze-isolation testing.
  - mixed attacker roster enabled:
    - x-wing attack drones re-enabled (`DISABLE_XWING_ATTACK_DRONES=0`),
    - fixed-wing attackers enabled,
    - force-Shahed-only disabled (`FORCE_SHAHED_ONLY_DRONES=0`).
  - attacker city-hit sequence update:
    - impact now renders blast/fireball first,
    - target then enters short dying window,
    - respawn occurs after the visible blast window.
  - rollback baseline restored to last stable gameplay logic branch, while keeping only the smaller terminal impact-size render tweak.
  - top-center liquid-glass HUD now includes touch audio mute toggle (`AUDIO ON` / `AUDIO OFF`).
  - ground-stall livelock guards:
    - hunter near-floor stall timeout triggers horizon egress escape,
    - attacker near-floor stall timeout triggers forced respawn.
  - strategic stochastic attacker release composition (doctrine-weighted, run-varying spawn order),
  - splash-start entropy reseed + reset to prevent boot-time replay patterns,
  - top-center debug stage banner (`DBG:*`) is available for freeze tracing when compile-time enabled,
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
  - freeze investigation update (city-fire insertion isolation):
    - added centralized city-fire state sanitizer and insert-index fail-safe in `add_city_fire()`,
    - added explicit debug markers around city-fire insertion path:
      - `DBG:CITY_FIRE_ADD`,
      - `DBG:CITY_FIRE_DONE`.
  - freeze investigation update (audio/runtime race focus):
    - bounded audio FIFO fill with no-progress break conditions,
    - removed non-ISR heartbeat mixer mutation,
    - made audio event queue push IRQ-safe,
    - moved looped city event application into single audio context path,
    - retained arena `DBG:*` tracer instrumentation in code path (compile-time visibility currently off).
  - known active issue:
    - freeze remains unresolved and is still the top blocker:
      - latest user observations indicate runtime can still lock during gameplay after 30-180s windows,
      - working hypothesis remains decision/path/state-dependent livelock.
  - additional open issues (newly prioritized):
    - `Pelican`, `TYTAN`, and `Merops` icon visibility can still blink on/off intermittently,
    - fireballs need additional rendering stabilization for consistent visual behavior.
  - latest attack-impact pass:
    - Shahed dive behavior now applies stronger terminal acceleration and deeper near-ground scale reduction,
    - city-hit explosions/fire use attacker visual center to keep impact effects aligned to landing point.

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
- `wrote 3907584 bytes` / `verified 3903408 bytes`
- `** Resetting Target **`

## Active Runbook Script
- `/home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh`

## Next Execution Focus
1. Freeze root-cause isolation and fix (highest priority):
   - continue using attack-only mode to isolate attacker/effects paths without defender interactions,
   - validate if freeze persists when only city-impact/fireball paths are active.
2. Fix hunter icon blink issue (`Pelican`, `TYTAN`, `Merops`) and validate stable icon visibility.
3. Re-add/stabilize fireball rendering and verify impact-centered visual correctness across drone classes.
4. Re-enable defenders in staged order (hunters first, then CIWS) once freeze path is characterized.
5. Improve attacker/defender `ALGO` strategic behavior for more engaging play.
6. Add project settings/help files and integrate with current workflow.
7. Prepare flame redraw work package (detailed visual scope to be defined at start of that step).
8. Run extended soak validation to confirm freeze resolution under current stability guard settings.
