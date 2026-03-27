# STATUS

- Date: 2026-03-27
- State: Golden + failsafe restore points promoted to latest sprite-cleanup + CIWS ballistic + 4-edge attack spawn baseline with Sting-II normalization and play-area nose-forward flight orientation

## Current validated baseline
- `proj_cm55` rebuild path confirmed.
- `qprogram_proj` flash confirmed on hardware.
- Confirmed flash result:
  - `wrote 2314240 bytes`
  - `verified 2311448 bytes`

## UX/Gameplay baseline captured in this restore point
- Dual CIWS layout active with transparent render assets.
- Left and right CIWS use 90-degree sweep arcs instead of pure target-lock behavior.
- CIWS engagement range is capped to 75% of screen width from each CIWS origin.
- CIWS tracer stream behaves as dense ballistic bullet fire:
  - High-rate burst-dot emission.
  - Stronger curvature near horizontal sweep angles due to gravity.
- Attack drones enter from all four edges (top/bottom/left/right).
- Hunter visuals cleaned and normalized:
  - Sprite artifacts removed via normalized source conversion.
  - Extra fallback geometry overlays hidden on hunter play pieces.
  - Bottom icon labels and counts centered under each icon.
- Sting-II normalization for this checkpoint:
  - Carousel Sting-II scaled down to align with other lineup drones.
  - Bottom Sting-II icon path restored to icon sprite and normalized to standard deck zoom.
- Orientation scope for this checkpoint:
  - Icon and carousel orientations are kept at prior baseline behavior.
  - In-play drones in arena flight use nose-forward orientation tied to heading.
- Arena phase progression HUD remains active:
  - `PHASE 1: PATROL`
  - `PHASE 2: INTERCEPT`
  - `PHASE 3: SWARM`
- Continuous gameplay remains enabled.

## Environment notes
- EdgeProtect combine-sign step remains unavailable in this environment.
- Reliable programming flow is `build_proj` then `qprogram_proj MTB_SIGN_COMBINE__SKIP_CHECK=1`.
