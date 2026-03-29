# START_HERE

1. Read `README.md` for project overview and current baseline.
2. Read `ToDo.md` for phase status and next work.
3. Build:
   - `cd firmware_kit_epc2/proj_cm55`
   - `ninja -f build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.ninja -v`
4. Flash with validated full sequence:
   - `bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh`
5. If recovery is needed, use artifacts under:
   - `/home/user/Documents/DroneHunter_Golden_2026-03-28/restore_points/`
