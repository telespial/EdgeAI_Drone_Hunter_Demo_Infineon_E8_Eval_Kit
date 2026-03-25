# COMMAND_LOG

- 2026-03-25 | Imported `firmware_kit_epc2` baseline into Drone Hunter repo.
- 2026-03-25 | Added Drone Hunter runtime macro and removed Smart Pong app dependency.
- 2026-03-25 | Implemented startup splash + animated POV intercept arena (`proj_cm55/app/drone_hunter/drone_hunter_arena.c`).
- 2026-03-25 | Build+flash verified on `PSE846GPS2DBZC4A` with `W4P3INCH_DISP`.
- 2026-03-25 | Promoted this baseline as golden/failsafe restore point.
- 2026-03-25 | Reworked splash UX to title -> image-based 8-drone carousel -> `START ARENA`.
- 2026-03-25 | Added `drone_hunter_images.c/.h` sprite pack sourced from user-provided drone sheets.
- 2026-03-25 | Investigated LVGL transform clipping with large orbit zoom; shifted to direct `lv_image` orbit objects and reduced base sprite size for full-height rendering.
- 2026-03-25 | Verified program/write/verify after carousel updates; prepared new golden restore point only (failsafe intentionally not updated).
