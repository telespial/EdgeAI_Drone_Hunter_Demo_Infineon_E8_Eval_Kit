# STATUS

- Date: 2026-03-28
- State: Stable runtime baseline validated on hardware; Phases 1/2/3/4/5/6/7/8/9/10/11/12 are complete. Phases 13 and 14 are planned for guidance/movement doctrine hardening.

## Current validated baseline
- `proj_cm55` rebuild/program path confirmed.
- `qprogram_proj` flash confirmed on hardware.
- Latest confirmed flash result:
  - `wrote 2404352 bytes`
  - `verified 2403032 bytes`
- Build quality confirmation:
  - using correct SDK path works for flashing:
    - `CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7`
    - `qprogram_proj ... MTB_SIGN_COMBINE__SKIP_CHECK=1` succeeds.
  - build still fails in this shell because GCC package is not resolved by the current Modus setup.

## Memory footprint
- External SMIF programmed image (`verified`):
  - `2,403,032 / 134,217,728` bytes used (`1.79%`),
  - `131,814,696` bytes remaining (`98.21%`).
- Internal RRAM fit check (512 KB):
  - current image would exceed internal-only capacity by `1,878,744` bytes (`~4.58x` too large).

## UX/Gameplay baseline
- Splash + lineup + `START ARENA` flow retained.
- Attack goals distributed across playable city area.
- Leak impacts produce persistent, flame-like fire patches.
- CIWS finite-ammo behavior, sweep envelope, lock/heat penalties, and per-gun HUD telemetry are active.
- Hunter/Attacker score HUD, menu pill split-touch (`SET | HELP`), and `WHY` failure explainability are active.
- Phase 9 IFF advanced mode is active (strict gate for blue-on-blue only).
- Phase 10 wave pacing is complete:
  - rotating wave archetypes: `SHAHED`, `X-SWARM`, `MIXED`, `TERM-SAT`,
  - composition-based scaling with mission milestones (`OPENING`, `ESCALATE`, `CRISIS`, `SATURATE`),
  - auto mid-wave + late-wave strategy shifts in `AUTO` mode.
- Hunter launch origin allocator remains 8-sector with fallback.
- Phase 4 hunter assignment is complete:
  - matchup-bias selection is active by target class,
  - no-fit fallback now switches to recommended or highest-stock class with explicit WHY messaging,
  - no-stock fallback and launch-sector depletion now report explicit WHY messaging.
- Phase 7 HUD/UX is complete:
  - defender telemetry now includes stock/endurance, CIWS cooldown/lock, envelope fit, and availability/lockout indicators.
- Phase 11 win/loss and collateral rules are complete:
  - strategic defender win/loss evaluation is active,
  - round-end overlay now reports causal summary metrics (wave/core/leaks/kills/stock/CIWS).

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
