#include "drone_hunter_arena.h"
#include "drone_hunter_images.h"
#include "drone_hunter_bagnet_detailed.h"
#include "drone_hunter_nightscape.h"
#include "drone_hunter_odin_detailed.h"
#include "drone_hunter_skyfall_detailed.h"
#include "drone_hunter_sting_detailed.h"
#include "drone_hunter_ciws.h"

#include <math.h>
#include <stdio.h>

#define PI_F                      (3.14159265359f)
#define TICK_MS                   (33)
#define SPLASH_TITLE_SEC          (5.0f)
#define DT_SEC                    (0.033f)
#define SPLASH_DRONE_COUNT        (4)

#define HUNTER_COUNT              (2)
#define KILLER_COUNT              (2)
#define HUNTER_STOCK_PER_TYPE     (120)
#define ATTACK_POOL_BASE          (240)
#define ATTACK_POOL_GROWTH        (180)
#define ATTACK_POOL_MAX           (2000)

#define ROUND_TIME_SEC            (36000.0f)
#define CORE_HP_START             (999)
#define CORE_RADIUS_PX            (18.0f)
#define ROUND_WIN_MARGIN          (10000)
#define FX_INTERCEPT_SEC          (0.42f)
#define FX_SPAWN_SEC              (0.36f)
#define FX_CORE_HIT_SEC           (0.40f)
#define CIWS_AMMO_UNLIMITED       (-1)
#define CIWS_TRACER_COUNT         (88)
#define CIWS_TRACER_LIFE_SEC      (0.20f)
#define CIWS_FIRE_COOLDOWN_SEC    (0.012f)
#define CIWS_BURST_PELLETS        (5)
#define CIWS_RANGE_FRAC           (0.75f)
#define CIWS_SWEEP_SPEED_RAD      (1.9f)
#define CIWS_SWEEP_HALF_CONE      (0.20f)
#define HUD_H                     (72)
#define DECK_H                    (122)
#define ARENA_MARGIN_X            (24)

typedef enum
{
    TARGET_FPV = 0,
    TARGET_FIXED_WING
} target_type_t;

typedef enum
{
    CTRL_ALGO = 0,
    CTRL_EDGEAI
} controller_t;

typedef enum
{
    MODE_ALGO_VS_ALGO = 0,
    MODE_EDGEAI_VS_EDGEAI,
    MODE_ALGO_VS_EDGEAI,
    MODE_COUNT
} match_mode_t;

typedef enum
{
    THREAT_FACTION_RUSSIA = 0,
    THREAT_FACTION_USA
} threat_faction_t;

typedef enum
{
    HUNTER_STING_II = 0,
    HUNTER_BAGNET,
    HUNTER_SKYFALL_P1,
    HUNTER_OCTOPUS_100,
    HUNTER_ODIN_WIN_HIT,
    HUNTER_VB140_FLAMINGO,
    HUNTER_TYTAN,
    HUNTER_MEROPS,
    HUNTER_TYPE_COUNT
} hunter_type_t;

typedef struct
{
    const char *name;
    float speed;
    float accel;
    float damping;
    float lead_gain;
    float kill_radius;
    int points_fpv;
    int points_fixed;
    int frame_w;
    int frame_h;
    int body_w;
    int body_h;
    int wing_w;
    int wing_h;
    int pod_w;
    int pod_h;
    uint32_t color_body;
    uint32_t color_wing;
    const char *desc;
} hunter_profile_t;

typedef struct
{
    lv_obj_t *screen;
    lv_obj_t *splash;
    lv_obj_t *arena;
    lv_obj_t *arena_bg;
    lv_obj_t *lineup_cont;
    lv_obj_t *lineup_cards[HUNTER_TYPE_COUNT];
    lv_obj_t *lineup_desc;
    lv_obj_t *splash_title;
    lv_obj_t *splash_subtitle;
    lv_obj_t *splash_hero;
    lv_obj_t *splash_hero_odin;
    lv_obj_t *splash_hero_skyfall;
    lv_obj_t *splash_start_btn;

    lv_obj_t *core;
    lv_obj_t *ciws;
    lv_obj_t *ciws_left;
    lv_obj_t *ciws_turret;
    lv_obj_t *ciws_tracer[CIWS_TRACER_COUNT];

    lv_obj_t *hunters[HUNTER_COUNT];
    lv_obj_t *hunter_tail[HUNTER_COUNT];
    lv_obj_t *hunter_body[HUNTER_COUNT];
    lv_obj_t *hunter_wing[HUNTER_COUNT];
    lv_obj_t *hunter_pod_upper[HUNTER_COUNT];
    lv_obj_t *hunter_pod_lower[HUNTER_COUNT];
    lv_obj_t *hunter_prop_upper[HUNTER_COUNT];
    lv_obj_t *hunter_prop_lower[HUNTER_COUNT];

    lv_obj_t *killers[KILLER_COUNT];
    lv_obj_t *killer_wing[KILLER_COUNT];
    lv_obj_t *killer_wing_upper[KILLER_COUNT];
    lv_obj_t *killer_wing_lower[KILLER_COUNT];
    lv_obj_t *killer_body[KILLER_COUNT];
    lv_obj_t *killer_nose[KILLER_COUNT];

    lv_obj_t *fx_intercept[KILLER_COUNT];
    lv_obj_t *fx_spawn[KILLER_COUNT];

    lv_obj_t *hud_mode;
    lv_obj_t *hud_score;
    lv_obj_t *hud_info;
    lv_obj_t *hud_wave;

    lv_obj_t *deck_bar;
    lv_obj_t *deck_icon[HUNTER_TYPE_COUNT];
    lv_obj_t *deck_name[HUNTER_TYPE_COUNT];
    lv_obj_t *deck_count[HUNTER_TYPE_COUNT];
    lv_obj_t *deck_ciws_icon;
    lv_obj_t *deck_ciws_name;
    lv_obj_t *deck_ciws_count;

    lv_obj_t *mode_btn;
    lv_obj_t *mode_btn_label;

    lv_obj_t *overlay;
    lv_obj_t *overlay_title;
    lv_obj_t *overlay_subtitle;
    lv_obj_t *restart_btn;

    lv_timer_t *anim_timer;
    lv_timer_t *splash_timer;

    int splash_phase;
    float splash_phase_sec;
    int lineup_focus_idx;
    float lineup_angle;
    float lineup_target_angle;
    int splash_center_idx;
    int splash_touch_active;
    int16_t splash_touch_start_x;
    int splash_last_center_idx;

    int32_t arena_x;
    int32_t arena_y;
    int32_t arena_w;
    int32_t arena_h;

    float hx[HUNTER_COUNT];
    float hy[HUNTER_COUNT];
    float hvx[HUNTER_COUNT];
    float hvy[HUNTER_COUNT];
    float h_heading[HUNTER_COUNT];
    hunter_type_t h_type[HUNTER_COUNT];
    float h_reselect_sec[HUNTER_COUNT];

    float kx[KILLER_COUNT];
    float ky[KILLER_COUNT];
    float kvx[KILLER_COUNT];
    float kvy[KILLER_COUNT];
    float k_heading[KILLER_COUNT];

    float fx_intercept_t[KILLER_COUNT];
    float fx_spawn_t[KILLER_COUNT];
    float fx_intercept_x[KILLER_COUNT];
    float fx_intercept_y[KILLER_COUNT];
    float fx_spawn_x[KILLER_COUNT];
    float fx_spawn_y[KILLER_COUNT];
    float fx_core_hit_t;
    float ciws_cooldown_sec;
    float ciws_cooldown_left_sec;
    float ciws_sweep_right_rad;
    float ciws_sweep_left_rad;
    float ciws_sweep_right_dir;
    float ciws_sweep_left_dir;
    float ciws_tracer_t[CIWS_TRACER_COUNT];
    float ciws_tracer_x0[CIWS_TRACER_COUNT];
    float ciws_tracer_y0[CIWS_TRACER_COUNT];
    float ciws_tracer_x1[CIWS_TRACER_COUNT];
    float ciws_tracer_y1[CIWS_TRACER_COUNT];
    float ciws_tracer_g[CIWS_TRACER_COUNT];

    int killer_spawn_tick;
    target_type_t ktype[KILLER_COUNT];
    int k_tier[KILLER_COUNT];
    int killer_active[KILLER_COUNT];
    int k_missed_by_hunter[KILLER_COUNT];

    match_mode_t mode;
    controller_t team_ctrl[HUNTER_COUNT];

    int team_score[HUNTER_COUNT];
    int hunter_stock[HUNTER_TYPE_COUNT];
    int attack_remaining_to_spawn;
    int attack_destroyed;
    int attack_leaked;
    int wave_idx;
    int wave_target_total;
    threat_faction_t threat_faction;
    int defense_kills;
    int defense_misses;
    int ciws_ammo;
    int ciws_shots;
    int ciws_kills;
    int ciws_tracer_head;
    int hunter_loaded[HUNTER_COUNT];
    int core_hp;
    float round_time_sec;
    int round_over;

    float t;
} drone_hunter_scene_t;

static drone_hunter_scene_t g_scene;
static void update_hunter_deck_ui(drone_hunter_scene_t *s);
static void respawn_killer(drone_hunter_scene_t *s, int k, int side);

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

static void create_ciws_model(lv_obj_t *parent, lv_obj_t **turret_out)
{
    lv_obj_t *base = lv_obj_create(parent);
    lv_obj_t *pedestal = lv_obj_create(parent);
    lv_obj_t *turret = lv_obj_create(parent);
    lv_obj_t *radome = lv_obj_create(parent);
    lv_obj_t *barrel = lv_obj_create(turret);

    lv_obj_remove_style_all(base);
    lv_obj_set_size(base, 54, 14);
    lv_obj_set_pos(base, 13, 54);
    lv_obj_set_style_radius(base, 3, 0);
    lv_obj_set_style_bg_color(base, lv_color_hex(0x4B5563), 0);
    lv_obj_set_style_bg_opa(base, LV_OPA_COVER, 0);

    lv_obj_remove_style_all(pedestal);
    lv_obj_set_size(pedestal, 22, 18);
    lv_obj_set_pos(pedestal, 29, 38);
    lv_obj_set_style_radius(pedestal, 3, 0);
    lv_obj_set_style_bg_color(pedestal, lv_color_hex(0x6B7280), 0);
    lv_obj_set_style_bg_opa(pedestal, LV_OPA_COVER, 0);

    lv_obj_remove_style_all(turret);
    lv_obj_set_size(turret, 34, 14);
    lv_obj_set_pos(turret, 24, 26);
    lv_obj_set_style_radius(turret, 4, 0);
    lv_obj_set_style_bg_color(turret, lv_color_hex(0x9CA3AF), 0);
    lv_obj_set_style_bg_opa(turret, LV_OPA_COVER, 0);
    lv_obj_set_style_transform_pivot_x(turret, 6, 0);
    lv_obj_set_style_transform_pivot_y(turret, 7, 0);

    lv_obj_remove_style_all(radome);
    lv_obj_set_size(radome, 16, 16);
    lv_obj_set_pos(radome, 35, 12);
    lv_obj_set_style_radius(radome, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(radome, lv_color_hex(0xE5E7EB), 0);
    lv_obj_set_style_bg_opa(radome, LV_OPA_COVER, 0);

    lv_obj_remove_style_all(barrel);
    lv_obj_set_size(barrel, 18, 3);
    lv_obj_set_pos(barrel, 16, 6);
    lv_obj_set_style_radius(barrel, 1, 0);
    lv_obj_set_style_bg_color(barrel, lv_color_hex(0x111827), 0);
    lv_obj_set_style_bg_opa(barrel, LV_OPA_COVER, 0);

    if (turret_out != NULL)
    {
        *turret_out = turret;
    }
}


static void update_fixed_wing_orientation(drone_hunter_scene_t *s, int k)
{
    int16_t angle_tenth = (int16_t)(s->k_heading[k] * (1800.0f / PI_F));
    int32_t w = lv_obj_get_width(s->killers[k]);
    int32_t h = lv_obj_get_height(s->killers[k]);

    lv_obj_set_style_transform_pivot_x(s->killers[k], (int32_t)(w / 2), 0);
    lv_obj_set_style_transform_pivot_y(s->killers[k], (int32_t)(h / 2), 0);
    lv_obj_set_style_transform_angle(s->killers[k], angle_tenth, 0);
}

static float wrap_angle_pi(float a)
{
    while (a > PI_F)
    {
        a -= 2.0f * PI_F;
    }
    while (a < -PI_F)
    {
        a += 2.0f * PI_F;
    }
    return a;
}

static int ciws_target_in_sweep(float gun_x, float gun_y,
                                float sweep_rad, float range_px,
                                float target_x, float target_y)
{
    float dx = target_x - gun_x;
    float dy = target_y - gun_y;
    float d2 = (dx * dx) + (dy * dy);
    float range2 = range_px * range_px;
    float target_ang;
    float d_ang;

    if (d2 < 1.0f || d2 > range2)
    {
        return 0;
    }

    target_ang = atan2f(dy, dx);
    d_ang = wrap_angle_pi(target_ang - sweep_rad);
    return (fabsf(d_ang) <= CIWS_SWEEP_HALF_CONE);
}

static int ciws_fire_at(drone_hunter_scene_t *s, int k, float gun_x, float gun_y,
                        float sweep_rad, float *cooldown_sec)
{
    float dir_x = cosf(sweep_rad);
    float dir_y = sinf(sweep_rad);
    float p_ciws = (s->k_tier[k] == 0) ? 0.88f : ((s->k_tier[k] == 1) ? 0.74f : 0.58f);
    float roll = 0.5f + (0.5f * sinf((s->t * 3.7f) + (float)(k * 5 + s->wave_idx)));

    *cooldown_sec = CIWS_FIRE_COOLDOWN_SEC;
    if (s->ciws_ammo > 0)
    {
        s->ciws_ammo--;
    }
    s->ciws_shots++;
    {
        int burst;
        float horizontal = fabsf(dir_x);
        float gravity = 520.0f + (horizontal * 2300.0f);

        for (burst = 0; burst < CIWS_BURST_PELLETS; ++burst)
        {
            int ti = s->ciws_tracer_head % CIWS_TRACER_COUNT;
            float spread = (((float)burst - 2.0f) * 0.03f) + (((float)((s->ciws_shots + burst) % 3) - 1.0f) * 0.012f);
            float speed = 780.0f + ((float)(burst * 28)) + ((float)(s->ciws_shots % 4) * 18.0f);

            s->ciws_tracer_head++;
            s->ciws_tracer_t[ti] = CIWS_TRACER_LIFE_SEC;
            s->ciws_tracer_x0[ti] = gun_x + (dir_x * 24.0f);
            s->ciws_tracer_y0[ti] = gun_y + (dir_y * 24.0f);
            s->ciws_tracer_x1[ti] = (dir_x - (dir_y * spread)) * speed;
            s->ciws_tracer_y1[ti] = (dir_y + (dir_x * spread)) * speed;
            s->ciws_tracer_g[ti] = gravity;
        }
    }
    update_hunter_deck_ui(s);

    if (roll <= p_ciws)
    {
        s->attack_destroyed++;
        s->defense_kills++;
        s->ciws_kills++;
        respawn_killer(s, k, (k + s->killer_spawn_tick + 1) % 4);
        return 1;
    }
    return 0;
}

static const char *ctrl_name(controller_t c)
{
    return (c == CTRL_EDGEAI) ? "EdgeAI" : "Algo";
}

static const char *mode_name(match_mode_t m)
{
    switch (m)
    {
        case MODE_ALGO_VS_ALGO: return "ALGO vs ALGO";
        case MODE_EDGEAI_VS_EDGEAI: return "EdgeAI vs EdgeAI";
        case MODE_ALGO_VS_EDGEAI: return "ALGO vs EdgeAI";
        default: return "?";
    }
}

static const char *threat_faction_name(threat_faction_t f)
{
    return (f == THREAT_FACTION_RUSSIA) ? "RUS ATTACK SWARM" : "USA ATTACK SWARM";
}

static int arena_phase(drone_hunter_scene_t *s)
{
    if (s->t < 20.0f)
    {
        return 0; /* Patrol */
    }
    if (s->t < 45.0f)
    {
        return 1; /* Intercept */
    }
    return 2; /* Swarm */
}

static const char *arena_phase_name(int phase)
{
    switch (phase)
    {
        case 0: return "PHASE 1: PATROL";
        case 1: return "PHASE 2: INTERCEPT";
        default: return "PHASE 3: SWARM";
    }
}

static const hunter_profile_t g_hunter_profiles[HUNTER_TYPE_COUNT] =
{
    {"Sting-II",      2.24f, 0.020f, 0.66f, 1.55f, 14.0f, 1, 3, 32, 20, 18, 7, 15, 4, 10, 5, 0xD1D5DB, 0x9CA3AF, "Sting-II is tuned for rapid intercepts against fast threats. It reaches speed quickly and keeps a stable nose-in pursuit path."},
    {"Bagnet",        1.86f, 0.016f, 0.72f, 1.10f, 13.0f, 2, 2, 30, 20, 16, 7, 14, 4,  9, 5, 0xD4D4D8, 0xA1A1AA, "Bagnet is a close-range hunter built for tight FPV engagements. It favors control and positioning over pure top speed."},
    {"Skyfall P1",    2.38f, 0.023f, 0.62f, 1.70f, 13.0f, 1, 3, 34, 20, 20, 6, 15, 3, 10, 4, 0xE5E7EB, 0x9CA3AF, "Skyfall P1 is optimized for predictive lead pursuit. It is one of the fastest options in the lineup for fixed-wing intercepts."},
    {"Octopus-100",   2.02f, 0.018f, 0.70f, 1.30f, 14.0f, 2, 2, 32, 20, 17, 7, 16, 4, 10, 5, 0xD6D3D1, 0xA8A29E, "Octopus-100 is a balanced multirotor platform for mixed threat environments. It trades extremes for consistent all-round behavior."},
    {"ODIN Win_Hit",  2.10f, 0.019f, 0.68f, 1.25f, 15.0f, 2, 2, 33, 21, 18, 8, 15, 4, 10, 5, 0xCBD5E1, 0x94A3B8, "ODIN Win_Hit emphasizes stable tracking and reliable direct-hit geometry. It performs well when holding line and avoiding overshoot."},
    {"VB140",         1.96f, 0.015f, 0.75f, 1.05f, 16.0f, 3, 1, 36, 18, 20, 6, 18, 3,  8, 4, 0xE7E5E4, 0xA8A29E, "VB140 uses a larger frame for close interception pressure. It is resilient in dense engagements and favors high-contact scoring."},
    {"Tytan",         2.22f, 0.021f, 0.64f, 1.50f, 14.0f, 1, 3, 34, 20, 19, 7, 15, 4, 10, 5, 0xE5E7EB, 0x9CA3AF, "Tytan is an aggressive fixed-wing hunter with strong approach speed. It is suited for decisive first-pass intercept attempts."},
    {"Merops",        2.06f, 0.018f, 0.69f, 1.20f, 15.0f, 2, 2, 35, 20, 19, 7, 16, 4, 10, 5, 0xD6D3D1, 0xA8A29E, "Merops is a general-purpose interceptor for variable weather and threat mixes. It balances predictability, control, and endurance."}
};

static const lv_image_dsc_t *hunter_image_src(hunter_type_t type)
{
    switch (type)
    {
        case HUNTER_STING_II:        return &img_hunter_sting_ii;
        case HUNTER_BAGNET:          return &img_hunter_bagnet;
        case HUNTER_SKYFALL_P1:      return &img_hunter_skyfall_p1;
        case HUNTER_OCTOPUS_100:     return &img_hunter_octopus_100;
        case HUNTER_ODIN_WIN_HIT:    return &img_hunter_odin_win_hit;
        case HUNTER_VB140_FLAMINGO:  return &img_hunter_vb140;
        case HUNTER_TYTAN:           return &img_hunter_tytan;
        case HUNTER_MEROPS:          return &img_hunter_merops;
        default:                     return &img_hunter_sting_ii;
    }
}

static int is_xwing_hunter(hunter_type_t type)
{
    return (type == HUNTER_STING_II) ||
           (type == HUNTER_ODIN_WIN_HIT) ||
           (type == HUNTER_SKYFALL_P1) ||
           (type == HUNTER_BAGNET) ||
           (type == HUNTER_OCTOPUS_100);
}

static const char *hunter_type_name(hunter_type_t t)
{
    return g_hunter_profiles[(int)t].name;
}

static int any_hunter_stock_remaining(drone_hunter_scene_t *s)
{
    int i;
    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        if (s->hunter_stock[i] > 0)
        {
            return 1;
        }
    }
    return 0;
}

static hunter_type_t fallback_hunter_type(drone_hunter_scene_t *s, target_type_t threat)
{
    int i;
    hunter_type_t best = HUNTER_STING_II;
    int best_stock = -1;

    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        if (s->hunter_stock[i] <= 0)
        {
            continue;
        }

        if (threat == TARGET_FIXED_WING)
        {
            if ((i == HUNTER_STING_II) || (i == HUNTER_SKYFALL_P1) || (i == HUNTER_TYTAN) || (i == HUNTER_MEROPS))
            {
                return (hunter_type_t)i;
            }
        }
        else
        {
            if ((i == HUNTER_BAGNET) || (i == HUNTER_OCTOPUS_100) || (i == HUNTER_ODIN_WIN_HIT) || (i == HUNTER_VB140_FLAMINGO))
            {
                return (hunter_type_t)i;
            }
        }

        if (s->hunter_stock[i] > best_stock)
        {
            best_stock = s->hunter_stock[i];
            best = (hunter_type_t)i;
        }
    }

    return best;
}

static float hunter_capability_score(hunter_type_t t)
{
    const hunter_profile_t *p = &g_hunter_profiles[(int)t];
    return (p->speed * 0.55f) + (p->lead_gain * 0.95f) + (p->kill_radius * 0.07f);
}

static float threat_required_score(int tier, target_type_t ktype)
{
    float base = (tier == 0) ? 2.10f : ((tier == 1) ? 2.65f : 3.20f);
    if (ktype == TARGET_FIXED_WING)
    {
        base += 0.22f;
    }
    return base;
}

static hunter_type_t choose_conservative_hunter(drone_hunter_scene_t *s, int target)
{
    int i;
    float req = threat_required_score(s->k_tier[target], s->ktype[target]);
    hunter_type_t best_fit = HUNTER_STING_II;
    float best_fit_score = 9999.0f;
    hunter_type_t best_any = HUNTER_STING_II;
    int best_any_stock = -1;

    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        float cap;
        if (s->hunter_stock[i] <= 0)
        {
            continue;
        }

        cap = hunter_capability_score((hunter_type_t)i);
        if (cap >= req)
        {
            float fit = cap - req;
            if (fit < best_fit_score)
            {
                best_fit_score = fit;
                best_fit = (hunter_type_t)i;
            }
        }

        if (s->hunter_stock[i] > best_any_stock)
        {
            best_any_stock = s->hunter_stock[i];
            best_any = (hunter_type_t)i;
        }
    }

    if (best_fit_score < 9999.0f)
    {
        return best_fit;
    }
    return best_any;
}

static hunter_type_t choose_hunter_type(target_type_t threat, int selector)
{
    static const hunter_type_t fast_vs_fixed[] =
    {
        HUNTER_STING_II,
        HUNTER_SKYFALL_P1,
        HUNTER_TYTAN,
        HUNTER_MEROPS
    };
    static const hunter_type_t agile_vs_fpv[] =
    {
        HUNTER_BAGNET,
        HUNTER_OCTOPUS_100,
        HUNTER_ODIN_WIN_HIT,
        HUNTER_VB140_FLAMINGO
    };

    if (threat == TARGET_FIXED_WING)
    {
        return fast_vs_fixed[selector & 0x3];
    }

    return agile_vs_fpv[selector & 0x3];
}

static void apply_hunter_profile(drone_hunter_scene_t *s, int h)
{
    const hunter_profile_t *p = &g_hunter_profiles[(int)s->h_type[h]];
    uint16_t zoom = (uint16_t)clampf(188.0f + (p->speed * 22.0f), 180.0f, 296.0f);
    lv_image_set_src(s->hunters[h], hunter_image_src(s->h_type[h]));
    lv_obj_set_style_transform_zoom(s->hunters[h], zoom, 0);
    lv_obj_set_style_opa(s->hunters[h], LV_OPA_COVER, 0);
    lv_obj_set_style_transform_width(s->hunters[h], 0, 0);
    lv_obj_set_style_transform_height(s->hunters[h], 0, 0);
    lv_obj_add_flag(s->hunter_tail[h], LV_OBJ_FLAG_HIDDEN);
}

static lv_obj_t *create_hunter_preview(lv_obj_t *parent, hunter_type_t type)
{
    const lv_image_dsc_t *img_src = hunter_image_src(type);
    lv_obj_t *img;
    int32_t iw;
    int32_t ih;

    img = lv_image_create(parent);
    lv_image_set_src(img, img_src);
    iw = lv_obj_get_width(img);
    ih = lv_obj_get_height(img);
    lv_obj_set_style_transform_pivot_x(img, iw / 2, 0);
    lv_obj_set_style_transform_pivot_y(img, ih / 2, 0);
    lv_obj_add_flag(img, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return img;
}

static void begin_arena_from_splash(drone_hunter_scene_t *s)
{
    if (!lv_obj_has_flag(s->splash, LV_OBJ_FLAG_HIDDEN))
    {
        lv_obj_add_flag(s->splash, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(s->arena, LV_OBJ_FLAG_HIDDEN);
    }
}

static float wrap_angle(float a)
{
    while (a > PI_F)
    {
        a -= 2.0f * PI_F;
    }
    while (a < -PI_F)
    {
        a += 2.0f * PI_F;
    }
    return a;
}

static lv_obj_t *splash_dock_obj(drone_hunter_scene_t *s, int idx)
{
    switch (idx)
    {
        case 0: return s->splash_hero_odin;     /* left slot */
        case 1: return s->splash_hero;          /* center slot */
        case 2: return s->splash_hero_skyfall;  /* right slot */
        default: return s->splash_hero_odin;
    }
}

static void update_splash_dock(drone_hunter_scene_t *s)
{
    int slot;
    int32_t sw = lv_obj_get_width(s->splash);
    int32_t sh = lv_obj_get_height(s->splash);
    int32_t btn_top = lv_obj_get_y(s->splash_start_btn);
    int32_t base_bottom = btn_top - 6;
    static const int slot_x_pct[3] = {23, 50, 77};
    static const float slot_lift[3] = {0.08f, 0.24f, 0.12f};
    static const uint16_t slot_zoom[3] = {560, 300, 420};
    static const lv_opa_t slot_opa[3] = {LV_OPA_COVER, LV_OPA_70, LV_OPA_90};
    static const lv_image_dsc_t *drone_src[SPLASH_DRONE_COUNT] =
    {
        &img_hunter_sting_ii_detailed,
        &img_hunter_odin_win_hit_detailed,
        &img_hunter_skyfall_p1_sun_detailed,
        &img_hunter_bagnet
    };
    static const float drone_scale[SPLASH_DRONE_COUNT] = {1.00f, 1.00f, 0.78f, 1.55f};

    if (btn_top <= 0 || btn_top >= sh)
    {
        base_bottom = sh - 52;
    }

    for (slot = 0; slot < 3; ++slot)
    {
        int drone_idx = (s->splash_center_idx + slot + SPLASH_DRONE_COUNT) % SPLASH_DRONE_COUNT;
        lv_obj_t *obj = splash_dock_obj(s, slot);
        uint16_t zoom = slot_zoom[slot];
        lv_opa_t opa = slot_opa[slot];
        int32_t x = (sw * slot_x_pct[slot]) / 100;
        int32_t bw = lv_obj_get_width(obj);
        int32_t bh = lv_obj_get_height(obj);
        int32_t scaled_w = (bw * (int32_t)zoom) / 256;
        int32_t scaled_h = (bh * (int32_t)zoom) / 256;
        float y = (float)base_bottom - ((float)scaled_h * 0.5f) - ((float)sh * slot_lift[slot]);
        int32_t xmin = 8 + (scaled_w / 2);
        int32_t xmax = sw - 8 - (scaled_w / 2);
        float ymin = 78.0f + ((float)scaled_h * 0.5f);
        float ymax = ((float)sh - 8.0f) - ((float)scaled_h * 0.5f);
        int32_t iw;
        int32_t ih;

        if (s->splash_last_center_idx != s->splash_center_idx)
        {
            lv_image_set_src(obj, drone_src[drone_idx]);
        }
        iw = lv_obj_get_width(obj);
        ih = lv_obj_get_height(obj);
        lv_obj_set_style_transform_pivot_x(obj, iw / 2, 0);
        lv_obj_set_style_transform_pivot_y(obj, ih / 2, 0);

        bw = iw;
        bh = ih;
        zoom = (uint16_t)((float)zoom * drone_scale[drone_idx]);
        scaled_w = (bw * (int32_t)zoom) / 256;
        scaled_h = (bh * (int32_t)zoom) / 256;

        if ((drone_idx == 3) && (slot == 0))
        {
            /* Bagnet featured-left: slightly larger and lifted to avoid lower artifact region. */
            zoom = (uint16_t)((float)zoom * 1.22f);
            scaled_w = (bw * (int32_t)zoom) / 256;
            scaled_h = (bh * (int32_t)zoom) / 256;
            y -= (float)sh * 0.06f;
            x -= sw / 40;
        }

        if (x < xmin)
        {
            x = xmin;
        }
        if (x > xmax)
        {
            x = xmax;
        }
        if (y < ymin)
        {
            y = ymin;
        }
        if (y > ymax)
        {
            y = ymax;
        }

        lv_obj_set_style_transform_zoom(obj, zoom, 0);
        lv_obj_set_style_transform_width(obj, 0, 0);
        lv_obj_set_style_transform_height(obj, 0, 0);
        lv_obj_set_style_opa(obj, opa, 0);
        set_obj_center(obj, x, y);
    }

    lv_obj_move_foreground(splash_dock_obj(s, 1));
    lv_obj_move_foreground(splash_dock_obj(s, 2));
    lv_obj_move_foreground(splash_dock_obj(s, 0));
    lv_obj_move_foreground(s->splash_title);
    lv_obj_move_foreground(s->splash_subtitle);
    lv_obj_move_foreground(s->splash_start_btn);
    s->splash_last_center_idx = s->splash_center_idx;
}

static void splash_spin_step(drone_hunter_scene_t *s, int dir)
{
    s->splash_center_idx = (s->splash_center_idx + dir + SPLASH_DRONE_COUNT) % SPLASH_DRONE_COUNT;
    update_splash_dock(s);
}

static void splash_gesture_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    lv_indev_t *indev = lv_event_get_indev(e);
    lv_point_t p;
    int16_t dx;
    lv_event_code_t code = lv_event_get_code(e);

    if (indev == NULL)
    {
        return;
    }

    lv_indev_get_point(indev, &p);

    if (code == LV_EVENT_PRESSED)
    {
        s->splash_touch_active = 1;
        s->splash_touch_start_x = p.x;
        return;
    }

    if ((code != LV_EVENT_RELEASED) && (code != LV_EVENT_PRESS_LOST))
    {
        return;
    }

    if (!s->splash_touch_active)
    {
        return;
    }

    s->splash_touch_active = 0;
    dx = (int16_t)(p.x - s->splash_touch_start_x);
    if (dx < -20)
    {
        splash_spin_step(s, 1);
    }
    else if (dx > 20)
    {
        splash_spin_step(s, -1);
    }
}

static void update_lineup_3d(drone_hunter_scene_t *s)
{
    int i;
    int best_idx = 0;
    float best_depth = -10.0f;
    float depth[HUNTER_TYPE_COUNT];
    int order[HUNTER_TYPE_COUNT];
    int32_t cw = lv_obj_get_width(s->lineup_cont);
    int32_t ch = lv_obj_get_height(s->lineup_cont);
    int32_t cx = cw / 2;
    int32_t cy = (ch / 2) + 8;
    float rx = (float)cw * 0.34f;
    float ry = (float)ch * 0.22f;
    float step = (2.0f * PI_F) / (float)HUNTER_TYPE_COUNT;

    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        lv_obj_t *card = s->lineup_cards[i];
        hunter_type_t t = (hunter_type_t)i;
        float a = s->lineup_angle + ((float)i * step);
        float z = (cosf(a) + 1.0f) * 0.5f;
        uint16_t zoom = (uint16_t)(320 + (uint16_t)(z * 200.0f));
        lv_opa_t opa = (lv_opa_t)(135 + (int32_t)(z * 120.0f));
        int16_t yoff = (int16_t)(9.0f - (z * 14.0f));
        int16_t x = (int16_t)((float)cx + sinf(a) * rx);
        int16_t y = (int16_t)((float)cy + cosf(a) * ry);
        int32_t bw = lv_obj_get_width(card);
        int32_t bh = lv_obj_get_height(card);
        int32_t sw = (bw * (int32_t)zoom) / 256;
        int32_t sh = (bh * (int32_t)zoom) / 256;
        int32_t extw = ((sw - bw) / 2) + 2;
        int32_t exth = ((sh - bh) / 2) + 2;

        depth[i] = z;
        order[i] = i;

        if (z > best_depth)
        {
            best_depth = z;
            best_idx = i;
        }

        lv_obj_set_style_transform_zoom(card, zoom, 0);
        lv_obj_set_style_transform_angle(card, is_xwing_hunter(t) ? 0 : 900, 0);
        lv_obj_set_style_transform_width(card, extw, 0);
        lv_obj_set_style_transform_height(card, exth, 0);
        lv_obj_set_style_translate_y(card, yoff, 0);
        lv_obj_set_style_opa(card, opa, 0);
        set_obj_center(card, (float)x, (float)y);
    }

    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        int j;
        for (j = i + 1; j < HUNTER_TYPE_COUNT; ++j)
        {
            if (depth[order[i]] > depth[order[j]])
            {
                int tmp = order[i];
                order[i] = order[j];
                order[j] = tmp;
            }
        }
    }

    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        lv_obj_move_foreground(s->lineup_cards[order[i]]);
    }

    s->lineup_focus_idx = best_idx;
    lv_label_set_text_fmt(s->lineup_desc,
                          "%s: %s",
                          g_hunter_profiles[s->lineup_focus_idx].name,
                          g_hunter_profiles[s->lineup_focus_idx].desc);
}

static void lineup_spin_step(drone_hunter_scene_t *s, int dir)
{
    float step = (2.0f * PI_F) / (float)HUNTER_TYPE_COUNT;
    s->lineup_target_angle = wrap_angle(s->lineup_target_angle + ((float)dir * step));
}

static void lineup_gesture_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);

    if (lv_event_get_code(e) != LV_EVENT_GESTURE)
    {
        return;
    }

    switch (lv_indev_get_gesture_dir(lv_indev_get_act()))
    {
        case LV_DIR_LEFT:
            lineup_spin_step(s, -1);
            break;
        case LV_DIR_RIGHT:
            lineup_spin_step(s, 1);
            break;
        default:
            break;
    }
}

static void splash_start_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);

    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        begin_arena_from_splash(s);
    }
}

static void splash_show_title(drone_hunter_scene_t *s)
{
    lv_label_set_text(s->splash_title, "EdgeAI Drone Hunter");
    lv_label_set_text(s->splash_subtitle, "Swipe left/right to rotate drones - Press START ARENA");

    lv_obj_clear_flag(s->splash_title, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s->splash_subtitle, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s->splash_hero, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s->splash_hero_odin, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s->splash_hero_skyfall, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s->splash_start_btn, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(s->lineup_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->lineup_desc, LV_OBJ_FLAG_HIDDEN);
    s->splash_last_center_idx = -1;
    update_splash_dock(s);
}

static void splash_tick_cb(lv_timer_t *timer)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_timer_get_user_data(timer);

    if (lv_obj_has_flag(s->splash, LV_OBJ_FLAG_HIDDEN))
    {
        lv_timer_del(timer);
        s->splash_timer = NULL;
        return;
    }

    (void)s;
}

static void assign_mode(drone_hunter_scene_t *s, match_mode_t m)


{
    s->mode = m;

    if (m == MODE_ALGO_VS_ALGO)
    {
        s->team_ctrl[0] = CTRL_ALGO;
        s->team_ctrl[1] = CTRL_ALGO;
    }
    else if (m == MODE_EDGEAI_VS_EDGEAI)
    {
        s->team_ctrl[0] = CTRL_EDGEAI;
        s->team_ctrl[1] = CTRL_EDGEAI;
    }
    else
    {
        s->team_ctrl[0] = CTRL_ALGO;
        s->team_ctrl[1] = CTRL_EDGEAI;
    }

    if (s->mode_btn_label)
    {
        lv_label_set_text_fmt(s->mode_btn_label, "Mode: %s", mode_name(s->mode));
    }
}

static void style_target(drone_hunter_scene_t *s, int k)
{
    const lv_image_dsc_t *src;
    uint16_t zoom = (s->k_tier[k] >= 2) ? 230 : ((s->k_tier[k] == 1) ? 205 : 185);

    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        src = (s->threat_faction == THREAT_FACTION_RUSSIA) ? &img_hunter_merops : &img_hunter_tytan;
    }
    else
    {
        src = (s->threat_faction == THREAT_FACTION_RUSSIA) ? &img_hunter_odin_win_hit : &img_hunter_sting_ii;
    }

    lv_image_set_src(s->killers[k], src);
    lv_obj_set_style_transform_zoom(s->killers[k], zoom, 0);
    lv_obj_set_style_opa(s->killers[k], LV_OPA_COVER, 0);
    lv_obj_set_style_transform_width(s->killers[k], 0, 0);
    lv_obj_set_style_transform_height(s->killers[k], 0, 0);

    lv_obj_add_flag(s->killer_wing[k], LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->killer_wing_upper[k], LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->killer_wing_lower[k], LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->killer_body[k], LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->killer_nose[k], LV_OBJ_FLAG_HIDDEN);

    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        update_fixed_wing_orientation(s, k);
    }
    else
    {
        lv_obj_set_style_transform_angle(s->killers[k], 0, 0);
    }
}

static int wave_target_count(int wave_idx)
{
    int total = ATTACK_POOL_BASE + ((wave_idx - 1) * ATTACK_POOL_GROWTH);
    if (total > ATTACK_POOL_MAX)
    {
        total = ATTACK_POOL_MAX;
    }
    return total;
}

static void update_hunter_deck_ui(drone_hunter_scene_t *s)
{
    int i;
    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        int in_use = (s->h_type[0] == (hunter_type_t)i) || (s->h_type[1] == (hunter_type_t)i);
        lv_obj_set_style_opa(s->deck_icon[i], (s->hunter_stock[i] > 0) ? LV_OPA_COVER : LV_OPA_30, 0);
        lv_obj_set_style_outline_width(s->deck_icon[i], in_use ? 2 : 0, 0);
        lv_obj_set_style_outline_color(s->deck_icon[i], lv_color_hex(0x22D3EE), 0);
        lv_label_set_text_fmt(s->deck_count[i], "x%d", s->hunter_stock[i]);
        lv_obj_set_style_text_color(s->deck_count[i],
                                    (s->hunter_stock[i] > 0) ? lv_color_hex(0xFDE68A) : lv_color_hex(0x6B7280),
                                    0);
        lv_obj_set_style_text_color(s->deck_name[i],
                                    (s->hunter_stock[i] > 0) ? lv_color_hex(0xE5E7EB) : lv_color_hex(0x6B7280),
                                    0);
    }

    if (s->deck_ciws_count != NULL)
    {
        if (s->ciws_ammo < 0)
        {
            lv_label_set_text(s->deck_ciws_count, "INF");
        }
        else
        {
            lv_label_set_text_fmt(s->deck_ciws_count, "x%d", s->ciws_ammo);
        }
        lv_obj_set_style_text_color(s->deck_ciws_count,
                                    (s->ciws_ammo != 0) ? lv_color_hex(0x93C5FD) : lv_color_hex(0x6B7280),
                                    0);
        lv_obj_set_style_opa(s->deck_ciws_icon, (s->ciws_ammo != 0) ? LV_OPA_COVER : LV_OPA_30, 0);
        lv_obj_set_style_text_color(s->deck_ciws_name,
                                    (s->ciws_ammo != 0) ? lv_color_hex(0xE5E7EB) : lv_color_hex(0x6B7280),
                                    0);
    }
}

static void set_killer_hidden(drone_hunter_scene_t *s, int k)
{
    s->killer_active[k] = 0;
    lv_obj_add_flag(s->killers[k], LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->fx_intercept[k], LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->fx_spawn[k], LV_OBJ_FLAG_HIDDEN);
}

static void set_killer_visible(drone_hunter_scene_t *s, int k)
{
    s->killer_active[k] = 1;
    lv_obj_clear_flag(s->killers[k], LV_OBJ_FLAG_HIDDEN);
}

static void respawn_killer(drone_hunter_scene_t *s, int k, int side)
{
    float min_x = (float)s->arena_x + 20.0f;
    float max_x = (float)(s->arena_x + s->arena_w - 20);
    float min_y = (float)s->arena_y + 20.0f;
    float max_y = (float)(s->arena_y + s->arena_h - 20);
    float span_x = (max_x - min_x > 1.0f) ? (max_x - min_x) : 1.0f;
    float span_y = (max_y - min_y > 1.0f) ? (max_y - min_y) : 1.0f;
    float pos_x = min_x;
    float pos_y = min_y;
    float core_x = (float)(s->arena_x + (s->arena_w / 2));
    float core_y = (float)(s->arena_y + (s->arena_h / 2));
    float edge_phase = (float)((s->killer_spawn_tick * 47) + (k * 29));
    int edge = side & 0x3;
    int phase = arena_phase(s);

    if (s->attack_remaining_to_spawn <= 0)
    {
        set_killer_hidden(s, k);
        return;
    }

    set_killer_visible(s, k);
    s->attack_remaining_to_spawn--;
    s->killer_spawn_tick++;

    if (phase == 0)
    {
        s->ktype[k] = TARGET_FPV;
    }
    else if (phase == 1)
    {
        if (k == 0)
        {
            s->ktype[k] = TARGET_FPV;
        }
        else
        {
            s->ktype[k] = ((s->killer_spawn_tick / 2) % 2 == 0) ? TARGET_FIXED_WING : TARGET_FPV;
        }
    }
    else
    {
        s->ktype[k] = ((s->killer_spawn_tick + k) % 3 == 0) ? TARGET_FPV : TARGET_FIXED_WING;
    }
    if (s->wave_idx <= 2)
    {
        s->k_tier[k] = (s->killer_spawn_tick % 4 == 0) ? 1 : 0;
    }
    else if (s->wave_idx <= 5)
    {
        s->k_tier[k] = (s->killer_spawn_tick % 3 == 0) ? 2 : 1;
    }
    else
    {
        s->k_tier[k] = (s->killer_spawn_tick % 2 == 0) ? 2 : 1;
    }

    style_target(s, k);
    if (s->ktype[k] == TARGET_FPV)
    {
        lv_obj_set_style_bg_color(s->killers[k],
                                  (s->k_tier[k] >= 2) ? lv_color_hex(0xDC2626) :
                                  ((s->k_tier[k] == 1) ? lv_color_hex(0xF97316) : lv_color_hex(0xEF4444)),
                                  0);
    }

    switch (edge)
    {
        case 0: /* left edge */
            pos_x = (float)s->arena_x - 18.0f;
            pos_y = min_y + fmodf(edge_phase * 1.13f, span_y);
            break;
        case 1: /* right edge */
            pos_x = (float)(s->arena_x + s->arena_w) + 18.0f;
            pos_y = min_y + fmodf(edge_phase * 0.97f, span_y);
            break;
        case 2: /* top edge */
            pos_x = min_x + fmodf(edge_phase * 1.07f, span_x);
            pos_y = (float)s->arena_y - 18.0f;
            break;
        default: /* bottom edge */
            pos_x = min_x + fmodf(edge_phase * 0.91f, span_x);
            pos_y = (float)(s->arena_y + s->arena_h) + 18.0f;
            break;
    }

    s->kx[k] = pos_x;
    s->ky[k] = pos_y;
    s->kvx[k] = 0.0f;
    s->kvy[k] = 0.0f;
    s->k_heading[k] = atan2f(core_y - s->ky[k], core_x - s->kx[k]);

    s->fx_spawn_t[k] = FX_SPAWN_SEC;
    s->fx_spawn_x[k] = s->kx[k];
    s->fx_spawn_y[k] = s->ky[k];
    s->k_missed_by_hunter[k] = 0;

}

static void start_wave(drone_hunter_scene_t *s, int wave_idx)
{
    s->wave_idx = wave_idx;
    s->wave_target_total = wave_target_count(wave_idx);
    s->attack_remaining_to_spawn = s->wave_target_total;
    s->attack_destroyed = 0;
    s->attack_leaked = 0;
    s->threat_faction = (wave_idx & 1) ? THREAT_FACTION_RUSSIA : THREAT_FACTION_USA;

    respawn_killer(s, 0, 0);
    respawn_killer(s, 1, 1);
}

static void reset_hunters(drone_hunter_scene_t *s)
{
    int i;

    s->hx[0] = (float)(s->arena_x + 34);
    s->hy[0] = (float)(s->arena_y + s->arena_h - 22);

    s->hx[1] = (float)(s->arena_x + s->arena_w - 34);
    s->hy[1] = (float)(s->arena_y + s->arena_h - 22);

    for (i = 0; i < HUNTER_COUNT; ++i)
    {
        s->hvx[i] = 0.0f;
        s->hvy[i] = 0.0f;
        s->h_heading[i] = -PI_F * 0.5f;
        s->h_type[i] = HUNTER_STING_II;
        s->h_reselect_sec[i] = 0.0f;
        s->hunter_loaded[i] = 0;
        apply_hunter_profile(s, i);
    }
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

static void reset_round(drone_hunter_scene_t *s)
{
    int i;

    s->team_score[0] = 0;
    s->team_score[1] = 0;
    s->core_hp = CORE_HP_START;
    s->round_time_sec = ROUND_TIME_SEC;
    s->round_over = 0;
    s->t = 0.0f;
    s->killer_spawn_tick = 0;
    s->fx_core_hit_t = 0.0f;
    s->ciws_cooldown_sec = 0.0f;
    s->ciws_cooldown_left_sec = 0.0f;
    s->ciws_sweep_right_rad = -PI_F;
    s->ciws_sweep_left_rad = -PI_F * 0.5f;
    s->ciws_sweep_right_dir = 1.0f;
    s->ciws_sweep_left_dir = 1.0f;
    s->defense_kills = 0;
    s->defense_misses = 0;
    s->ciws_ammo = CIWS_AMMO_UNLIMITED;
    s->ciws_shots = 0;
    s->ciws_kills = 0;
    s->ciws_tracer_head = 0;
    for (i = 0; i < CIWS_TRACER_COUNT; ++i)
    {
        s->ciws_tracer_t[i] = 0.0f;
        s->ciws_tracer_g[i] = 0.0f;
    }
    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        s->hunter_stock[i] = HUNTER_STOCK_PER_TYPE;
    }

    for (i = 0; i < KILLER_COUNT; ++i)
    {
        s->fx_intercept_t[i] = 0.0f;
        s->fx_spawn_t[i] = 0.0f;
        s->killer_active[i] = 0;
        s->k_tier[i] = 0;
        s->k_missed_by_hunter[i] = 0;
    }

    reset_hunters(s);
    start_wave(s, 1);
    update_hunter_deck_ui(s);

    hide_overlay(s);
}

static void restart_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    reset_round(s);
}

static void mode_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);

    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
    {
        return;
    }

    assign_mode(s, (match_mode_t)((s->mode + 1) % MODE_COUNT));
    reset_round(s);
}

static void update_killers(drone_hunter_scene_t *s, float core_x, float core_y)
{
    int k;
    int phase = arena_phase(s);
    float phase_speed_gain = (phase == 0) ? 0.92f : ((phase == 1) ? 1.08f : 1.24f);
    float ciws_x = (float)(s->arena_x + s->arena_w - 8);
    float ciws_y = (float)(s->arena_y + s->arena_h - 49);
    float ciws_left_x = (float)(s->arena_x + 18);
    float ciws_left_y = (float)(s->arena_y + s->arena_h - 49);
    float ciws_range = (float)s->arena_w * CIWS_RANGE_FRAC;
    float sweep_step = CIWS_SWEEP_SPEED_RAD * DT_SEC;

    s->ciws_cooldown_sec = clampf(s->ciws_cooldown_sec - DT_SEC, 0.0f, 2.0f);
    s->ciws_cooldown_left_sec = clampf(s->ciws_cooldown_left_sec - DT_SEC, 0.0f, 2.0f);
    s->ciws_sweep_right_rad += s->ciws_sweep_right_dir * sweep_step;
    s->ciws_sweep_left_rad += s->ciws_sweep_left_dir * sweep_step;

    if (s->ciws_sweep_right_rad > -PI_F * 0.5f)
    {
        s->ciws_sweep_right_rad = -PI_F * 0.5f;
        s->ciws_sweep_right_dir = -1.0f;
    }
    else if (s->ciws_sweep_right_rad < -PI_F)
    {
        s->ciws_sweep_right_rad = -PI_F;
        s->ciws_sweep_right_dir = 1.0f;
    }

    if (s->ciws_sweep_left_rad > 0.0f)
    {
        s->ciws_sweep_left_rad = 0.0f;
        s->ciws_sweep_left_dir = -1.0f;
    }
    else if (s->ciws_sweep_left_rad < -PI_F * 0.5f)
    {
        s->ciws_sweep_left_rad = -PI_F * 0.5f;
        s->ciws_sweep_left_dir = 1.0f;
    }

    for (k = 0; k < KILLER_COUNT; ++k)
    {
        if (!s->killer_active[k])
        {
            continue;
        }

        float dir_x = core_x - s->kx[k];
        float dir_y = core_y - s->ky[k];
        float d = sqrtf((dir_x * dir_x) + (dir_y * dir_y));
        float noise_x = sinf(s->t * (2.0f + 0.4f * (float)k) + (float)k);
        float noise_y = cosf(s->t * (1.7f + 0.2f * (float)k) + (float)k);

        if (d > 1.0f)
        {
            dir_x /= d;
            dir_y /= d;
        }

        if (s->ktype[k] == TARGET_FIXED_WING)
        {
            float desired = atan2f(dir_y, dir_x) + (noise_y * 0.35f);
            float delta = desired - s->k_heading[k];
            float turn_limit = (phase == 2) ? 0.038f : 0.030f;
            float speed = 2.00f * phase_speed_gain;

            while (delta > PI_F)
            {
                delta -= 2.0f * PI_F;
            }
            while (delta < -PI_F)
            {
                delta += 2.0f * PI_F;
            }

            delta = clampf(delta, -turn_limit, turn_limit);
            s->k_heading[k] += delta;
            s->kvx[k] = (cosf(s->k_heading[k]) * speed) + (noise_x * 0.12f);
            s->kvy[k] = (sinf(s->k_heading[k]) * speed) + (noise_y * 0.12f);
        }
        else
        {
            float speed = 1.15f * phase_speed_gain;
            s->kvx[k] = (s->kvx[k] * 0.84f) + ((dir_x * speed) * 0.14f) + (noise_x * 0.05f);
            s->kvy[k] = (s->kvy[k] * 0.84f) + ((dir_y * speed) * 0.14f) + (noise_y * 0.05f);
        }

        s->kx[k] += s->kvx[k];
        s->ky[k] += s->kvy[k];

        s->kx[k] = clampf(s->kx[k], (float)s->arena_x + 10.0f, (float)(s->arena_x + s->arena_w - 10));
        s->ky[k] = clampf(s->ky[k], (float)s->arena_y + 10.0f, (float)(s->arena_y + s->arena_h - 10));

        if ((s->ciws_ammo != 0) &&
            (s->ciws_cooldown_sec <= 0.0f) &&
            ciws_target_in_sweep(ciws_x, ciws_y, s->ciws_sweep_right_rad, ciws_range, s->kx[k], s->ky[k]))
        {
            if (ciws_fire_at(s, k, ciws_x, ciws_y, s->ciws_sweep_right_rad, &s->ciws_cooldown_sec))
            {
                continue;
            }
        }
        if ((s->ciws_ammo != 0) &&
            (s->ciws_cooldown_left_sec <= 0.0f) &&
            ciws_target_in_sweep(ciws_left_x, ciws_left_y, s->ciws_sweep_left_rad, ciws_range, s->kx[k], s->ky[k]))
        {
            if (ciws_fire_at(s, k, ciws_left_x, ciws_left_y, s->ciws_sweep_left_rad, &s->ciws_cooldown_left_sec))
            {
                continue;
            }
        }

        if (dist2(s->kx[k], s->ky[k], core_x, core_y) < (CORE_RADIUS_PX * CORE_RADIUS_PX))
        {
            s->core_hp--;
            s->attack_leaked++;
            s->fx_core_hit_t = FX_CORE_HIT_SEC;
            respawn_killer(s, k, (k + s->killer_spawn_tick) % 4);
        }
    }
}

static void update_hunter(drone_hunter_scene_t *s, int h, float core_x, float core_y)
{
    int target = -1;
    float tx;
    float ty;
    float speed;
    float accel;
    float damping;
    const hunter_profile_t *p;

    if (!any_hunter_stock_remaining(s))
    {
        return;
    }

    if (s->killer_active[0] && s->killer_active[1])
    {
        if (s->team_ctrl[h] == CTRL_EDGEAI)
        {
            float eta0 = sqrtf(dist2(s->kx[0], s->ky[0], core_x, core_y)) / ((s->ktype[0] == TARGET_FIXED_WING) ? 2.1f : 1.2f);
            float eta1 = sqrtf(dist2(s->kx[1], s->ky[1], core_x, core_y)) / ((s->ktype[1] == TARGET_FIXED_WING) ? 2.1f : 1.2f);
            target = (eta1 < eta0) ? 1 : 0;
        }
        else
        {
            float d0 = dist2(s->hx[h], s->hy[h], s->kx[0], s->ky[0]);
            float d1 = dist2(s->hx[h], s->hy[h], s->kx[1], s->ky[1]);
            target = (d1 < d0) ? 1 : 0;
        }
    }
    else if (s->killer_active[0] || s->killer_active[1])
    {
        target = s->killer_active[0] ? 0 : 1;
    }
    else
    {
        float regroup_x = core_x + ((h == 0) ? -48.0f : 48.0f);
        float regroup_y = (float)s->arena_y + (float)s->arena_h - 22.0f;
        s->hvx[h] = (s->hvx[h] * 0.82f) + ((regroup_x - s->hx[h]) * 0.018f);
        s->hvy[h] = (s->hvy[h] * 0.82f) + ((regroup_y - s->hy[h]) * 0.018f);
        s->hx[h] = clampf(s->hx[h] + (s->hvx[h] * 1.2f), (float)s->arena_x + 10.0f, (float)(s->arena_x + s->arena_w - 10));
        s->hy[h] = clampf(s->hy[h] + (s->hvy[h] * 1.2f), (float)s->arena_y + 10.0f, (float)(s->arena_y + s->arena_h - 10));
        return;
    }

    if (target < 0)
    {
        return;
    }

    if (!s->hunter_loaded[h])
    {
        hunter_type_t pick = choose_conservative_hunter(s, target);
        if (s->hunter_stock[(int)pick] <= 0)
        {
            return;
        }
        s->h_type[h] = pick;
        s->hunter_stock[(int)pick]--;
        s->hunter_loaded[h] = 1;
        update_hunter_deck_ui(s);
    }

    s->h_reselect_sec[h] -= DT_SEC;
    if (s->h_reselect_sec[h] <= 0.0f)
    {
        apply_hunter_profile(s, h);
        s->h_reselect_sec[h] = 2.6f;
    }

    p = &g_hunter_profiles[(int)s->h_type[h]];

    if (s->team_ctrl[h] == CTRL_EDGEAI)
    {
        tx = s->kx[target] + (s->kvx[target] * (16.0f * p->lead_gain));
        ty = s->ky[target] + (s->kvy[target] * (16.0f * p->lead_gain));
        speed = p->speed * 1.08f;
        accel = p->accel * 1.10f;
        damping = clampf(p->damping - 0.05f, 0.55f, 0.90f);
    }
    else
    {
        tx = s->kx[target] + (sinf((s->t * 3.1f) + (float)h) * 3.0f);
        ty = s->ky[target] + (cosf((s->t * 2.8f) + (float)h) * 3.0f);
        speed = p->speed * 0.92f;
        accel = p->accel * 0.88f;
        damping = clampf(p->damping + 0.08f, 0.62f, 0.94f);
    }

    s->hvx[h] = (s->hvx[h] * damping) + ((tx - s->hx[h]) * accel);
    s->hvy[h] = (s->hvy[h] * damping) + ((ty - s->hy[h]) * accel);

    s->hx[h] += s->hvx[h] * speed;
    s->hy[h] += s->hvy[h] * speed;

    s->hx[h] = clampf(s->hx[h], (float)s->arena_x + 10.0f, (float)(s->arena_x + s->arena_w - 10));
    s->hy[h] = clampf(s->hy[h], (float)s->arena_y + 10.0f, (float)(s->arena_y + s->arena_h - 10));

    if ((s->hvx[h] * s->hvx[h]) + (s->hvy[h] * s->hvy[h]) > 0.006f)
    {
        s->h_heading[h] = atan2f(s->hvy[h], s->hvx[h]);
    }

    if (dist2(s->hx[h], s->hy[h], s->kx[target], s->ky[target]) < (p->kill_radius * p->kill_radius))
    {
        float cap = hunter_capability_score(s->h_type[h]);
        float req = threat_required_score(s->k_tier[target], s->ktype[target]);
        float p_kill = clampf(0.34f + ((cap - req) * 0.22f), 0.12f, 0.93f);
        float roll = 0.5f + (0.5f * sinf((s->t * 2.3f) + (float)(h * 7 + target * 11)));

        s->hunter_loaded[h] = 0;
        s->h_reselect_sec[h] = 0.0f;
        s->hx[h] = (h == 0) ? (float)(s->arena_x + 34) : (float)(s->arena_x + s->arena_w - 34);
        s->hy[h] = (float)(s->arena_y + s->arena_h - 22);
        s->hvx[h] = 0.0f;
        s->hvy[h] = 0.0f;

        if (roll <= p_kill)
        {
            int pts = (s->ktype[target] == TARGET_FIXED_WING) ? p->points_fixed : p->points_fpv;
            s->team_score[h] += pts;
            s->attack_destroyed++;
            s->defense_kills++;
            s->fx_intercept_t[target] = FX_INTERCEPT_SEC;
            s->fx_intercept_x[target] = s->kx[target];
            s->fx_intercept_y[target] = s->ky[target];
            respawn_killer(s, target, (target + h + s->killer_spawn_tick) % 4);
        }
        else
        {
            s->defense_misses++;
            s->k_missed_by_hunter[target] = 1;
            s->kx[target] += s->kvx[target] * 3.0f;
            s->ky[target] += s->kvy[target] * 3.0f;
            s->kx[target] = clampf(s->kx[target], (float)s->arena_x + 12.0f, (float)(s->arena_x + s->arena_w - 12));
            s->ky[target] = clampf(s->ky[target], (float)s->arena_y + 12.0f, (float)(s->arena_y + s->arena_h - 12));
        }
    }
}

static void update_effects(drone_hunter_scene_t *s, float core_x, float core_y)
{
    int k;
    (void)core_x;
    (void)core_y;

    for (k = 0; k < KILLER_COUNT; ++k)
    {
        lv_obj_add_flag(s->fx_intercept[k], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s->fx_spawn[k], LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_add_flag(s->core, LV_OBJ_FLAG_HIDDEN);

    for (k = 0; k < CIWS_TRACER_COUNT; ++k)
    {
        if (s->ciws_tracer_t[k] > 0.0f)
        {
            float life_ratio = clampf(s->ciws_tracer_t[k] / CIWS_TRACER_LIFE_SEC, 0.0f, 1.0f);
            lv_opa_t opa = (lv_opa_t)(60 + (int32_t)(life_ratio * 195.0f));
            int32_t dot = (life_ratio > 0.74f) ? 9 : ((life_ratio > 0.38f) ? 7 : 5);

            lv_obj_clear_flag(s->ciws_tracer[k], LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(s->ciws_tracer[k], dot, dot);
            lv_obj_set_style_bg_color(s->ciws_tracer[k], lv_color_hex(0xFDBA74), 0);
            lv_obj_set_style_bg_opa(s->ciws_tracer[k], opa, 0);
            set_obj_center(s->ciws_tracer[k], s->ciws_tracer_x0[k], s->ciws_tracer_y0[k]);
        }
        else
        {
            lv_obj_add_flag(s->ciws_tracer[k], LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void update_positions(drone_hunter_scene_t *s)
{
    int h;
    int k;
    float ciws_x = (float)(s->arena_x + s->arena_w - 8);
    float ciws_y = (float)(s->arena_y + s->arena_h - 49);
    float ciws_left_x = (float)(s->arena_x + 18);
    float ciws_left_y = (float)(s->arena_y + s->arena_h - 49);

    for (h = 0; h < HUNTER_COUNT; ++h)
    {
        int16_t angle_tenth = (int16_t)(s->h_heading[h] * (1800.0f / PI_F));

        int32_t hw = lv_obj_get_width(s->hunters[h]);
        int32_t hh = lv_obj_get_height(s->hunters[h]);

        set_obj_center(s->hunters[h], s->hx[h], s->hy[h]);
        lv_obj_set_style_transform_pivot_x(s->hunters[h], (int32_t)(hw / 2), 0);
        lv_obj_set_style_transform_pivot_y(s->hunters[h], (int32_t)(hh / 2), 0);
        lv_obj_set_style_transform_angle(s->hunters[h], angle_tenth, 0);
        lv_obj_add_flag(s->hunter_tail[h], LV_OBJ_FLAG_HIDDEN);
    }

    for (k = 0; k < KILLER_COUNT; ++k)
    {
        if (!s->killer_active[k])
        {
            lv_obj_add_flag(s->killers[k], LV_OBJ_FLAG_HIDDEN);
            continue;
        }
        if (s->ktype[k] == TARGET_FIXED_WING)
        {
            update_fixed_wing_orientation(s, k);
        }
        lv_obj_clear_flag(s->killers[k], LV_OBJ_FLAG_HIDDEN);
        set_obj_center(s->killers[k], s->kx[k], s->ky[k]);
    }

    set_obj_center(s->ciws, ciws_x, ciws_y);
    set_obj_center(s->ciws_left, ciws_left_x, ciws_left_y);
}

static void update_hud(drone_hunter_scene_t *s)
{
    int phase = arena_phase(s);
    int elapsed = (int)s->t;

    lv_label_set_text_fmt(s->hud_mode, "MODE: %s  |  %s", mode_name(s->mode), arena_phase_name(phase));
    lv_label_set_text_fmt(s->hud_score,
                          "KILLS %d  |  MISSES %d  |  CORE %d  |  ELAPSED %03ds",
                          s->defense_kills, s->defense_misses,
                          s->core_hp, elapsed);
    lv_label_set_text_fmt(s->hud_wave,
                          "WAVE %d  |  %s  |  NEUTRALIZED %d/%d  |  LEAKED %d",
                          s->wave_idx,
                          threat_faction_name(s->threat_faction),
                          s->attack_destroyed, s->wave_target_total,
                          s->attack_leaked);
    lv_label_set_text_fmt(s->hud_info,
                          "Active launchers: %s / %s  |  WAVE REM %d",
                          hunter_type_name(s->h_type[0]),
                          hunter_type_name(s->h_type[1]),
                          s->attack_remaining_to_spawn);
}

static void maybe_end_round(drone_hunter_scene_t *s)
{
    /* Continuous demo mode: never end round automatically. */
    (void)s;
    return;

    if (s->round_over)
    {
        return;
    }

    if (s->core_hp <= 0)
    {
        s->round_over = 1;
        show_overlay(s, "ROUND END", "Core destroyed. Tap RESTART.");
        return;
    }

    if ((s->team_score[0] - s->team_score[1] >= ROUND_WIN_MARGIN) ||
        (s->team_score[1] - s->team_score[0] >= ROUND_WIN_MARGIN) ||
        (s->round_time_sec <= 0.0f))
    {
        char subtitle[120];

        s->round_time_sec = clampf(s->round_time_sec, 0.0f, ROUND_TIME_SEC);
        s->round_over = 1;

        if (s->team_score[0] == s->team_score[1])
        {
            (void)snprintf(subtitle, sizeof(subtitle), "Draw %d - %d. Tap RESTART.",
                           s->team_score[0], s->team_score[1]);
        }
        else if (s->team_score[0] > s->team_score[1])
        {
            (void)snprintf(subtitle, sizeof(subtitle), "T1 (%s) wins %d - %d. Tap RESTART.",
                           ctrl_name(s->team_ctrl[0]), s->team_score[0], s->team_score[1]);
        }
        else
        {
            (void)snprintf(subtitle, sizeof(subtitle), "T2 (%s) wins %d - %d. Tap RESTART.",
                           ctrl_name(s->team_ctrl[1]), s->team_score[1], s->team_score[0]);
        }

        show_overlay(s, "ROUND END", subtitle);
    }
}

static void anim_cb(lv_timer_t *timer)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_timer_get_user_data(timer);
    float core_x = (float)(s->arena_x + s->arena_w / 2);
    float core_y = (float)(s->arena_y + s->arena_h / 2);
    int h;

    if (lv_obj_has_flag(s->arena, LV_OBJ_FLAG_HIDDEN))
    {
        return;
    }

    s->t += 0.040f;

    if (!s->round_over)
    {
        int k;

        s->round_time_sec -= DT_SEC;
        update_killers(s, core_x, core_y);

        for (h = 0; h < HUNTER_COUNT; ++h)
        {
            update_hunter(s, h, core_x, core_y);
        }

        for (k = 0; k < KILLER_COUNT; ++k)
        {
            s->fx_intercept_t[k] = clampf(s->fx_intercept_t[k] - DT_SEC, 0.0f, FX_INTERCEPT_SEC);
            s->fx_spawn_t[k] = clampf(s->fx_spawn_t[k] - DT_SEC, 0.0f, FX_SPAWN_SEC);
        }
        for (k = 0; k < CIWS_TRACER_COUNT; ++k)
        {
            if (s->ciws_tracer_t[k] > 0.0f)
            {
                s->ciws_tracer_x0[k] += s->ciws_tracer_x1[k] * DT_SEC;
                s->ciws_tracer_y0[k] += s->ciws_tracer_y1[k] * DT_SEC;
                s->ciws_tracer_y1[k] += s->ciws_tracer_g[k] * DT_SEC;
            }
            s->ciws_tracer_t[k] = clampf(s->ciws_tracer_t[k] - DT_SEC, 0.0f, CIWS_TRACER_LIFE_SEC);
        }

        if ((s->attack_remaining_to_spawn <= 0) && !s->killer_active[0] && !s->killer_active[1])
        {
            start_wave(s, s->wave_idx + 1);
            for (h = 0; h < HUNTER_COUNT; ++h)
            {
                s->h_reselect_sec[h] = 0.0f;
            }
        }

        s->fx_core_hit_t = clampf(s->fx_core_hit_t - DT_SEC, 0.0f, FX_CORE_HIT_SEC);
        maybe_end_round(s);
    }

    update_positions(s);
    update_effects(s, core_x, core_y);
    update_hud(s);
}

void drone_hunter_arena_start(lv_obj_t *screen)
{
    drone_hunter_scene_t *s = &g_scene;
    int32_t sw;
    int32_t sh;
    int32_t lineup_y;
    int32_t lineup_h;
    int i;

    s->screen = screen;
    sw = lv_obj_get_width(screen);
    sh = lv_obj_get_height(screen);

    s->arena_x = ARENA_MARGIN_X;
    s->arena_y = HUD_H + 8;
    s->arena_w = sw - (ARENA_MARGIN_X * 2);
    s->arena_h = sh - s->arena_y - DECK_H - 8;

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x05070B), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    s->splash = lv_obj_create(screen);
    lv_obj_remove_style_all(s->splash);
    lv_obj_set_size(s->splash, sw, sh);
    lv_obj_set_style_bg_color(s->splash, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(s->splash, LV_OPA_COVER, 0);

    {
        int idx;
        lv_obj_t *btn_lbl;

        s->splash_title = lv_label_create(s->splash);
        lv_obj_set_style_text_color(s->splash_title, lv_color_hex(0x00FF99), 0);
        lv_obj_set_style_text_font(s->splash_title, &lv_font_montserrat_24, 0);
        lv_obj_align(s->splash_title, LV_ALIGN_TOP_MID, 0, 16);

        s->splash_subtitle = lv_label_create(s->splash);
        lv_obj_set_style_text_color(s->splash_subtitle, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_font(s->splash_subtitle, &lv_font_montserrat_14, 0);
        lv_obj_align(s->splash_subtitle, LV_ALIGN_TOP_MID, 0, 50);
        lv_obj_add_flag(s->splash, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(s->splash, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(s->splash, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_event_cb(s->splash, splash_gesture_cb, LV_EVENT_ALL, s);

        s->splash_hero = lv_image_create(s->splash);
        lv_image_set_src(s->splash_hero, &img_hunter_sting_ii_detailed);
        lv_obj_align(s->splash_hero, LV_ALIGN_BOTTOM_MID, 0, -68);
        lv_obj_set_style_transform_pivot_x(s->splash_hero, 83, 0);
        lv_obj_set_style_transform_pivot_y(s->splash_hero, 84, 0);
        lv_obj_set_style_transform_angle(s->splash_hero, 0, 0);
        lv_obj_set_style_transform_zoom(s->splash_hero, 288, 0);
        lv_obj_set_style_transform_width(s->splash_hero, 10, 0);
        lv_obj_set_style_transform_height(s->splash_hero, 10, 0);
        lv_obj_set_style_opa(s->splash_hero, LV_OPA_COVER, 0);

        s->splash_hero_odin = lv_image_create(s->splash);
        lv_image_set_src(s->splash_hero_odin, &img_hunter_odin_win_hit_detailed);
        lv_obj_set_style_transform_pivot_x(s->splash_hero_odin, 50, 0);
        lv_obj_set_style_transform_pivot_y(s->splash_hero_odin, 82, 0);
        lv_obj_set_style_transform_angle(s->splash_hero_odin, 0, 0);
        lv_obj_set_style_transform_zoom(s->splash_hero_odin, 288, 0);
        lv_obj_set_style_transform_width(s->splash_hero_odin, 6, 0);
        lv_obj_set_style_transform_height(s->splash_hero_odin, 21, 0);
        lv_obj_set_style_opa(s->splash_hero_odin, LV_OPA_COVER, 0);
        lv_obj_move_foreground(s->splash_hero_odin);

        s->splash_hero_skyfall = lv_image_create(s->splash);
        lv_image_set_src(s->splash_hero_skyfall, &img_hunter_skyfall_p1);
        lv_obj_set_style_transform_pivot_x(s->splash_hero_skyfall, 66, 0);
        lv_obj_set_style_transform_pivot_y(s->splash_hero_skyfall, 82, 0);
        lv_obj_set_style_transform_angle(s->splash_hero_skyfall, 0, 0);
        lv_obj_set_style_transform_zoom(s->splash_hero_skyfall, 256, 0);
        lv_obj_set_style_transform_width(s->splash_hero_skyfall, 0, 0);
        lv_obj_set_style_transform_height(s->splash_hero_skyfall, 0, 0);
        lv_obj_set_style_opa(s->splash_hero_skyfall, LV_OPA_COVER, 0);
        lv_obj_move_foreground(s->splash_hero_skyfall);

        s->lineup_cont = lv_obj_create(s->splash);
        lv_obj_remove_style_all(s->lineup_cont);
        lineup_y = 64;
        lineup_h = sh - 152;
        if (lineup_h < 150)
        {
            lineup_h = 150;
        }
        lv_obj_set_size(s->lineup_cont, sw - 8, lineup_h);
        lv_obj_set_pos(s->lineup_cont, 4, lineup_y);
        lv_obj_set_style_bg_opa(s->lineup_cont, LV_OPA_0, 0);
        lv_obj_set_style_border_width(s->lineup_cont, 0, 0);
        lv_obj_add_flag(s->lineup_cont, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
        lv_obj_add_flag(s->lineup_cont, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_event_cb(s->lineup_cont, lineup_gesture_cb, LV_EVENT_GESTURE, s);

        for (idx = 0; idx < HUNTER_TYPE_COUNT; ++idx)
        {
            s->lineup_cards[idx] = create_hunter_preview(s->lineup_cont, (hunter_type_t)idx);
        }

        s->lineup_desc = lv_label_create(s->splash);
        lv_obj_set_width(s->lineup_desc, sw - 28);
        lv_obj_set_style_text_color(s->lineup_desc, lv_color_hex(0x93C5FD), 0);
        lv_obj_set_style_text_font(s->lineup_desc, &lv_font_montserrat_12, 0);
        lv_obj_align(s->lineup_desc, LV_ALIGN_BOTTOM_MID, 0, -42);
        lv_label_set_long_mode(s->lineup_desc, LV_LABEL_LONG_WRAP);
        lv_label_set_text(s->lineup_desc, "");

        s->splash_start_btn = lv_btn_create(s->splash);
        lv_obj_set_size(s->splash_start_btn, 180, 34);
        lv_obj_align(s->splash_start_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
        lv_obj_set_style_bg_color(s->splash_start_btn, lv_color_hex(0x2563EB), 0);
        lv_obj_set_style_bg_opa(s->splash_start_btn, LV_OPA_COVER, 0);
        lv_obj_add_event_cb(s->splash_start_btn, splash_start_cb, LV_EVENT_CLICKED, s);

        btn_lbl = lv_label_create(s->splash_start_btn);
        lv_label_set_text(btn_lbl, "START ARENA");
        lv_obj_set_style_text_color(btn_lbl, lv_color_hex(0xFFFFFF), 0);
        lv_obj_center(btn_lbl);

        s->splash_phase = 0;
        s->splash_phase_sec = 0.0f;
        s->lineup_focus_idx = 0;
        s->lineup_angle = 0.0f;
        s->lineup_target_angle = 0.0f;
        s->splash_center_idx = 0;
        s->splash_touch_active = 0;
        s->splash_touch_start_x = 0;
        s->splash_last_center_idx = -1;
        splash_show_title(s);
    }

    s->arena = lv_obj_create(screen);
    lv_obj_remove_style_all(s->arena);
    lv_obj_set_size(s->arena, sw, sh);
    lv_obj_add_flag(s->arena, LV_OBJ_FLAG_HIDDEN);
    s->arena_bg = lv_image_create(s->arena);
    lv_image_set_src(s->arena_bg, &img_hunter_nightscape_bg);
    lv_obj_set_pos(s->arena_bg, 0, 0);
    lv_obj_set_size(s->arena_bg, sw, sh);
    lv_obj_move_background(s->arena_bg);

    {
        lv_obj_t *hud = lv_obj_create(s->arena);
        lv_obj_remove_style_all(hud);
        lv_obj_set_size(hud, sw - 16, HUD_H - 4);
        lv_obj_set_pos(hud, 8, 8);
        lv_obj_set_style_bg_color(hud, lv_color_hex(0x0F172A), 0);
        lv_obj_set_style_bg_opa(hud, LV_OPA_80, 0);
        lv_obj_set_style_border_color(hud, lv_color_hex(0x334155), 0);
        lv_obj_set_style_border_width(hud, 1, 0);

        s->hud_mode = lv_label_create(hud);
        lv_obj_set_style_text_color(s->hud_mode, lv_color_hex(0x93C5FD), 0);
        lv_obj_set_style_text_font(s->hud_mode, &lv_font_montserrat_12, 0);
        lv_obj_align(s->hud_mode, LV_ALIGN_TOP_LEFT, 8, 3);

        s->hud_score = lv_label_create(hud);
        lv_obj_set_style_text_color(s->hud_score, lv_color_hex(0xFBBF24), 0);
        lv_obj_set_style_text_font(s->hud_score, &lv_font_montserrat_12, 0);
        lv_obj_align(s->hud_score, LV_ALIGN_CENTER, 0, -6);

        s->hud_info = lv_label_create(hud);
        lv_obj_set_style_text_color(s->hud_info, lv_color_hex(0x67E8F9), 0);
        lv_obj_set_style_text_font(s->hud_info, &lv_font_montserrat_12, 0);
        lv_obj_align(s->hud_info, LV_ALIGN_BOTTOM_LEFT, 8, -3);

        s->hud_wave = lv_label_create(hud);
        lv_obj_set_style_text_color(s->hud_wave, lv_color_hex(0xFCA5A5), 0);
        lv_obj_set_style_text_font(s->hud_wave, &lv_font_montserrat_12, 0);
        lv_obj_align(s->hud_wave, LV_ALIGN_TOP_LEFT, 8, 19);

        s->mode_btn = lv_btn_create(hud);
        lv_obj_set_size(s->mode_btn, 190, 26);
        lv_obj_align(s->mode_btn, LV_ALIGN_TOP_RIGHT, -8, 4);
        lv_obj_set_style_bg_color(s->mode_btn, lv_color_hex(0x0EA5E9), 0);
        lv_obj_set_style_bg_opa(s->mode_btn, LV_OPA_80, 0);
        lv_obj_add_event_cb(s->mode_btn, mode_cb, LV_EVENT_CLICKED, s);

        s->mode_btn_label = lv_label_create(s->mode_btn);
        lv_obj_set_style_text_color(s->mode_btn_label, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_font(s->mode_btn_label, &lv_font_montserrat_12, 0);
        lv_obj_center(s->mode_btn_label);
    }

    {
        lv_obj_t *field = lv_obj_create(s->arena);
        lv_obj_remove_style_all(field);
        lv_obj_set_pos(field, s->arena_x, s->arena_y);
        lv_obj_set_size(field, s->arena_w, s->arena_h);
        lv_obj_set_style_bg_opa(field, LV_OPA_0, 0);
        lv_obj_set_style_border_color(field, lv_color_hex(0x1F2937), 0);
        lv_obj_set_style_border_width(field, 2, 0);
    }

    s->deck_bar = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->deck_bar);
    lv_obj_set_size(s->deck_bar, sw - 16, DECK_H);
    lv_obj_set_pos(s->deck_bar, 8, sh - DECK_H - 6);
    lv_obj_set_style_bg_color(s->deck_bar, lv_color_hex(0x0B1220), 0);
    lv_obj_set_style_bg_opa(s->deck_bar, LV_OPA_70, 0);
    lv_obj_set_style_border_color(s->deck_bar, lv_color_hex(0x1E293B), 0);
    lv_obj_set_style_border_width(s->deck_bar, 1, 0);
    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        int32_t slot_w = (sw - 20) / (HUNTER_TYPE_COUNT + 1);
        int32_t x = 4 + (i * slot_w);
        s->deck_icon[i] = lv_image_create(s->deck_bar);
        lv_image_set_src(s->deck_icon[i], hunter_image_src((hunter_type_t)i));
        lv_obj_set_pos(s->deck_icon[i], x + 5, 2);
        lv_obj_set_size(s->deck_icon[i], slot_w - 10, 62);
        lv_obj_set_style_transform_zoom(s->deck_icon[i], 384, 0);
        lv_obj_set_style_transform_pivot_x(s->deck_icon[i], lv_obj_get_width(s->deck_icon[i]) / 2, 0);
        lv_obj_set_style_transform_pivot_y(s->deck_icon[i], lv_obj_get_height(s->deck_icon[i]) / 2, 0);
        lv_obj_set_style_outline_pad(s->deck_icon[i], 1, 0);

        s->deck_name[i] = lv_label_create(s->deck_bar);
        lv_obj_set_style_text_font(s->deck_name[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(s->deck_name[i], lv_color_hex(0xE5E7EB), 0);
        lv_obj_set_width(s->deck_name[i], slot_w - 6);
        lv_obj_set_pos(s->deck_name[i], x + 3, 72);
        lv_label_set_text(s->deck_name[i], hunter_type_name((hunter_type_t)i));
        lv_label_set_long_mode(s->deck_name[i], LV_LABEL_LONG_DOT);

        s->deck_count[i] = lv_label_create(s->deck_bar);
        lv_obj_set_style_text_font(s->deck_count[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(s->deck_count[i], lv_color_hex(0xFDE68A), 0);
        lv_obj_set_pos(s->deck_count[i], x + 3, 95);
        lv_label_set_text(s->deck_count[i], "0");
    }
    {
        int32_t slot_w = (sw - 20) / (HUNTER_TYPE_COUNT + 1);
        int32_t x = 4 + (HUNTER_TYPE_COUNT * slot_w);
        s->deck_ciws_icon = lv_image_create(s->deck_bar);
        lv_obj_set_pos(s->deck_ciws_icon, x + 5, 2);
        lv_image_set_src(s->deck_ciws_icon, &img_hunter_ciws);
        lv_obj_set_style_transform_zoom(s->deck_ciws_icon, 165, 0);

        s->deck_ciws_name = lv_label_create(s->deck_bar);
        lv_obj_set_style_text_font(s->deck_ciws_name, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(s->deck_ciws_name, lv_color_hex(0xE5E7EB), 0);
        lv_obj_set_width(s->deck_ciws_name, slot_w - 6);
        lv_obj_set_pos(s->deck_ciws_name, x + 3, 72);
        lv_label_set_text(s->deck_ciws_name, "Phalanx");
        lv_label_set_long_mode(s->deck_ciws_name, LV_LABEL_LONG_DOT);

        s->deck_ciws_count = lv_label_create(s->deck_bar);
        lv_obj_set_style_text_font(s->deck_ciws_count, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(s->deck_ciws_count, lv_color_hex(0x93C5FD), 0);
        lv_obj_set_pos(s->deck_ciws_count, x + 3, 95);
        lv_label_set_text(s->deck_ciws_count, "x0");
    }

    s->core = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->core);
    lv_obj_set_size(s->core, (int32_t)(CORE_RADIUS_PX * 2.0f), (int32_t)(CORE_RADIUS_PX * 2.0f));
    lv_obj_set_style_bg_opa(s->core, LV_OPA_0, 0);
    lv_obj_set_style_border_width(s->core, 0, 0);
    lv_obj_add_flag(s->core, LV_OBJ_FLAG_HIDDEN);
    set_obj_center(s->core, (float)(s->arena_x + s->arena_w / 2), (float)(s->arena_y + s->arena_h / 2));

    s->ciws = lv_image_create(s->arena);
    lv_image_set_src(s->ciws, &img_hunter_ciws);
    lv_obj_set_style_transform_zoom(s->ciws, 190, 0);
    lv_obj_set_style_transform_pivot_x(s->ciws, 22, 0);
    lv_obj_set_style_transform_pivot_y(s->ciws, 70, 0);
    s->ciws_turret = s->ciws;
    set_obj_center(s->ciws, (float)(s->arena_x + s->arena_w - 8), (float)(s->arena_y + s->arena_h - 49));
    lv_obj_set_style_transform_angle(s->ciws, 0, 0);
    s->ciws_left = lv_image_create(s->arena);
    lv_image_set_src(s->ciws_left, &img_hunter_ciws_flipped);
    lv_obj_set_style_transform_zoom(s->ciws_left, 190, 0);
    lv_obj_set_style_transform_pivot_x(s->ciws_left, 22, 0);
    lv_obj_set_style_transform_pivot_y(s->ciws_left, 70, 0);
    set_obj_center(s->ciws_left, (float)(s->arena_x + 18), (float)(s->arena_y + s->arena_h - 49));
    lv_obj_set_style_transform_angle(s->ciws_left, 0, 0);
    for (i = 0; i < CIWS_TRACER_COUNT; ++i)
    {
        s->ciws_tracer[i] = lv_obj_create(s->arena);
        lv_obj_remove_style_all(s->ciws_tracer[i]);
        lv_obj_set_size(s->ciws_tracer[i], 6, 6);
        lv_obj_set_style_radius(s->ciws_tracer[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(s->ciws_tracer[i], lv_color_hex(0xFDE68A), 0);
        lv_obj_set_style_bg_opa(s->ciws_tracer[i], LV_OPA_0, 0);
        lv_obj_add_flag(s->ciws_tracer[i], LV_OBJ_FLAG_HIDDEN);
    }

    for (i = 0; i < KILLER_COUNT; ++i)
    {
        s->killers[i] = lv_image_create(s->arena);
        lv_image_set_src(s->killers[i], &img_hunter_odin_win_hit);
        lv_obj_remove_style_all(s->killers[i]);
        lv_obj_set_style_transform_zoom(s->killers[i], 185, 0);

        s->killer_wing[i] = lv_obj_create(s->killers[i]);
        lv_obj_remove_style_all(s->killer_wing[i]);

        s->killer_wing_upper[i] = lv_obj_create(s->killers[i]);
        lv_obj_remove_style_all(s->killer_wing_upper[i]);

        s->killer_wing_lower[i] = lv_obj_create(s->killers[i]);
        lv_obj_remove_style_all(s->killer_wing_lower[i]);

        s->killer_body[i] = lv_obj_create(s->killers[i]);
        lv_obj_remove_style_all(s->killer_body[i]);

        s->killer_nose[i] = lv_obj_create(s->killers[i]);
        lv_obj_remove_style_all(s->killer_nose[i]);

        s->fx_intercept[i] = lv_obj_create(s->arena);
        lv_obj_remove_style_all(s->fx_intercept[i]);
        lv_obj_set_style_bg_opa(s->fx_intercept[i], LV_OPA_0, 0);
        lv_obj_add_flag(s->fx_intercept[i], LV_OBJ_FLAG_HIDDEN);

        s->fx_spawn[i] = lv_obj_create(s->arena);
        lv_obj_remove_style_all(s->fx_spawn[i]);
        lv_obj_set_style_bg_opa(s->fx_spawn[i], LV_OPA_0, 0);
        lv_obj_add_flag(s->fx_spawn[i], LV_OBJ_FLAG_HIDDEN);
    }

    for (i = 0; i < HUNTER_COUNT; ++i)
    {
        s->hunter_tail[i] = lv_obj_create(s->arena);
        lv_obj_remove_style_all(s->hunter_tail[i]);
        lv_obj_set_size(s->hunter_tail[i], 8, 8);
        lv_obj_set_style_radius(s->hunter_tail[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(s->hunter_tail[i], (i == 0) ? lv_color_hex(0x67E8F9) : lv_color_hex(0xFCD34D), 0);
        lv_obj_set_style_bg_opa(s->hunter_tail[i], LV_OPA_80, 0);

        lv_color_t team_main = (i == 0) ? lv_color_hex(0x06B6D4) : lv_color_hex(0xF59E0B);
        lv_color_t team_light = (i == 0) ? lv_color_hex(0x67E8F9) : lv_color_hex(0xFCD34D);

        s->hunters[i] = lv_image_create(s->arena);
        lv_image_set_src(s->hunters[i], &img_hunter_sting_ii);
        lv_obj_remove_style_all(s->hunters[i]);
        lv_obj_set_style_bg_opa(s->hunters[i], LV_OPA_0, 0);
        lv_obj_set_style_border_width(s->hunters[i], 0, 0);

        s->hunter_body[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_body[i]);
        lv_obj_set_size(s->hunter_body[i], 16, 7);
        lv_obj_set_pos(s->hunter_body[i], 11, 7);
        lv_obj_set_style_radius(s->hunter_body[i], 3, 0);
        lv_obj_set_style_bg_color(s->hunter_body[i], team_main, 0);
        lv_obj_set_style_bg_opa(s->hunter_body[i], LV_OPA_COVER, 0);

        s->hunter_wing[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_wing[i]);
        lv_obj_set_size(s->hunter_wing[i], 14, 4);
        lv_obj_set_pos(s->hunter_wing[i], 8, 8);
        lv_obj_set_style_radius(s->hunter_wing[i], 2, 0);
        lv_obj_set_style_bg_color(s->hunter_wing[i], team_light, 0);
        lv_obj_set_style_bg_opa(s->hunter_wing[i], LV_OPA_COVER, 0);

        s->hunter_pod_upper[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_pod_upper[i]);
        lv_obj_set_size(s->hunter_pod_upper[i], 9, 5);
        lv_obj_set_pos(s->hunter_pod_upper[i], 5, 3);
        lv_obj_set_style_radius(s->hunter_pod_upper[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(s->hunter_pod_upper[i], team_light, 0);
        lv_obj_set_style_bg_opa(s->hunter_pod_upper[i], LV_OPA_COVER, 0);

        s->hunter_pod_lower[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_pod_lower[i]);
        lv_obj_set_size(s->hunter_pod_lower[i], 9, 5);
        lv_obj_set_pos(s->hunter_pod_lower[i], 5, 12);
        lv_obj_set_style_radius(s->hunter_pod_lower[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(s->hunter_pod_lower[i], team_light, 0);
        lv_obj_set_style_bg_opa(s->hunter_pod_lower[i], LV_OPA_COVER, 0);

        s->hunter_prop_upper[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_prop_upper[i]);
        lv_obj_set_size(s->hunter_prop_upper[i], 2, 8);
        lv_obj_set_pos(s->hunter_prop_upper[i], 4, 2);
        lv_obj_set_style_bg_color(s->hunter_prop_upper[i], lv_color_hex(0x111827), 0);
        lv_obj_set_style_bg_opa(s->hunter_prop_upper[i], LV_OPA_COVER, 0);

        s->hunter_prop_lower[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_prop_lower[i]);
        lv_obj_set_size(s->hunter_prop_lower[i], 2, 8);
        lv_obj_set_pos(s->hunter_prop_lower[i], 4, 10);
        lv_obj_set_style_bg_color(s->hunter_prop_lower[i], lv_color_hex(0x111827), 0);
        lv_obj_set_style_bg_opa(s->hunter_prop_lower[i], LV_OPA_COVER, 0);
    }

    s->overlay = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->overlay);
    lv_obj_set_size(s->overlay, sw - 90, 130);
    lv_obj_align(s->overlay, LV_ALIGN_CENTER, 0, 8);
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

    assign_mode(s, MODE_ALGO_VS_EDGEAI);
    reset_round(s);

    s->anim_timer = lv_timer_create(anim_cb, TICK_MS, s);
    s->splash_timer = lv_timer_create(splash_tick_cb, TICK_MS, s);
}
