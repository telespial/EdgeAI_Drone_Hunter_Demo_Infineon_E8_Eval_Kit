# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260327-phase2-complete`
- Date: `2026-03-27`
- Baseline summary:
  - Full active-target HUD telemetry integrated:
    - type, speed, altitude, range-to-core, ETA, threat score, recommended counter.
  - Rules-engine Phase 2 completed:
    - detect/classify/commit pipeline fully wired,
    - LOS + corridor gating in commit stage,
    - track-history + noise shaping,
    - commit hold reason accounting.
  - Icon deck flashing reduced by throttled hunter deck UI updates during CIWS fire.
  - Persistent leak-fire model remains active.
  - CIWS remains constrained to short-range near-front envelope.
  - Hunter vs Attacker score model + continuous phase flow retained.
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2371584 bytes`
  - `verified 2369544 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260327-phase2-complete`
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
