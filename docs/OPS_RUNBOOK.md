# OPS_RUNBOOK

## Preflight
- Confirm E8 board connected by KitProg3.
- Export ModusToolbox toolchain vars.
- Work from repo root.

## Build + flash (validated path)
```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1

make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP
```

## Expected flash confirmation (current baseline)
- `wrote 1466368 bytes`
- `verified 1465272 bytes`

## Post-run checks
- Confirm splash and hunter lineup render.
- Confirm `START ARENA` transitions into gameplay.
- Confirm HUD phase progression text appears in-game.

## Notes
- Environment may emit EdgeProtect combine-sign warning/error on broader app build targets.
- `proj_cm55` build + `qprogram_proj` is the current reliable programming route.
