# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260327-fire-ciws-envelope`
- Date: `2026-03-27`
- Baseline summary:
  - Persistent leak-fire system active with larger flame-like fire patches.
  - Fires persist until round/game reset.
  - Fire pressure scales with attacker lead.
  - CIWS now cannot reach top/far city grids:
    - short effective range + hard cutoff,
    - explicit top-grid/vertical envelope gating,
    - tracers shortened to match envelope.
  - Hunter vs Attacker score model, distributed city targets, and continuous phase flow retained.
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2367488 bytes`
  - `verified 2362856 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260327-fire-ciws-envelope`
- Date: `2026-03-27`
- Policy for this milestone: failsafe is aligned to the current golden baseline.

## Restore commands
```bash
git fetch --tags
git checkout golden-e8-drone-hunter
# or
# git checkout failsafe-e8-drone-hunter
```

## Build/flash commands for this baseline
```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1

cd firmware_kit_epc2/proj_cm55
make build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1
```
