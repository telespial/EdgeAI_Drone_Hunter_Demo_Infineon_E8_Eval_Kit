# EdgeAI Drone Hunter (Infineon PSoC Edge E84)

![Drone Hunter](https://github.com/user-attachments/assets/26f93aa3-4248-4bb2-9a74-70144fa8b4e0)

EdgeAI Drone Hunter is a real-time attacker-vs-defender interception simulation/game running on the Infineon `KIT_PSE84_EVAL_EPC2`.
It combines deterministic doctrine (`ALGO`) with adaptive behavior (`EDGEAI`) and ships with board-recovery workflows for reliable flashing.

## What This Repo Contains
- Embedded firmware project for PSoC Edge E84 (`firmware_kit_epc2`)
- Active gameplay runtime (`drone_hunter_arena.c`)
- Rules/phase roadmap and operations docs
- Recovery scripts and restore-governance process (golden + failsafe)

Primary runtime file:
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Program Status (2026-03-28)
- Completed phases: `1` through `14`
- Current active work: `Phase 15`
- Next planned phase: `Phase 16` (audio layer)

Roadmap:
- [`ToDo.md`](./ToDo.md)

## Core Gameplay Model
### Attacker Side
- Wave archetypes and strategy layer (`AUTO/CENTER/FLANK/MIXED/TERMINAL`)
- Goal-constrained detonation behavior (detonate only when reaching assigned target)
- Dynamic attacker movement with evade pressure
- Adaptive strategy shifts during wave progression
- Randomized opening behavior so reboots do not replay the same opening sequence

### Defender Side
- Hunter inventory classes and stock-constrained launch behavior
- CIWS with finite ammunition, lock quality, cooldown/heat behavior, and envelope constraints
- ALGO/EDGEAI/HUMAN defender mode support
- Friendly-fire/IFF consequence path and telemetry

### Match / Rules Layer
- Inventory-exhaustion battle-end mode
- Mission milestones and wave-shift pressure model
- Explainability/telemetry fields for decision and failure reasons

## Latest Integrated Runtime Items
- Fire/explosion pipeline updated to sprite-driven city-fire effects with intensity-based profile selection
- Fire placement anchored at impact depth/ground context with depth scaling
- CIWS bullet streams updated for depth-aware flight scaling
- Gun structures kept stationary while streams track targets
- Settings popup supports touch-cycle controls:
  - `ATTACKER`: `ALGO/EDGEAI`
  - `DEFENDER`: `ALGO/EDGEAI/HUMAN`
  - `NPU`: `ON/OFF`
  - `DIFFICULTY`: `EASY/MED/HARD`
  - `SPEED++`: `NORMAL/FAST+/FAST++`

## Build (Validated)
```bash
cd firmware_kit_epc2/proj_cm55
ninja -f build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.ninja -v
```

Generate fresh CM55 HEX/BIN from rebuilt ELF:
```bash
OBJCOPY=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/mtb-gcc-arm-eabi/14.2.1/gcc/bin/arm-none-eabi-objcopy
ELF=firmware_kit_epc2/proj_cm55/build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.elf

$OBJCOPY -O ihex "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.hex
$OBJCOPY -O binary "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.bin
```

## Flash (Validated Full Sequence)
Known-good script:
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```

Latest validated signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 2859008 bytes` / `verified 2854888 bytes`
- `** Resetting Target **`

## Memory Snapshot
- CM55 image verified in external SMIF: `2,854,888 bytes`
- External SMIF capacity: `134,217,728 bytes`

## Restore Governance
- `Golden`: milestone-quality validated baseline
- `Failsafe`: latest direct-recovery fallback baseline

Details:
- [`docs/RESTORE_POINTS.md`](./docs/RESTORE_POINTS.md)

## Documentation Index
- Status: [`docs/STATUS.md`](./docs/STATUS.md)
- Project state: [`docs/PROJECT_STATE.md`](./docs/PROJECT_STATE.md)
- Restore points: [`docs/RESTORE_POINTS.md`](./docs/RESTORE_POINTS.md)
- Command log: [`docs/COMMAND_LOG.md`](./docs/COMMAND_LOG.md)
- Runbook: [`docs/OPS_RUNBOOK.md`](./docs/OPS_RUNBOOK.md)
- Start guide: [`docs/START_HERE.md`](./docs/START_HERE.md)
- Rules: [`rules.md`](./rules.md)
- Roadmap: [`ToDo.md`](./ToDo.md)
