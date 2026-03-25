# RESTORE_POINTS

## Golden restore point
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-e8-drone-hunter-20260325`
- Date: `2026-03-25`
- Baseline: 5s title splash + image-based 8-drone 3D carousel + arena entry flow
- Notes: includes clipping mitigation for large zoom carousel rendering.

## Failsafe restore point
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260325`
- Notes: intentionally not advanced in this milestone (remains prior baseline).

## Verified commands
```bash
cd firmware_kit_epc2
make getlibs
make build TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make program TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP
```
