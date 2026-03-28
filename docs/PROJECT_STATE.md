# PROJECT_STATE

- Project: EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Last Updated: 2026-03-28
- Primary runtime file: `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Current Baseline Summary
- Phases 1-14 complete.
- Phase 14 has been built and flashed on board.
- Phase 15 and Phase 16 remain planned.

## Verified Board + Device
- Kit: `KIT_PSE84_EVAL`
- Device: `PSE846GPS2DBZC4A`
- Debug adapter: `KitProg3`

## Verified Recovery Programming Sequence
1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

Observed successful signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 2408448 bytes` / `verified 2407264 bytes`
- `** Resetting Target **`

## Known-Good Restore Script
- `/home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh`

## Next Workfront
- Execute Phase 15 settings/HUD/cleanup scope.
- Then begin Phase 16 audio layer.
