# STATUS

- Date: 2026-03-28
- Repo head intent: Phase 15 kickoff + lower-deck bounce hardening + docs/restore refresh
- Runtime baseline: validated on hardware using full 3-image programming sequence

## Phase Status
- Complete: `1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14`
- In progress: `15`
- Planned next: `16`

## Latest Validated Programming Results
Full recovery sequence (cm33_s_signed -> cm33_ns_shifted -> cm55):
- `wrote 32768 bytes`, `verified 30456 bytes`
- `wrote 12288 bytes`, `verified 8732 bytes`
- `wrote 2408448 bytes`, `verified 2407304 bytes`
- `** Resetting Target **`

## Current Gameplay Baseline (high level)
- Phase 14 movement doctrine active on board.
- ALGO baseline + EDGEAI override/fallback path active.
- Hunter lock persistence + opportunistic switch gates active.
- CIWS finite-ammo doctrine with lock/heat/penalty telemetry active.
- Explosion class mapping + transformed-center anchoring + depth scaling active.
- Lower icon/HUD deck bounce hardening active:
  - attacker/hunter floor constraints now use combat floor above deck strip.

## Memory Snapshot
- External SMIF used (cm55 image write): `2,408,448` bytes
- External SMIF verified: `2,407,304` bytes
- External capacity: `134,217,728` bytes

## Restore Policy
- Golden: points to latest verified milestone release.
- Failsafe: points to latest validated recovery fallback baseline.
- Details: `docs/RESTORE_POINTS.md`
