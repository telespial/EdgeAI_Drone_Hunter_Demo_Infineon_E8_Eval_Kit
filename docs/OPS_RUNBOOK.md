# OPS_RUNBOOK

## Preflight
- Confirm E8 board connected via KitProg3.
- Run commands from repo root.
- Verify the board serial if multiple kits are connected.

## Build + artifact refresh (validated path)
In this workspace, use the generated Ninja build graph for `proj_cm55`:

```bash
cd firmware_kit_epc2/proj_cm55
ninja -f build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.ninja -v
```

Export fresh CM55 artifacts from the rebuilt ELF:

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

## Expected flash confirmation (Phase 14 validated baseline)
- `wrote 32768 bytes`, `verified 30456 bytes`
- `wrote 12288 bytes`, `verified 8732 bytes`
- `wrote 2408448 bytes`, `verified 2407264 bytes`
- `** Resetting Target **`

## Post-flash checks
- Confirm splash and lineup flow appears.
- Confirm `START ARENA` enters gameplay.
- Confirm attacker/hunter movement doctrine is active (Phase 14 behavior).
- Confirm kill FX behavior:
  - Shahed (yellow fixed-wing): giant orange explosion,
  - red fixed-wing: medium white circular explosion,
  - orange X-wing: small bright white circular explosion,
  - explosion anchoring matches destroyed drone center,
  - explosion size scales by depth (larger near bottom, smaller near top).
