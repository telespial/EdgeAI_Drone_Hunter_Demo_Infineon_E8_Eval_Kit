# RESTORE_POINTS

## Current Rolling Restore Labels
- Golden rolling label: `golden-e8-drone-hunter`
- Failsafe rolling label: `failsafe-e8-drone-hunter`

## Latest Dated Restore Folders (2026-04-01)
- Golden folder: `golden-20260401-phase15-freeze-audio-race-tracer-20260401_104044`
- Failsafe folder: `failsafe-e8-drone-hunter-20260401-phase15-freeze-audio-race-tracer-20260401_104044`

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
- 2026-04-01 probe note: direct flash retry blocked (`unable to find a matching CMSIS-DAP device`).

## Recovery Command
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```
