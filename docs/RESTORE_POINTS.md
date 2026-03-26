# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260326`
- Date: `2026-03-26`
- Build baseline:
  - recovered `drone_hunter_arena.c.pre_recovery` state with:
  - Bagnet transparent render path + featured-slot boost
  - Arena phase progression HUD (`PHASE 1: PATROL`, `PHASE 2: INTERCEPT`, `PHASE 3: SWARM`)
  - Continuous gameplay (no 10s auto-end)
  - plus gameplay Nightscape integration and visibility fix:
    - Nightscape active in arena only (not splash)
    - Arena field background set transparent to prevent black overlay over Nightscape
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2236416 bytes`
  - `verified 2233312 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260326`
- Date: `2026-03-26`
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
