# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260327-phase3-complete`
- Date: `2026-03-27`
- Baseline summary:
  - Attack drone render path fixed:
    - runtime attacker sprites forced to generated attack set only,
    - ODIN fallback removed from attacker update path.
  - Hunter defense launch reliability fixed:
    - urgency fallback added when strict commit gate confidence is low.
  - Hunter runtime visibility/size fixes:
    - mixed hunter selection restored (removed Sting-only forced fallback),
    - stable Sting sprite source in gameplay,
    - minimum visible launch window before intercept resolve,
    - tightened zoom and bottom-edge clamp to avoid oversized icon-bar popups.
  - CIWS engagement restored:
    - gameplay-visible sweep range uses `CIWS_RANGE_FRAC` screen scaling,
    - no longer effectively suppressed by map-km hard-cutoff conversion.
  - Phase 3 completed:
    - weighted threat score formula implemented,
    - target-value modifier implemented,
    - 16-site lane-pressure model with decay + adjacent spillover implemented,
    - deterministic active-target priority ordering implemented.
  - Existing stability guards and telemetry remain active.
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2375680 bytes`
  - `verified 2371784 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260327-phase3-complete`
- Date: `2026-03-27`
- Policy: failsafe is aligned to current golden baseline.

## Restore commands
```bash
git fetch --tags
git checkout golden-e8-drone-hunter
# or
# git checkout failsafe-e8-drone-hunter
```

## Build/flash commands
```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1

cd firmware_kit_epc2/proj_cm55
make build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1
```
