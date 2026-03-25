# RESTORE_POINTS

## Golden baseline (2026-03-25)
- `golden-e8-drone-hunter`
- `golden-e8-drone-hunter-20260325`
- Scope: title splash + image-based 8-drone 3D carousel + arena entry.

## Failsafe baseline (2026-03-25)
- `failsafe-e8-drone-hunter`
- `failsafe-e8-drone-hunter-20260325`
- Note: intentionally not advanced in this milestone.

## Validation
- Build: `make build TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8`
- Flash: `make program TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP`
- Artifact: `build/app_combined.hex`
