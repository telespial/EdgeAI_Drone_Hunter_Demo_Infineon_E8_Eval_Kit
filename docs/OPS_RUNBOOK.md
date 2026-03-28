# OPS_RUNBOOK

## Preflight
- Confirm E8 board connected via KitProg3.
- Run commands from repo root.
- Verify board serial if multiple kits are connected.

## Build + artifact refresh (validated path)

```bash
cd firmware_kit_epc2/proj_cm55
ninja -f build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.ninja -v
```

```bash
OBJCOPY=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/mtb-gcc-arm-eabi/14.2.1/gcc/bin/arm-none-eabi-objcopy
ELF=firmware_kit_epc2/proj_cm55/build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.elf

$OBJCOPY -O ihex "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.hex
$OBJCOPY -O binary "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.bin
```

## Flash (validated full recovery sequence)

```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```

## Expected flash confirmation (current baseline)
- `wrote 32768 bytes`, `verified 30456 bytes`
- `wrote 12288 bytes`, `verified 8732 bytes`
- `wrote 2412544 bytes`, `verified 2408416 bytes`
- `** Resetting Target **`

## Post-flash checks
- Settings popup supports tap-to-cycle control rows.
- NPU toggle affects EDGEAI attacker path usage.
- Difficulty and speed++ affect movement pace.
- No drone rebounds from bottom icon/HUD strip.
