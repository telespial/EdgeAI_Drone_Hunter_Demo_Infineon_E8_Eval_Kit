# STATUS

- Date: 2026-03-27
- State: Golden + failsafe restore points updated to full target-telemetry HUD baseline.

## Current validated baseline
- `proj_cm55` rebuild/program path confirmed.
- `qprogram_proj` flash confirmed on hardware.
- Confirmed flash result:
  - `wrote 2367488 bytes`
  - `verified 2362856 bytes`

## UX/Gameplay baseline
- Splash + lineup + `START ARENA` flow retained.
- Attack targets distributed across playable city area.
- Leak impacts produce persistent, flame-like fire patches.
- CIWS constrained to near-front, short-range envelope (cannot effectively reach far/top grids).
- Hunter vs Attacker scoring active.
- Rules-engine progression active:
  - detect/classify/commit confidence,
  - dynamic threat score,
  - launch commit gating,
  - recommended counter logic.
- HUD now includes full live threat telemetry for active tracks:
  - type, speed, altitude, range-to-core, ETA, threat score, recommended counter.

## Environment notes
- EdgeProtect combine-sign step remains unavailable in this environment.
- Reliable programming path is:
  - `build_proj` then
  - `qprogram_proj MTB_SIGN_COMBINE__SKIP_CHECK=1`.
