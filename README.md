# EdgeAI Drone Hunter (Infineon PSoC Edge E84)

![Drone Hunter](https://github.com/user-attachments/assets/26f93aa3-4248-4bb2-9a74-70144fa8b4e0)

`EdgeAI Drone Hunter` is a real-time attacker-vs-defender interception game/demo for the Infineon E84 platform.
It runs on `KIT_PSE84_EVAL_EPC2` with the 4.3-inch Waveshare display and models:

- attacker drone doctrine,
- defender hunter doctrine,
- CIWS behavior,
- explainable HUD telemetry,
- wave/milestone progression,
- inventory-exhaustion battle mode.

This repository also includes hardened board recovery and restore-point governance.

## Platform
- Board: `KIT_PSE84_EVAL_EPC2`
- Device: `PSE846GPS2DBZC4A`
- Display configuration: `W4P3INCH_DISP`
- Primary runtime file:
  - `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Current Project Status
- Phases complete: `1` through `14`
- Next phase: `15` (HUD/settings/cleanup)
- Canonical roadmap: [`ToDo.md`](./ToDo.md)

## Gameplay Features (Current Baseline)
- Dual-side attacker/defender scoring loop
- Class-specific attack drones and kill effects
- 3D-depth-scaled explosion rendering
- CIWS doctrine with:
  - finite ammo,
  - lock quality,
  - cooldown/heat pressure,
  - effective/hard engagement envelope behavior
- Inventory-exhaustion round-end mode
- WHY/failure explainability telemetry
- Hunter guidance hardening:
  - continuous steering,
  - swept-hit collision checks,
  - reacquire logic
- Phase 14 movement doctrine:
  - attacker goal+evasion arbitration (ALGO baseline),
  - EDGEAI override with deterministic ALGO fallback,
  - hunter lock persistence + opportunistic switch gating

## Build (Validated In This Repo)
In this environment, the reliable compile path is the generated Ninja graph in `proj_cm55`.

```bash
cd firmware_kit_epc2/proj_cm55
ninja -f build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.ninja -v
```

Then regenerate project hex/bin from the new ELF:

```bash
OBJCOPY=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/mtb-gcc-arm-eabi/14.2.1/gcc/bin/arm-none-eabi-objcopy
ELF=firmware_kit_epc2/proj_cm55/build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.elf

$OBJCOPY -O ihex "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.hex
$OBJCOPY -O binary "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.bin
```

## Flash (Validated Recovery Sequence)
Program all required images in this order:

1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

Use the known-good script:

```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```

Recent successful programming signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 2408448 bytes` / `verified 2407264 bytes`
- `** Resetting Target **`

## Restore Point Policy
- Golden: advances to latest verified milestone
- Failsafe: pinned to latest fully validated fallback baseline
- Details: [`docs/RESTORE_POINTS.md`](./docs/RESTORE_POINTS.md)

## Memory Snapshot (Latest Phase 14 Validation)
- External SMIF write (CM55): `2,408,448 bytes`
- External SMIF verify (CM55): `2,407,264 bytes`
- External capacity: `134,217,728 bytes`

## Documentation
- Status: [`docs/STATUS.md`](./docs/STATUS.md)
- Project state: [`docs/PROJECT_STATE.md`](./docs/PROJECT_STATE.md)
- Restore points: [`docs/RESTORE_POINTS.md`](./docs/RESTORE_POINTS.md)
- Command history: [`docs/COMMAND_LOG.md`](./docs/COMMAND_LOG.md)
- Roadmap: [`ToDo.md`](./ToDo.md)
