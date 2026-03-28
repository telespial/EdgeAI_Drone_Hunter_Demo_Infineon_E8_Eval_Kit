# Phase 15 Kickoff Audit

Date: 2026-03-28  
Scope: first-step cleanup and guardrail setup before HUD/settings/fire rendering changes.

## Objective
Start Phase 15 with a documented repo hygiene baseline so code/UI changes are done on a clean, predictable tree.

## Findings (Initial Inventory)
- Canonical roadmap file is `ToDo.md`.
- TODO mirrors are pointer-only and currently correct:
  - `docs/TODO.md`
  - `firmware_kit_epc2/docs/TODO.md`
- Multiple `start_here` and `README` files exist, including framework/vendor paths:
  - `start_here.md`
  - `docs/START_HERE.md`
  - `firmware_kit_epc2/start_here.md`
  - `firmware_kit_epc2/docs/START_HERE.md`
  - `firmware_kit_epc2/README.md`
  - `firmware_kit_epc2/bsps/TARGET_APP_KIT_PSE84_EVAL_EPC2/README.md`

## Cleanup Guardrails
- Do not remove vendor/BSP documentation required by ModusToolbox build flow.
- Keep exactly one gameplay-facing "start here" path and one primary repo README.
- Mirror docs should remain pointers only to avoid drift.
- Any deletion/move must preserve build/flash reproducibility and restore policy docs.

## Next Execution Steps
1. Decide canonical gameplay onboarding doc and downgrade others to pointers.
2. Remove busy top HUD text and replace with simple ATTACKER/DEFENDER score presentation.
3. Add settings popup touch controls:
   - Attacker mode: `ALGO/EDGEAI`
   - Defender mode: `ALGO/EDGEAI/HUMAN`
   - NPU: `ON/OFF`
   - Difficulty: `EASY/MED/HARD`
   - Speed++: runtime global speed multiplier
4. Begin phase-15 fire rendering pass (shape/size variation and depth scaling consistency).
