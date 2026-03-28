# RESTORE_POINTS

## Golden restore point (new)
- Tag: `golden-2026-03-28-phase14-validated`
- Date: `2026-03-28`
- Purpose:
  - capture Phase 14 completion,
  - capture the validated board flash signatures,
  - publish updated docs/README and roadmap status.

## Failsafe restore point (new)
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260328-phase14-validated`
- Policy:
  - pinned to the latest known-good runtime baseline that can be restored directly,
  - moved together with this Phase 14 validated release.

## Verified recovery programming sequence
Program in this order:
1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

Successful verification signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 2408448 bytes` / `verified 2407264 bytes`

## Restore commands
```bash
git fetch --tags
git checkout golden-2026-03-28-phase14-validated
# or failsafe baseline
# git checkout failsafe-e8-drone-hunter
```

## Recovery script
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```
