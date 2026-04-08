# START_HERE

1. Read `README.md` for project overview and current baseline.
2. Read `ToDo.md` for phase status and next work.
3. Build:
   - `cd firmware_kit_epc2/proj_cm55`
   - `ninja -f build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.ninja -v`
4. Refresh CM55 flash artifacts from latest ELF:
   - `OBJCOPY=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/mtb-gcc-arm-eabi/14.2.1/gcc/bin/arm-none-eabi-objcopy`
   - `ELF=firmware_kit_epc2/proj_cm55/build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.elf`
   - `$OBJCOPY -O ihex \"$ELF\" firmware_kit_epc2/build/project_hex/proj_cm55.hex`
   - `$OBJCOPY -O binary \"$ELF\" firmware_kit_epc2/build/project_hex/proj_cm55.bin`
5. Flash with validated full sequence:
   - `bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh`
6. If recovery is needed, use artifacts under:
   - `/home/user/Documents/DroneHunter_Golden_2026-03-28/restore_points/`
