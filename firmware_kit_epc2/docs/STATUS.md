# STATUS

- Date: 2026-03-28
- Repo head intent: documentation + restore hardening pass
- Runtime baseline: validated on hardware using full 3-image programming sequence

## Phase Status
- Complete: `1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13`
- Planned: `14, 15`

## Latest Validated Programming Results
Full recovery sequence (cm33_s_signed -> cm33_ns_shifted -> cm55):
- `wrote 32768 bytes`, `verified 30456 bytes`
- `wrote 12288 bytes`, `verified 8732 bytes`
- `wrote 2408448 bytes`, `verified 2406412 bytes`
- `** Resetting Target **`

## Current Gameplay Baseline (high level)
- Attacker/defender score loop and wave pacing active.
- CIWS finite-ammo doctrine active with lock/heat/penalty telemetry.
- WHY explainability path active.
- Hunter guidance hardening (continuous steer + swept hit + reacquire) active.
- Flicker mitigation via HUD refresh throttling active.

## Memory Snapshot
- External SMIF used (cm55 image write): `2,408,448` bytes
- External SMIF verified: `2,406,412` bytes
- External capacity: `134,217,728` bytes

## Restore Policy
- Golden: can advance when milestone is verified.
- Failsafe: remains pinned to current validated runtime baseline until explicitly changed.
- Details: `docs/RESTORE_POINTS.md`
