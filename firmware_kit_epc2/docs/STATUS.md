# STATUS

- Date: 2026-03-29
- Branch: `main`
- Runtime baseline: full clean rebuild and full 3-image flash validated on hardware.

## Phase Status
- Complete: `1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14`
- In progress: `15`
- Planned next: `16`

## Latest Validated Flash Results
- `wrote 32768 bytes`, `verified 30456 bytes`
- `wrote 12288 bytes`, `verified 8732 bytes`
- `wrote 2863104 bytes`, `verified 2858508 bytes`
- `** Resetting Target **`

## Current Runtime Baseline
- Settings popup controls are integrated and active:
  - attacker mode, defender mode, NPU toggle, difficulty, speed++.
- Top HUD text rows and `SET | HELP` button are removed.
- Explosion mapping is enforced by visual drone class:
  - Shahed: large orange, fixed-wing: red, X-wing DJI: small white.
- Fire palette now blends hot + vivid + smoke families with stronger per-fire variation.
- Bright fire visibility pass added evenly-selected bright profiles:
  - bright orange, bright red, bright red/orange.
- Skyfall steering set to plane model to remove hybrid wiggle behavior.
- Stinger steering set to plane model to remove wiggle behavior.
- Hunter labels normalized to: `Stinger`, `Pelican`, `TYTAN`.
- Additional freeze-hardening deployed:
  - city-fire render budget reduced,
  - flame sprite/style updates throttled to reduce LVGL load.
- CIWS structures remain fixed while bullet streams target dynamically.
- Exhaustion-mode battle ending is active.
- Restore script path remains stable and validated.

## Memory Snapshot
- CM55 verified image size (external SMIF): `2,858,508 bytes`
- External SMIF capacity: `134,217,728 bytes`

## Restore Policy
- Golden: latest validated milestone baseline.
- Failsafe: latest validated direct-recovery baseline.
