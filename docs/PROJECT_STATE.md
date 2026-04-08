# PROJECT_STATE

- Project: EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Last Updated: 2026-04-08
- Primary runtime file:
  - `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Baseline Summary
- Phases `1-14` are complete.
- Phase `15` is active.
- Current compile-time/runtime baseline:
  - `DISABLE_HUNTER_DRONES=0`
  - `DISABLE_CIWS_GUNS=0`
  - `HIDE_ATTACK_DRONE_SPRITES=0`
  - `RENDER_TARGET_LOCK_BOXES=0`
  - tracking boxes are hard-disabled in render path (`lock_on = 0`),
  - attack-drone sprites are visible,
  - mixed attacker visuals are enabled (`x-wing` + fixed-wing/Shahed logic),
  - CIWS tracers and city-fire rendering remain enabled.
- Current top blocker remains intermittent gameplay freeze.

## Flash/Artifact Safety Update
- Confirmed issue: stale `proj_cm55.hex` can be flashed if CM55 artifacts are not regenerated after rebuild.
- Required workflow before every flash:
  1. Build ELF.
  2. Regenerate `proj_cm55.hex` + `proj_cm55.bin` from latest ELF via `arm-none-eabi-objcopy`.
  3. Run full 3-image flash script.

## Verified Hardware
- Kit: `KIT_PSE84_EVAL`
- Device: `PSE846GPS2DBZC4A`
- Probe: `KitProg3`

## Verified Recovery Program Order
1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

Observed success signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 3923968 bytes` / `verified 3920192 bytes`
- `** Resetting Target **`

## Active Runbook Script
- `/home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh`

## Restore Snapshot
- Golden label: `golden-20260408-phase15-trackbox-off-artifact-refresh-20260408_141411`
- Failsafe label (unchanged): `failsafe-e8-drone-hunter-20260407-phase15-ciws-tracer-icon-fire-anim-20260407_160138`

## Next Execution Focus
1. Freeze root-cause isolation and fix (highest priority).
2. Keep lock-box rendering disabled until explicit re-enable request.
3. Run extended soak validation with current mixed-attacker baseline.
4. Continue strategic `ALGO` gameplay improvements after freeze path is stabilized.
