# COMMAND_LOG

- 2026-03-25 | Imported firmware baseline and established Drone Hunter app flow.
- 2026-03-25..2026-03-26 | Iterated splash/drone carousel behavior and gameplay baseline.
- 2026-03-27 | Stabilized dual-CIWS arena baseline.
- 2026-03-27 | Added transparent CIWS render source and flipped-left CIWS variant.
- 2026-03-27 | Fixed stale-programming issue by forcing `build_proj` before flash validation.
- 2026-03-27 | Positioned CIWS guns per latest hardware tuning:
  - left: moved down/left and inward-facing
  - right: rotated and moved down/right (final x offset `arena_w - 8`)
- 2026-03-27 | Rebuild + flash validated:
  - `wrote 2314240 bytes`
  - `verified 2310160 bytes`
- 2026-03-27 | Promoted current state as golden restore point.
