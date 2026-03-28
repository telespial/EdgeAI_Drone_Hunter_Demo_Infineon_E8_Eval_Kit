# STATUS

- Date: 2026-03-27
- State: Stable runtime baseline validated on hardware; hunter egress/top-edge behavior and CIWS tracer stream/range tuned on board.

## Current validated baseline
- `proj_cm55` rebuild/program path confirmed.
- `qprogram_proj` flash confirmed on hardware.
- Confirmed flash result:
  - `wrote 2375680 bytes`
  - `verified 2374456 bytes`
- Build quality confirmation:
  - `drone_hunter_arena.c` cleanup build has no compiler warnings in local build step.

## UX/Gameplay baseline
- Splash + lineup + `START ARENA` flow retained.
- Attack targets distributed across playable city area.
- Leak impacts produce persistent, flame-like fire patches.
- CIWS now uses gameplay-visible sweep range (~75% screen width) and engages reliably in arc.
- CIWS tracer stream tuned for density/visibility while preserving bounded range behavior.
- CIWS per-gun ammo now decreases by burst-sized consumption each trigger event (`CIWS_AMMO_PER_TRIGGER`), with faster deck-counter refresh.
- Hunter top-edge skid behavior fixed via forced horizon egress (shrink/fade/despawn) instead of line sliding.
- Falling hunters no longer bounce/reverse off lower icon/deck region:
  - failed/missed hunters continue descending off the bottom of the screen,
  - auto-reset occurs only after off-screen exit.
- Hunter launch origin now uses 8 hidden city sectors with nearest-target selection and nearest-stock fallback.
- Hunter vs Attacker scoring active.
- Attack drones now forced to generated attack render set at runtime (ODIN fallback removed):
  - `attack_shahed_yellow`
  - `attack_vb140_like_red`
  - `attack_dji_x_orange`
- Hunter launch logic now includes urgency fallback when commit gate confidence is low, preventing no-defense stalls.
- Hunter render/launch stability fixes applied:
  - removed forced Sting-only hunter selection fallback,
  - switched in-game Sting render path to stable sprite source,
  - added minimum visible flight window before intercept resolution,
  - tightened hunter zoom and bottom-edge clamps to prevent oversized icon-bar popups.
- Rules-engine progression active:
  - detect/classify/commit confidence,
  - dynamic threat score,
  - launch commit gating,
  - recommended counter logic.
- Phase 2 completion details:
  - Added LOS masking into commit gate.
  - Added track-history and noise shaping into detect/classify/commit confidence.
  - Added commit hold reason telemetry counters:
    - detect/class/confidence/corridor/LOS.
  - Reduced icon-bar flashing by throttling deck UI updates during CIWS firing.
- Phase 3 completion details:
  - Added weighted dynamic threat formula:
    - `Payload x Proximity/Urgency x Survivability x Confidence x TargetValueModifier x LanePressure`.
  - Added per-target `target_value_mod` and lane-pressure weighting.
  - Added lane-pressure model over 16 launch sites with decay + adjacent-lane spillover.
  - Added deterministic active-target priority ordering from computed threat score.
  - HUD telemetry now includes `TV` (target value) and `LP` (lane pressure) per active track.
- HUD now includes full live threat telemetry for active tracks:
  - type, speed, altitude, range-to-core, ETA, threat score, recommended counter.
- Code hygiene improvements:
  - Removed unused/dead helper functions from arena runtime.
  - Hardened HUD info text composition to avoid truncation-prone formatting.
  - Added runtime image-dimension guards and zoom clamps to prevent invalid giant sprites.
  - Clamped hunter/attacker movement to arena bounds to prevent icon-bar/deck bleed-through.
  - Reduced render pressure by capping simultaneous fire renders and removing heavy shadow load.
  - Removed attacker respawn foreground forcing that contributed to redraw flashing.

## Environment notes
- EdgeProtect combine-sign step remains unavailable in this environment.
- Reliable programming path is:
  - `build_proj` then
  - `qprogram_proj MTB_SIGN_COMBINE__SKIP_CHECK=1`.
