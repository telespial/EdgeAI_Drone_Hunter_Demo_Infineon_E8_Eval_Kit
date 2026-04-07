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

## Program Status (2026-04-07)
- Completed phases: `1` through `14`.
- In progress: `Phase 15`.
- Planned next: `Phase 16` (soundscape/audio layer).

## Current Gameplay Baseline
- Attack-only freeze-isolation mode is currently active:
  - hunter drones are compile-time disabled,
  - CIWS guns are compile-time enabled,
  - attackers are mixed (`x-wing` + fixed-wing),
  - city-hit path now renders fireball first, then target is destroyed/respawned.
- Latest attacker dive + impact alignment pass is active:
  - Shahed terminal dive now accelerates and scales down as altitude closes,
  - city-hit explosion/fx/fire placement is centered on attacker visual impact point.
- Latest CIWS + effects visual pass is active:
  - CIWS tracer streams are enabled and visible again,
  - city fire rendering is enabled in runtime,
  - safe-mode city fires now animate using lightweight frame stepping.
- Latest deck icon stability pass is active:
  - deck bar is forced foreground each refresh,
  - icon/name/count styling is synchronized every HUD update to prevent blink/dropout.
- Top-center liquid-glass HUD now includes a touch `AUDIO ON/OFF` mute toggle.
- Attack-drone terminal impact-size rendering is reduced further (smaller near-ground visual scale).
- Startup sequencing no longer replays deterministic openings:
  - round reseeds/resets at splash start,
  - attacker composition uses strategic stochastic weighting (doctrine-driven probabilities + RNG per spawn).
- Runtime debug-stage tracer hooks (`DBG:*`) are retained in code and can be compile-time enabled for freeze localization.
- Phase 14 movement doctrine is integrated (attacker evasion + hunter intercept behavior).
- Skyfall guidance no longer uses hybrid endgame blending (plane steering applied to remove wiggle).
- Stinger guidance now uses plane steering for smoother non-wiggling pursuit.
- Hunter labels updated: `Stinger`, `Pelican`, `TYTAN`.
- Phase 15 settings are integrated in popup:
  - `ATTACKER`: `ALGO/EDGEAI`
  - `DEFENDER`: `ALGO/EDGEAI/HUMAN`
  - `NPU`: `ON/OFF`
  - `DIFFICULTY`: `EASY/MED/HARD`
  - `SPEED++`: `NORMAL/FAST+/FAST++`
- CIWS structures are stationary while streams target dynamically.
- Match doctrine uses inventory exhaustion end mode.
- Freeze hardening pass is active: reduced city-fire render concurrency and throttled flame sprite updates.
- Low-load city-fire rendering fallback remains active, now with lightweight animation (not static).
- Bottom deck liquid-glass bar opacity was increased by ~20% for readability.
- Fire palette now includes explicit bright-visible profiles with even bright-color selection:
  - bright orange,
  - bright red,
  - bright red/orange.
- Current verification focus:
  - run soak validation to confirm icon blink regression remains resolved,
  - run soak validation to confirm fireball/city-fire rendering remains stable under long play.

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
- `wrote 3911680 bytes` / `verified 3908008 bytes`
- `** Resetting Target **`

## Memory Snapshot
- CM55 image verified (external SMIF): `3,908,008 bytes`.
- External SMIF capacity: `134,217,728 bytes`.

## Restore Governance
- `Golden`: milestone-quality validated restore baseline.
- `Failsafe`: latest direct-recovery fallback baseline.
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
