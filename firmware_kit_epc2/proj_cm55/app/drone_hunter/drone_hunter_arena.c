#include "drone_hunter_arena.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
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

typedef enum
{
    CAM_STRICT_FPV = 0,
    CAM_ASSIST_CHASE
} camera_mode_t;

typedef struct
{
    lv_obj_t *screen;
    lv_obj_t *splash;
    lv_obj_t *arena;

    lv_obj_t *hunters[HUNTER_COUNT];
    lv_obj_t *killers[KILLER_COUNT];
    lv_obj_t *core;
    lv_obj_t *reticle;
    lv_obj_t *intercept_cue;
    lv_obj_t *pov_marker;

    lv_obj_t *hud_mode;
    lv_obj_t *hud_stats;
    lv_obj_t *hud_dist;

    lv_obj_t *ctrl_panel;
    lv_obj_t *btn_up;
    lv_obj_t *btn_down;
    lv_obj_t *btn_left;
    lv_obj_t *btn_right;
    lv_obj_t *btn_boost;
    lv_obj_t *btn_brake;
    lv_obj_t *camera_btn;
    lv_obj_t *camera_label;
    lv_obj_t *throttle;

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
    float hvx[HUNTER_COUNT];
    float hvy[HUNTER_COUNT];

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

    float throttle_level;
    bool move_up;
    bool move_down;
    bool move_left;
    bool move_right;
    bool boost;
    bool brake;
    camera_mode_t cam_mode;

    int lock_target;
    float lock_distance_px;
    float lock_quality;
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

static const char *camera_name(camera_mode_t c)
{
    return (c == CAM_STRICT_FPV) ? "STRICT FPV" : "CHASE FPV";
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
    int i;

    s->hx[0] = (float)(s->arena_x + s->arena_w / 2);
    s->hy[0] = (float)(s->arena_y + s->arena_h - 34);
    s->hx[1] = (float)(s->arena_x + s->arena_w / 2 - 58);
    s->hy[1] = (float)(s->arena_y + s->arena_h - 28);
    s->hx[2] = (float)(s->arena_x + s->arena_w / 2 + 58);
    s->hy[2] = (float)(s->arena_y + s->arena_h - 28);

    for (i = 0; i < HUNTER_COUNT; ++i)
    {
        s->hvx[i] = 0.0f;
        s->hvy[i] = 0.0f;
    }
}

static void reset_player_controls(drone_hunter_scene_t *s)
{
    s->throttle_level = 0.45f;
    s->move_up = false;
    s->move_down = false;
    s->move_left = false;
    s->move_right = false;
    s->boost = false;
    s->brake = false;
    s->cam_mode = CAM_STRICT_FPV;
    s->lock_target = 0;
    s->lock_distance_px = 999.0f;
    s->lock_quality = 0.0f;

    if (s->throttle)
    {
        lv_slider_set_value(s->throttle, 45, LV_ANIM_OFF);
    }
    if (s->camera_label)
    {
        lv_label_set_text(s->camera_label, "CAM: STRICT FPV");
    }
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

    reset_player_controls(s);
    hide_overlay(s);
    reset_wave(s, 1);
}

static void throttle_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    int v = lv_slider_get_value(lv_event_get_target(e));
    s->throttle_level = (float)v / 100.0f;
}

static void movement_btn_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    lv_obj_t *target = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    bool active = (code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING);

    if (code != LV_EVENT_PRESSED && code != LV_EVENT_PRESSING && code != LV_EVENT_RELEASED && code != LV_EVENT_PRESS_LOST)
    {
        return;
    }

    if (target == s->btn_up)
    {
        s->move_up = active;
    }
    else if (target == s->btn_down)
    {
        s->move_down = active;
    }
    else if (target == s->btn_left)
    {
        s->move_left = active;
    }
    else if (target == s->btn_right)
    {
        s->move_right = active;
    }
}

static void boost_btn_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING)
    {
        s->boost = true;
    }
    else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
    {
        s->boost = false;
    }
}

static void brake_btn_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING)
    {
        s->brake = true;
    }
    else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
    {
        s->brake = false;
    }
}

static void camera_btn_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);

    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
    {
        return;
    }

    s->cam_mode = (s->cam_mode == CAM_STRICT_FPV) ? CAM_ASSIST_CHASE : CAM_STRICT_FPV;

    if (s->cam_mode == CAM_STRICT_FPV)
    {
        lv_label_set_text(s->camera_label, "CAM: STRICT FPV");
    }
    else
    {
        lv_label_set_text(s->camera_label, "CAM: CHASE FPV");
    }
}

static int nearest_killer_to_point(drone_hunter_scene_t *s, float x, float y, float *d2_out)
{
    float d0 = dist2(x, y, s->kx[0], s->ky[0]);
    float d1 = dist2(x, y, s->kx[1], s->ky[1]);

    if (d1 < d0)
    {
        if (d2_out)
        {
            *d2_out = d1;
        }
        return 1;
    }

    if (d2_out)
    {
        *d2_out = d0;
    }
    return 0;
}

static int most_threatening_killer(drone_hunter_scene_t *s, float core_x, float core_y)
{
    float d0 = dist2(core_x, core_y, s->kx[0], s->ky[0]);
    float d1 = dist2(core_x, core_y, s->kx[1], s->ky[1]);
    return (d1 < d0) ? 1 : 0;
}

static void update_lock_and_cue(drone_hunter_scene_t *s)
{
    float d2 = 0.0f;
    int tgt = nearest_killer_to_point(s, s->hx[0], s->hy[0], &d2);
    float d = sqrtf(d2);
    char line[96];
    float cue_x;
    float cue_y;

    s->lock_target = tgt;
    s->lock_distance_px = d;
    s->lock_quality = clampf(1.0f - (d / 240.0f), 0.0f, 1.0f);

    set_obj_center(s->reticle, s->kx[tgt], s->ky[tgt]);

    if (s->lock_quality > 0.70f)
    {
        lv_obj_set_style_border_color(s->reticle, lv_color_hex(0x22C55E), 0);
    }
    else if (s->lock_quality > 0.40f)
    {
        lv_obj_set_style_border_color(s->reticle, lv_color_hex(0xF59E0B), 0);
    }
    else
    {
        lv_obj_set_style_border_color(s->reticle, lv_color_hex(0xEF4444), 0);
    }

    cue_x = (s->hx[0] + s->kx[tgt]) * 0.5f;
    cue_y = (s->hy[0] + s->ky[tgt]) * 0.5f;
    set_obj_center(s->intercept_cue, cue_x, cue_y);

    (void)snprintf(line, sizeof(line), "LOCK %.0f%%", s->lock_quality * 100.0f);
    lv_label_set_text(s->intercept_cue, line);
}

static void update_hud(drone_hunter_scene_t *s)
{
    char line[160];

    float core_x = (float)(s->arena_x + s->arena_w / 2);
    float core_y = (float)(s->arena_y + s->arena_h / 2);

    float d_k1_core = sqrtf(dist2(s->kx[0], s->ky[0], core_x, core_y));
    float d_k2_core = sqrtf(dist2(s->kx[1], s->ky[1], core_x, core_y));

    (void)snprintf(line, sizeof(line),
                   "STATE: %s | %s | WAVE %d/%d | TIME %02d | LIVES %d",
                   state_name(s->state), camera_name(s->cam_mode), s->wave, MAX_WAVES, (int)s->mission_time_sec, s->lives);
    lv_label_set_text(s->hud_mode, line);

    (void)snprintf(line, sizeof(line),
                   "SCORE %d | INTERCEPTS %d | TARGET %d | THR %d%%",
                   s->score, s->total_intercepts, target_intercepts_for_wave(s->wave), (int)(s->throttle_level * 100.0f));
    lv_label_set_text(s->hud_stats, line);

    (void)snprintf(line, sizeof(line),
                   "K1->CORE %3d | K2->CORE %3d | LOCK T%d %3dpx",
                   (int)d_k1_core, (int)d_k2_core, s->lock_target + 1, (int)s->lock_distance_px);
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

    if (s->cam_mode == CAM_STRICT_FPV)
    {
        lv_obj_add_flag(s->hunters[0], LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(s->pov_marker, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_clear_flag(s->hunters[0], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s->pov_marker, LV_OBJ_FLAG_HIDDEN);
    }
}

static void update_player_hunter(drone_hunter_scene_t *s)
{
    float yaw = ((s->move_right ? 1.0f : 0.0f) - (s->move_left ? 1.0f : 0.0f));
    float pitch = ((s->move_down ? 1.0f : 0.0f) - (s->move_up ? 1.0f : 0.0f));
    float accel = 0.75f + (s->throttle_level * 2.5f);

    if (s->boost)
    {
        accel *= 1.7f;
    }
    if (s->brake)
    {
        accel *= 0.45f;
    }

    s->hvx[0] = (s->hvx[0] * 0.78f) + (yaw * accel * 0.55f);
    s->hvy[0] = (s->hvy[0] * 0.78f) + (pitch * accel * 0.55f);

    s->hx[0] += s->hvx[0];
    s->hy[0] += s->hvy[0];

    s->hx[0] = clampf(s->hx[0], (float)s->arena_x + 12.0f, (float)(s->arena_x + s->arena_w - 12));
    s->hy[0] = clampf(s->hy[0], (float)s->arena_y + 12.0f, (float)(s->arena_y + s->arena_h - 12));
}

static void update_wingmen(drone_hunter_scene_t *s, float core_x, float core_y)
{
    int h;

    for (h = 1; h < HUNTER_COUNT; ++h)
    {
        int threat = most_threatening_killer(s, core_x, core_y);
        float side = (h == 1) ? -38.0f : 38.0f;
        float escort_x = s->hx[0] + side;
        float escort_y = s->hy[0] + 22.0f;
        float seek_x = (escort_x * 0.45f) + (s->kx[threat] * 0.55f);
        float seek_y = (escort_y * 0.45f) + (s->ky[threat] * 0.55f);

        s->hvx[h] = (s->hvx[h] * 0.74f) + ((seek_x - s->hx[h]) * 0.09f);
        s->hvy[h] = (s->hvy[h] * 0.74f) + ((seek_y - s->hy[h]) * 0.09f);

        s->hx[h] += s->hvx[h];
        s->hy[h] += s->hvy[h];

        s->hx[h] = clampf(s->hx[h], (float)s->arena_x + 12.0f, (float)(s->arena_x + s->arena_w - 12));
        s->hy[h] = clampf(s->hy[h], (float)s->arena_y + 12.0f, (float)(s->arena_y + s->arena_h - 12));
    }
}

static void try_hunter_intercepts(drone_hunter_scene_t *s, int *any_capture, float *min_d_hk)
{
    int h;

    for (h = 0; h < HUNTER_COUNT; ++h)
    {
        float d2 = 0.0f;
        int tgt = nearest_killer_to_point(s, s->hx[h], s->hy[h], &d2);

        if (d2 < *min_d_hk)
        {
            *min_d_hk = d2;
        }

        if (d2 < (11.0f * 11.0f))
        {
            *any_capture = 1;
            s->score += (h == 0) ? 120 : 90;
            s->total_intercepts++;
            s->wave_intercepts++;
            respawn_killer(s, tgt, (tgt + s->wave + h) % 2);
        }
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

    if (lv_obj_has_flag(s->arena, LV_OBJ_FLAG_HIDDEN))
    {
        return;
    }

    if (s->state == STATE_GAME_OVER || s->state == STATE_VICTORY)
    {
        lv_obj_add_flag(s->reticle, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s->intercept_cue, LV_OBJ_FLAG_HIDDEN);
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

    update_player_hunter(s);
    update_wingmen(s, core_x, core_y);
    try_hunter_intercepts(s, &any_capture, &min_d_hk);

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

    lv_obj_clear_flag(s->reticle, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s->intercept_cue, LV_OBJ_FLAG_HIDDEN);
    update_lock_and_cue(s);
    update_positions(s);
    update_hud(s);
}

static void splash_done_cb(lv_timer_t *timer)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_timer_get_user_data(timer);
    lv_obj_add_flag(s->splash, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s->arena, LV_OBJ_FLAG_HIDDEN);
}

static void style_ctrl_btn(lv_obj_t *btn, lv_color_t color)
{
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_80, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0xE2E8F0), 0);
    lv_obj_set_style_border_width(btn, 1, 0);
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
    s->arena_h = sh - 176;

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
        lv_obj_set_size(hud, sw - 16, 52);
        lv_obj_set_pos(hud, 8, 8);
        lv_obj_set_style_bg_color(hud, lv_color_hex(0x111827), 0);
        lv_obj_set_style_bg_opa(hud, LV_OPA_70, 0);
        lv_obj_set_style_border_color(hud, lv_color_hex(0x334155), 0);
        lv_obj_set_style_border_width(hud, 1, 0);

        s->hud_mode = lv_label_create(hud);
        lv_obj_set_style_text_color(s->hud_mode, lv_color_hex(0x93C5FD), 0);
        lv_obj_set_style_text_font(s->hud_mode, &lv_font_montserrat_12, 0);
        lv_obj_align(s->hud_mode, LV_ALIGN_TOP_LEFT, 8, 3);

        s->hud_stats = lv_label_create(hud);
        lv_obj_set_style_text_color(s->hud_stats, lv_color_hex(0xFBBF24), 0);
        lv_obj_set_style_text_font(s->hud_stats, &lv_font_montserrat_12, 0);
        lv_obj_align(s->hud_stats, LV_ALIGN_CENTER, 0, 0);

        s->hud_dist = lv_label_create(hud);
        lv_obj_set_style_text_color(s->hud_dist, lv_color_hex(0x67E8F9), 0);
        lv_obj_set_style_text_font(s->hud_dist, &lv_font_montserrat_12, 0);
        lv_obj_align(s->hud_dist, LV_ALIGN_BOTTOM_LEFT, 8, -3);

        s->camera_btn = lv_btn_create(hud);
        lv_obj_set_size(s->camera_btn, 130, 24);
        lv_obj_align(s->camera_btn, LV_ALIGN_RIGHT_MID, -8, 0);
        lv_obj_set_style_bg_color(s->camera_btn, lv_color_hex(0x0EA5E9), 0);
        lv_obj_set_style_bg_opa(s->camera_btn, LV_OPA_80, 0);
        lv_obj_add_event_cb(s->camera_btn, camera_btn_cb, LV_EVENT_CLICKED, s);

        s->camera_label = lv_label_create(s->camera_btn);
        lv_label_set_text(s->camera_label, "CAM: STRICT FPV");
        lv_obj_set_style_text_color(s->camera_label, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_font(s->camera_label, &lv_font_montserrat_12, 0);
        lv_obj_center(s->camera_label);
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
        lv_obj_set_size(s->hunters[i], (i == 0) ? 14 : 12, (i == 0) ? 14 : 12);
        lv_obj_set_style_radius(s->hunters[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(s->hunters[i], (i == 0) ? lv_color_hex(0xF59E0B) : lv_color_hex(0x22D3EE), 0);
        lv_obj_set_style_bg_opa(s->hunters[i], LV_OPA_COVER, 0);
    }

    s->reticle = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->reticle);
    lv_obj_set_size(s->reticle, 24, 24);
    lv_obj_set_style_radius(s->reticle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(s->reticle, LV_OPA_0, 0);
    lv_obj_set_style_border_width(s->reticle, 2, 0);
    lv_obj_set_style_border_color(s->reticle, lv_color_hex(0x22C55E), 0);

    s->intercept_cue = lv_label_create(s->arena);
    lv_obj_set_style_text_color(s->intercept_cue, lv_color_hex(0xE2E8F0), 0);
    lv_obj_set_style_text_font(s->intercept_cue, &lv_font_montserrat_12, 0);
    lv_label_set_text(s->intercept_cue, "LOCK 0%");

    s->pov_marker = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->pov_marker);
    lv_obj_set_size(s->pov_marker, 8, 8);
    lv_obj_set_style_radius(s->pov_marker, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s->pov_marker, lv_color_hex(0xF8FAFC), 0);
    lv_obj_set_style_bg_opa(s->pov_marker, LV_OPA_COVER, 0);
    set_obj_center(s->pov_marker,
                   (float)(s->arena_x + s->arena_w / 2),
                   (float)(s->arena_y + s->arena_h - 10));

    s->ctrl_panel = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->ctrl_panel);
    lv_obj_set_size(s->ctrl_panel, sw - 16, 108);
    lv_obj_set_pos(s->ctrl_panel, 8, sh - 116);
    lv_obj_set_style_bg_color(s->ctrl_panel, lv_color_hex(0x0B1220), 0);
    lv_obj_set_style_bg_opa(s->ctrl_panel, LV_OPA_80, 0);
    lv_obj_set_style_border_color(s->ctrl_panel, lv_color_hex(0x334155), 0);
    lv_obj_set_style_border_width(s->ctrl_panel, 1, 0);

    s->btn_up = lv_btn_create(s->ctrl_panel);
    lv_obj_set_size(s->btn_up, 34, 28);
    lv_obj_set_pos(s->btn_up, 28, 8);
    style_ctrl_btn(s->btn_up, lv_color_hex(0x2563EB));
    lv_obj_add_event_cb(s->btn_up, movement_btn_cb, LV_EVENT_PRESSED, s);
    lv_obj_add_event_cb(s->btn_up, movement_btn_cb, LV_EVENT_PRESSING, s);
    lv_obj_add_event_cb(s->btn_up, movement_btn_cb, LV_EVENT_RELEASED, s);
    lv_obj_add_event_cb(s->btn_up, movement_btn_cb, LV_EVENT_PRESS_LOST, s);
    {
        lv_obj_t *t = lv_label_create(s->btn_up);
        lv_label_set_text(t, "UP");
        lv_obj_center(t);
    }

    s->btn_down = lv_btn_create(s->ctrl_panel);
    lv_obj_set_size(s->btn_down, 34, 28);
    lv_obj_set_pos(s->btn_down, 28, 72);
    style_ctrl_btn(s->btn_down, lv_color_hex(0x2563EB));
    lv_obj_add_event_cb(s->btn_down, movement_btn_cb, LV_EVENT_PRESSED, s);
    lv_obj_add_event_cb(s->btn_down, movement_btn_cb, LV_EVENT_PRESSING, s);
    lv_obj_add_event_cb(s->btn_down, movement_btn_cb, LV_EVENT_RELEASED, s);
    lv_obj_add_event_cb(s->btn_down, movement_btn_cb, LV_EVENT_PRESS_LOST, s);
    {
        lv_obj_t *t = lv_label_create(s->btn_down);
        lv_label_set_text(t, "DN");
        lv_obj_center(t);
    }

    s->btn_left = lv_btn_create(s->ctrl_panel);
    lv_obj_set_size(s->btn_left, 34, 28);
    lv_obj_set_pos(s->btn_left, 0, 40);
    style_ctrl_btn(s->btn_left, lv_color_hex(0x2563EB));
    lv_obj_add_event_cb(s->btn_left, movement_btn_cb, LV_EVENT_PRESSED, s);
    lv_obj_add_event_cb(s->btn_left, movement_btn_cb, LV_EVENT_PRESSING, s);
    lv_obj_add_event_cb(s->btn_left, movement_btn_cb, LV_EVENT_RELEASED, s);
    lv_obj_add_event_cb(s->btn_left, movement_btn_cb, LV_EVENT_PRESS_LOST, s);
    {
        lv_obj_t *t = lv_label_create(s->btn_left);
        lv_label_set_text(t, "LT");
        lv_obj_center(t);
    }

    s->btn_right = lv_btn_create(s->ctrl_panel);
    lv_obj_set_size(s->btn_right, 34, 28);
    lv_obj_set_pos(s->btn_right, 56, 40);
    style_ctrl_btn(s->btn_right, lv_color_hex(0x2563EB));
    lv_obj_add_event_cb(s->btn_right, movement_btn_cb, LV_EVENT_PRESSED, s);
    lv_obj_add_event_cb(s->btn_right, movement_btn_cb, LV_EVENT_PRESSING, s);
    lv_obj_add_event_cb(s->btn_right, movement_btn_cb, LV_EVENT_RELEASED, s);
    lv_obj_add_event_cb(s->btn_right, movement_btn_cb, LV_EVENT_PRESS_LOST, s);
    {
        lv_obj_t *t = lv_label_create(s->btn_right);
        lv_label_set_text(t, "RT");
        lv_obj_center(t);
    }

    s->btn_boost = lv_btn_create(s->ctrl_panel);
    lv_obj_set_size(s->btn_boost, 70, 36);
    lv_obj_set_pos(s->btn_boost, 106, 12);
    style_ctrl_btn(s->btn_boost, lv_color_hex(0x16A34A));
    lv_obj_add_event_cb(s->btn_boost, boost_btn_cb, LV_EVENT_PRESSED, s);
    lv_obj_add_event_cb(s->btn_boost, boost_btn_cb, LV_EVENT_PRESSING, s);
    lv_obj_add_event_cb(s->btn_boost, boost_btn_cb, LV_EVENT_RELEASED, s);
    lv_obj_add_event_cb(s->btn_boost, boost_btn_cb, LV_EVENT_PRESS_LOST, s);
    {
        lv_obj_t *t = lv_label_create(s->btn_boost);
        lv_label_set_text(t, "BOOST");
        lv_obj_center(t);
    }

    s->btn_brake = lv_btn_create(s->ctrl_panel);
    lv_obj_set_size(s->btn_brake, 70, 36);
    lv_obj_set_pos(s->btn_brake, 106, 58);
    style_ctrl_btn(s->btn_brake, lv_color_hex(0xB91C1C));
    lv_obj_add_event_cb(s->btn_brake, brake_btn_cb, LV_EVENT_PRESSED, s);
    lv_obj_add_event_cb(s->btn_brake, brake_btn_cb, LV_EVENT_PRESSING, s);
    lv_obj_add_event_cb(s->btn_brake, brake_btn_cb, LV_EVENT_RELEASED, s);
    lv_obj_add_event_cb(s->btn_brake, brake_btn_cb, LV_EVENT_PRESS_LOST, s);
    {
        lv_obj_t *t = lv_label_create(s->btn_brake);
        lv_label_set_text(t, "BRAKE");
        lv_obj_center(t);
    }

    s->throttle = lv_slider_create(s->ctrl_panel);
    lv_obj_set_size(s->throttle, 18, 88);
    lv_obj_set_pos(s->throttle, sw - 80, 10);
    lv_slider_set_range(s->throttle, 0, 100);
    lv_slider_set_value(s->throttle, 45, LV_ANIM_OFF);
    lv_obj_add_event_cb(s->throttle, throttle_cb, LV_EVENT_VALUE_CHANGED, s);
    lv_obj_set_style_bg_color(s->throttle, lv_color_hex(0x1E293B), LV_PART_MAIN);
    lv_obj_set_style_bg_color(s->throttle, lv_color_hex(0x06B6D4), LV_PART_INDICATOR);

    {
        lv_obj_t *t = lv_label_create(s->ctrl_panel);
        lv_label_set_text(t, "THR");
        lv_obj_set_style_text_color(t, lv_color_hex(0xE2E8F0), 0);
        lv_obj_set_style_text_font(t, &lv_font_montserrat_12, 0);
        lv_obj_set_pos(t, sw - 86, 0);
    }

    s->overlay = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->overlay);
    lv_obj_set_size(s->overlay, sw - 80, 130);
    lv_obj_align(s->overlay, LV_ALIGN_CENTER, 0, -18);
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

    reset_player_controls(s);
    reset_wave(s, 1);
    update_lock_and_cue(s);
    update_positions(s);
    update_hud(s);

    s->anim_timer = lv_timer_create(anim_cb, TICK_MS, s);
    s->splash_timer = lv_timer_create(splash_done_cb, SPLASH_MS, s);
    lv_timer_set_repeat_count(s->splash_timer, 1);
}
