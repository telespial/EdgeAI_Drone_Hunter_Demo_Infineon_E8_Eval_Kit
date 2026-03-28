# STATUS

- Date: 2026-03-28
- Repo head intent: Phase 15 settings-control integration + docs/restore-point refresh
- Runtime baseline: validated on hardware using full 3-image programming sequence

## Phase Status
- Complete: `1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14`
- In progress: `15`
- Planned next: `16`

## Latest Validated Programming Results
Full recovery sequence (cm33_s_signed -> cm33_ns_shifted -> cm55):
- `wrote 32768 bytes`, `verified 30456 bytes`
- `wrote 12288 bytes`, `verified 8732 bytes`
- `wrote 2412544 bytes`, `verified 2408416 bytes`
- `** Resetting Target **`

## Current Gameplay Baseline (high level)
- Phase 14 movement doctrine active on board.
- Phase 15 bounce-floor hardening active (no bottom icon-strip rebounds).
- Phase 15 settings controls active in popup:
  - attacker `ALGO/EDGEAI`,
  - defender `ALGO/EDGEAI/HUMAN`,
  - `NPU ON/OFF`,
  - difficulty `EASY/MED/HARD`,
  - speed++ `NORMAL/FAST+/FAST++`.
- Settings now influence runtime movement/control behavior.

## Memory Snapshot
- External SMIF used (cm55 image write): `2,412,544` bytes
- External SMIF verified: `2,408,416` bytes
- External capacity: `134,217,728` bytes

## Restore Policy
- Golden: points to latest verified milestone release.
- Failsafe: points to latest validated recovery fallback baseline.
- Details: `docs/RESTORE_POINTS.md`
