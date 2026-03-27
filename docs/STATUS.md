# STATUS

- Date: 2026-03-27
- State: Golden restore point promoted to latest CIWS-positioned baseline

## Current validated baseline
- `proj_cm55` rebuild path confirmed.
- `qprogram_proj` flash confirmed on hardware.
- Confirmed flash result:
  - `wrote 2314240 bytes`
  - `verified 2310160 bytes`

## UX/Gameplay baseline captured in this restore point
- Dual CIWS layout active with transparent CIWS render assets.
- Left CIWS correctly inward-facing and positioned lower-left.
- Right CIWS rotated/positioned per latest alignment request (right/down adjustment).
- CIWS tracer stream uses dotted bullet-style fire effect.
- Arena phase progression HUD remains active:
  - `PHASE 1: PATROL`
  - `PHASE 2: INTERCEPT`
  - `PHASE 3: SWARM`
- Continuous gameplay remains enabled.

## Environment notes
- EdgeProtect combine-sign step remains unavailable in this environment.
- Reliable programming flow is `build_proj` then `qprogram_proj MTB_SIGN_COMBINE__SKIP_CHECK=1`.
