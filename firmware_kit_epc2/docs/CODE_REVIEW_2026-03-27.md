# CODE_REVIEW_2026-03-27

Scope reviewed:
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`
- `ToDo.md` + mirrored TODO docs
- `docs/STATUS.md`, `docs/PROJECT_STATE.md`, `docs/RESTORE_POINTS.md`

## Findings (ordered by severity)

1. Medium - Dead/unused helper functions increased maintenance risk and warning noise
- Removed unused functions that were not referenced by runtime paths.
- Impact: lowers confusion and keeps warning surface clean.

2. Medium - HUD info composition could exceed safe practical string budget
- Hardened `snprintf` usage for the composed HUD info line by capping embedded segment lengths.
- Impact: avoids truncation-prone formatting and keeps text handling deterministic.

3. Low - Documentation drift risk between root/docs/firmware docs
- Re-synced TODO and status/restore documentation with the current verified baseline.

## Residual risks / test gaps
- Device-level freeze reported by user around ~3 seconds remains a runtime behavior to reproduce under board-observed conditions; static code cleanup and build checks alone cannot fully close this.
- No unit-test harness exists for arena loop; validation is currently hardware-observation based.

## Verification notes
- Build completed successfully for `proj_cm55`.
- Flash completed successfully with:
  - `wrote 2371584 bytes`
  - `verified 2369544 bytes`
- Programming path used:
  - `qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1`
