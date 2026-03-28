# RESTORE_POINTS

## Golden restore point (current)
- Rolling tag: `golden-e8-drone-hunter`
- Dated tag: `golden-2026-03-28-phase15-settings-controls-validated`
- Date: `2026-03-28`
- Purpose:
  - capture Phase 15 settings-control integration,
  - capture bounce-floor hardening,
  - capture latest validated 3-image flash baseline.

## Failsafe restore point (current)
- Rolling tag: `failsafe-e8-drone-hunter`
- Dated tag: `failsafe-e8-drone-hunter-20260328-phase15-settings-controls-validated`
- Policy:
  - pinned to latest known-good directly recoverable runtime,
  - moved with this validated flash pass.

## Verified recovery programming sequence
Program in this order:
1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

Successful verification signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 2412544 bytes` / `verified 2408416 bytes`

## Restore commands
```bash
git fetch --tags
git checkout golden-2026-03-28-phase15-settings-controls-validated
# or failsafe baseline
# git checkout failsafe-e8-drone-hunter
```

## Recovery script
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```
