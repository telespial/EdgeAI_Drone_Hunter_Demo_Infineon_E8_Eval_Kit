# EdgeAI Drone Hunter (Infineon PSoC Edge E84)

![Drone Hunter](https://github.com/user-attachments/assets/26f93aa3-4248-4bb2-9a74-70144fa8b4e0)

`EdgeAI Drone Hunter` is a real-time attacker-vs-defender interception game/demo running on Infineon `KIT_PSE84_EVAL_EPC2`.
It combines deterministic defense doctrine (`ALGO`) with adaptive behavior (`EDGEAI`) and hardware-validated board recovery workflows.

## Repository Purpose
This repo is both:
- a playable embedded game/simulation, and
- a continuously tracked doctrine implementation program with phased delivery and restore governance.

Primary runtime implementation:
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Project Status (2026-03-28)
- Completed: Phases `1` to `14`.
- In progress: Phase `15`.
- Planned next: Phase `16` (audio layer).

Canonical roadmap:
- [`ToDo.md`](./ToDo.md)

## Implemented Gameplay Capabilities

### Attacker Model
- Multi-site launch behavior over distributed edge/lane sites.
- Goal-seeking movement with evasion arbitration.
- ALGO baseline plus EDGEAI-style override/fallback path.
- Goal-only detonation behavior for attack drones.

### Defender Model
- Multi-class hunter inventory with stock constraints.
- CIWS doctrine with:
  - finite ammo,
  - lock quality,
  - cooldown/heat pressure,
  - constrained engagement envelope behavior.
- Threat/commit pipeline and tactical reassignment logic.

### Match Loop
- Inventory-exhaustion end mode.
- Wave/milestone progression and strategy shifts.
- Explainability telemetry (`WHY`) and consequence counters.

## Phase 15 Progress (Current)
- Lower deck/HUD bounce bug fixed:
  - shared combat floor boundary prevents drone rebound on icon strip.
- Settings popup touch controls added:
  - `ATTACKER`: `ALGO/EDGEAI`
  - `DEFENDER`: `ALGO/EDGEAI/HUMAN`
  - `NPU`: `ON/OFF`
  - `DIFFICULTY`: `EASY/MED/HARD`
  - `SPEED++`: `NORMAL/FAST+/FAST++`
- Runtime now applies settings selections to movement/control behavior.

## Visual Effects Baseline
- Explosion mapping by class:
  - Shahed (yellow fixed-wing): giant orange blast.
  - red fixed-wing: medium white circular ring.
  - orange X-wing: small bright white circular ring.
- Explosion center anchoring tracks rendered target center.
- Explosion scale follows scene depth.

## Build (Validated Path)
Use the generated Ninja graph in `proj_cm55`:

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

## Flash (Validated Full Recovery Sequence)
Program in order:
1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

Known-good script:

```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```

Latest validated flash signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 2412544 bytes` / `verified 2408416 bytes`
- `** Resetting Target **`

## Memory Snapshot
- CM55 external SMIF write: `2,412,544 bytes`
- CM55 external SMIF verified: `2,408,416 bytes`
- External SMIF capacity: `134,217,728 bytes`

## Restore Governance
- `Golden`: latest milestone-quality validated baseline.
- `Failsafe`: latest direct-recovery fallback baseline.

See:
- [`docs/RESTORE_POINTS.md`](./docs/RESTORE_POINTS.md)

## Documentation Index
- Status: [`docs/STATUS.md`](./docs/STATUS.md)
- Project state: [`docs/PROJECT_STATE.md`](./docs/PROJECT_STATE.md)
- Runbook: [`docs/OPS_RUNBOOK.md`](./docs/OPS_RUNBOOK.md)
- Hardware setup: [`docs/HARDWARE_SETUP.md`](./docs/HARDWARE_SETUP.md)
- Restore points: [`docs/RESTORE_POINTS.md`](./docs/RESTORE_POINTS.md)
- Command log: [`docs/COMMAND_LOG.md`](./docs/COMMAND_LOG.md)
- Roadmap: [`ToDo.md`](./ToDo.md)
- Rules: [`rules.md`](./rules.md)
