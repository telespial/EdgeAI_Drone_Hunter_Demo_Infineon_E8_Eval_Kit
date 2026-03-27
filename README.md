# EdgeAI Drone Hunter Demo - Infineon E8 Eval Kit

Drone Hunter demo firmware for `KIT_PSE84_EVAL_EPC2` on the Waveshare 4.3-inch LCD.

Current restored baseline (2026-03-27):
- Splash/title and hunter lineup flow in `drone_hunter_arena.c`
- Bagnet transparent source path + featured-slot scaling
- `START ARENA` gameplay entry
- In-game phase progression HUD:
  - `PHASE 1: PATROL`
  - `PHASE 2: INTERCEPT`
  - `PHASE 3: SWARM`
- Continuous gameplay enabled
- Sting-II visual baseline stabilized:
  - Opening carousel Sting-II reduced to match lineup scale.
  - Bottom Sting-II icon restored and normalized to same-size icon class.
- Orientation baseline stabilized:
  - Carousel and icon orientations reverted to prior visual baseline.
  - Play-area flight orientation now aligns nose-forward with movement heading.
- CIWS baseline tuned:
  - Engagement range kept at 75% screen-width from each gun origin.
  - Tracer bullets reduced in size.
  - Tracer flow density increased with longer visible travel.

## Build + Flash (validated path)

```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1

make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP
```

Recent flash confirmation:
- `wrote 2314240 bytes`
- `verified 2311832 bytes`

## Restore points
See `docs/RESTORE_POINTS.md` for the current golden and failsafe tags.
