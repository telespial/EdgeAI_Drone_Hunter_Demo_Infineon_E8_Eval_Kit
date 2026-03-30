# RESTORE_POINTS

## Current Rolling Restore Labels
- Golden rolling label: `golden-e8-drone-hunter`
- Failsafe rolling label: `failsafe-e8-drone-hunter`

## Latest Dated Restore Folders (2026-03-30)
- Golden folder: `golden-20260330-phase15-targeting-lockbox-freeze-stability-20260330_145332`
- Failsafe folder: `failsafe-e8-drone-hunter-20260330-phase15-targeting-lockbox-freeze-stability-20260330_145332`

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
- `wrote 3923968 bytes` / `verified 3920440 bytes`

## Recovery Command
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```
