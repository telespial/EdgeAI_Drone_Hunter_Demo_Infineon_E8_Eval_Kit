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
- `wrote 2859008 bytes`, `verified 2857120 bytes`
- `** Resetting Target **`

## Current Runtime Baseline
- Settings popup controls are integrated and active:
  - attacker mode, defender mode, NPU toggle, difficulty, speed++.
- CIWS structures remain fixed while bullet streams target dynamically.
- Exhaustion-mode battle ending is active.
- Restore script path remains stable and validated.

## Memory Snapshot
- CM55 verified image size (external SMIF): `2,857,120 bytes`
- External SMIF capacity: `134,217,728 bytes`

## Restore Policy
- Golden: latest validated milestone baseline.
- Failsafe: latest validated direct-recovery baseline.
