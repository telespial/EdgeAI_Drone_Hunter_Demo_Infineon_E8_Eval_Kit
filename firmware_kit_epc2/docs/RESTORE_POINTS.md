# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260327`
- Date: `2026-03-27`
- Baseline summary:
  - Dual CIWS emplacements active with transparent CIWS sprites.
  - CIWS sweep-arc behavior implemented:
    - Left CIWS sweeps a 90-degree arc from horizontal to vertical on its side.
    - Right CIWS sweeps a 90-degree arc from horizontal to vertical on its side.
  - CIWS effective engagement range capped to 75% of arena/screen width from each gun origin.
  - CIWS tracer model uses dense firehose-style ballistic streams:
    - Burst pellet emission per fire event.
    - High-rate sustained fire cadence.
    - Angle-weighted gravity drop (more arc at flatter/horizontal fire angles).
  - Attack drone spawn logic supports 4-edge ingress:
    - Drones enter from top, bottom, left, and right edges.
    - Initial heading is aligned inward toward arena center from spawn edge.
  - Hunter visual pipeline normalized:
    - Hunter sprite assets cleaned and normalized to a consistent icon/play-piece format.
    - Extra fallback geometry overlays hidden so only rendered sprites are visible.
    - Bottom deck icon labels/counts are centered under each icon slot (including CIWS).
  - Arena phase HUD and continuous-play flow retained.
- Flash confirmation (`proj_cm55 qprogram_proj`):
  - `wrote 2314240 bytes`
  - `verified 2311176 bytes`

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260327`
- Date: `2026-03-27`
- Policy for this milestone: failsafe is aligned to current golden baseline.

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
