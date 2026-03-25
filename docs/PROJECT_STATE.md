# PROJECT_STATE

- Project: EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Last Updated: 2026-03-25
- Repo Root: /home/user/projects/embedded/codemaster/projects/Infineon/PSOC_EDGE_E8_EVAL/projects/EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Build Command: `cd firmware_kit_epc2 && make build TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8`
- Flash Command: `cd firmware_kit_epc2 && make program TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP`
- Binary: `firmware_kit_epc2/build/app_combined.hex`
- Status File: `docs/STATUS.md`

## Current Status
- Startup sequence implemented:
  - `EdgeAI Drone Hunter` boot title shown for ~5s.
  - Hunter drone carousel shown after title.
  - Swipe left/right spins the 8-drone orbit.
  - `START ARENA` enters gameplay scene.
- Carousel implementation is image-based (`drone_hunter_images.c/.h`) and rendered as an invisible 3D ring.
- Clipping issue tracked:
  - Symptom: top/bottom truncation when using ~3x transform zoom.
  - Cause: transform clipping with nested/object bounds in LVGL orbit view.
  - Mitigation: orbit now uses direct `lv_image` objects and reduced base sprite dimensions to preserve full drone visibility at large zoom.
- Verified program/write/verify on board `PSE846GPS2DBZC4A`.

## Next Actions
- Run final on-device visual acceptance for all 8 drones across full spin arc.
- Continue arena behavior tuning (mode readability, scoring clarity, demo pacing).
