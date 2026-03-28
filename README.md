# EdgeAI Drone Hunter (PSoC Edge E84 / KIT_PSE84_EVAL_EPC2)

![Drone Hunter](https://github.com/user-attachments/assets/26f93aa3-4248-4bb2-9a74-70144fa8b4e0)

A real-time attacker-vs-defender drone interception game/demo running on Infineon PSoC Edge E84 with a 4.3-inch Waveshare display.

This repo now includes a verified golden recovery workflow and documented board restore path.

## Hardware + Target
- Board: `KIT_PSE84_EVAL_EPC2` (`PSE846GPS2DBZC4A`)
- Display path: `W4P3INCH_DISP`
- Main gameplay runtime: `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## What Is Implemented
- Full gameplay loop with attacker/defender scoring
- Attack classes and class-specific kill FX
- CIWS doctrine with finite ammo, lock/heat penalties, effective/hard range behavior
- Mission/wave progression and inventory exhaustion end mode
- Explainability telemetry (`WHY`) for failures and bad choices
- Hunter guidance hardening (continuous steer, swept-hit checks, reacquire path)
- Flicker mitigation on HUD refresh path
- Golden recovery + board restore documentation

## Phase Status
- Complete: Phases `1` through `13`
- Planned: Phases `14` and `15`
- Canonical phase tracker: [`ToDo.md`](./ToDo.md)

## Build
```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1

make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
```

## Flash (Standard)
```bash
make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1
```

## Flash (Known-Good Recovery Sequence)
The known-good restore sequence programs all required images in order:
1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

Use the packaged script:
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```

## Current Golden/Failsafe Policy
- Golden: advances to latest verified project milestone
- Failsafe: remains pinned to the current validated runtime baseline until explicitly changed
- Details: [`docs/RESTORE_POINTS.md`](./docs/RESTORE_POINTS.md)

## Memory Snapshot (Current Runtime Baseline)
- External SMIF used (CM55 image): `2,408,448 bytes`
- External SMIF verified: `2,406,412 bytes`
- External capacity: `134,217,728 bytes`

## Documentation Index
- Project status: [`docs/STATUS.md`](./docs/STATUS.md)
- Current state: [`docs/PROJECT_STATE.md`](./docs/PROJECT_STATE.md)
- Restore points: [`docs/RESTORE_POINTS.md`](./docs/RESTORE_POINTS.md)
- Command history: [`docs/COMMAND_LOG.md`](./docs/COMMAND_LOG.md)
- Roadmap/TODO: [`ToDo.md`](./ToDo.md)

## Notes
- EdgeProtect combine-sign tooling may be unavailable in some shells; use the validated build/flash flow documented in `docs`.
- If board appears blank after partial programming, run the full 3-image recovery script above.
