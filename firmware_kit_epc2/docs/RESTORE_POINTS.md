# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260327-score-pill-killflash`
- Date: `2026-03-27`
- Baseline summary:
  - HUD score visibility update:
    - dedicated hunter/attacker score labels are rendered in fixed left/right positions,
    - zero-padded score format retained with control mode tags.
  - Top-right control pill update:
    - shortened and relabeled `SET   |   HELP`,
    - left/right half touch opens Settings/Help popups.
  - Hunter kill-feedback update:
    - bright white circular flash appears at attack-drone kill point,
    - kill flash separated from spawn FX path to prevent overwrite.
  - Phase 8 completed:
    - explicit wrong-choice penalties now drive predictable losses for:
      - range mismatch,
      - altitude mismatch,
      - overkill allocation,
      - CIWS misuse,
      - manual low-confidence override.
    - HUD explainability now surfaces causal `WHY` reason text and cumulative failure counters.
  - Phase 9 completed:
    - advanced IFF mode toggle added (long-press Phalanx deck item),
    - blue-on-blue can occur only under strict multi-condition gate:
      - IFF degraded + merged tracks + manual override + low confidence.
    - collateral outcomes and recovery telemetry added (`FF`, `COL`, recovery timer).
  - Existing baseline preserved:
    - attack strategy system (`AUTO`, `CENTER`, `FLANK`, `MIXED`, `TERMINAL`) over 16 launch sites,
    - 8-sector hunter launch allocator,
    - CIWS doctrine + lock/heat/ammo telemetry,
    - stable generated attack render path.
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2400256 bytes`
  - `verified 2396432 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260327-phase9-iff-advanced`
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
