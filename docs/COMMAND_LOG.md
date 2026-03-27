# COMMAND_LOG

- 2026-03-25 | Imported `firmware_kit_epc2` baseline into Drone Hunter repo.
- 2026-03-25 | Added Drone Hunter runtime macro and removed Smart Pong app dependency.
- 2026-03-25 | Implemented startup splash + animated POV intercept arena (`proj_cm55/app/drone_hunter/drone_hunter_arena.c`).
- 2026-03-25 | Build+flash verified on `PSE846GPS2DBZC4A` with `W4P3INCH_DISP`.
- 2026-03-25 | Promoted baseline as golden/failsafe restore point.
- 2026-03-25 | Reworked splash UX to title -> image-based carousel flow.
- 2026-03-26 | Restored/rebased runtime to known-good arena flow with phase HUD and continuous play.
- 2026-03-26 | Added gameplay-only Nightscape background handling and removed gameplay black overlay.
- 2026-03-26 | Multiple hardware reflashes validated intermediary restore points.
- 2026-03-27 | Reapplied dual-CIWS arena baseline and CIWS image source integration (`drone_hunter_ciws.c/.h`).
- 2026-03-27 | Locked CIWS baseline height to `arena_h - 61` and retained dotted tracer stream behavior.
- 2026-03-27 | Full rebuild + hardware flash revalidated:
  - `wrote 2277376 bytes`
  - `verified 2273224 bytes`
- 2026-03-27 | Promoted this exact build as both golden and failsafe restore points.
