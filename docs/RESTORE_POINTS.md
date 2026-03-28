# RESTORE_POINTS

## Golden restore point (updated)
- Tag: `golden-2026-03-28-docs-release`
- Date: `2026-03-28`
- Purpose:
  - lock in documentation cleanup + recovery procedure hardening,
  - capture known-good board restore workflow.

## Failsafe restore point (unchanged)
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260328-phase13-plus-exhaustion-and-ciws-doctrine`
- Policy:
  - intentionally not moved during this docs-only golden update,
  - remains pinned to current validated runtime build line.

## Verified recovery programming sequence
Program in this order:
1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

Successful verification signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 2408448 bytes` / `verified 2406412 bytes`

## Restore commands
```bash
git fetch --tags
git checkout golden-2026-03-28-docs-release
# or failsafe baseline
# git checkout failsafe-e8-drone-hunter
```

## Recovery script
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```
