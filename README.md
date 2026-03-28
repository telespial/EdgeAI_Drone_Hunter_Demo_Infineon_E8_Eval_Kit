# EdgeAI Drone Hunter Demo - Infineon E8 Eval Kit

Drone Hunter demo firmware for `KIT_PSE84_EVAL_EPC2` on the Waveshare 4.3-inch LCD.

## Current baseline (2026-03-28)
- Two-player gameplay model: `Hunter` vs `Attacker` points.
- Attack goals distributed across the playable city area (not bottom-pinned).
- Leaked attack drones impact city targets and trigger explosions.
- Persistent city fires remain after leaks and grow as defender performance drops.
- Dual CIWS emplacements with sweep-arc behavior and finite per-gun ammo.
- CIWS now tuned as last-ditch defense with effective/hard-cutoff range behavior and overuse penalties.
- CIWS ammo now visibly decrements by burst-sized trigger consumption (left/right counters).
- CIWS doctrine finalization now active:
  - per-gun lock-quality modeling,
  - stronger misuse penalties via extra cooldown/reaction delay under bad geometry,
  - live per-gun ammo/heat/lock telemetry in HUD.
- Attack classes use rendered image assets:
  - `Shahed`-style bright yellow fixed-wing
  - red VB140-like fixed-wing
  - orange DJI-style X-wing
- Hunter inventory deck + stock counters remain active.
- Hunter launches now originate from 8 hidden city sectors with nearest-target + nearest-stock fallback.
- Missed/failed hunters now continue descending off bottom screen instead of bouncing at deck boundary.
- Phase 5 attacker strategy layer is active with 16-site spawn control:
  - `AUTO`, `CENTER`, `FLANK`, `MIXED`, `TERMINAL`.
- On-device control:
  - `Mode` button click = cycle match mode,
  - `Mode` button long-press = cycle attacker strategy.
- Continuous gameplay with phase HUD retained.
- Phase 10 wave pacing advanced:
  - rotating wave archetypes (`SHAHED`, `X-SWARM`, `MIXED`, `TERM-SAT`),
  - composition-driven difficulty and mid-wave strategy shifts in `AUTO`.
- Phase 7 completed:
  - defender HUD panel now includes stock/endurance/availability, envelope fit, CIWS lock, and CIWS cooldown telemetry.
- Phase 11 completed:
  - strategic mission-end rules now active with causal round-end summaries (win/loss reason + key metrics).
- Phase 8 completed:
  - explicit wrong-choice penalties (range/altitude/overkill/CIWS misuse/manual low-confidence),
  - live `WHY` failure explainability cues in HUD.
- Phase 9 completed:
  - optional advanced IFF mode toggle (long-press Phalanx deck item),
  - blue-on-blue enabled only under strict combined-failure gate,
  - collateral + recovery telemetry wired (`FF`, `COL`, recovery timer).
- HUD score/menu polish:
  - dedicated left/right score labels with control tags and zero-padded counters,
  - shortened top-right pill `SET   |   HELP` with split-touch Settings/Help popup behavior.
- Hunter kill visual feedback:
  - class-specific blast profiles:
    - Shahed: giant orange explosion,
    - red fixed-wing: medium white circular explosion,
    - orange DJI X-wing: small bright white circular explosion.
  - explosion anchor now uses rendered target center for both hunter and CIWS kills.
  - explosion size now scales with scene depth (larger near screen bottom, smaller toward top).
- Phase 14 specification added (planned):
  - `ALGO` is baseline attacker+defender function logic,
  - `EDGEAI` is an embedded intelligence layer that improves ALGO via trained/adaptive reasoning.
- Phase 13 is now in progress:
  - hunters continuously re-steer while committed,
  - swept-hit collision checks reduce fly-past misses,
  - reacquire path and telemetry counters (`SH`, `RQ`, `OS`) are active.
- CIWS strategy refinement:
  - constrained accidental CIWS-on-hunter risk window is defined in rules,
  - fratricide event consumes hunter supply while attacker kill outcome remains uncertain.

## Rules spec
- Canonical rules are documented in `rules.md`.

## Build + Flash (validated path)
```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/.local/opt/xpack-arm-none-eabi-gcc-14.2.1-1.1

make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1
```

Latest flash confirmation:
- `wrote 2408448 bytes`
- `verified 2404672 bytes`

Memory snapshot from latest programmed image:
- External SMIF flash usage: `2,404,672 / 134,217,728 bytes` (`1.79%` used, `98.21%` free).
- Internal RRAM equivalent fit check: image would exceed 512 KB internal capacity by `1,880,384 bytes` (`~4.59x` too large).

## Restore points
See `docs/RESTORE_POINTS.md` for current golden and failsafe tags.
