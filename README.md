# EdgeAI Drone Hunter Demo (Infineon E84 Eval Kit)

Work In Progress!

EdgeAI Drone Hunter is a real-time attacker-vs-defender simulation running on Infineon PSoC Edge E84 (`KIT_PSE84_EVAL_EPC2`).
The demo combines deterministic doctrine (`ALGO`) and adaptive behavior (`EDGEAI`) with a hardware-validated recovery and flashing workflow.

![Drone Hunter](https://github.com/user-attachments/assets/26f93aa3-4248-4bb2-9a74-70144fa8b4e0)

## Highlights
- Two-sided air-defense simulation: attacker drones vs hunter drones + CIWS.
- Doctrine split:
  - `ALGO` = base tactical/gameplay logic layer.
  - `EDGEAI` = adaptive decision layer on top of ALGO with deterministic fallback.
- On-device controls for attacker/defender mode, NPU, difficulty, and speed.
- Full restore workflow with `golden` and `failsafe` baselines.

## Repository Layout
- `firmware_kit_epc2/`: ModusToolbox firmware workspace.
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`: primary gameplay runtime.
- `docs/`: operational status, runbook, restore governance, logs.
- `drones/`: drone art assets and flame packs.
- `rules.md`: canonical rules/spec intent.
- `ToDo.md`: roadmap and phase tracker.

## Program Status (2026-04-08)
- Completed phases: `1` through `14`.
- In progress: `Phase 15`.
- Planned next: `Phase 16` (soundscape/audio layer).

## Current Gameplay Baseline
- Current compile-time runtime toggles:
  - `DISABLE_HUNTER_DRONES=0`,
  - `DISABLE_CIWS_GUNS=0`,
  - `HIDE_ATTACK_DRONE_SPRITES=0`.
- Tracking boxes are intentionally disabled by request:
  - `RENDER_TARGET_LOCK_BOXES=0`,
  - `lock_on` is forced off in render path.
- Mixed attacker visuals remain enabled (`x-wing` + fixed-wing/Shahed logic).
- Full flash workflow requirement:
  - always regenerate `proj_cm55.hex` and `proj_cm55.bin` from latest ELF before flashing,
  - this prevents stale CM55 artifact flashes when only ELF is rebuilt.
- Freeze investigation remains the top gameplay blocker.

## Build
```bash
cd firmware_kit_epc2/proj_cm55
ninja -f build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.ninja -v
```

Regenerate CM55 artifacts:
```bash
OBJCOPY=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/mtb-gcc-arm-eabi/14.2.1/gcc/bin/arm-none-eabi-objcopy
ELF=firmware_kit_epc2/proj_cm55/build/APP_KIT_PSE84_EVAL_EPC2/Debug/proj_cm55.elf

$OBJCOPY -O ihex "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.hex
$OBJCOPY -O binary "$ELF" firmware_kit_epc2/build/project_hex/proj_cm55.bin
```

## Flash (Validated Full Sequence)
```bash
bash /home/user/Documents/DroneHunter_Golden_2026-03-28/scripts/flash_golden.sh
```

Latest validated flash signatures:
- `wrote 32768 bytes` / `verified 30456 bytes`
- `wrote 12288 bytes` / `verified 8732 bytes`
- `wrote 3923968 bytes` / `verified 3920192 bytes`
- `** Resetting Target **`

## Memory Snapshot
- CM55 image verified (external SMIF): `3,908,008 bytes`.
- External SMIF capacity: `134,217,728 bytes`.

## Restore Governance
- `Golden`: milestone-quality validated restore baseline.
- `Failsafe`: latest direct-recovery fallback baseline.
- Latest golden label:
  - `golden-20260408-phase15-trackbox-off-artifact-refresh-20260408_141411`
- Failsafe remains unchanged:
  - `failsafe-e8-drone-hunter-20260407-phase15-ciws-tracer-icon-fire-anim-20260407_160138`
- See: `docs/RESTORE_POINTS.md`.

## Documentation Index
- `docs/START_HERE.md`
- `docs/STATUS.md`
- `docs/PROJECT_STATE.md`
- `docs/OPS_RUNBOOK.md`
- `docs/RESTORE_POINTS.md`
- `docs/COMMAND_LOG.md`
- `ToDo.md`
- `rules.md`
