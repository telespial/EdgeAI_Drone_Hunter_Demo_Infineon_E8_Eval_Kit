# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260327`
- Date: `2026-03-27`
- Baseline summary:
  - Dual CIWS emplacements active in arena with transparent-render CIWS sprites.
  - Left CIWS: horizontally flipped, positioned lower-left (`x + 18`, `y = arena_h - 49`).
  - Right CIWS: rotated 180 from prior baseline and moved farther right/down (`x = arena_w - 8`, `y = arena_h - 49`).
  - CIWS tracer stream remains dotted bullet-style (not solid laser line).
  - Arena phase HUD + continuous-play flow retained.
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2314240 bytes`
  - `verified 2310160 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260327`
- Date: `2026-03-27`
- Note: failsafe tag may intentionally remain pinned to previous fallback unless explicitly promoted.

## Restore commands
```bash
git fetch --tags
git checkout golden-e8-drone-hunter
```

## Build/flash commands for this baseline
```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1

cd firmware_kit_epc2/proj_cm55
make build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1
```
