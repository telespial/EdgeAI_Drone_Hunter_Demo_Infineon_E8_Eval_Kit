# EdgeAI Drone Hunter Firmware (firmware_kit_epc2)

This firmware workspace contains the active Drone Hunter implementation for the PSOC Edge E84 EPC2 platform.

## Active target
- Board: `KIT_PSE84_EVAL_EPC2`
- Display config: `CONFIG_DISPLAY=W4P3INCH_DISP`
- Primary app: `proj_cm55/app/drone_hunter/`

## Primary source file
- `proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Build + flash (validated)
```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1

make -C proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make -C proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1
```

## Documentation order
1. `../README.md`
2. `../rules.md`
3. `docs/START_HERE.md`
4. `docs/PROJECT_STATE.md`
5. `docs/RESTORE_POINTS.md`
6. `docs/OPS_RUNBOOK.md`

## Notes
- This repo is scoped to Drone Hunter. Smart Pong references and generic demo workflows are not part of the active baseline.
