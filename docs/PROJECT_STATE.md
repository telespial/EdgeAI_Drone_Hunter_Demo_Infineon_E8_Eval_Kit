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
  - full animated city-fire rendering active (safe-mode fallback disabled),
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
  - known active issue:
    - intermittent gameplay freeze still observed in some runs; freeze-hardening remains an active priority item.

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
- `wrote 3932160 bytes` / `verified 3927688 bytes`
- `** Resetting Target **`

## Active Runbook Script
- `/home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh`

## Next Execution Focus
1. Audio overhaul kickoff:
   - replace speaker-test/pong audio with mapped gameplay soundscape (city/drone/explosion/emergency timing model).
2. Improve attacker/defender `ALGO` strategic behavior for more engaging play.
3. Add project settings/help files and integrate with current workflow.
4. Prepare flame redraw work package (detailed visual scope to be defined at start of that step).
5. Investigate/fix intermittent runtime freeze and publish a stabilized soak-tested baseline.
