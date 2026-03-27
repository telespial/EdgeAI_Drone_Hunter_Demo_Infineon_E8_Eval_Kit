# PROJECT_STATE

- Project: EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Last Updated: 2026-03-27
- Repo Root: /home/user/projects/embedded/codemaster/projects/Infineon/PSOC_EDGE_E8_EVAL/projects/EdgeAI_Drone_Hunter_Demo_Infineon_E8_Eval_Kit
- Build Target: `firmware_kit_epc2/proj_cm55`
- Build Command: `make -C firmware_kit_epc2/proj_cm55 build_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8`
- Flash Command: `make -C firmware_kit_epc2/proj_cm55 qprogram_proj TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP MTB_SIGN_COMBINE__SKIP_CHECK=1`
- Programmed Image: `firmware_kit_epc2/proj_cm55/build/last_config/proj_cm55.hex`
- Status File: `docs/STATUS.md`

## Core implementation files
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_attack_images.h`
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/attack_shahed_yellow.c`
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/attack_vb140_like_red.c`
- `firmware_kit_epc2/proj_cm55/app/drone_hunter/attack_dji_x_orange.c`
- `rules.md`

## Current behavior in code
- Hunter vs Attacker point model in HUD.
- Attack goals distributed through city play region.
- Leak impacts create persistent fire patches (flame clusters) that remain until round reset.
- Additional fire pressure appears as attacker lead increases.
- CIWS constrained to short-range/near-front envelope:
  - effective/hard cutoff range model,
  - top-grid and vertical reach limits,
  - tracer travel tuned to prevent far-grid visual reach mismatch.
- Existing phase HUD and continuous-play loop retained.

## Build/Flash confirmation (2026-03-27)
- Rebuild/program completed on board `PSE846GPS2DBZC4A`.
- OpenOCD results:
  - `wrote 2367488 bytes`
  - `verified 2362856 bytes`
