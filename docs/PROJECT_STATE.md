# PROJECT_STATE

- Project: EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Last Updated: 2026-03-28
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
- Shahed destruction visual fix:
  - hunter kills on Shahed targets now trigger a visible explosion sequence before despawn,
  - killed Shahed briefly enters a short dying state (blast + fade) before respawn,
  - dying targets are excluded from retargeting/commit selection during the effect window.
  - blast visibility was further increased for on-device readability:
    - much larger impact radii for intercept/kill FX,
    - brighter fill/border opacity and thicker rings,
    - longer Shahed death hold duration for clearer blast readability.
  - explosion center anchoring fix:
    - kill FX now anchors to rendered target object center at intercept time (not stale logical track center),
    - improves visual centering of ring/blast on Shahed sprite body.
  - visual-center anchoring refinement:
    - kill FX now uses transformed object bounding-box center in arena coordinates,
    - addresses offset caused by runtime transform/zoom differences between logical track and rendered sprite.
  - CIWS kill-anchor parity fix:
    - CIWS kill path now emits kill/intercept FX from rendered target center before despawn/respawn,
    - eliminates off-target explosions caused by respawn-only visual feedback on gun kills.
  - Per-class blast profile mapping is now explicit:
    - Shahed-class: giant orange explosion,
    - Strike-X (DJI X-wing): small bright white circular explosion,
    - Strike-Prop (red fixed-wing): medium white circular explosion.
  - Explosion perspective scaling is now active:
    - all drone explosion FX (intercept ring, spawn burst ring, kill ring/fill) scale with Y-depth,
    - bottom of screen (closest) renders larger effects, top of screen (farther) renders smaller effects.
- Phase 10 wave-structure progression advanced:
  - explicit wave archetypes now rotate by wave (`SHAHED`, `X-SWARM`, `MIXED`, `TERM-SAT`),
  - per-archetype composition now scales target mix/tier pressure (not count-only growth),
  - mission milestones are now explicit (`OPENING`, `ESCALATE`, `CRISIS`, `SATURATE`) and feed wave scaling,
  - auto strategy mode now performs primary and late-wave strategic shifts (55% and late pressure trigger),
  - HUD wave row shows milestone and shift markers (`*` and `+`).
- Phase 4 hunter assignment and fallback hardening:
  - no-fit fallback now switches to recommended counter or highest-stock hunter class,
  - no-stock fallback now attempts recommended/highest-stock replacement before launch hold,
  - launch-sector depletion and fallback events now emit explicit `WHY` reason messages.
- Phase 7 HUD/UX completion:
  - defender panel telemetry is now surfaced in runtime HUD:
    - endurance proxy, stock/airborne availability,
    - CIWS lock and cooldown state,
    - envelope fit hint and FF lockout mode.
- Phase 11 win/loss + collateral completion:
  - re-enabled strategic mission end evaluation (previous demo no-end bypass removed),
  - defender win requires asset intact + defense layer remaining + collateral below threshold,
  - defender loss triggers on asset destruction, early CIWS exhaustion plus leaks, or collateral/critical-node threshold breach,
  - round-end overlay now shows causal summary metrics.
- Phase 14 doctrine semantics are now explicit in rules/docs:
  - `ALGO` is baseline attacker+defender function logic,
  - `EDGEAI` is adaptive embedded intelligence that improves ALGO using trained/adaptive reasoning.
- Phase 13 hunter guidance/intercept hardening is now complete:
  - committed hunters are continuously re-steered each tick with turn-rate limits,
  - swept-hit geometry checks are active to reduce one-frame fly-through misses,
  - target-loss handling now attempts bounded reacquire before forced miss/fall,
  - telemetry counters added for swept-hit (`SH`), reacquire (`RQ`), and overshoot (`OS`) events.
- Phase 13 completion calibration is now implemented:
  - class-aware guidance lead/speed multipliers added for launch + pursuit steering,
  - near-intercept overshoot damping added to reduce fly-past behavior in terminal window,
  - per-class kill radius values rebalanced after guidance stabilization,
  - explicit WHY messages added for unreacquired target-loss misses and terminal evade misses.
- Display stability hotfix:
  - HUD text refresh is now rate-limited to reduce occasional panel flicker,
  - `hud_elapsed` line now uses single-write composition per refresh pass.
- Strategy refinement:
  - CIWS accidental hunter-kill rule now includes explicit penalty semantics:
    - hunter inventory can be consumed by fratricide,
    - attacking drone may still survive the same burst event.
- Phase 9 IFF advanced mode is implemented:
  - optional advanced toggle via long-press on Phalanx deck item,
  - blue-on-blue path only opens under strict combined gate:
    - IFF degraded + merged tracks + manual override + low confidence,
  - collateral and recovery telemetry added (`FF`, `COL`, recovery timer).
- Phase 8 wrong-choice consequence system is implemented:
  - explicit penalties for range mismatch, altitude mismatch, overkill allocation, CIWS misuse, and low-confidence override,
  - HUD explainability cues added (`WHY`) with aggregate failure counters.
- HUD control/menu polish is implemented:
  - top-right pill reduced and relabeled `SET   |   HELP`,
  - split touch behavior by pill half:
    - left half opens settings popup,
    - right half opens help popup.
- Hunter kill flash is implemented:
  - white circular flash ring shown at attacker kill location on hunter success,
  - both drones disappear immediately after successful intercept.
- Scoreboard presentation is implemented as dedicated labels:
  - `HUNTER(<CTRL>): 0000` (left),
  - `ATTACKER(<CTRL>): 0000` (right),
  - `CORE` centered.
- Phase 6 CIWS doctrine finalization is implemented:
  - per-gun lock quality computed from geometry/lead conditions,
  - poor lock and out-of-effective-envelope firing now consume extra reaction budget (cooldown penalties),
  - heat penalties now scale with lock quality.
- CIWS state telemetry now includes per-gun ammo/heat/lock in HUD.
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
  - `wrote 2400256 bytes`
  - `verified 2396432 bytes`

## Build confirmation (2026-03-28)
- Rebuild completed for `proj_cm55` after wave-archetype update.
- Build reached compile/link/hex generation successfully.
- Expected environment limitation remains:
  - `Error: EdgeProtect Secure Suite not found. Combine-Sign step not executed.`

## Flash confirmation (2026-03-28)
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2400256 bytes`
  - `verified 2398272 bytes`

## Build + Flash confirmation (2026-03-28, Shahed explosion fix)
- Rebuild completed for `proj_cm55` after Shahed kill-FX update.
- Build reached compile/link/hex generation successfully (combine-sign environment limitation unchanged).
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2400256 bytes`
  - `verified 2398944 bytes`

## Build + Flash confirmation (2026-03-28, Shahed blast visibility boost)
- Rebuild completed for `proj_cm55` after Shahed blast-size/opacity tuning.
- Build reached compile/link/hex generation successfully (combine-sign environment limitation unchanged).
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2404352 bytes`
  - `verified 2399336 bytes`

## Build + Flash confirmation (2026-03-28, Shahed blast center anchoring fix)
- Rebuild completed for `proj_cm55` after kill-FX center anchoring update.
- Build reached compile/link/hex generation successfully (combine-sign environment limitation unchanged).
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2404352 bytes`
  - `verified 2399496 bytes`

## Build + Flash confirmation (2026-03-28, transformed visual-center anchoring)
- Rebuild completed for `proj_cm55` after transformed visual-center anchor update.
- Build reached compile/link/hex generation successfully (combine-sign environment limitation unchanged).
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2404352 bytes`
  - `verified 2399472 bytes`

## Build + Flash confirmation (2026-03-28, CIWS kill-anchor parity fix)
- Rebuild completed for `proj_cm55` after CIWS kill-FX anchor update.
- Build reached compile/link/hex generation successfully (combine-sign environment limitation unchanged).
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2404352 bytes`
  - `verified 2399760 bytes`

## Build + Flash confirmation (2026-03-28, per-class blast profile mapping)
- Rebuild completed for `proj_cm55` after blast-style mapping update across hunter + CIWS kill paths.
- Build reached compile/link/hex generation successfully (combine-sign environment limitation unchanged).
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2404352 bytes`
  - `verified 2400112 bytes`

## Build + Flash confirmation (2026-03-28, revised white profile mapping)
- Rebuild completed for `proj_cm55` after profile remap:
  - red fixed-wing -> medium white circular explosion,
  - X-wing (DJI) -> small bright white circular explosion.
- Build reached compile/link/hex generation successfully (combine-sign environment limitation unchanged).
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2404352 bytes`
  - `verified 2400160 bytes`

## Build + Flash confirmation (2026-03-28, depth-scaled explosion FX)
- Rebuild completed for `proj_cm55` after depth scaling was applied to explosion effects.
- Build reached compile/link/hex generation successfully (combine-sign environment limitation unchanged).
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2404352 bytes`
  - `verified 2400360 bytes`

## Build confirmation (2026-03-28, Phase 4/10 completion pass)
- Local rebuild attempt command:
  - `make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8`
- Result in this shell environment:
  - failed early with `Unable to find any of the available CY_TOOLS_PATHS`.
- Impact:
  - code/doc updates completed, but compile/flash verification must run in the provisioned build environment used for prior board flashes.

## Flash confirmation (2026-03-28, corrected SDK path)
- Environment used:
  - `CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7`
- Program command completed on board `PSE846GPS2DBZC4A`:
  - `make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1`
- OpenOCD results:
  - `wrote 2404352 bytes`
  - `verified 2400360 bytes`
- Note:
  - `build_proj` in current shell still reports GCC package resolution issue.

## Memory snapshot (2026-03-28)
- Programmed image (`verified`): `2,405,104` bytes.
- External SMIF capacity (`0x60000000..0x67FFFFFF`): `134,217,728` bytes.
- Usage:
  - used: `1.79%`
  - free: `98.21%` (`131,812,624` bytes).
- Internal-only fit check (512 KB RRAM):
  - image exceeds capacity by `1,880,816` bytes (`~4.59x` larger than internal capacity).

## Build + Flash confirmation (2026-03-28, Phase 12 closure pass)
- Build command completed through compile/link/hex generation:
  - `make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8`
- Expected environment limitation remained:
  - `EdgeProtect Secure Suite not found. Combine-Sign step not executed.`
- Program command completed on board `PSE846GPS2DBZC4A`:
  - `make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1`
- OpenOCD results:
  - `wrote 2404352 bytes`
  - `verified 2403032 bytes`

## Build + Flash confirmation (2026-03-28, Phase 13 start pass)
- Build command completed through compile/link/hex generation:
  - `make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8`
- Expected environment limitation remained:
  - `EdgeProtect Secure Suite not found. Combine-Sign step not executed.`
- Program command completed on board `PSE846GPS2DBZC4A`:
  - `make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1`
- OpenOCD results:
  - `wrote 2408448 bytes`
  - `verified 2404672 bytes`

## Build + Flash confirmation (2026-03-28, flicker mitigation hotfix)
- Rebuild completed for `proj_cm55` after HUD redraw-throttle update.
- Build reached compile/link/hex generation successfully (combine-sign environment limitation unchanged).
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2408448 bytes`
  - `verified 2404608 bytes`

## Build + Flash confirmation (2026-03-28, Phase 13 completion pass)
- Rebuild completed for `proj_cm55` after guidance calibration + miss explainability updates.
- Build reached compile/link/hex generation successfully (combine-sign environment limitation unchanged).
- Program command completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2408448 bytes`
  - `verified 2405104 bytes`
