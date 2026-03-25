#include "drone_hunter_arena.h"

#include <math.h>
#include <stdio.h>

#define PI_F                      (3.14159265359f)
#define STAR_COUNT                (48)
#define TICK_MS                   (33)
#define SPLASH_MS                 (1800)
#define DT_SEC                    (0.033f)

#define HUNTER_COUNT              (3)
#define KILLER_COUNT              (2)

#define START_LIVES               (3)
#define START_TIME_SEC            (90.0f)
#define MAX_WAVES                 (3)
#define CORE_RADIUS_PX            (18.0f)

typedef enum
{
    STATE_SEARCH = 0,
    STATE_LOCK,
    STATE_INTERCEPT,
    STATE_CONFIRM,
    STATE_WAVE_END,
    STATE_GAME_OVER,
    STATE_VICTORY
} mission_state_t;

typedef struct
{
    lv_obj_t *screen;
    lv_obj_t *splash;
    lv_obj_t *arena;

    lv_obj_t *hunters[HUNTER_COUNT];
    lv_obj_t *killers[KILLER_COUNT];
    lv_obj_t *core;

    lv_obj_t *hud_mode;
    lv_obj_t *hud_stats;
    lv_obj_t *hud_dist;

    lv_obj_t *overlay;
    lv_obj_t *overlay_title;
    lv_obj_t *overlay_subtitle;
    lv_obj_t *restart_btn;

    lv_timer_t *anim_timer;
    lv_timer_t *splash_timer;

    int32_t arena_x;
    int32_t arena_y;
    int32_t arena_w;
    int32_t arena_h;

    float t;
    float hx[HUNTER_COUNT];
    float hy[HUNTER_COUNT];

    float kx[KILLER_COUNT];
    float ky[KILLER_COUNT];
    float kvx[KILLER_COUNT];
    float kvy[KILLER_COUNT];

    mission_state_t state;
    float state_timer;

    int lives;
    int score;
    int wave;
    int wave_intercepts;
    int total_intercepts;
    float mission_time_sec;
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

static float dist2(float ax, float ay, float bx, float by)
{
    float dx = ax - bx;
    float dy = ay - by;
    return (dx * dx) + (dy * dy);
}

static void set_obj_center(lv_obj_t *obj, float x, float y)
{
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    lv_obj_set_pos(obj, (int32_t)(x - (float)w * 0.5f), (int32_t)(y - (float)h * 0.5f));
}

static const char *state_name(mission_state_t s)
{
    switch (s)
    {
        case STATE_SEARCH: return "SEARCH";
        case STATE_LOCK: return "LOCK";
        case STATE_INTERCEPT: return "INTERCEPT";
        case STATE_CONFIRM: return "CONFIRM";
        case STATE_WAVE_END: return "WAVE END";
        case STATE_GAME_OVER: return "GAME OVER";
        case STATE_VICTORY: return "VICTORY";
        default: return "-";
    }
}

static int target_intercepts_for_wave(int wave)
{
    return 5 + ((wave - 1) * 3);
}

static void set_state(drone_hunter_scene_t *s, mission_state_t st, float timeout_sec)
{
    s->state = st;
    s->state_timer = timeout_sec;
}

static void respawn_killer(drone_hunter_scene_t *s, int k, int side)
{
    float top_y = (float)s->arena_y + 30.0f + 10.0f * (float)k;
    float left_x = (float)s->arena_x + 40.0f;
    float right_x = (float)(s->arena_x + s->arena_w - 40);

    s->kx[k] = (side == 0) ? left_x : right_x;
    s->ky[k] = top_y;
    s->kvx[k] = 0.0f;
    s->kvy[k] = 0.0f;
}

static void reset_hunters(drone_hunter_scene_t *s)
{
    s->hx[0] = (float)(s->arena_x + 32);
    s->hy[0] = (float)(s->arena_y + s->arena_h - 24);
    s->hx[1] = (float)(s->arena_x + s->arena_w / 2);
    s->hy[1] = (float)(s->arena_y + s->arena_h - 18);
    s->hx[2] = (float)(s->arena_x + s->arena_w - 32);
    s->hy[2] = (float)(s->arena_y + s->arena_h - 24);
}

static void reset_wave(drone_hunter_scene_t *s, int wave)
{
    s->wave = wave;
    s->wave_intercepts = 0;
    s->t = 0.0f;

    reset_hunters(s);
    respawn_killer(s, 0, 0);
    respawn_killer(s, 1, 1);

    set_state(s, STATE_SEARCH, 0.8f);
}

static void hide_overlay(drone_hunter_scene_t *s)
{
    lv_obj_add_flag(s->overlay, LV_OBJ_FLAG_HIDDEN);
}

static void show_overlay(drone_hunter_scene_t *s, const char *title, const char *subtitle)
{
    lv_label_set_text(s->overlay_title, title);
    lv_label_set_text(s->overlay_subtitle, subtitle);
    lv_obj_clear_flag(s->overlay, LV_OBJ_FLAG_HIDDEN);
}

static void restart_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);

    s->lives = START_LIVES;
    s->score = 0;
    s->total_intercepts = 0;
    s->mission_time_sec = START_TIME_SEC;

    hide_overlay(s);
    reset_wave(s, 1);
}

static void update_hud(drone_hunter_scene_t *s)
{
    char line[128];

    float core_x = (float)(s->arena_x + s->arena_w / 2);
    float core_y = (float)(s->arena_y + s->arena_h / 2);

    float d_k1_core = sqrtf(dist2(s->kx[0], s->ky[0], core_x, core_y));
    float d_k2_core = sqrtf(dist2(s->kx[1], s->ky[1], core_x, core_y));

    (void)snprintf(line, sizeof(line),
                   "STATE: %s | WAVE %d/%d | TIME %02d | LIVES %d",
                   state_name(s->state), s->wave, MAX_WAVES, (int)s->mission_time_sec, s->lives);
    lv_label_set_text(s->hud_mode, line);

    (void)snprintf(line, sizeof(line),
                   "SCORE %d | INTERCEPTS %d | WAVE TARGET %d",
                   s->score, s->total_intercepts, target_intercepts_for_wave(s->wave));
    lv_label_set_text(s->hud_stats, line);

    (void)snprintf(line, sizeof(line),
                   "K1->CORE %3d px | K2->CORE %3d px",
                   (int)d_k1_core, (int)d_k2_core);
    lv_label_set_text(s->hud_dist, line);
}

static void update_positions(drone_hunter_scene_t *s)
{
    int h;
    int k;

    for (h = 0; h < HUNTER_COUNT; ++h)
    {
        set_obj_center(s->hunters[h], s->hx[h], s->hy[h]);
    }
    for (k = 0; k < KILLER_COUNT; ++k)
    {
        set_obj_center(s->killers[k], s->kx[k], s->ky[k]);
    }
}

static void anim_cb(lv_timer_t *timer)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_timer_get_user_data(timer);
    float core_x = (float)(s->arena_x + s->arena_w / 2);
    float core_y = (float)(s->arena_y + s->arena_h / 2);
    float min_d_hk = 10000.0f;
    int any_capture = 0;
    int k;
    int h;

    if (lv_obj_has_flag(s->arena, LV_OBJ_FLAG_HIDDEN))
    {
        return;
    }

    if (s->state == STATE_GAME_OVER || s->state == STATE_VICTORY)
    {
        update_hud(s);
        update_positions(s);
        return;
    }

    s->mission_time_sec -= DT_SEC;
    if (s->mission_time_sec <= 0.0f)
    {
        s->mission_time_sec = 0.0f;
        set_state(s, STATE_GAME_OVER, 0.0f);
        show_overlay(s, "MISSION FAILED", "Time expired. Tap RESTART.");
        update_hud(s);
        return;
    }

    s->t += 0.040f;

    for (k = 0; k < KILLER_COUNT; ++k)
    {
        float dir_x = core_x - s->kx[k];
        float dir_y = core_y - s->ky[k];
        float d = sqrtf((dir_x * dir_x) + (dir_y * dir_y));
        float speed = 1.10f + 0.22f * (float)s->wave;
        float evasive_x;
        float evasive_y;

        if (d > 1.0f)
        {
            dir_x /= d;
            dir_y /= d;
        }

        evasive_x = sinf(s->t * (1.8f + 0.3f * (float)k) + (float)k) * 0.65f;
        evasive_y = cosf(s->t * (2.1f + 0.2f * (float)k) + 0.7f * (float)k) * 0.65f;

        s->kvx[k] = (s->kvx[k] * 0.86f) + ((dir_x * speed) * 0.12f) + (evasive_x * 0.08f);
        s->kvy[k] = (s->kvy[k] * 0.86f) + ((dir_y * speed) * 0.12f) + (evasive_y * 0.08f);

        s->kx[k] += s->kvx[k];
        s->ky[k] += s->kvy[k];

        s->kx[k] = clampf(s->kx[k], (float)s->arena_x + 10.0f, (float)(s->arena_x + s->arena_w - 10));
        s->ky[k] = clampf(s->ky[k], (float)s->arena_y + 10.0f, (float)(s->arena_y + s->arena_h - 10));

        if (dist2(s->kx[k], s->ky[k], core_x, core_y) < (CORE_RADIUS_PX * CORE_RADIUS_PX))
        {
            s->lives--;
            respawn_killer(s, k, (k % 2));
            if (s->lives <= 0)
            {
                s->lives = 0;
                set_state(s, STATE_GAME_OVER, 0.0f);
                show_overlay(s, "MISSION FAILED", "Core destroyed. Tap RESTART.");
                update_hud(s);
                return;
            }
        }
    }

    for (h = 0; h < HUNTER_COUNT; ++h)
    {
        int tgt = 0;
        float d0 = dist2(s->hx[h], s->hy[h], s->kx[0], s->ky[0]);
        float d1 = dist2(s->hx[h], s->hy[h], s->kx[1], s->ky[1]);
        float gain = 0.082f + (0.018f * (float)h);

        if (d1 < d0)
        {
            tgt = 1;
            d0 = d1;
        }

        if (d0 < min_d_hk)
        {
            min_d_hk = d0;
        }

        s->hx[h] += (s->kx[tgt] - s->hx[h]) * gain;
        s->hy[h] += (s->ky[tgt] - s->hy[h]) * gain;

        if (d0 < (11.0f * 11.0f))
        {
            any_capture = 1;
            s->score += 100;
            s->total_intercepts++;
            s->wave_intercepts++;
            respawn_killer(s, tgt, (tgt + s->wave) % 2);
        }
    }

    if (s->state == STATE_SEARCH)
    {
        if (min_d_hk < (120.0f * 120.0f))
        {
            set_state(s, STATE_LOCK, 0.7f);
        }
    }
    else if (s->state == STATE_LOCK)
    {
        s->state_timer -= DT_SEC;
        if (s->state_timer <= 0.0f)
        {
            set_state(s, STATE_INTERCEPT, 0.0f);
        }
    }
    else if (s->state == STATE_INTERCEPT)
    {
        if (any_capture)
        {
            set_state(s, STATE_CONFIRM, 0.45f);
        }
    }
    else if (s->state == STATE_CONFIRM)
    {
        s->state_timer -= DT_SEC;
        if (s->state_timer <= 0.0f)
        {
            set_state(s, STATE_INTERCEPT, 0.0f);
        }
    }

    if (s->state != STATE_WAVE_END && s->wave_intercepts >= target_intercepts_for_wave(s->wave))
    {
        set_state(s, STATE_WAVE_END, 1.4f);
    }

    if (s->state == STATE_WAVE_END)
    {
        s->state_timer -= DT_SEC;
        if (s->state_timer <= 0.0f)
        {
            if (s->wave >= MAX_WAVES)
            {
                set_state(s, STATE_VICTORY, 0.0f);
                show_overlay(s, "MISSION SUCCESS", "All waves cleared. Tap RESTART.");
            }
            else
            {
                reset_wave(s, s->wave + 1);
            }
        }
    }

    update_positions(s);
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
    int32_t sw;
    int32_t sh;
    int i;

    s->screen = screen;
    sw = lv_obj_get_width(screen);
    sh = lv_obj_get_height(screen);

    s->arena_x = 24;
    s->arena_y = 56;
    s->arena_w = sw - 48;
    s->arena_h = sh - 88;

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x05070B), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    s->splash = lv_obj_create(screen);
    lv_obj_remove_style_all(s->splash);
    lv_obj_set_size(s->splash, sw, sh);
    lv_obj_set_style_bg_color(s->splash, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(s->splash, LV_OPA_COVER, 0);

    {
        lv_obj_t *title = lv_label_create(s->splash);
        lv_obj_t *subtitle = lv_label_create(s->splash);

        lv_label_set_text(title, "EdgeAI Drone Hunter");
        lv_obj_set_style_text_color(title, lv_color_hex(0x00FF99), 0);
        lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
        lv_obj_align(title, LV_ALIGN_CENTER, 0, -12);

        lv_label_set_text(subtitle, "Infineon PSOC Edge E8");
        lv_obj_set_style_text_color(subtitle, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_20, 0);
        lv_obj_align(subtitle, LV_ALIGN_CENTER, 0, 22);
    }

    s->arena = lv_obj_create(screen);
    lv_obj_remove_style_all(s->arena);
    lv_obj_set_size(s->arena, sw, sh);
    lv_obj_add_flag(s->arena, LV_OBJ_FLAG_HIDDEN);

    {
        lv_obj_t *hud = lv_obj_create(s->arena);
        lv_obj_remove_style_all(hud);
        lv_obj_set_size(hud, sw - 16, 44);
        lv_obj_set_pos(hud, 8, 8);
        lv_obj_set_style_bg_color(hud, lv_color_hex(0x111827), 0);
        lv_obj_set_style_bg_opa(hud, LV_OPA_70, 0);
        lv_obj_set_style_border_color(hud, lv_color_hex(0x334155), 0);
        lv_obj_set_style_border_width(hud, 1, 0);

        s->hud_mode = lv_label_create(hud);
        lv_obj_set_style_text_color(s->hud_mode, lv_color_hex(0x93C5FD), 0);
        lv_obj_set_style_text_font(s->hud_mode, &lv_font_montserrat_12, 0);
        lv_obj_align(s->hud_mode, LV_ALIGN_TOP_LEFT, 8, 4);

        s->hud_stats = lv_label_create(hud);
        lv_obj_set_style_text_color(s->hud_stats, lv_color_hex(0xFBBF24), 0);
        lv_obj_set_style_text_font(s->hud_stats, &lv_font_montserrat_12, 0);
        lv_obj_align(s->hud_stats, LV_ALIGN_BOTTOM_LEFT, 8, -4);

        s->hud_dist = lv_label_create(hud);
        lv_obj_set_style_text_color(s->hud_dist, lv_color_hex(0x67E8F9), 0);
        lv_obj_set_style_text_font(s->hud_dist, &lv_font_montserrat_12, 0);
        lv_obj_align(s->hud_dist, LV_ALIGN_BOTTOM_RIGHT, -8, -4);
    }

    {
        lv_obj_t *field = lv_obj_create(s->arena);
        lv_obj_remove_style_all(field);
        lv_obj_set_pos(field, s->arena_x, s->arena_y);
        lv_obj_set_size(field, s->arena_w, s->arena_h);
        lv_obj_set_style_bg_color(field, lv_color_hex(0x030712), 0);
        lv_obj_set_style_bg_opa(field, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(field, lv_color_hex(0x1F2937), 0);
        lv_obj_set_style_border_width(field, 2, 0);
    }

    for (i = 0; i < STAR_COUNT; ++i)
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

    s->core = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->core);
    lv_obj_set_size(s->core, (int32_t)(CORE_RADIUS_PX * 2.0f), (int32_t)(CORE_RADIUS_PX * 2.0f));
    lv_obj_set_style_radius(s->core, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s->core, lv_color_hex(0xA78BFA), 0);
    lv_obj_set_style_bg_opa(s->core, LV_OPA_70, 0);
    set_obj_center(s->core, (float)(s->arena_x + s->arena_w / 2), (float)(s->arena_y + s->arena_h / 2));

    for (i = 0; i < KILLER_COUNT; ++i)
    {
        s->killers[i] = lv_obj_create(s->arena);
        lv_obj_remove_style_all(s->killers[i]);
        lv_obj_set_size(s->killers[i], 14, 14);
        lv_obj_set_style_radius(s->killers[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(s->killers[i], lv_color_hex(0xEF4444), 0);
        lv_obj_set_style_bg_opa(s->killers[i], LV_OPA_COVER, 0);
    }

    for (i = 0; i < HUNTER_COUNT; ++i)
    {
        s->hunters[i] = lv_obj_create(s->arena);
        lv_obj_remove_style_all(s->hunters[i]);
        lv_obj_set_size(s->hunters[i], 12, 12);
        lv_obj_set_style_radius(s->hunters[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(s->hunters[i], (i == 0) ? lv_color_hex(0xF59E0B) : lv_color_hex(0x22D3EE), 0);
        lv_obj_set_style_bg_opa(s->hunters[i], LV_OPA_COVER, 0);
    }

    s->overlay = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->overlay);
    lv_obj_set_size(s->overlay, sw - 80, 130);
    lv_obj_align(s->overlay, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_bg_color(s->overlay, lv_color_hex(0x0F172A), 0);
    lv_obj_set_style_bg_opa(s->overlay, LV_OPA_80, 0);
    lv_obj_set_style_border_color(s->overlay, lv_color_hex(0x334155), 0);
    lv_obj_set_style_border_width(s->overlay, 2, 0);

    s->overlay_title = lv_label_create(s->overlay);
    lv_obj_set_style_text_color(s->overlay_title, lv_color_hex(0xE2E8F0), 0);
    lv_obj_set_style_text_font(s->overlay_title, &lv_font_montserrat_20, 0);
    lv_obj_align(s->overlay_title, LV_ALIGN_TOP_MID, 0, 12);

    s->overlay_subtitle = lv_label_create(s->overlay);
    lv_obj_set_style_text_color(s->overlay_subtitle, lv_color_hex(0x93C5FD), 0);
    lv_obj_set_style_text_font(s->overlay_subtitle, &lv_font_montserrat_14, 0);
    lv_obj_align(s->overlay_subtitle, LV_ALIGN_TOP_MID, 0, 46);

    s->restart_btn = lv_btn_create(s->overlay);
    lv_obj_set_size(s->restart_btn, 120, 34);
    lv_obj_align(s->restart_btn, LV_ALIGN_BOTTOM_MID, 0, -12);
    lv_obj_set_style_bg_color(s->restart_btn, lv_color_hex(0x2563EB), 0);
    lv_obj_set_style_bg_opa(s->restart_btn, LV_OPA_COVER, 0);
    lv_obj_add_event_cb(s->restart_btn, restart_cb, LV_EVENT_CLICKED, s);

    {
        lv_obj_t *btn_txt = lv_label_create(s->restart_btn);
        lv_label_set_text(btn_txt, "RESTART");
        lv_obj_set_style_text_color(btn_txt, lv_color_hex(0xFFFFFF), 0);
        lv_obj_center(btn_txt);
    }

    hide_overlay(s);

    s->lives = START_LIVES;
    s->score = 0;
    s->total_intercepts = 0;
    s->mission_time_sec = START_TIME_SEC;

    reset_wave(s, 1);
    update_positions(s);
    update_hud(s);

    s->anim_timer = lv_timer_create(anim_cb, TICK_MS, s);
    s->splash_timer = lv_timer_create(splash_done_cb, SPLASH_MS, s);
    lv_timer_set_repeat_count(s->splash_timer, 1);
}
