# EdgeAI Drone Hunter Firmware (`firmware_kit_epc2`)

Firmware workspace for the active Drone Hunter runtime on Infineon PSoC Edge E84 (`KIT_PSE84_EVAL_EPC2`).

## Active Target
- Board: `KIT_PSE84_EVAL_EPC2`
- Display config: `CONFIG_DISPLAY=W4P3INCH_DISP`
- Primary app path: `proj_cm55/app/drone_hunter/`

## Primary Source File
- `proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Build (validated)
```bash
cd proj_cm55
ninja -f build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.ninja -v
```

## Regenerate CM55 HEX/BIN
```bash
OBJCOPY=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/mtb-gcc-arm-eabi/14.2.1/gcc/bin/arm-none-eabi-objcopy
ELF=build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.elf

$OBJCOPY -O ihex "$ELF" ../build/project_hex/proj_cm55.hex
$OBJCOPY -O binary "$ELF" ../build/project_hex/proj_cm55.bin
```

## Flash (validated full recovery)
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```

Latest verified signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 2859008 bytes` / `verified 2857892 bytes`
- `** Resetting Target **`

## Documentation Order
1. `../README.md`
2. `../rules.md`
3. `docs/START_HERE.md`
4. `docs/PROJECT_STATE.md`
5. `docs/RESTORE_POINTS.md`
6. `docs/OPS_RUNBOOK.md`

## Notes
- Repo scope is Drone Hunter only.
- Keep restore governance current whenever flashing a new validated baseline.
