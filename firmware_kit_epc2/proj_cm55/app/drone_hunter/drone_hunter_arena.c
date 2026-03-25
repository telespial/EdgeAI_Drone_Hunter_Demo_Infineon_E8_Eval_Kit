#include "drone_hunter_arena.h"

#include <math.h>
#include <stdio.h>

#define PI_F                      (3.14159265359f)
#define STAR_COUNT                (48)
#define TICK_MS                   (33)
#define SPLASH_MS                 (1800)

typedef struct
{
    lv_obj_t *screen;
    lv_obj_t *splash;
    lv_obj_t *arena;
    lv_obj_t *target;
    lv_obj_t *algo_drone;
    lv_obj_t *edge_drone;
    lv_obj_t *algo_pred;
    lv_obj_t *edge_pred;
    lv_obj_t *hud_algo;
    lv_obj_t *hud_edge;
    lv_obj_t *hud_hits;

    lv_timer_t *anim_timer;
    lv_timer_t *splash_timer;

    int32_t arena_x;
    int32_t arena_y;
    int32_t arena_w;
    int32_t arena_h;

    float t;
    float target_x;
    float target_y;
    float target_vx;
    float target_vy;
    float target_prev_x;
    float target_prev_y;
    float target_prev_vx;
    float target_prev_vy;

    float algo_x;
    float algo_y;
    float edge_x;
    float edge_y;
    float edge_lead;

    uint32_t algo_hits;
    uint32_t edge_hits;
} drone_hunter_scene_t;

static drone_hunter_scene_t g_scene;

static float clampf(float v, float lo, float hi)
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

static void set_obj_center(lv_obj_t *obj, float x, float y)
{
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    lv_obj_set_pos(obj, (int32_t)(x - (float)w * 0.5f), (int32_t)(y - (float)h * 0.5f));
}

static void update_hud(drone_hunter_scene_t *s)
{
    char line[96];

    float algo_err = sqrtf((s->target_x - s->algo_x) * (s->target_x - s->algo_x) +
                           (s->target_y - s->algo_y) * (s->target_y - s->algo_y));
    float edge_err = sqrtf((s->target_x - s->edge_x) * (s->target_x - s->edge_x) +
                           (s->target_y - s->edge_y) * (s->target_y - s->edge_y));

    (void)snprintf(line, sizeof(line), "ALGO miss: %3d px", (int)algo_err);
    lv_label_set_text(s->hud_algo, line);

    (void)snprintf(line, sizeof(line), "EdgeAI miss: %3d px", (int)edge_err);
    lv_label_set_text(s->hud_edge, line);

    (void)snprintf(line, sizeof(line), "Hits A/E: %lu / %lu", (unsigned long)s->algo_hits, (unsigned long)s->edge_hits);
    lv_label_set_text(s->hud_hits, line);
}

static void anim_cb(lv_timer_t *timer)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_timer_get_user_data(timer);

    float cx = (float)(s->arena_x + s->arena_w / 2);
    float cy = (float)(s->arena_y + s->arena_h / 2);
    float rx = (float)s->arena_w * 0.34f;
    float ry = (float)s->arena_h * 0.29f;

    s->t += 0.042f;

    s->target_prev_x = s->target_x;
    s->target_prev_y = s->target_y;
    s->target_prev_vx = s->target_vx;
    s->target_prev_vy = s->target_vy;

    s->target_x = cx + sinf(s->t * 1.05f) * rx + sinf(s->t * 2.60f) * (rx * 0.22f);
    s->target_y = cy + cosf(s->t * 0.92f) * ry + cosf(s->t * 2.10f) * (ry * 0.18f);

    s->target_x = clampf(s->target_x, (float)s->arena_x + 12.0f, (float)(s->arena_x + s->arena_w - 12));
    s->target_y = clampf(s->target_y, (float)s->arena_y + 12.0f, (float)(s->arena_y + s->arena_h - 12));

    s->target_vx = s->target_x - s->target_prev_x;
    s->target_vy = s->target_y - s->target_prev_y;

    /* ALGO predictor: constant-velocity lead */
    float algo_pred_x = s->target_x + s->target_vx * 6.5f;
    float algo_pred_y = s->target_y + s->target_vy * 6.5f;

    /* EdgeAI-like predictor: velocity + curvature estimate + adaptive lead */
    float ax = s->target_vx - s->target_prev_vx;
    float ay = s->target_vy - s->target_prev_vy;
    float edge_pred_x = s->target_x + s->target_vx * s->edge_lead + ax * 7.0f;
    float edge_pred_y = s->target_y + s->target_vy * s->edge_lead + ay * 7.0f;

    algo_pred_x = clampf(algo_pred_x, (float)s->arena_x + 10.0f, (float)(s->arena_x + s->arena_w - 10));
    algo_pred_y = clampf(algo_pred_y, (float)s->arena_y + 10.0f, (float)(s->arena_y + s->arena_h - 10));
    edge_pred_x = clampf(edge_pred_x, (float)s->arena_x + 10.0f, (float)(s->arena_x + s->arena_w - 10));
    edge_pred_y = clampf(edge_pred_y, (float)s->arena_y + 10.0f, (float)(s->arena_y + s->arena_h - 10));

    /* Pursuit update */
    s->algo_x += (algo_pred_x - s->algo_x) * 0.090f;
    s->algo_y += (algo_pred_y - s->algo_y) * 0.090f;
    s->edge_x += (edge_pred_x - s->edge_x) * 0.120f;
    s->edge_y += (edge_pred_y - s->edge_y) * 0.120f;

    float algo_err = sqrtf((s->target_x - s->algo_x) * (s->target_x - s->algo_x) +
                           (s->target_y - s->algo_y) * (s->target_y - s->algo_y));
    float edge_err = sqrtf((s->target_x - s->edge_x) * (s->target_x - s->edge_x) +
                           (s->target_y - s->edge_y) * (s->target_y - s->edge_y));

    /* Simple adaptation: if EdgeAI error rises, increase lead slightly; else relax */
    if (edge_err > algo_err)
    {
        s->edge_lead = clampf(s->edge_lead + 0.03f, 5.5f, 11.0f);
    }
    else
    {
        s->edge_lead = clampf(s->edge_lead - 0.01f, 5.5f, 11.0f);
    }

    if (algo_err < 11.0f)
    {
        s->algo_hits++;
    }
    if (edge_err < 11.0f)
    {
        s->edge_hits++;
    }

    set_obj_center(s->target, s->target_x, s->target_y);
    set_obj_center(s->algo_drone, s->algo_x, s->algo_y);
    set_obj_center(s->edge_drone, s->edge_x, s->edge_y);
    set_obj_center(s->algo_pred, algo_pred_x, algo_pred_y);
    set_obj_center(s->edge_pred, edge_pred_x, edge_pred_y);

    update_hud(s);
}

static void splash_done_cb(lv_timer_t *timer)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_timer_get_user_data(timer);
    lv_obj_add_flag(s->splash, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s->arena, LV_OBJ_FLAG_HIDDEN);
}

void drone_hunter_arena_start(lv_obj_t *screen)
{
    drone_hunter_scene_t *s = &g_scene;
    s->screen = screen;

    int32_t sw = lv_obj_get_width(screen);
    int32_t sh = lv_obj_get_height(screen);

    s->arena_x = 24;
    s->arena_y = 56;
    s->arena_w = sw - 48;
    s->arena_h = sh - 88;

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x05070B), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    /* Splash layer (kept as startup identity) */
    s->splash = lv_obj_create(screen);
    lv_obj_remove_style_all(s->splash);
    lv_obj_set_size(s->splash, sw, sh);
    lv_obj_set_style_bg_color(s->splash, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(s->splash, LV_OPA_COVER, 0);

    lv_obj_t *title = lv_label_create(s->splash);
    lv_label_set_text(title, "EdgeAI Drone Hunter");
    lv_obj_set_style_text_color(title, lv_color_hex(0x00FF99), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -12);

    lv_obj_t *subtitle = lv_label_create(s->splash);
    lv_label_set_text(subtitle, "Infineon PSOC Edge E8");
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_20, 0);
    lv_obj_align(subtitle, LV_ALIGN_CENTER, 0, 22);

    /* Arena root */
    s->arena = lv_obj_create(screen);
    lv_obj_remove_style_all(s->arena);
    lv_obj_set_size(s->arena, sw, sh);
    lv_obj_add_flag(s->arena, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *hud = lv_obj_create(s->arena);
    lv_obj_remove_style_all(hud);
    lv_obj_set_size(hud, sw - 16, 44);
    lv_obj_set_pos(hud, 8, 8);
    lv_obj_set_style_bg_color(hud, lv_color_hex(0x111827), 0);
    lv_obj_set_style_bg_opa(hud, LV_OPA_70, 0);
    lv_obj_set_style_border_color(hud, lv_color_hex(0x334155), 0);
    lv_obj_set_style_border_width(hud, 1, 0);

    lv_obj_t *mode = lv_label_create(hud);
    lv_label_set_text(mode, "MODE: POV INTERCEPT ARENA  |  TARGET: X-WING / FIXED-WING");
    lv_obj_set_style_text_color(mode, lv_color_hex(0x93C5FD), 0);
    lv_obj_set_style_text_font(mode, &lv_font_montserrat_12, 0);
    lv_obj_align(mode, LV_ALIGN_TOP_LEFT, 8, 4);

    s->hud_algo = lv_label_create(hud);
    lv_obj_set_style_text_color(s->hud_algo, lv_color_hex(0xF59E0B), 0);
    lv_obj_set_style_text_font(s->hud_algo, &lv_font_montserrat_12, 0);
    lv_obj_align(s->hud_algo, LV_ALIGN_BOTTOM_LEFT, 8, -4);

    s->hud_edge = lv_label_create(hud);
    lv_obj_set_style_text_color(s->hud_edge, lv_color_hex(0x22D3EE), 0);
    lv_obj_set_style_text_font(s->hud_edge, &lv_font_montserrat_12, 0);
    lv_obj_align(s->hud_edge, LV_ALIGN_BOTTOM_LEFT, 180, -4);

    s->hud_hits = lv_label_create(hud);
    lv_obj_set_style_text_color(s->hud_hits, lv_color_hex(0xE5E7EB), 0);
    lv_obj_set_style_text_font(s->hud_hits, &lv_font_montserrat_12, 0);
    lv_obj_align(s->hud_hits, LV_ALIGN_BOTTOM_RIGHT, -10, -4);

    lv_obj_t *field = lv_obj_create(s->arena);
    lv_obj_remove_style_all(field);
    lv_obj_set_pos(field, s->arena_x, s->arena_y);
    lv_obj_set_size(field, s->arena_w, s->arena_h);
    lv_obj_set_style_bg_color(field, lv_color_hex(0x030712), 0);
    lv_obj_set_style_bg_opa(field, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(field, lv_color_hex(0x1F2937), 0);
    lv_obj_set_style_border_width(field, 2, 0);

    for (int i = 0; i < STAR_COUNT; ++i)
    {
        float a = ((float)i / (float)STAR_COUNT) * 2.0f * PI_F;
        float r = 0.15f + ((float)(i % 11) / 11.0f) * 0.82f;
        int32_t sx = s->arena_x + (int32_t)((s->arena_w * 0.5f) + cosf(a * 2.3f) * (s->arena_w * 0.46f) * r);
        int32_t sy = s->arena_y + (int32_t)((s->arena_h * 0.5f) + sinf(a * 1.7f) * (s->arena_h * 0.44f) * r);

        lv_obj_t *dot = lv_obj_create(s->arena);
        lv_obj_remove_style_all(dot);
        lv_obj_set_size(dot, 2 + (i % 2), 2 + (i % 2));
        lv_obj_set_pos(dot, sx, sy);
        lv_obj_set_style_bg_color(dot, (i % 3 == 0) ? lv_color_hex(0x38BDF8) : lv_color_hex(0x94A3B8), 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_60, 0);
    }

    s->target = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->target);
    lv_obj_set_size(s->target, 14, 14);
    lv_obj_set_style_radius(s->target, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s->target, lv_color_hex(0xEF4444), 0);
    lv_obj_set_style_bg_opa(s->target, LV_OPA_COVER, 0);

    s->algo_drone = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->algo_drone);
    lv_obj_set_size(s->algo_drone, 12, 12);
    lv_obj_set_style_radius(s->algo_drone, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s->algo_drone, lv_color_hex(0xF59E0B), 0);
    lv_obj_set_style_bg_opa(s->algo_drone, LV_OPA_COVER, 0);

    s->edge_drone = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->edge_drone);
    lv_obj_set_size(s->edge_drone, 12, 12);
    lv_obj_set_style_radius(s->edge_drone, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s->edge_drone, lv_color_hex(0x22D3EE), 0);
    lv_obj_set_style_bg_opa(s->edge_drone, LV_OPA_COVER, 0);

    s->algo_pred = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->algo_pred);
    lv_obj_set_size(s->algo_pred, 6, 6);
    lv_obj_set_style_radius(s->algo_pred, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s->algo_pred, lv_color_hex(0xFDBA74), 0);
    lv_obj_set_style_bg_opa(s->algo_pred, LV_OPA_80, 0);

    s->edge_pred = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->edge_pred);
    lv_obj_set_size(s->edge_pred, 6, 6);
    lv_obj_set_style_radius(s->edge_pred, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s->edge_pred, lv_color_hex(0x67E8F9), 0);
    lv_obj_set_style_bg_opa(s->edge_pred, LV_OPA_80, 0);

    s->t = 0.0f;
    s->target_x = (float)(s->arena_x + s->arena_w / 2);
    s->target_y = (float)(s->arena_y + s->arena_h / 2);
    s->target_prev_x = s->target_x;
    s->target_prev_y = s->target_y;
    s->target_vx = 0.0f;
    s->target_vy = 0.0f;
    s->target_prev_vx = 0.0f;
    s->target_prev_vy = 0.0f;

    s->algo_x = (float)(s->arena_x + 28);
    s->algo_y = (float)(s->arena_y + s->arena_h - 24);
    s->edge_x = (float)(s->arena_x + s->arena_w - 28);
    s->edge_y = (float)(s->arena_y + s->arena_h - 24);
    s->edge_lead = 7.5f;
    s->algo_hits = 0;
    s->edge_hits = 0;

    update_hud(s);
    set_obj_center(s->target, s->target_x, s->target_y);
    set_obj_center(s->algo_drone, s->algo_x, s->algo_y);
    set_obj_center(s->edge_drone, s->edge_x, s->edge_y);

    s->anim_timer = lv_timer_create(anim_cb, TICK_MS, s);
    s->splash_timer = lv_timer_create(splash_done_cb, SPLASH_MS, s);
    lv_timer_set_repeat_count(s->splash_timer, 1);
}
