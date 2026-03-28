# STATUS

- Date: 2026-03-28
- State: Stable runtime baseline validated on hardware; Phases 1/2/3/5/6/8/9 are complete, Phase 10 is active.

## Current validated baseline
- `proj_cm55` rebuild/program path confirmed.
- `qprogram_proj` flash confirmed on hardware.
- Latest confirmed flash result:
  - `wrote 2404352 bytes`
  - `verified 2400360 bytes`
- Build quality confirmation:
  - `drone_hunter_arena.c` compiles/links cleanly; environment still lacks EdgeProtect combine-sign.

## UX/Gameplay baseline
- Splash + lineup + `START ARENA` flow retained.
- Attack goals distributed across playable city area.
- Leak impacts produce persistent, flame-like fire patches.
- CIWS finite-ammo behavior, sweep envelope, lock/heat penalties, and per-gun HUD telemetry are active.
- Hunter/Attacker score HUD, menu pill split-touch (`SET | HELP`), and `WHY` failure explainability are active.
- Phase 9 IFF advanced mode is active (strict gate for blue-on-blue only).
- Phase 10 wave pacing is active:
  - rotating wave archetypes: `SHAHED`, `X-SWARM`, `MIXED`, `TERM-SAT`,
  - composition-based scaling and auto mid-wave strategy shifts.
- Hunter launch origin allocator remains 8-sector with fallback.

## Explosion/FX baseline (2026-03-28)
- Explosion anchoring now uses rendered target center for both hunter kills and CIWS kills.
- Per-class blast profiles are active:
  - Shahed (yellow fixed-wing): giant orange explosion,
  - Strike-Prop (red fixed-wing): medium white circular explosion,
  - Strike-X (orange DJI X-wing): small bright white circular explosion.
- Explosion size now scales with depth perspective:
  - larger near screen bottom (closer),
  - smaller toward top of arena (farther).

## Environment notes
- EdgeProtect combine-sign step remains unavailable in this environment.
- Reliable programming path is:
  - `build_proj` then
  - `qprogram_proj MTB_SIGN_COMBINE__SKIP_CHECK=1`.
