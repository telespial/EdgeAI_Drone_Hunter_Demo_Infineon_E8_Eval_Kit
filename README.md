# EdgeAI Drone Hunter (Infineon PSoC Edge E84)

![Drone Hunter](https://github.com/user-attachments/assets/26f93aa3-4248-4bb2-9a74-70144fa8b4e0)

`EdgeAI Drone Hunter` is a real-time attacker-vs-defender interception game/demo for Infineon `KIT_PSE84_EVAL_EPC2`.
It simulates coordinated attack drones, defender hunter drones, and dual-gun CIWS behavior with rule-based doctrine (`ALGO`) and adaptive control overlay (`EDGEAI`).

## What This Repo Contains
- Playable drone-defense game runtime on E84 hardware.
- Multi-phase doctrine implementation (Phases `1` to `14` complete).
- Recovery-safe build/flash flow with documented restore points.
- Operational docs for day-to-day engineering, debugging, and board recovery.

Primary gameplay runtime:
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## Current Status
- Completed: Phases `1` through `14`.
- Active: Phase `15` (cleanup + settings/HUD simplification + polish hardening).
- Planned next: Phase `16` (audio layer).

Canonical roadmap:
- [`ToDo.md`](./ToDo.md)

## Core Gameplay Model
- Attacker drones:
  - launch from distributed edges/lane sites,
  - navigate to assigned strike goals,
  - evade hunters under ALGO/EDGEAI movement doctrine,
  - detonate only at target coordinates.
- Defender side:
  - hunter drones with class-specific capabilities and stock limits,
  - CIWS doctrine with finite ammo, lock quality, cooldown, and heat constraints,
  - strategic penalties and explainability telemetry (`WHY` path).
- Match doctrine:
  - inventory-exhaustion battle mode,
  - threat scoring + commit gating,
  - lock persistence + opportunistic retargeting.

## Visual/FX Baseline
- Class-specific destruction FX:
  - Shahed (yellow fixed-wing): giant orange blast,
  - red fixed-wing: medium white ring,
  - orange X-wing: small bright white ring.
- Explosion anchors track rendered target center.
- Explosion scale adapts with scene depth.
- Lower deck/HUD bounce bug hardened (flight floor constrained above icon strip).

## Build (Validated Path)
Use the generated Ninja graph for `proj_cm55`:

```bash
cd firmware_kit_epc2/proj_cm55
ninja -f build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.ninja -v
```

Export fresh CM55 artifacts from rebuilt ELF:

```bash
OBJCOPY=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/mtb-gcc-arm-eabi/14.2.1/gcc/bin/arm-none-eabi-objcopy
ELF=firmware_kit_epc2/proj_cm55/build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.elf

$OBJCOPY -O ihex "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.hex
$OBJCOPY -O binary "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.bin
```

## Flash (Validated Full Recovery Sequence)
Program all three images in this order:
1. `proj_cm33_s_signed.hex`
2. `proj_cm33_ns_shifted.hex`
3. `proj_cm55.hex`

Known-good script:

```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```

Latest validated signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 2408448 bytes` / `verified 2407304 bytes`
- `** Resetting Target **`

## Memory Snapshot
- CM55 external SMIF write size: `2,408,448 bytes`
- CM55 external SMIF verified: `2,407,304 bytes`
- External SMIF capacity: `134,217,728 bytes`

## Restore Point Policy
- `Golden`: latest milestone-quality validated baseline.
- `Failsafe`: latest direct-recovery baseline for immediate rollback.

See:
- [`docs/RESTORE_POINTS.md`](./docs/RESTORE_POINTS.md)

## Docs Index
- Status: [`docs/STATUS.md`](./docs/STATUS.md)
- Project state: [`docs/PROJECT_STATE.md`](./docs/PROJECT_STATE.md)
- Runbook: [`docs/OPS_RUNBOOK.md`](./docs/OPS_RUNBOOK.md)
- Hardware setup: [`docs/HARDWARE_SETUP.md`](./docs/HARDWARE_SETUP.md)
- Restore points: [`docs/RESTORE_POINTS.md`](./docs/RESTORE_POINTS.md)
- Command history: [`docs/COMMAND_LOG.md`](./docs/COMMAND_LOG.md)
- Roadmap: [`ToDo.md`](./ToDo.md)
- Rules: [`rules.md`](./rules.md)
