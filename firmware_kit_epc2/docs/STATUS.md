# STATUS

- Date: 2026-03-28
- Branch: `main`
- Runtime baseline: full clean rebuild and full 3-image flash validated on hardware.

## Phase Status
- Complete: `1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14`
- In progress: `15`
- Planned next: `16`

## Latest Validated Flash Results
- `wrote 32768 bytes`, `verified 30456 bytes`
- `wrote 12288 bytes`, `verified 8732 bytes`
- `wrote 2859008 bytes`, `verified 2856420 bytes`
- `** Resetting Target **`

## Current Runtime Baseline
- Settings popup controls are integrated and active:
  - attacker mode, defender mode, NPU toggle, difficulty, speed++.
- Top HUD text rows and `SET | HELP` button are removed.
- Explosion mapping is enforced by visual drone class:
  - Shahed: large orange, fixed-wing: red, X-wing DJI: small white.
- Fire palette selector enforces >=35% share for red/orange, orange/white, and bright-red/white styles.
- CIWS structures remain fixed while bullet streams target dynamically.
- Exhaustion-mode battle ending is active.
- Restore script path remains stable and validated.

## Memory Snapshot
- CM55 verified image size (external SMIF): `2,856,420 bytes`
- External SMIF capacity: `134,217,728 bytes`

## Restore Policy
- Golden: latest validated milestone baseline.
- Failsafe: latest validated direct-recovery baseline.
