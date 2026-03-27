# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260327-rules`
- Date: `2026-03-27`
- Baseline summary:
  - Rules-driven Hunter vs Attacker gameplay baseline captured in `rules.md`.
  - Attack goal distribution fixed to city play area.
  - Leak impact + persistent fire accumulation active.
  - Fire pressure scales up as defender falls behind.
  - CIWS now modeled as last-ditch with short effective range and hard cutoff.
  - CIWS misuse/overuse pressure enabled (ammo waste + cooldown/heat pressure).
  - Color-distinct rendered attack drone classes integrated.
  - Continuous phase-driven gameplay and existing deck/HUD systems retained.
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2367488 bytes`
  - `verified 2362856 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260327-rules`
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
