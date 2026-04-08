# STATUS

- Date: 2026-04-08
- Branch: `main`
- Runtime baseline: full clean rebuild, CM55 artifact regeneration (`objcopy`), and full 3-image flash validated on hardware.

## Phase Status
- Complete: `1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14`
- In progress: `15`
- Planned next: `16`

## Current Priority Queue (2026-04-08)
1. Investigate and fix intermittent gameplay freeze (highest-priority blocker).
2. Keep tracking boxes disabled until explicitly re-enabled.
3. Enforce artifact freshness before every flash (`proj_cm55.hex/.bin` must be regenerated from latest ELF).
4. Continue soak validation for icon stability and fire/fireball rendering.
5. Resume `ALGO` attacker/defender strategy improvements after freeze path is stabilized.

## Latest Validated Flash Results
- `wrote 32768 bytes`, `verified 30456 bytes`
- `wrote 12288 bytes`, `verified 8732 bytes`
- `wrote 3923968 bytes`, `verified 3920192 bytes`
- `** Resetting Target **`

## Current Runtime Baseline
- Compile-time toggles:
  - `DISABLE_HUNTER_DRONES=0`
  - `DISABLE_CIWS_GUNS=0`
  - `HIDE_ATTACK_DRONE_SPRITES=0`
  - `RENDER_TARGET_LOCK_BOXES=0`
- Render behavior:
  - tracking box draw path is hard-disabled (`lock_on = 0`),
  - attack drone sprites remain visible.
- Mixed attacker roster remains active (`x-wing` + fixed-wing/Shahed logic).
- CIWS guns and tracer streams are active.
- Freeze tracing/hardening instrumentation remains available in code.

## Flash Safety Rule
- Always regenerate CM55 artifacts before flashing:
  - `proj_cm55.hex`
  - `proj_cm55.bin`
- Rationale: flashing can otherwise use stale CM55 hex even when ELF changed.

## Memory Snapshot
- CM55 verified image size (external SMIF): `3,920,192 bytes`
- External SMIF capacity: `134,217,728 bytes`

## Restore Policy
- Golden: latest validated milestone baseline.
- Failsafe: latest validated direct-recovery baseline.

## Restore Snapshot (2026-04-08)
- Golden label: `golden-20260408-phase15-trackbox-off-artifact-refresh-20260408_141411`
- Failsafe label (unchanged): `failsafe-e8-drone-hunter-20260407-phase15-ciws-tracer-icon-fire-anim-20260407_160138`
