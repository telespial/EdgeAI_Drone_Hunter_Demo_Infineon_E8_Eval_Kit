# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260327`
- Date: `2026-03-27`
- Baseline summary:
  - Dual fixed CIWS emplacements active in arena (`left` + `right`).
  - CIWS base placement locked to `arena_h - 61` (raised placement baseline).
  - Tracer rendering uses tiny bullet-style dots (not solid beam line).
  - Existing arena phase HUD and continuous-play flow retained.
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2277376 bytes`
  - `verified 2273224 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260327`
- Date: `2026-03-27`
- Policy for this milestone: failsafe intentionally aligned to current golden baseline.

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
make qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP
```
