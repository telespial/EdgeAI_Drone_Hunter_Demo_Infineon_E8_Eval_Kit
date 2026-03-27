# OPS_RUNBOOK

## Preflight
- Confirm E8 board connected via KitProg3.
- Export ModusToolbox toolchain variables.
- Run commands from repo root.

## Build + flash (validated path)
```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1

make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1
```

## Expected flash confirmation (current baseline)
- `wrote 2367488 bytes`
- `verified 2362856 bytes`

## Post-flash checks
- Confirm splash and lineup flow appears.
- Confirm `START ARENA` enters gameplay.
- Confirm attack goals are spread in city play region (not bottom-edge clustered).
- Confirm leaked attacks create persistent fire markers.
- Confirm `Hunter`/`Attacker` score line updates.
- Confirm CIWS per-gun ammo display updates and depletion behavior.

## Notes
- EdgeProtect combine-sign step may fail in this environment; use the validated `qprogram_proj` path above.
