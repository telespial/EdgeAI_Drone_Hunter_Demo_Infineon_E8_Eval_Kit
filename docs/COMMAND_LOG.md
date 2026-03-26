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
- 2026-03-26 | Replaced 8-drone splash orbit with 3-drone swipe row prototype (Sting/ODIN/Skyfall focus flow).
- 2026-03-26 | Switched splash interaction from `LV_EVENT_GESTURE`-only handling to press/release drag delta logic for more reliable swipe detection on-device.
- 2026-03-26 | Added center-magnified slot behavior with side slots scaled down; iterated spacing and Y-offset on hardware.
- 2026-03-26 | Added positional clamps to keep splash drones within visible LCD bounds during scaling.
- 2026-03-26 | Investigated right-side boxed artifact and alternated Skyfall source between detailed and base variants.
- 2026-03-26 | Restored `drone_hunter_arena.c` to `drone_hunter_arena.c.pre_recovery` baseline.
- 2026-03-26 | Generated build-order source manifests from `proj_cm55.elf.rsp` + `proj_cm55.ninja` under `docs/build_manifests/`.
- 2026-03-26 | Rebuild + flash re-confirmed via `proj_cm55 qprogram_proj`: wrote `1466368` bytes, verified `1465272` bytes.
- 2026-03-26 | Promoted current state as both golden and failsafe restore points.
