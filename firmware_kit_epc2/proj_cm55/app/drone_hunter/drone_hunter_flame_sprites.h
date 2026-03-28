#pragma once
#include "lvgl.h"

#define FLAME_SPRITE_FRAME_COUNT 6
#define FLAME_SPRITE_PROFILE_COUNT 4

extern const lv_image_dsc_t flame_torch_00;
extern const lv_image_dsc_t flame_torch_01;
extern const lv_image_dsc_t flame_torch_02;
extern const lv_image_dsc_t flame_torch_03;
extern const lv_image_dsc_t flame_torch_04;
extern const lv_image_dsc_t flame_torch_05;
extern const lv_image_dsc_t flame_burst_00;
extern const lv_image_dsc_t flame_burst_01;
extern const lv_image_dsc_t flame_burst_02;
extern const lv_image_dsc_t flame_burst_03;
extern const lv_image_dsc_t flame_burst_04;
extern const lv_image_dsc_t flame_burst_05;
extern const lv_image_dsc_t flame_smoke_00;
extern const lv_image_dsc_t flame_smoke_01;
extern const lv_image_dsc_t flame_smoke_02;
extern const lv_image_dsc_t flame_smoke_03;
extern const lv_image_dsc_t flame_smoke_04;
extern const lv_image_dsc_t flame_smoke_05;
extern const lv_image_dsc_t flame_ground_00;
extern const lv_image_dsc_t flame_ground_01;
extern const lv_image_dsc_t flame_ground_02;
extern const lv_image_dsc_t flame_ground_03;
extern const lv_image_dsc_t flame_ground_04;
extern const lv_image_dsc_t flame_ground_05;

extern const lv_image_dsc_t *g_flame_profile_torch[FLAME_SPRITE_FRAME_COUNT];
extern const lv_image_dsc_t *g_flame_profile_burst[FLAME_SPRITE_FRAME_COUNT];
extern const lv_image_dsc_t *g_flame_profile_smoke[FLAME_SPRITE_FRAME_COUNT];
extern const lv_image_dsc_t *g_flame_profile_ground[FLAME_SPRITE_FRAME_COUNT];
