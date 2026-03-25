# EdgeAI Drone Hunter Demo - Infineon E8 Eval Kit

Drone Hunter demo firmware for `KIT_PSE84_EVAL_EPC2` on the Waveshare 4.3-inch LCD.

Current UX flow:
- 5-second title splash: `EdgeAI Drone Hunter`
- Image-based 8-drone swipe carousel (invisible 3D orbit)
- `START ARENA` transitions into gameplay

Recent issue tracked:
- Large carousel zoom caused top/bottom clipping of drone images.
- Mitigated by rendering direct `lv_image` orbit objects with reduced base sprite dimensions.

## Build + Flash

```bash
export CY_TOOLS_PATHS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox/tools_3.7
export CY_COMPILER_GCC_ARM_DIR=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/mtb-gcc-arm-eabi/14.2.1/gcc
export CY_TOOL_edgeprotecttools_EXE_ABS=/home/user/toolchains/infineon/ModusToolbox_local/opt/Tools/ModusToolbox-Edge-Protect-Security-Suite-1.6.1/tools/edgeprotecttools/bin/edgeprotecttools

cd firmware_kit_epc2
make getlibs
make build TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP -j8
make program TOOLCHAIN=GCC_ARM CONFIG_DISPLAY=W4P3INCH_DISP
```
