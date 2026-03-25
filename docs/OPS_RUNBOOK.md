# OPS_RUNBOOK

## Preflight
- Confirm E8 board connected by KitProg3.
- Export ModusToolbox toolchain vars.
- Work from `firmware_kit_epc2`.

## Build
```bash
make getlibs
make build TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
```

## Flash
```bash
make program TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP
```

## Post-run
- Confirm splash text appears on LCD: `EdgeAI Drone Hunter`.
- Update `docs/PROJECT_STATE.md` and append command notes to `docs/COMMAND_LOG.md`.
