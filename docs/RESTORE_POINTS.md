# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260327-phase5-attacker-strategy-layer`
- Date: `2026-03-27`
- Baseline summary:
  - Phase 5 attacker strategy layer completed:
    - attacker profiles implemented: `AUTO`, `CENTER`, `FLANK`, `MIXED`, `TERMINAL`,
    - profile-based lane selection over 16 launch sites,
    - even edge launch rotation preserved across left/right/top/bottom.
  - Strategy control and telemetry wired:
    - mode button long-press cycles attacker strategy and resets round,
    - mode button label + wave HUD line show active strategy (`STRAT`).
  - 8-sector hunter launch allocator added:
    - hidden city sectors with balanced stock,
    - nearest-target sector selection with nearest-stock fallback.
  - CIWS ammo accounting tightened:
    - each trigger event now consumes burst-sized ammo (`CIWS_AMMO_PER_TRIGGER`),
    - deck ammo display refreshes more frequently for visible decrement.
  - Falling-hunter lower boundary behavior fixed:
    - missed hunters no longer bounce/reverse at icon/deck boundary,
    - they continue descending and reset only after exiting bottom of screen.
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
  - Hunter top-edge skid fixed:
    - reaching upper arena boundary now triggers horizon egress (shrink/fade/despawn), eliminating top-line sliding.
  - CIWS stream/range tuning pass:
    - denser/larger bullets retained,
    - spread/coverage narrowed,
    - tracer persistence increased for visibility,
    - practical stream distance bounded to gun-to-~75% screen width envelope.
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2379776 bytes`
  - `verified 2374944 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260327-phase5-attacker-strategy-layer`
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
