# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260328-depth-scaled-class-fx`
- Date: `2026-03-28`
- Baseline summary:
  - Phase 10 wave pacing baseline retained:
    - rotating wave archetypes (`SHAHED`, `X-SWARM`, `MIXED`, `TERM-SAT`),
    - composition-driven scaling,
    - auto mid-wave strategy shift behavior.
  - Explosion anchor correctness baseline:
    - both hunter-kill and CIWS-kill FX are anchored to rendered target center.
  - Per-class kill FX profile mapping:
    - Shahed (yellow fixed-wing): giant orange explosion,
    - red fixed-wing Strike-Prop: medium white circular explosion,
    - orange DJI X-wing Strike-X: small bright white circular explosion.
  - Explosion perspective scaling baseline:
    - kill/intercept/spawn FX size scales with depth (larger near bottom, smaller near top).
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2404352 bytes`
  - `verified 2400360 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260328-depth-scaled-class-fx`
- Date: `2026-03-28`
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
