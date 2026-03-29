# OPS_RUNBOOK

## Preflight
- Connect E84 kit via KitProg3.
- Run from repository root.
- Confirm KitProg serial when multiple boards are attached.

## Build
```bash
cd firmware_kit_epc2/proj_cm55
ninja -f build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.ninja -v
```

## Refresh CM55 Hex/Bin
```bash
OBJCOPY=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/mtb-gcc-arm-eabi/14.2.1/gcc/bin/arm-none-eabi-objcopy
ELF=firmware_kit_epc2/proj_cm55/build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.elf

$OBJCOPY -O ihex "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.hex
$OBJCOPY -O binary "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.bin
```

## Full Recovery Flash
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```

## Expected Success Signatures
- `wrote 32768 bytes`, `verified 30456 bytes`
- `wrote 12288 bytes`, `verified 8732 bytes`
- `wrote 2859008 bytes`, `verified 2854888 bytes`
- `** Resetting Target **`

## Post-Flash Smoke Check
1. Splash screen appears and transitions into arena.
2. HUD and settings popup render correctly.
3. CIWS structures are visible in fixed positions.
4. No immediate freeze within first gameplay cycle.
