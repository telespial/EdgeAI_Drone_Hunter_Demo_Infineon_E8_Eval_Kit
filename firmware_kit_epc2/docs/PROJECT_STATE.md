# PROJECT_STATE

## Project
PSOC Edge E84 Eval (EPC2), Drone Hunter splash baseline.

## Date
2026-03-25

## Working Hardware
- Kit: `KIT_PSE84_EVAL_EPC2`
- MCU detected during flash: `PSE846GPS2DBZC4A`
- Display: Waveshare 4.3-inch DSI panel

## Build Configuration
- `TARGET=KIT_PSE84_EVAL_EPC2`
- `TOOLCHAIN=GCC_ARM`
- `CONFIG_DISPLAY=W4P3INCH_DISP`
- `APP_DRONE_HUNTER_MODE=1`

## Current State
- Splash firmware now runs:
  - `EdgeAI Drone Hunter` title for ~5s.
  - Image-based 8-drone 3D carousel.
  - `START ARENA` transition into gameplay.
- Known issue history:
  - Large zoom orbit initially clipped drone tops/bottoms.
  - Mitigated by direct `lv_image` orbit objects and reduced base sprite dimensions.
- Build and program/verify confirmed on hardware on `2026-03-25`.
- Program artifact in active flow: `proj_cm55/build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.hex`.
