# PROJECT_STATE

- Project: EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Last Updated: 2026-03-27
- Repo Root: /home/user/projects/embedded/codemaster/projects/Infineon/PSOC_EDGE_E8_EVAL/projects/EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Build Target: `firmware_kit_epc2/proj_cm55`
- Build Command: `make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8`
- Flash Command: `make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1`
- Programmed Image: `firmware_kit_epc2/proj_cm55/build/last_config/proj_cm55.hex`
- Status File: `docs/STATUS.md`

## Core implementation files
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_attack_images.h`
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/attack_shahed_yellow.c`
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/attack_vb140_like_red.c`
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/attack_dji_x_orange.c`
- `rules.md`

## Current behavior in code
- Phase 5 attacker strategy layer is implemented and active:
  - profile set: `AUTO`, `CENTER`, `FLANK`, `MIXED`, `TERMINAL`,
  - 4-edge spawn balancing is retained,
  - lane selection is strategy-driven on top of 16-site model.
- On-device strategy selection is wired:
  - mode button click cycles game mode,
  - mode button long-press cycles attacker strategy and resets round.
- HUD wave line includes live strategy telemetry (`STRAT`).
- Hunter launch origin distribution now uses 8 hidden city sectors:
  - nearest-sector first selection per target,
  - automatic nearest-stock fallback when local sector is depleted.
- Attack drone runtime rendering is locked to generated attack sprites (ODIN no longer used in attacker path).
- Hunter launch path includes urgency fallback so defense still launches when strict commit confidence is temporarily low.
- Hunter selection/render stability fixes:
  - removed forced Sting-only fallback in hunter pick sanitizer,
  - switched in-game Sting render to stable sprite source,
  - added minimum visible launch window before intercept resolution,
  - tightened hunter zoom and ground clamp to prevent oversized bottom-edge artifacts.
- CIWS uses gameplay arc range scaling (`CIWS_RANGE_FRAC`) instead of map-km hard cutoff, restoring visible engagements.
- CIWS ammo accounting now subtracts a burst-sized amount per trigger event (`CIWS_AMMO_PER_TRIGGER`) with more frequent HUD counter updates.
- CIWS tracer stream is tuned for denser/clearer bullets with bounded range and narrowed coverage width.
- Hunter top-edge behavior now prevents blue-line skating by forcing horizon egress (shrink/fade/despawn) at top-boundary contact.
- Hunter lower-region fall behavior now exits off-screen downward after misses instead of bouncing at the icon/deck boundary.
- Phase 3 threat-prioritization engine is now fully wired:
  - weighted threat score includes target-value and lane-pressure multipliers,
  - 16-site lane-pressure model with decay + adjacent-lane spillover,
  - deterministic active-target priority ranking from live threat score.
- Persistent city-fire model from leak impacts, with scaling pressure.
- CIWS near-front short-range gating + tracer visual range alignment.
- Per-target runtime model fields implemented:
  - speed estimate, altitude norm, range-to-core, ETA-to-goal,
  - detect/classify confidence, threat score,
  - priority rank and recommended counter.
- HUD telemetry now surfaces these threat metrics live for active tracks.
- Detect/classify/commit pipeline is now fully gated with:
  - corridor checks,
  - LOS checks,
  - track history + noise influence,
  - explicit commit-reason accounting for hold/launch paths.
- Continuous phase-driven gameplay retained.
- Runtime code hygiene pass completed on `drone_hunter_arena.c`:
  - dead/unused helper functions removed,
  - HUD line formatting hardened,
  - workspace restored to normal gameplay baseline.
- Runtime stability hotfix applied for launch-time corruption:
  - hunter ground clamp moved to arena floor (prevents sprites dropping into icon deck),
  - render-time dimension sanity checks for hunter/attacker image objects,
  - base/depth zoom clamping hardened,
  - city fire render cap to reduce frame spikes and flashing.

## Build/Flash confirmation (2026-03-27)
- Rebuild/program completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2379776 bytes`
  - `verified 2374944 bytes`
