# RESTORE_POINTS

## Current Rolling Restore Labels
- Golden rolling label: `golden-e8-drone-hunter`
- Failsafe rolling label: `failsafe-e8-drone-hunter`

## Latest Dated Restore Folders (2026-03-29)
- Golden folder: `golden-20260329-phase15-effects-freeze-hardening-and-size-mapping-20260329_092958`
- Failsafe folder: `failsafe-e8-drone-hunter-20260329-phase15-effects-freeze-hardening-and-size-mapping-20260329_092958`

## Restore Artifacts Location
- `/home/user/Documents/DroneHunter_Golden_2026-03-28/restore_points/`

Each restore point contains:
- `artifacts/proj_cm33_s_signed.hex`
- `artifacts/proj_cm33_ns_shifted.hex`
- `artifacts/proj_cm55.hex`
- `artifacts/proj_cm55.bin`
- `artifacts/proj_cm55.elf`
- `artifacts/proj_cm55.map`
- `manifest.txt`

## Verified Recovery Programming Order
1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

## Latest Flash Signatures
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 2633728 bytes` / `verified 2628796 bytes`

## Recovery Command
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```
