# STATUS

- Date: 2026-03-27
- State: Golden + failsafe restore points updated to persistent-fire + near-front CIWS envelope baseline.

## Current validated baseline
- `proj_cm55` rebuild/program path confirmed.
- `qprogram_proj` flash confirmed on hardware.
- Confirmed flash result:
  - `wrote 2367488 bytes`
  - `verified 2362856 bytes`

## UX/Gameplay baseline
- Splash + lineup + `START ARENA` flow retained.
- Attack targets are distributed across playable city area.
- Leak impacts now create persistent city fire patches.
- Fire clusters are larger and flame-like (flicker + glow), and persist for the round.
- Fire pressure scales with attacker advantage.
- CIWS behavior:
  - dual guns with per-gun finite ammo,
  - short-range doctrine (effective/hard-cutoff),
  - restricted vertical/top-grid engagement envelope,
  - reduced tracer lifetime/speed so visuals match limited reach.
- Score model remains explicit: `Hunter` vs `Attacker`.

## Environment notes
- EdgeProtect combine-sign step remains unavailable in this environment.
- Reliable programming path is:
  - `build_proj` then
  - `qprogram_proj MTB_SIGN_COMBINE__SKIP_CHECK=1`.
