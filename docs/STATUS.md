# STATUS

- Date: 2026-03-28
- State: Stable runtime baseline validated on hardware; Phases 1/2/3/4/5/6/7/8/9/10/11/12/13 are complete. Phase 14 remains planned.

## Current validated baseline
- `proj_cm55` rebuild/program path confirmed.
- `qprogram_proj` flash confirmed on hardware.
- Latest confirmed flash result:
  - `wrote 2408448 bytes`
  - `verified 2404648 bytes`
- Build quality confirmation:
  - using correct SDK path works for build + flashing:
    - `CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7`
    - `CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1`
    - `build_proj` and `qprogram_proj ... MTB_SIGN_COMBINE__SKIP_CHECK=1` succeed.
  - expected environment limitation remains:
    - `EdgeProtect Secure Suite not found. Combine-Sign step not executed.`

## Memory footprint
- External SMIF programmed image (`verified`):
  - `2,404,648 / 134,217,728` bytes used (`1.79%`),
  - `131,813,080` bytes remaining (`98.21%`).
- Internal RRAM fit check (512 KB):
  - current image would exceed internal-only capacity by `1,880,360` bytes (`~4.59x` too large).

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
  - inventory-exhaustion defender win/loss evaluation is active,
  - round-end overlay now reports causal summary metrics (wave/core/leaks/kills/stock/CIWS).
- Round-end doctrine update:
  - match now ends on inventory exhaustion mode:
    - defender loss when defender inventory/layer is exhausted,
    - defender win when attacker inventory is exhausted (after final mission wave).
- Strategy update:
  - CIWS friendly-fire doctrine now includes constrained hunter-fratricide risk windows.
  - Penalty model: hunter supply can be consumed while attacker kill outcome remains uncertain.
- CIWS doctrine tuning update:
  - each gun now starts with two 1,550-round magazines (3,100 rounds per gun),
  - ammo burn rate per trigger is 4x previous setting (`CIWS_AMMO_PER_TRIGGER = 24`),
  - engagement envelope tuned to effective `1.5 km` and hard cutoff `5.0 km`.
- Display stability update:
  - HUD refresh is now throttled to reduce occasional screen flicker from high-frequency text redraw.

## Phase 13 status (2026-03-28)
- Completed in runtime code:
  - continuous hunter re-steering while committed (turn-rate limited),
  - swept-hit interception checks to reduce frame-step tunneling misses,
  - target-loss reacquire path before forced miss/fall behavior,
  - telemetry counters added for swept-hit, reacquire, and overshoot events,
  - terminal guidance tuning pass completed (lead/speed calibration + near-intercept overshoot damping),
  - explicit WHY messaging added for target-loss/no-reacquire and terminal-evade misses.
- Hardware flash baseline updated with Phase 13 completion build:
  - `wrote 2408448 bytes`
  - `verified 2404648 bytes`

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
