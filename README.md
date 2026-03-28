# EdgeAI Drone Hunter Demo - Infineon E8 Eval Kit

Drone Hunter demo firmware for `KIT_PSE84_EVAL_EPC2` on the Waveshare 4.3-inch LCD.

## Current baseline (2026-03-27)
- Two-player gameplay model: `Hunter` vs `Attacker` points.
- Attack goals distributed across the playable city area (not bottom-pinned).
- Leaked attack drones impact city targets and trigger explosions.
- Persistent city fires remain after leaks and grow as defender performance drops.
- Dual CIWS emplacements with sweep-arc behavior and finite per-gun ammo.
- CIWS now tuned as last-ditch defense with effective/hard-cutoff range behavior and overuse penalties.
- CIWS ammo now visibly decrements by burst-sized trigger consumption (left/right counters).
- Attack classes use rendered image assets:
  - `Shahed`-style bright yellow fixed-wing
  - red VB140-like fixed-wing
  - orange DJI-style X-wing
- Hunter inventory deck + stock counters remain active.
- Hunter launches now originate from 8 hidden city sectors with nearest-target + nearest-stock fallback.
- Missed/failed hunters now continue descending off bottom screen instead of bouncing at deck boundary.
- Continuous gameplay with phase HUD retained.

## Rules spec
- Canonical rules are documented in `rules.md`.

## Build + Flash (validated path)
```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1

make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1
```

Latest flash confirmation:
- `wrote 2375680 bytes`
- `verified 2374456 bytes`

## Restore points
See `docs/RESTORE_POINTS.md` for current golden and failsafe tags.
