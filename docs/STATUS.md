# STATUS

- Date: 2026-03-27
- State: Golden + failsafe restore points updated to rules-driven Hunter vs Attacker baseline with distributed city targets, persistent leak fires, and last-ditch CIWS behavior.

## Current validated baseline
- `proj_cm55` rebuild/program path confirmed.
- `qprogram_proj` flash confirmed on hardware.
- Confirmed flash result:
  - `wrote 2367488 bytes`
  - `verified 2362856 bytes`

## UX/Gameplay baseline
- Splash + lineup + `START ARENA` flow retained.
- Arena now uses explicit score model:
  - `Hunter` points increase on successful intercepts.
  - `Attacker` points increase on leaked impacts.
- Attack target destinations are spread across playable city area.
- Leak impacts now:
  - trigger visible city-target explosions,
  - decrement `CORE`,
  - leave persistent city fires.
- Persistent fires scale upward as defender falls behind.
- CIWS baseline:
  - dual left/right guns,
  - per-gun finite ammo,
  - sweep-cone targeting,
  - effective/hard-cutoff engagement behavior,
  - overuse/heat penalty pressure.
- Attack classes remain image-based and color-distinct (yellow/red/orange).
- Continuous phase-driven gameplay remains enabled.

## Environment notes
- EdgeProtect combine-sign step remains unavailable in this environment.
- Reliable programming path is:
  - `build_proj` then
  - `qprogram_proj MTB_SIGN_COMBINE__SKIP_CHECK=1`.
