# COMMAND_LOG

- 2026-03-25 | Imported firmware baseline and established Drone Hunter app flow.
- 2026-03-25..2026-03-26 | Iterated splash/carousel and gameplay baseline.
- 2026-03-27 | Stabilized dual-CIWS layout, transparent CIWS assets, and position tuning.
- 2026-03-27 | Increased tracer visibility and tuned deck CIWS icon scale.
- 2026-03-27 | Implemented CIWS sweep-arc gameplay model:
  - 90-degree sweep per side
  - 75% screen-range engagement cap
  - sweep-cone hit gating and sweep-directed tracer emission
- 2026-03-27 | Upgraded CIWS projectile visuals/physics:
  - dense high-rate burst-dot tracer stream (firehose effect)
  - ballistic drop with stronger curvature at flatter firing angles
- 2026-03-27 | Expanded attack ingress to all 4 screen edges (top/bottom/left/right).
- 2026-03-27 | Normalized hunter sprite pipeline and cleaned icon/play-piece artifacts.
- 2026-03-27 | Center-aligned bottom deck labels/counts under each icon slot.
- 2026-03-27 | Rebuild + flash validated:
  - `wrote 2314240 bytes`
  - `verified 2311176 bytes`
- 2026-03-27 | Promoted this exact state as both golden and failsafe restore points.
