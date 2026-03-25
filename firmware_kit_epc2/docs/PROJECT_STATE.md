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
- Splash firmware initializes display/peripherals and renders `EdgeAI Drone Hunter`.
- Build and flash verified on hardware on `2026-03-25`.
- Program artifact: `build/app_combined.hex`.
