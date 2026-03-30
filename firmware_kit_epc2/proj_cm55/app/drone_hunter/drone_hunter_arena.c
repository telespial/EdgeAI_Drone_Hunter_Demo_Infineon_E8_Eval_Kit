#include "drone_hunter_arena.h"
#include "drone_hunter_images.h"
#include "drone_hunter_bagnet_detailed.h"
#include "drone_hunter_nightscape.h"
#include "drone_hunter_odin_detailed.h"
#include "drone_hunter_skyfall_detailed.h"
#include "drone_hunter_sting_detailed.h"
#include "drone_hunter_ciws.h"
#include "drone_hunter_attack_images.h"
#include "drone_hunter_flame_sprites.h"
/* Keep sprite frames in this translation unit for fixed ninja graphs that don't auto-pick new .c files. */
#include "drone_hunter_flame_sprites.c"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define PI_F                      (3.14159265359f)
#define SPRITE_NOSE_FWD_TENTH     (900)
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
#define FX_KILL_SEC               (0.32f)
#define FX_SHAHED_DEATH_SEC       (0.30f)
#define FX_CORE_HIT_SEC           (0.40f)
#define MAP_SIZE_KM               (120.0f)
#define PHALANX_EFFECTIVE_KM      (1.5f)
#define PHALANX_HARD_CUTOFF_KM    (5.0f)
#define CIWS_MAG_CAPACITY         (1550)
#define CIWS_MAGS_PER_GUN         (2)
#define CIWS_AMMO_PER_GUN         (CIWS_MAG_CAPACITY * CIWS_MAGS_PER_GUN)
#define CIWS_TRACER_COUNT         (160)
#define CIWS_TRACER_LIFE_SEC      (0.30f)
#define CIWS_FIRE_COOLDOWN_SEC    (0.006f)
#define CIWS_BURST_PELLETS        (14)
#define CIWS_AMMO_PER_TRIGGER     (24)
#define CIWS_LOCK_BAD_THRESH      (0.35f)
#define CIWS_RANGE_FRAC           (0.75f)
#define CIWS_SWEEP_SPEED_RAD      (1.9f)
#define CIWS_SWEEP_HALF_CONE      (0.14f)
#define CIWS_SWEEP_PITCH_SPEED    (1.15f)
#define CIWS_SWEEP_PITCH_MAX      (0.26f)
#define CIWS_SWEEP_HALF_CONE_PITCH (0.11f)
#define CIWS_TOP_GRID_BLOCK_FRAC  (0.58f)
#define CIWS_MAX_VERTICAL_FRAC    (0.44f)
#define CITY_FIRE_MAX             (64)
#define CITY_FIRE_RENDER_MAX      (64)
#define RENDER_STABILITY_SAFE_MODE (0)
#define FREEZE_DIAG_EFFECTS_BYPASS (0)
#define FREEZE_DIAG_LOGIC_BYPASS   (0)
#define FREEZE_DIAG_MINIMAL_LOOP   (0)
#define LANE_SITE_COUNT           (16)
#define LAUNCH_SECTOR_COUNT       (8)
#define HUD_H                     (72)
#define DECK_H                    (100)
#define ARENA_MARGIN_X            (24)
#define ATTACK_SPRITE_STABLE_ONLY (0)
#define HUD_REFRESH_SEC           (0.10f)
#define H_LOCK_PERSIST_ALGO_SEC   (0.62f)
#define H_LOCK_PERSIST_EDGE_SEC   (0.42f)
#define H_SWITCH_COOLDOWN_ALGO    (0.54f)
#define H_SWITCH_COOLDOWN_EDGE    (0.34f)
#define ATTACK_EVADE_RADIUS_FRAC  (0.23f)
#define SETTINGS_ROW_COUNT        (5)
#define BOOT_FLAME_GALLERY_MODE   (0)
#define BOOT_FLAME_PROFILE_SEC    (5.0f)

#define BLAST_STYLE_SMALL_WHITE   (0)
#define BLAST_STYLE_MEDIUM_RED    (1)
#define BLAST_STYLE_GIANT_ORANGE  (2)
/* Flame pack v2 style identity list (manifest-aligned). */
#define CITY_FIRE_PROFILE_TORCH      (0)
#define CITY_FIRE_PROFILE_BLUE_JET   (1)
#define CITY_FIRE_PROFILE_FLICKER    (2)
#define CITY_FIRE_PROFILE_SPIRAL     (3)
#define CITY_FIRE_PROFILE_BURST      (4)
#define CITY_FIRE_PROFILE_PULSE      (5)
#define CITY_FIRE_PROFILE_SMOKE      (6)
#define CITY_FIRE_PROFILE_DUAL_JET   (7)
#define CITY_FIRE_PROFILE_WHIP       (8)
#define CITY_FIRE_PROFILE_WIDE_TORCH (9)
#define CITY_FIRE_PROFILE_SPUTTER    (10)
#define CITY_FIRE_PROFILE_GROUND     (11)
#define CITY_FIRE_PROFILE_RED_ORANGE (12)
#define CITY_FIRE_PROFILE_ORANGE_WHITE (13)
#define CITY_FIRE_PROFILE_BRIGHT_RED (14)
#define CITY_FIRE_PROFILE_BRIGHT_ORANGE (15)
#define CITY_FIRE_PROFILE_BRIGHT_RED_HOT (16)
#define CITY_FIRE_PROFILE_BRIGHT_YELLOW_ORANGE (17)
#define CITY_FIRE_PROFILE_COUNT      (18)
#define CITY_FIRE_INTENSITY_SMALL (0)
#define CITY_FIRE_INTENSITY_BIG   (1)

#define COMMIT_REASON_READY       (0)
#define COMMIT_REASON_DETECT_LOW  (1)
#define COMMIT_REASON_CLASS_LOW   (2)
#define COMMIT_REASON_COMMIT_LOW  (3)
#define COMMIT_REASON_NO_TARGET   (4)
#define COMMIT_REASON_CORRIDOR    (5)
#define COMMIT_REASON_LOS_BLOCK   (6)

#define MISSION_MILESTONE_OPENING    (0)
#define MISSION_MILESTONE_ESCALATION (1)
#define MISSION_MILESTONE_CRISIS     (2)
#define MISSION_MILESTONE_SATURATION (3)
#define COLLATERAL_LOSS_THRESHOLD    (22)
#define WIN_WAVE_TARGET              (8)

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
    DEFENDER_MODE_ALGO = 0,
    DEFENDER_MODE_EDGEAI,
    DEFENDER_MODE_HUMAN
} defender_mode_t;

typedef enum
{
    DIFFICULTY_EASY = 0,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
} difficulty_t;

typedef enum
{
    THREAT_FACTION_RUSSIA = 0,
    THREAT_FACTION_USA
} threat_faction_t;

typedef enum
{
    ATTACK_STRATEGY_AUTO = 0,
    ATTACK_STRATEGY_CENTER_PRESSURE,
    ATTACK_STRATEGY_FLANK_PRESSURE,
    ATTACK_STRATEGY_MIXED_LURE_STRIKE,
    ATTACK_STRATEGY_TERMINAL_SATURATION,
    ATTACK_STRATEGY_COUNT
} attack_strategy_t;

typedef enum
{
    WAVE_ARCHETYPE_SHAHED_HEAVY = 0,
    WAVE_ARCHETYPE_STRIKE_X_SWARM,
    WAVE_ARCHETYPE_MIXED_DECEPTION,
    WAVE_ARCHETYPE_TERMINAL_SATURATION,
    WAVE_ARCHETYPE_COUNT
} wave_archetype_t;

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

typedef enum
{
    HUNTER_FLIGHT_VERTICAL = 0,
    HUNTER_FLIGHT_PLANE = 1,
    HUNTER_FLIGHT_HYBRID = 2
} hunter_flight_model_t;

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
    lv_obj_t *fx_kill[KILLER_COUNT];
    lv_obj_t *city_fire[CITY_FIRE_MAX];

    lv_obj_t *hud_mode;
    lv_obj_t *hud_hunter_score;
    lv_obj_t *hud_score;
    lv_obj_t *hud_attacker_score;
    lv_obj_t *hud_info;
    lv_obj_t *hud_wave;
    lv_obj_t *hud_elapsed;
    lv_obj_t *hud_attack_panel;
    lv_obj_t *hud_defend_panel;
    lv_obj_t *hud_attack_label;
    lv_obj_t *hud_defend_label;
    lv_obj_t *hud_attack_digits;
    lv_obj_t *hud_defend_digits;
    lv_obj_t *hud_attack_mode_text;
    lv_obj_t *hud_defend_mode_text;
    lv_obj_t *hud_diag_stage;
    char hud_diag_text[40];

    lv_obj_t *deck_bar;
    lv_obj_t *deck_icon[HUNTER_TYPE_COUNT];
    lv_obj_t *deck_name[HUNTER_TYPE_COUNT];
    lv_obj_t *deck_count[HUNTER_TYPE_COUNT];
    lv_obj_t *deck_ciws_icon;
    lv_obj_t *deck_ciws_name;
    lv_obj_t *deck_ciws_count;

    lv_obj_t *mode_btn;
    lv_obj_t *mode_btn_label;
    lv_obj_t *quick_menu;
    lv_obj_t *quick_menu_title;
    lv_obj_t *quick_menu_body;
    lv_obj_t *quick_menu_close;
    int quick_menu_settings_active;
    lv_obj_t *flame_gallery;
    int flame_gallery_visible;

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
    uint16_t h_base_zoom[HUNTER_COUNT];
    hunter_type_t h_type[HUNTER_COUNT];
    float h_reselect_sec[HUNTER_COUNT];
    int h_target_idx[HUNTER_COUNT];
    int h_target_serial[HUNTER_COUNT];
    int h_falling[HUNTER_COUNT];
    float h_launch_penalty[HUNTER_COUNT];
    float h_last_target_d2[HUNTER_COUNT];
    float h_overshoot_cooldown[HUNTER_COUNT];
    float h_lock_persist_t[HUNTER_COUNT];
    float h_switch_cooldown_t[HUNTER_COUNT];

    float kx[KILLER_COUNT];
    float ky[KILLER_COUNT];
    float kvx[KILLER_COUNT];
    float kvy[KILLER_COUNT];
    float k_heading[KILLER_COUNT];
    uint16_t k_base_zoom[KILLER_COUNT];
    const lv_image_dsc_t *k_last_src[KILLER_COUNT];

    float fx_intercept_t[KILLER_COUNT];
    float fx_spawn_t[KILLER_COUNT];
    float fx_intercept_x[KILLER_COUNT];
    float fx_intercept_y[KILLER_COUNT];
    float fx_spawn_x[KILLER_COUNT];
    float fx_spawn_y[KILLER_COUNT];
    float fx_kill_t[KILLER_COUNT];
    float fx_kill_x[KILLER_COUNT];
    float fx_kill_y[KILLER_COUNT];
    int fx_blast_style[KILLER_COUNT];
    float k_dying_t[KILLER_COUNT];
    float fx_core_hit_t;
    float ciws_cooldown_sec;
    float ciws_cooldown_left_sec;
    float ciws_sweep_right_rad;
    float ciws_sweep_left_rad;
    float ciws_sweep_right_pitch;
    float ciws_sweep_left_pitch;
    float ciws_sweep_right_dir;
    float ciws_sweep_left_dir;
    float ciws_sweep_right_pitch_dir;
    float ciws_sweep_left_pitch_dir;
    float ciws_heat_right;
    float ciws_heat_left;
    float ciws_lock_right;
    float ciws_lock_left;
    float ciws_tracer_t[CIWS_TRACER_COUNT];
    float ciws_tracer_x0[CIWS_TRACER_COUNT];
    float ciws_tracer_y0[CIWS_TRACER_COUNT];
    float ciws_tracer_x1[CIWS_TRACER_COUNT];
    float ciws_tracer_y1[CIWS_TRACER_COUNT];
    float ciws_tracer_g[CIWS_TRACER_COUNT];
    float ciws_tracer_scale[CIWS_TRACER_COUNT];

    int killer_spawn_tick;
    target_type_t ktype[KILLER_COUNT];
    int k_tier[KILLER_COUNT];
    int killer_active[KILLER_COUNT];
    int k_missed_by_hunter[KILLER_COUNT];
    int k_dying[KILLER_COUNT];
    int k_serial[KILLER_COUNT];
    float k_goal_x[KILLER_COUNT];
    float k_goal_y[KILLER_COUNT];
    float k_detect_conf[KILLER_COUNT];
    float k_class_conf[KILLER_COUNT];
    float k_commit_conf[KILLER_COUNT];
    float k_track_history[KILLER_COUNT];
    float k_noise[KILLER_COUNT];
    float k_threat_score[KILLER_COUNT];
    float k_payload_score[KILLER_COUNT];
    float k_survivability[KILLER_COUNT];
    float k_speed_est[KILLER_COUNT];
    float k_altitude_norm[KILLER_COUNT];
    float k_range_to_core[KILLER_COUNT];
    float k_eta_to_goal[KILLER_COUNT];
    int k_priority_rank[KILLER_COUNT];
    int k_spawn_site[KILLER_COUNT];
    int k_variant_seed[KILLER_COUNT];
    float k_target_value_mod[KILLER_COUNT];
    float k_lane_pressure[KILLER_COUNT];
    float lane_pressure[LANE_SITE_COUNT];
    hunter_type_t k_recommended_counter[KILLER_COUNT];
    int k_detect_ok[KILLER_COUNT];
    int k_class_ok[KILLER_COUNT];
    int k_commit_ok[KILLER_COUNT];
    int k_corridor_ok[KILLER_COUNT];
    int k_los_ok[KILLER_COUNT];
    int k_commit_reason[KILLER_COUNT];

    int commit_attempts;
    int commit_launched;
    int commit_hold_detect;
    int commit_hold_class;
    int commit_hold_conf;
    int commit_hold_corridor;
    int commit_hold_los;
    int h_swept_hit_events;
    int h_reacquire_events;
    int h_overshoot_events;
    int h_opportunistic_switch_events;
    int attacker_evasion_events;
    int attacker_goal_detonations;
    int attacker_algo_ticks;
    int attacker_edgeai_overrides;
    int attacker_edgeai_fallbacks;
    int fail_range_mismatch;
    int fail_altitude_mismatch;
    int fail_overkill;
    int fail_ciws_misuse;
    int fail_manual_override_low_conf;
    float fail_reason_ttl;
    char fail_reason_text[96];

    match_mode_t mode;
    controller_t team_ctrl[HUNTER_COUNT];

    int team_score[HUNTER_COUNT];
    int hunter_stock[HUNTER_TYPE_COUNT];
    int launch_sector_stock[LAUNCH_SECTOR_COUNT];
    float launch_sector_x[LAUNCH_SECTOR_COUNT];
    float launch_sector_y[LAUNCH_SECTOR_COUNT];
    int h_launch_sector[HUNTER_COUNT];
    int manual_selected_hunter;
    int manual_selected_target;
    float manual_select_ttl;
    int iff_advanced_mode;
    int iff_degraded;
    int iff_merged_tracks;
    int iff_ff_events;
    int iff_collateral_events;
    float iff_recovery_ttl;
    int attack_remaining_to_spawn;
    int attack_destroyed;
    int attack_leaked;
    int wave_idx;
    int wave_target_total;
    wave_archetype_t wave_archetype;
    int mission_milestone;
    int wave_shift_applied;
    threat_faction_t threat_faction;
    attack_strategy_t attack_strategy_select;
    attack_strategy_t attack_strategy_live;
    int defense_kills;
    int defense_misses;
    int hunter_points;
    int attacker_points;
    int ciws_ammo_right;
    int ciws_ammo_left;
    int ciws_shots;
    int ciws_kills;
    int ciws_tracer_head;
    int city_fire_count;
    int city_fire_head;
    float city_fire_x[CITY_FIRE_MAX];
    float city_fire_y[CITY_FIRE_MAX];
    uint8_t city_fire_style[CITY_FIRE_MAX];
    uint8_t city_fire_intensity[CITY_FIRE_MAX];
    float city_fire_phase[CITY_FIRE_MAX];
    uint8_t city_fire_last_profile[CITY_FIRE_MAX];
    uint8_t city_fire_last_frame[CITY_FIRE_MAX];
    int hunter_loaded[HUNTER_COUNT];
    int core_hp;
    float round_time_sec;
    int round_over;
    uint32_t round_start_tick_ms;
    float hud_refresh_t;
    controller_t attacker_mode_sel;
    defender_mode_t defender_mode_sel;
    difficulty_t difficulty_sel;
    int npu_enabled;
    int speed_pp_idx;
    uint32_t rng_state;
    uint32_t strategy_replan_tick;

    float t;
} drone_hunter_scene_t;

static drone_hunter_scene_t g_scene;
/* Canonical fire buckets: reuse these for runtime selection and boot gallery loop. */
static const uint8_t g_city_fire_bright_profiles[8] = {
    CITY_FIRE_PROFILE_BRIGHT_ORANGE,
    CITY_FIRE_PROFILE_BRIGHT_RED,
    CITY_FIRE_PROFILE_BRIGHT_YELLOW_ORANGE,
    CITY_FIRE_PROFILE_ORANGE_WHITE,
    CITY_FIRE_PROFILE_RED_ORANGE,
    CITY_FIRE_PROFILE_BRIGHT_RED_HOT,
    CITY_FIRE_PROFILE_TORCH,
    CITY_FIRE_PROFILE_BURST
};
static const uint8_t g_city_fire_dark_profiles[8] = {
    CITY_FIRE_PROFILE_SMOKE,
    CITY_FIRE_PROFILE_GROUND,
    CITY_FIRE_PROFILE_BLUE_JET,
    CITY_FIRE_PROFILE_SPIRAL,
    CITY_FIRE_PROFILE_PULSE,
    CITY_FIRE_PROFILE_FLICKER,
    CITY_FIRE_PROFILE_DUAL_JET,
    CITY_FIRE_PROFILE_SPUTTER
};

static float clampf(float v, float lo, float hi);
static float combat_floor_y(const drone_hunter_scene_t *s);
static float dist2(float ax, float ay, float bx, float by);
static float threat_required_score(const drone_hunter_scene_t *s, int k);
static void update_hunter_deck_ui(drone_hunter_scene_t *s);
static void respawn_killer(drone_hunter_scene_t *s, int k, int side);
static void set_killer_hidden(drone_hunter_scene_t *s, int k);
static void note_failure(drone_hunter_scene_t *s, const char *txt, int *counter);
static int target_has_assigned_hunter(const drone_hunter_scene_t *s, int target);
static void deck_pick_cb(lv_event_t *e);
static void target_pick_cb(lv_event_t *e);
static void iff_toggle_cb(lv_event_t *e);
static void quick_menu_close_cb(lv_event_t *e);
static void quick_menu_body_cb(lv_event_t *e);
static void show_quick_menu(drone_hunter_scene_t *s, const char *title, const char *body);
static void hud_attack_card_cb(lv_event_t *e);
static void hud_defend_card_cb(lv_event_t *e);
static void update_hud(drone_hunter_scene_t *s);
static int target_is_shahed_visual(const drone_hunter_scene_t *s, int k);
static int target_blast_style_for_visual(const drone_hunter_scene_t *s, int k);
static void reset_round(drone_hunter_scene_t *s);

static uint32_t runtime_entropy32(void)
{
    volatile uint32_t systick_val = *((volatile uint32_t *)0xE000E018u);
    volatile uint32_t icsr = *((volatile uint32_t *)0xE000ED04u);
    uint32_t t = lv_tick_get();
    return (systick_val ^ (icsr << 7) ^ (t * 2654435761u));
}

static void set_diag_stage(drone_hunter_scene_t *s, const char *stage)
{
    if ((s == NULL) || (s->hud_diag_stage == NULL) || (stage == NULL))
    {
        return;
    }
    if (strncmp(s->hud_diag_text, stage, sizeof(s->hud_diag_text) - 1) == 0)
    {
        return;
    }
    (void)snprintf(s->hud_diag_text, sizeof(s->hud_diag_text), "%s", stage);
    lv_label_set_text(s->hud_diag_stage, s->hud_diag_text);
}

static uint32_t scene_rng_next(drone_hunter_scene_t *s)
{
    uint32_t x = s->rng_state;
    if (x == 0U)
    {
        x = 0xA5C31F2Du;
    }
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    s->rng_state = x;
    return x;
}

static int scene_rng_range(drone_hunter_scene_t *s, int limit)
{
    if (limit <= 1)
    {
        return 0;
    }
    return (int)(scene_rng_next(s) % (uint32_t)limit);
}

static const lv_image_dsc_t **city_fire_profile_frames(int profile)
{
    /* Map style identities onto stable sprite families from flame pack v2. */
    switch (profile)
    {
        case CITY_FIRE_PROFILE_TORCH:
        case CITY_FIRE_PROFILE_BLUE_JET:
        case CITY_FIRE_PROFILE_DUAL_JET:
        case CITY_FIRE_PROFILE_WIDE_TORCH:
        case CITY_FIRE_PROFILE_RED_ORANGE:
        case CITY_FIRE_PROFILE_ORANGE_WHITE:
        case CITY_FIRE_PROFILE_BRIGHT_ORANGE:
        case CITY_FIRE_PROFILE_BRIGHT_YELLOW_ORANGE:
            return g_flame_profile_torch;
        case CITY_FIRE_PROFILE_FLICKER:
        case CITY_FIRE_PROFILE_BURST:
        case CITY_FIRE_PROFILE_PULSE:
        case CITY_FIRE_PROFILE_WHIP:
        case CITY_FIRE_PROFILE_SPUTTER:
        case CITY_FIRE_PROFILE_BRIGHT_RED:
        case CITY_FIRE_PROFILE_BRIGHT_RED_HOT:
            return g_flame_profile_burst;
        case CITY_FIRE_PROFILE_SMOKE:
            return g_flame_profile_smoke;
        case CITY_FIRE_PROFILE_SPIRAL:
        case CITY_FIRE_PROFILE_GROUND:
        default:
            return g_flame_profile_ground;
    }
}

static const char *city_fire_profile_name(int profile)
{
    switch (profile)
    {
        case CITY_FIRE_PROFILE_TORCH: return "Torch";
        case CITY_FIRE_PROFILE_BLUE_JET: return "Blue Jet";
        case CITY_FIRE_PROFILE_FLICKER: return "Flicker";
        case CITY_FIRE_PROFILE_SPIRAL: return "Spiral";
        case CITY_FIRE_PROFILE_BURST: return "Burst";
        case CITY_FIRE_PROFILE_PULSE: return "Pulse";
        case CITY_FIRE_PROFILE_SMOKE: return "Smoke";
        case CITY_FIRE_PROFILE_DUAL_JET: return "Dual Jet";
        case CITY_FIRE_PROFILE_WHIP: return "Whip";
        case CITY_FIRE_PROFILE_WIDE_TORCH: return "Wide Torch";
        case CITY_FIRE_PROFILE_SPUTTER: return "Sputter";
        case CITY_FIRE_PROFILE_GROUND: return "Ground";
        case CITY_FIRE_PROFILE_RED_ORANGE: return "Red/Orange";
        case CITY_FIRE_PROFILE_ORANGE_WHITE: return "Orange/White";
        case CITY_FIRE_PROFILE_BRIGHT_RED: return "Bright Red";
        case CITY_FIRE_PROFILE_BRIGHT_ORANGE: return "Bright Orange";
        case CITY_FIRE_PROFILE_BRIGHT_RED_HOT: return "Red Hot";
        case CITY_FIRE_PROFILE_BRIGHT_YELLOW_ORANGE: return "Yellow/Orange";
        default: return "Unknown";
    }
}

static lv_color_t city_fire_profile_preview_tint(int profile)
{
    switch (profile)
    {
        case CITY_FIRE_PROFILE_TORCH: return lv_color_hex(0xFFA640);
        case CITY_FIRE_PROFILE_BLUE_JET: return lv_color_hex(0x58C8FF);
        case CITY_FIRE_PROFILE_FLICKER: return lv_color_hex(0x8CDD55);
        case CITY_FIRE_PROFILE_SPIRAL: return lv_color_hex(0xB56AF5);
        case CITY_FIRE_PROFILE_BURST: return lv_color_hex(0xFF6B2E);
        case CITY_FIRE_PROFILE_PULSE: return lv_color_hex(0xFFF2E5);
        case CITY_FIRE_PROFILE_SMOKE: return lv_color_hex(0x4A4038);
        case CITY_FIRE_PROFILE_DUAL_JET: return lv_color_hex(0x55D4C7);
        case CITY_FIRE_PROFILE_WHIP: return lv_color_hex(0xC756F0);
        case CITY_FIRE_PROFILE_WIDE_TORCH: return lv_color_hex(0xF3B23F);
        case CITY_FIRE_PROFILE_SPUTTER: return lv_color_hex(0x9BCC48);
        case CITY_FIRE_PROFILE_GROUND: return lv_color_hex(0xA43A2A);
        case CITY_FIRE_PROFILE_RED_ORANGE: return lv_color_hex(0xFF3B1F);
        case CITY_FIRE_PROFILE_ORANGE_WHITE: return lv_color_hex(0xFFB000);
        case CITY_FIRE_PROFILE_BRIGHT_RED: return lv_color_hex(0xFF1E1E);
        case CITY_FIRE_PROFILE_BRIGHT_ORANGE: return lv_color_hex(0xFF9800);
        case CITY_FIRE_PROFILE_BRIGHT_RED_HOT: return lv_color_hex(0xFF1A1A);
        case CITY_FIRE_PROFILE_BRIGHT_YELLOW_ORANGE: return lv_color_hex(0xFF9C00);
        default: return lv_color_hex(0xFFFFFF);
    }
}

static lv_opa_t city_fire_profile_preview_tint_opa(int profile)
{
    switch (profile)
    {
        case CITY_FIRE_PROFILE_GROUND: return (lv_opa_t)130;
        case CITY_FIRE_PROFILE_SMOKE: return (lv_opa_t)175;
        case CITY_FIRE_PROFILE_PULSE: return (lv_opa_t)120;
        case CITY_FIRE_PROFILE_ORANGE_WHITE: return (lv_opa_t)255;
        case CITY_FIRE_PROFILE_BRIGHT_RED: return (lv_opa_t)255;
        case CITY_FIRE_PROFILE_BRIGHT_ORANGE: return (lv_opa_t)255;
        case CITY_FIRE_PROFILE_BRIGHT_RED_HOT: return (lv_opa_t)255;
        case CITY_FIRE_PROFILE_BRIGHT_YELLOW_ORANGE: return (lv_opa_t)255;
        default: return (lv_opa_t)200;
    }
}

static int city_fire_is_dark_profile(int profile)
{
    return (profile == CITY_FIRE_PROFILE_SMOKE) ||
           (profile == CITY_FIRE_PROFILE_GROUND) ||
           (profile == CITY_FIRE_PROFILE_BLUE_JET) ||
           (profile == CITY_FIRE_PROFILE_SPIRAL) ||
           (profile == CITY_FIRE_PROFILE_PULSE) ||
           (profile == CITY_FIRE_PROFILE_FLICKER) ||
           (profile == CITY_FIRE_PROFILE_DUAL_JET) ||
           (profile == CITY_FIRE_PROFILE_SPUTTER);
}

static lv_color_t city_fire_bright_demo_tint(int phase_idx)
{
    static const uint32_t bright_palette[6] = {
        0xFF4B21, 0xFF8A00, 0xFFC21A, 0xFFD84A, 0xFF2D2D, 0xFFB020
    };
    return lv_color_hex(bright_palette[phase_idx % 6]);
}

static float city_fire_profile_fps(int profile)
{
    switch (profile)
    {
        case CITY_FIRE_PROFILE_TORCH: return 10.5f;
        case CITY_FIRE_PROFILE_BLUE_JET: return 10.0f;
        case CITY_FIRE_PROFILE_FLICKER: return 11.0f;
        case CITY_FIRE_PROFILE_SPIRAL: return 8.2f;
        case CITY_FIRE_PROFILE_BURST: return 11.8f;
        case CITY_FIRE_PROFILE_PULSE: return 7.0f;
        case CITY_FIRE_PROFILE_SMOKE: return 5.4f;
        case CITY_FIRE_PROFILE_DUAL_JET: return 10.4f;
        case CITY_FIRE_PROFILE_WHIP: return 9.8f;
        case CITY_FIRE_PROFILE_WIDE_TORCH: return 9.2f;
        case CITY_FIRE_PROFILE_SPUTTER: return 8.8f;
        case CITY_FIRE_PROFILE_RED_ORANGE: return 10.8f;
        case CITY_FIRE_PROFILE_ORANGE_WHITE: return 10.2f;
        case CITY_FIRE_PROFILE_BRIGHT_RED: return 11.4f;
        case CITY_FIRE_PROFILE_BRIGHT_ORANGE: return 11.0f;
        case CITY_FIRE_PROFILE_BRIGHT_RED_HOT: return 11.2f;
        case CITY_FIRE_PROFILE_BRIGHT_YELLOW_ORANGE: return 10.9f;
        case CITY_FIRE_PROFILE_GROUND:
        default: return 6.4f;
    }
}

static int city_fire_pick_profile(float seed, int intensity)
{
    int palette_gate = ((int)fabsf(fmodf(seed * 131.0f, 4096.0f))) % 100;
    int roll = ((int)fabsf(fmodf(seed * 173.0f, 4096.0f)));
    (void)intensity;
    if (palette_gate < 75)
    {
        return (int)g_city_fire_bright_profiles[roll % 8];
    }
    return (int)g_city_fire_dark_profiles[roll % 8];
}

static float city_fire_nearest_d2(const drone_hunter_scene_t *s, float x, float y)
{
    int i;
    float best = 1.0e30f;
    for (i = 0; i < s->city_fire_count; ++i)
    {
        float d2 = dist2(x, y, s->city_fire_x[i], s->city_fire_y[i]);
        if (d2 < best)
        {
            best = d2;
        }
    }
    return best;
}

static void add_city_fire(drone_hunter_scene_t *s, float x, float y, int intensity)
{
    int idx;
    float min_x = (float)s->arena_x + 10.0f;
    float max_x = (float)(s->arena_x + s->arena_w - 10);
    float floor_y = combat_floor_y(s);
    float min_y = (float)s->arena_y + ((float)s->arena_h * 0.20f);
    float max_y = floor_y - 2.0f;
    int style_pick;
    float best_x;
    float best_y;
    float best_d2;
    float spacing = (intensity >= CITY_FIRE_INTENSITY_BIG) ? 32.0f : 20.0f;
    float spacing2 = spacing * spacing;
    int attempt;

    x = clampf(x, min_x, max_x);
    y = clampf(y, min_y, max_y);
    best_x = x;
    best_y = y;
    best_d2 = city_fire_nearest_d2(s, x, y);

    /* Prevent stacked fires at the same impact pixel so each successful strike remains visible. */
    for (attempt = 0; attempt < 7; ++attempt)
    {
        float cand_x = x;
        float cand_y = y;
        if (attempt > 0)
        {
            float a = ((float)scene_rng_range(s, 628) / 100.0f);
            float r = spacing * (0.65f + ((float)scene_rng_range(s, 75) / 100.0f));
            cand_x += cosf(a) * r;
            cand_y += sinf(a) * (r * 0.64f);
            cand_x = clampf(cand_x, min_x, max_x);
            cand_y = clampf(cand_y, min_y, max_y);
        }
        {
            float d2 = city_fire_nearest_d2(s, cand_x, cand_y);
            if ((d2 > best_d2) || ((attempt == 0) && (best_d2 <= 0.1f)))
            {
                best_x = cand_x;
                best_y = cand_y;
                best_d2 = d2;
            }
            if (d2 >= spacing2)
            {
                best_x = cand_x;
                best_y = cand_y;
                break;
            }
        }
    }
    x = best_x;
    y = best_y;

    if (s->city_fire_count < CITY_FIRE_MAX)
    {
        idx = s->city_fire_count++;
    }
    else
    {
        idx = s->city_fire_head;
        s->city_fire_head = (s->city_fire_head + 1) % CITY_FIRE_MAX;
    }

    s->city_fire_x[idx] = x;
    s->city_fire_y[idx] = y;
    style_pick = city_fire_pick_profile((s->t * 13.0f) + (x * 0.07f) + (y * 0.11f) + (float)(idx * 3), intensity);
    s->city_fire_style[idx] = (uint8_t)style_pick;
    s->city_fire_intensity[idx] = (uint8_t)((intensity >= CITY_FIRE_INTENSITY_BIG) ? CITY_FIRE_INTENSITY_BIG : CITY_FIRE_INTENSITY_SMALL);
    s->city_fire_phase[idx] = s->t + ((float)idx * 0.37f);
    s->city_fire_last_profile[idx] = 0xFFu;
    s->city_fire_last_frame[idx] = 0xFFu;
}

static int lane_from_fraction(float t)
{
    if (t < 0.25f)
    {
        return 0;
    }
    if (t < 0.50f)
    {
        return 1;
    }
    if (t < 0.75f)
    {
        return 2;
    }
    return 3;
}

static int lane_site_edge(int site)
{
    return (site / 4) & 0x3;
}

static int lane_site_lane(int site)
{
    return site & 0x3;
}

static float goal_nearest_existing_d2(const drone_hunter_scene_t *s, int self_k, float gx, float gy)
{
    int i;
    float best = 1.0e30f;
    for (i = 0; i < KILLER_COUNT; ++i)
    {
        if ((i == self_k) || !s->killer_active[i] || s->k_dying[i])
        {
            continue;
        }
        {
            float d2 = dist2(gx, gy, s->k_goal_x[i], s->k_goal_y[i]);
            if (d2 < best)
            {
                best = d2;
            }
        }
    }
    for (i = 0; i < s->city_fire_count; ++i)
    {
        float d2 = dist2(gx, gy, s->city_fire_x[i], s->city_fire_y[i]);
        if (d2 < best)
        {
            best = d2;
        }
    }
    return best;
}

static void choose_attack_goal(drone_hunter_scene_t *s, int k,
                               float goal_min_x, float goal_max_x,
                               float goal_min_y, float goal_max_y)
{
    float span_x = (goal_max_x - goal_min_x > 1.0f) ? (goal_max_x - goal_min_x) : 1.0f;
    float span_y = (goal_max_y - goal_min_y > 1.0f) ? (goal_max_y - goal_min_y) : 1.0f;
    float best_x = goal_min_x + (float)scene_rng_range(s, 1000) * 0.001f * span_x;
    float best_y = goal_min_y + (float)scene_rng_range(s, 1000) * 0.001f * span_y;
    float best_score = -1.0e30f;
    int tries = 12;
    int t;

    for (t = 0; t < tries; ++t)
    {
        float ux = (float)scene_rng_range(s, 1000) * 0.001f;
        float uy = (float)scene_rng_range(s, 1000) * 0.001f;
        float gx;
        float gy;
        float center_bias;
        float spread_d2;
        float score;

        if (s->attack_strategy_live == ATTACK_STRATEGY_CENTER_PRESSURE)
        {
            ux = 0.5f + ((ux - 0.5f) * 0.56f);
        }
        else if (s->attack_strategy_live == ATTACK_STRATEGY_FLANK_PRESSURE)
        {
            if (ux < 0.5f)
            {
                ux *= 0.64f;
            }
            else
            {
                ux = 1.0f - ((1.0f - ux) * 0.64f);
            }
        }
        else if (s->attack_strategy_live == ATTACK_STRATEGY_TERMINAL_SATURATION)
        {
            uy = 0.40f + (uy * 0.60f);
        }

        gx = goal_min_x + (ux * span_x);
        gy = goal_min_y + (uy * span_y);
        spread_d2 = goal_nearest_existing_d2(s, k, gx, gy);
        center_bias = 1.0f - fabsf(((gx - goal_min_x) / span_x) - 0.5f) * 2.0f;
        score = spread_d2 * 0.90f;

        if (s->attack_strategy_live == ATTACK_STRATEGY_CENTER_PRESSURE)
        {
            score += center_bias * 2400.0f;
        }
        else if (s->attack_strategy_live == ATTACK_STRATEGY_FLANK_PRESSURE)
        {
            score += (1.0f - center_bias) * 2400.0f;
        }
        else if (s->attack_strategy_live == ATTACK_STRATEGY_MIXED_LURE_STRIKE)
        {
            score += fabsf(sinf((s->t * 0.72f) + (float)(k * 1.7f) + (float)t)) * 1200.0f;
        }
        else
        {
            score += 850.0f * uy;
        }

        if (score > best_score)
        {
            best_score = score;
            best_x = gx;
            best_y = gy;
        }
    }

    s->k_goal_x[k] = clampf(best_x, goal_min_x, goal_max_x);
    s->k_goal_y[k] = clampf(best_y, goal_min_y, goal_max_y);
}

static const char *attack_strategy_short_name(attack_strategy_t st)
{
    switch (st)
    {
        case ATTACK_STRATEGY_CENTER_PRESSURE: return "CENTER";
        case ATTACK_STRATEGY_FLANK_PRESSURE: return "FLANK";
        case ATTACK_STRATEGY_MIXED_LURE_STRIKE: return "MIXED";
        case ATTACK_STRATEGY_TERMINAL_SATURATION: return "TERMINAL";
        case ATTACK_STRATEGY_AUTO:
        default: return "AUTO";
    }
}

static const char *wave_archetype_short_name(wave_archetype_t archetype)
{
    switch (archetype)
    {
        case WAVE_ARCHETYPE_SHAHED_HEAVY: return "SHAHED";
        case WAVE_ARCHETYPE_STRIKE_X_SWARM: return "X-SWARM";
        case WAVE_ARCHETYPE_MIXED_DECEPTION: return "MIXED";
        case WAVE_ARCHETYPE_TERMINAL_SATURATION: return "TERM-SAT";
        default: return "ARCH";
    }
}

static int mission_milestone_from_wave(int wave_idx)
{
    if (wave_idx <= 2)
    {
        return MISSION_MILESTONE_OPENING;
    }
    if (wave_idx <= 4)
    {
        return MISSION_MILESTONE_ESCALATION;
    }
    if (wave_idx <= 7)
    {
        return MISSION_MILESTONE_CRISIS;
    }
    return MISSION_MILESTONE_SATURATION;
}

static const char *mission_milestone_short_name(int milestone)
{
    switch (milestone)
    {
        case MISSION_MILESTONE_OPENING:    return "OPENING";
        case MISSION_MILESTONE_ESCALATION: return "ESCALATE";
        case MISSION_MILESTONE_CRISIS:     return "CRISIS";
        case MISSION_MILESTONE_SATURATION:
        default:                           return "SATURATE";
    }
}

static float mission_milestone_spawn_mult(int milestone)
{
    switch (milestone)
    {
        case MISSION_MILESTONE_OPENING:    return 0.92f;
        case MISSION_MILESTONE_ESCALATION: return 1.00f;
        case MISSION_MILESTONE_CRISIS:     return 1.10f;
        case MISSION_MILESTONE_SATURATION:
        default:                           return 1.18f;
    }
}

static wave_archetype_t wave_archetype_from_wave(int wave_idx)
{
    int phase = (wave_idx - 1) % 4;
    if (phase < 0)
    {
        phase += 4;
    }
    return (wave_archetype_t)phase;
}

static attack_strategy_t attack_strategy_for_archetype(wave_archetype_t archetype)
{
    switch (archetype)
    {
        case WAVE_ARCHETYPE_SHAHED_HEAVY: return ATTACK_STRATEGY_CENTER_PRESSURE;
        case WAVE_ARCHETYPE_STRIKE_X_SWARM: return ATTACK_STRATEGY_FLANK_PRESSURE;
        case WAVE_ARCHETYPE_MIXED_DECEPTION: return ATTACK_STRATEGY_MIXED_LURE_STRIKE;
        case WAVE_ARCHETYPE_TERMINAL_SATURATION:
        default:
            return ATTACK_STRATEGY_TERMINAL_SATURATION;
    }
}

static attack_strategy_t attack_strategy_shift_for_archetype(wave_archetype_t archetype)
{
    switch (archetype)
    {
        case WAVE_ARCHETYPE_SHAHED_HEAVY: return ATTACK_STRATEGY_TERMINAL_SATURATION;
        case WAVE_ARCHETYPE_STRIKE_X_SWARM: return ATTACK_STRATEGY_CENTER_PRESSURE;
        case WAVE_ARCHETYPE_MIXED_DECEPTION: return ATTACK_STRATEGY_TERMINAL_SATURATION;
        case WAVE_ARCHETYPE_TERMINAL_SATURATION:
        default:
            return ATTACK_STRATEGY_FLANK_PRESSURE;
    }
}

static attack_strategy_t attack_strategy_late_shift_for_archetype(wave_archetype_t archetype)
{
    switch (archetype)
    {
        case WAVE_ARCHETYPE_SHAHED_HEAVY: return ATTACK_STRATEGY_FLANK_PRESSURE;
        case WAVE_ARCHETYPE_STRIKE_X_SWARM: return ATTACK_STRATEGY_TERMINAL_SATURATION;
        case WAVE_ARCHETYPE_MIXED_DECEPTION: return ATTACK_STRATEGY_CENTER_PRESSURE;
        case WAVE_ARCHETYPE_TERMINAL_SATURATION:
        default:
            return ATTACK_STRATEGY_CENTER_PRESSURE;
    }
}

static threat_faction_t threat_faction_for_archetype(wave_archetype_t archetype, int wave_idx)
{
    switch (archetype)
    {
        case WAVE_ARCHETYPE_SHAHED_HEAVY:
        case WAVE_ARCHETYPE_TERMINAL_SATURATION:
            return THREAT_FACTION_RUSSIA;
        case WAVE_ARCHETYPE_STRIKE_X_SWARM:
            return THREAT_FACTION_USA;
        case WAVE_ARCHETYPE_MIXED_DECEPTION:
        default:
            return (wave_idx & 1) ? THREAT_FACTION_RUSSIA : THREAT_FACTION_USA;
    }
}

static int nearest_center_lane(int lane)
{
    return (lane <= 1) ? 1 : 2;
}

static int nearest_flank_lane(int lane)
{
    return (lane <= 1) ? 0 : 3;
}

static int lane_with_max_pressure_on_edge(const drone_hunter_scene_t *s, int edge, int fallback_lane)
{
    int lane;
    int best_lane = fallback_lane;
    float best_p = -1.0f;

    for (lane = 0; lane < 4; ++lane)
    {
        int site = (edge * 4) + lane;
        float p = s->lane_pressure[site];
        if (p > best_p)
        {
            best_p = p;
            best_lane = lane;
        }
    }
    return best_lane;
}

static float target_value_modifier(const drone_hunter_scene_t *s, float gx, float gy, float core_x, float core_y)
{
    float ax = (float)s->arena_x + ((float)s->arena_w * 0.36f);
    float ay = (float)s->arena_y + ((float)s->arena_h * 0.68f);
    float bx = core_x;
    float by = (float)s->arena_y + ((float)s->arena_h * 0.74f);
    float cx = (float)s->arena_x + ((float)s->arena_w * 0.66f);
    float cy = (float)s->arena_y + ((float)s->arena_h * 0.70f);
    float d0 = sqrtf(dist2(gx, gy, ax, ay));
    float d1 = sqrtf(dist2(gx, gy, bx, by));
    float d2 = sqrtf(dist2(gx, gy, cx, cy));
    float dmin = fminf(d0, fminf(d1, d2));
    float diag = sqrtf(((float)s->arena_w * (float)s->arena_w) + ((float)s->arena_h * (float)s->arena_h));
    float near_asset = 1.0f - clampf(dmin / clampf(diag, 1.0f, 8000.0f), 0.0f, 1.0f);
    float y_norm = clampf((gy - (float)s->arena_y) / clampf((float)s->arena_h, 1.0f, 4000.0f), 0.0f, 1.0f);
    float v = 1.0f + (near_asset * 0.90f) + (y_norm * 0.25f);
    return clampf(v, 0.80f, 2.40f);
}

static int choose_spawn_site(drone_hunter_scene_t *s, int k, float goal_x, float goal_y)
{
    int edge = (s->killer_spawn_tick + k) & 0x3; /* strict even edge rotation */
    float arena_min_x = (float)s->arena_x;
    float arena_max_x = (float)(s->arena_x + s->arena_w);
    float arena_min_y = (float)s->arena_y;
    float arena_max_y = (float)(s->arena_y + s->arena_h);
    float tx = (goal_x - arena_min_x) / ((arena_max_x - arena_min_x > 1.0f) ? (arena_max_x - arena_min_x) : 1.0f);
    float ty = (goal_y - arena_min_y) / ((arena_max_y - arena_min_y > 1.0f) ? (arena_max_y - arena_min_y) : 1.0f);
    int lane_goal;
    int lane;
    attack_strategy_t strategy = s->attack_strategy_live;

    if (edge == 0 || edge == 1)
    {
        lane_goal = lane_from_fraction(ty);
    }
    else
    {
        lane_goal = lane_from_fraction(tx);
    }

    lane = lane_goal;
    if (strategy == ATTACK_STRATEGY_CENTER_PRESSURE)
    {
        lane = nearest_center_lane(lane_goal);
    }
    else if (strategy == ATTACK_STRATEGY_FLANK_PRESSURE)
    {
        lane = nearest_flank_lane(lane_goal);
    }
    else if (strategy == ATTACK_STRATEGY_MIXED_LURE_STRIKE)
    {
        if (((s->killer_spawn_tick + k) & 0x1) == 0)
        {
            lane = nearest_flank_lane(lane_goal); /* lure shot */
        }
        else
        {
            lane = nearest_center_lane(lane_goal); /* strike shot */
        }
    }
    else if (strategy == ATTACK_STRATEGY_TERMINAL_SATURATION)
    {
        lane = lane_with_max_pressure_on_edge(s, edge, lane_goal);
    }

    return (edge * 4) + lane; /* 0..15 */
}

static float depth_zoom_factor_for_y(const drone_hunter_scene_t *s, float y)
{
    float min_y = (float)s->arena_y + 6.0f;
    float max_y = (float)(s->arena_y + s->arena_h) - 6.0f;
    float span = (max_y - min_y > 1.0f) ? (max_y - min_y) : 1.0f;
    float t = (y - min_y) / span;
    if (t < 0.0f)
    {
        t = 0.0f;
    }
    else if (t > 1.0f)
    {
        t = 1.0f;
    }

    /* Perspective curve: horizon drones are much smaller, near-screen drones are larger. */
    return 0.62f + (t * 0.66f);
}

static float clampf(float v, float lo, float hi)
{
    if (isnan(v))
    {
        return lo;
    }
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

static float combat_floor_y(const drone_hunter_scene_t *s)
{
    float floor_y = (float)(s->arena_y + s->arena_h) - 20.0f;

    if (s->deck_bar != NULL)
    {
        /* Keep all flight objects above the icon/deck strip. */
        float deck_top = (float)s->arena_y + (float)lv_obj_get_y(s->deck_bar) - 8.0f;
        if (deck_top < floor_y)
        {
            floor_y = deck_top;
        }
    }
    return floor_y;
}

static float dist2(float ax, float ay, float bx, float by)
{
    float dx = ax - bx;
    float dy = ay - by;
    return (dx * dx) + (dy * dy);
}

static float segment_point_dist2(float ax, float ay, float bx, float by, float px, float py)
{
    float vx = bx - ax;
    float vy = by - ay;
    float wx = px - ax;
    float wy = py - ay;
    float c1 = (vx * wx) + (vy * wy);
    if (c1 <= 0.0f)
    {
        return dist2(ax, ay, px, py);
    }
    {
        float c2 = (vx * vx) + (vy * vy);
        if (c2 <= c1)
        {
            return dist2(bx, by, px, py);
        }
        {
            float t = c1 / c2;
            float qx = ax + (vx * t);
            float qy = ay + (vy * t);
            return dist2(qx, qy, px, py);
        }
    }
}

static void set_obj_center(lv_obj_t *obj, float x, float y)
{
    int32_t w;
    int32_t h;
    int32_t px;
    int32_t py;

    if (isnan(x) || isnan(y))
    {
        return;
    }
    if (!isfinite(x) || !isfinite(y))
    {
        return;
    }
    w = lv_obj_get_width(obj);
    h = lv_obj_get_height(obj);
    if ((w < 1) || (w > 512))
    {
        w = 64;
    }
    if ((h < 1) || (h > 512))
    {
        h = 64;
    }
    px = (int32_t)(x - ((float)w * 0.5f));
    py = (int32_t)(y - ((float)h * 0.5f));
    if (px < -2048)
    {
        px = -2048;
    }
    else if (px > 4096)
    {
        px = 4096;
    }
    if (py < -2048)
    {
        py = -2048;
    }
    else if (py > 4096)
    {
        py = 4096;
    }
    lv_obj_set_pos(obj, px, py);
}

static void get_obj_visual_center_in_parent(lv_obj_t *obj, lv_obj_t *parent, float *out_x, float *out_y)
{
    lv_area_t obj_area;
    lv_area_t parent_area;
    float cx;
    float cy;

    lv_obj_get_coords(obj, &obj_area);
    lv_obj_get_coords(parent, &parent_area);
    cx = ((float)obj_area.x1 + (float)obj_area.x2 + 1.0f) * 0.5f;
    cy = ((float)obj_area.y1 + (float)obj_area.y2 + 1.0f) * 0.5f;

    if (out_x != NULL)
    {
        *out_x = cx - (float)parent_area.x1;
    }
    if (out_y != NULL)
    {
        *out_y = cy - (float)parent_area.y1;
    }
}

static void update_fixed_wing_orientation(drone_hunter_scene_t *s, int k)
{
    int16_t angle_tenth = (int16_t)(s->k_heading[k] * (1800.0f / PI_F)) + SPRITE_NOSE_FWD_TENTH;
    int32_t w = lv_obj_get_width(s->killers[k]);
    int32_t h = lv_obj_get_height(s->killers[k]);

    lv_obj_set_style_transform_pivot_x(s->killers[k], (int32_t)(w / 2), 0);
    lv_obj_set_style_transform_pivot_y(s->killers[k], (int32_t)(h / 2), 0);
    lv_obj_set_style_transform_angle(s->killers[k], angle_tenth, 0);
}

static float wrap_angle_pi(float a)
{
    if (!isfinite(a))
    {
        return 0.0f;
    }
    a = remainderf(a, 2.0f * PI_F);
    if (!isfinite(a))
    {
        return 0.0f;
    }
    if (a > PI_F)
    {
        a -= 2.0f * PI_F;
    }
    else if (a < -PI_F)
    {
        a += 2.0f * PI_F;
    }
    return a;
}

static int normalize_vec2(float *x, float *y)
{
    float m2 = (*x * *x) + (*y * *y);
    if (m2 <= 1.0e-6f)
    {
        return 0;
    }
    {
        float inv = 1.0f / sqrtf(m2);
        *x *= inv;
        *y *= inv;
    }
    return 1;
}

static int iff_tracks_merged(const drone_hunter_scene_t *s, float core_x, float core_y)
{
    float a0;
    float a1;
    float da;
    float r0;
    float r1;

    if (!s->killer_active[0] || !s->killer_active[1])
    {
        return 0;
    }
    a0 = atan2f(s->ky[0] - core_y, s->kx[0] - core_x);
    a1 = atan2f(s->ky[1] - core_y, s->kx[1] - core_x);
    da = fabsf(wrap_angle_pi(a0 - a1));
    r0 = sqrtf(dist2(s->kx[0], s->ky[0], core_x, core_y));
    r1 = sqrtf(dist2(s->kx[1], s->ky[1], core_x, core_y));
    return (da < 0.22f) && (fabsf(r0 - r1) < 56.0f);
}

static float depth_norm_for_y(const drone_hunter_scene_t *s, float y)
{
    return clampf((y - (float)s->arena_y) / clampf((float)s->arena_h, 1.0f, 5000.0f), 0.0f, 1.0f);
}

static int ciws_target_in_sweep(const drone_hunter_scene_t *s, float gun_x, float gun_y,
                                float sweep_rad, float sweep_pitch, float range_px,
                                float target_x, float target_y)
{
    float dx = target_x - gun_x;
    float dy = target_y - gun_y;
    float d2 = (dx * dx) + (dy * dy);
    float range2 = range_px * range_px;
    float target_ang;
    float d_ang;
    float target_pitch;
    float d_pitch;

    if (d2 < 1.0f || d2 > range2)
    {
        return 0;
    }

    target_ang = atan2f(dy, dx);
    d_ang = wrap_angle_pi(target_ang - sweep_rad);
    target_pitch = (0.5f - depth_norm_for_y(s, target_y)) * CIWS_SWEEP_PITCH_MAX;
    d_pitch = fabsf(target_pitch - sweep_pitch);
    return (fabsf(d_ang) <= CIWS_SWEEP_HALF_CONE) && (d_pitch <= CIWS_SWEEP_HALF_CONE_PITCH);
}

static float ciws_lock_quality(const drone_hunter_scene_t *s, int k,
                               float gun_x, float gun_y,
                               float sweep_rad, float sweep_pitch, float range_px)
{
    float dx = s->kx[k] - gun_x;
    float dy = s->ky[k] - gun_y;
    float d2 = (dx * dx) + (dy * dy);
    float d = sqrtf(d2);
    float range_q;
    float ang_target;
    float ang_err;
    float ang_q;
    float pitch_target;
    float pitch_q;
    float speed = sqrtf((s->kvx[k] * s->kvx[k]) + (s->kvy[k] * s->kvy[k]));
    float rnx;
    float rny;
    float lateral_q;
    float q;

    if (d < 1.0f || d > range_px)
    {
        return 0.0f;
    }

    range_q = 1.0f - clampf(d / range_px, 0.0f, 1.0f);
    ang_target = atan2f(dy, dx);
    ang_err = fabsf(wrap_angle_pi(ang_target - sweep_rad));
    ang_q = 1.0f - clampf(ang_err / CIWS_SWEEP_HALF_CONE, 0.0f, 1.0f);
    pitch_target = (0.5f - depth_norm_for_y(s, s->ky[k])) * CIWS_SWEEP_PITCH_MAX;
    pitch_q = 1.0f - clampf(fabsf(pitch_target - sweep_pitch) / CIWS_SWEEP_HALF_CONE_PITCH, 0.0f, 1.0f);

    rnx = dx / d;
    rny = dy / d;
    if (speed < 0.05f)
    {
        lateral_q = 0.70f;
    }
    else
    {
        float lateral_ratio = fabsf((s->kvx[k] * rny) - (s->kvy[k] * rnx)) / speed;
        lateral_q = 1.0f - clampf(lateral_ratio, 0.0f, 1.0f);
    }

    q = (range_q * 0.28f) + (ang_q * 0.38f) + (pitch_q * 0.18f) + (lateral_q * 0.16f);
    return clampf(q, 0.0f, 1.0f);
}

static int ciws_fire_at(drone_hunter_scene_t *s, int k, float gun_x, float gun_y,
                        float sweep_rad, float sweep_pitch, float range_px, float lock_q, float *cooldown_sec, int *ammo)
{
    float dir_x = cosf(sweep_rad);
    float dir_y = sinf(sweep_rad) - (sweep_pitch * 0.50f);
    float d_px = sqrtf(dist2(gun_x, gun_y, s->kx[k], s->ky[k]));
    float effective_px = range_px * 0.75f;
    float p_ciws = (s->k_tier[k] == 0) ? 0.88f : ((s->k_tier[k] == 1) ? 0.74f : 0.58f);
    float roll = 0.5f + (0.5f * sinf((s->t * 3.7f) + (float)(k * 5 + s->wave_idx)));
    float heat_penalty = (*cooldown_sec > 0.015f) ? ((*cooldown_sec - 0.015f) * 12.0f) : 0.0f;
    int ammo_spent = CIWS_AMMO_PER_TRIGGER;

    (void)normalize_vec2(&dir_x, &dir_y);

    p_ciws -= heat_penalty;
    if (d_px > effective_px)
    {
        /* Outside effective range but inside hard cutoff: mostly wastes ammo. */
        p_ciws *= 0.22f;
    }
    p_ciws *= clampf(0.22f + (lock_q * 0.95f), 0.08f, 1.0f);
    p_ciws = clampf(p_ciws, 0.02f, 0.95f);

    if (*ammo <= 0)
    {
        return 0;
    }
    if (d_px > range_px)
    {
        return 0;
    }

    if (ammo_spent > *ammo)
    {
        ammo_spent = *ammo;
    }

    *cooldown_sec += CIWS_FIRE_COOLDOWN_SEC;
    if (lock_q < CIWS_LOCK_BAD_THRESH)
    {
        /* Poor lead/geometry still burns barrel time and reaction budget. */
        *cooldown_sec += (CIWS_LOCK_BAD_THRESH - lock_q) * 0.10f;
    }
    if (d_px > effective_px)
    {
        /* Out-of-envelope shots further slow follow-up response. */
        *cooldown_sec += 0.045f;
    }
    *ammo -= ammo_spent;
    s->ciws_shots++;
    if ((d_px > effective_px) || (lock_q < CIWS_LOCK_BAD_THRESH))
    {
        if ((s->ciws_shots % 8) == 0)
        {
            note_failure(s, "CIWS misuse: out-of-envelope/low-lock burst", &s->fail_ciws_misuse);
        }
    }
    {
        int burst;
        float horizontal = fabsf(dir_x);
        float gravity = 380.0f + (horizontal * 1800.0f);
        float half_span = ((float)CIWS_BURST_PELLETS - 1.0f) * 0.5f;

        for (burst = 0; burst < CIWS_BURST_PELLETS; ++burst)
        {
            int ti = s->ciws_tracer_head % CIWS_TRACER_COUNT;
            float target_depth = clampf(depth_zoom_factor_for_y(s, s->ky[k]), 0.64f, 1.30f);
            float spread = ((((float)burst - half_span) * 0.008f) + (((float)((s->ciws_shots + burst) % 3) - 1.0f) * 0.003f)) *
                           (1.18f - (target_depth * 0.28f));
            float speed = (2200.0f + ((float)(burst * 65)) + ((float)(s->ciws_shots % 4) * 50.0f)) *
                          (0.84f + (target_depth * 0.22f));
            float tracer_life = clampf((range_px - 24.0f) / speed, 0.09f, CIWS_TRACER_LIFE_SEC);

            s->ciws_tracer_head++;
            s->ciws_tracer_t[ti] = tracer_life;
            s->ciws_tracer_x0[ti] = gun_x + (dir_x * 6.0f);
            s->ciws_tracer_y0[ti] = gun_y + (dir_y * 6.0f);
            s->ciws_tracer_x1[ti] = (dir_x - (dir_y * spread)) * speed;
            s->ciws_tracer_y1[ti] = (dir_y + (dir_x * spread)) * speed;
            s->ciws_tracer_g[ti] = gravity;
            s->ciws_tracer_scale[ti] = target_depth;
        }
    }
    /* Avoid deck-bar flicker by throttling ammo UI refresh under high ROF CIWS fire. */
    if (((s->ciws_shots % 6) == 0) || (*ammo <= 0))
    {
        update_hunter_deck_ui(s);
    }

    if (roll <= p_ciws)
    {
        float hit_x;
        float hit_y;
        int shahed_kill = target_is_shahed_visual(s, k);
        int blast_style = target_blast_style_for_visual(s, k);

        get_obj_visual_center_in_parent(s->killers[k], s->arena, &hit_x, &hit_y);
        s->attack_destroyed++;
        s->defense_kills++;
        s->hunter_points++;
        s->ciws_kills++;
        s->fx_blast_style[k] = blast_style;
        s->fx_intercept_t[k] = FX_INTERCEPT_SEC;
        s->fx_intercept_x[k] = hit_x;
        s->fx_intercept_y[k] = hit_y;
        s->fx_kill_t[k] = shahed_kill ? (FX_KILL_SEC * 1.40f) : FX_KILL_SEC;
        s->fx_kill_x[k] = hit_x;
        s->fx_kill_y[k] = hit_y;
        set_diag_stage(s, shahed_kill ? "DBG:CIWS_KILL_SHAHED" : "DBG:CIWS_KILL_OTHER");
        if (shahed_kill)
        {
            s->k_dying[k] = 1;
            s->k_dying_t[k] = FX_SHAHED_DEATH_SEC;
            s->k_threat_score[k] = 0.0f;
            s->k_commit_ok[k] = 0;
            s->k_detect_ok[k] = 0;
            s->k_class_ok[k] = 0;
            s->k_corridor_ok[k] = 0;
            s->k_los_ok[k] = 0;
            s->kvx[k] = 0.0f;
            s->kvy[k] = 0.0f;
            s->k_heading[k] = -PI_F * 0.5f;
        }
        else
        {
            set_diag_stage(s, "DBG:CIWS_RESPAWN");
            set_killer_hidden(s, k);
            respawn_killer(s, k, -1);
        }
        return 1;
    }
    return 0;
}

static const char *ctrl_name(controller_t c)
{
    return (c == CTRL_EDGEAI) ? "EdgeAI" : "Algo";
}

static const char *ctrl_name_compact(controller_t c)
{
    return (c == CTRL_EDGEAI) ? "EDGEAI" : "ALGO";
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

static void update_mode_button_label(drone_hunter_scene_t *s)
{
    if (s->mode_btn_label != NULL)
    {
        lv_label_set_text(s->mode_btn_label, "SET   |   HELP");
    }
}

static const char *difficulty_name(difficulty_t d)
{
    switch (d)
    {
        case DIFFICULTY_EASY: return "EASY";
        case DIFFICULTY_HARD: return "HARD";
        case DIFFICULTY_MEDIUM:
        default: return "MED";
    }
}

static const char *speed_pp_name(int idx)
{
    switch (idx)
    {
        case 2: return "FAST++";
        case 1: return "FAST+";
        default: return "NORMAL";
    }
}

static float speed_pp_mult(const drone_hunter_scene_t *s)
{
    return (s->speed_pp_idx == 2) ? 1.35f : ((s->speed_pp_idx == 1) ? 1.18f : 1.00f);
}

static float difficulty_attack_mult(const drone_hunter_scene_t *s)
{
    if (s->difficulty_sel == DIFFICULTY_EASY)
    {
        return 0.90f;
    }
    if (s->difficulty_sel == DIFFICULTY_HARD)
    {
        return 1.12f;
    }
    return 1.00f;
}

static float difficulty_defense_mult(const drone_hunter_scene_t *s)
{
    if (s->difficulty_sel == DIFFICULTY_EASY)
    {
        return 1.08f;
    }
    if (s->difficulty_sel == DIFFICULTY_HARD)
    {
        return 0.92f;
    }
    return 1.00f;
}

static void apply_control_settings(drone_hunter_scene_t *s)
{
    s->team_ctrl[1] = s->attacker_mode_sel;
    if (s->defender_mode_sel == DEFENDER_MODE_EDGEAI)
    {
        s->team_ctrl[0] = CTRL_EDGEAI;
    }
    else
    {
        s->team_ctrl[0] = CTRL_ALGO;
    }

    if ((s->team_ctrl[0] == CTRL_ALGO) && (s->team_ctrl[1] == CTRL_ALGO))
    {
        s->mode = MODE_ALGO_VS_ALGO;
    }
    else if ((s->team_ctrl[0] == CTRL_EDGEAI) && (s->team_ctrl[1] == CTRL_EDGEAI))
    {
        s->mode = MODE_EDGEAI_VS_EDGEAI;
    }
    else
    {
        s->mode = MODE_ALGO_VS_EDGEAI;
    }
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
    {"Stinger",       2.20f, 0.020f, 0.66f, 1.55f, 14.8f, 1, 3, 32, 20, 18, 7, 15, 4, 10, 5, 0xD1D5DB, 0x9CA3AF, "Stinger is tuned for rapid intercepts against fast threats. It reaches speed quickly and keeps a stable nose-in pursuit path."},
    {"Bagnet",        1.68f, 0.016f, 0.72f, 1.10f, 13.8f, 2, 2, 30, 20, 16, 7, 14, 4,  9, 5, 0xD4D4D8, 0xA1A1AA, "Bagnet is a close-range hunter built for tight FPV engagements. It favors control and positioning over pure top speed."},
    {"Skyfall P1",    2.50f, 0.023f, 0.62f, 1.70f, 14.2f, 1, 3, 34, 20, 20, 6, 15, 3, 10, 4, 0xE5E7EB, 0x9CA3AF, "Skyfall P1 is optimized for predictive lead pursuit. It is one of the fastest options in the lineup for fixed-wing intercepts."},
    {"Octopus-100",   1.98f, 0.018f, 0.70f, 1.30f, 14.5f, 2, 2, 32, 20, 17, 7, 16, 4, 10, 5, 0xD6D3D1, 0xA8A29E, "Octopus-100 is a balanced multirotor platform for mixed threat environments. It trades extremes for consistent all-round behavior."},
    {"ODIN Win_Hit",  2.05f, 0.019f, 0.68f, 1.25f, 15.4f, 2, 2, 33, 21, 18, 8, 15, 4, 10, 5, 0xCBD5E1, 0x94A3B8, "ODIN Win_Hit emphasizes stable tracking and reliable direct-hit geometry. It performs well when holding line and avoiding overshoot."},
    {"Pelican",       1.10f, 0.015f, 0.75f, 1.05f, 16.4f, 3, 1, 36, 18, 20, 6, 18, 3,  8, 4, 0xE7E5E4, 0xA8A29E, "Pelican uses a larger frame for close interception pressure. It is resilient in dense engagements and favors high-contact scoring."},
    {"TYTAN",         1.50f, 0.021f, 0.64f, 1.50f, 14.6f, 1, 3, 34, 20, 19, 7, 15, 4, 10, 5, 0xE5E7EB, 0x9CA3AF, "TYTAN is an aggressive fixed-wing hunter with strong approach speed. It is suited for decisive first-pass intercept attempts."},
    {"Merops",        1.86f, 0.018f, 0.69f, 1.20f, 15.2f, 2, 2, 35, 20, 19, 7, 16, 4, 10, 5, 0xD6D3D1, 0xA8A29E, "Merops is a general-purpose interceptor for variable weather and threat mixes. It balances predictability, control, and endurance."}
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
        default:                     return &img_hunter_sting_ii_detailed;
    }
}

static const lv_image_dsc_t *hunter_icon_src(hunter_type_t type)
{
    if (type == HUNTER_STING_II)
    {
        return &img_hunter_sting_ii;
    }
    return hunter_image_src(type);
}

static const char *hunter_type_short_name(hunter_type_t t)
{
    switch (t)
    {
        case HUNTER_STING_II: return "Stinger";
        case HUNTER_BAGNET: return "Bagnet";
        case HUNTER_SKYFALL_P1: return "Skyfall";
        case HUNTER_OCTOPUS_100: return "Octopus";
        case HUNTER_ODIN_WIN_HIT: return "ODIN";
        case HUNTER_VB140_FLAMINGO: return "Pelican";
        case HUNTER_TYTAN: return "TYTAN";
        case HUNTER_MEROPS: return "Merops";
        default: return "Hunter";
    }
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

static void init_launch_sectors(drone_hunter_scene_t *s)
{
    int i;
    float cx = (float)s->arena_x + ((float)s->arena_w * 0.5f);
    float cy = (float)s->arena_y + ((float)s->arena_h * 0.66f);
    float rx = (float)s->arena_w * 0.34f;
    float ry = (float)s->arena_h * 0.20f;
    int per_sector = (HUNTER_STOCK_PER_TYPE * HUNTER_TYPE_COUNT) / LAUNCH_SECTOR_COUNT;

    for (i = 0; i < LAUNCH_SECTOR_COUNT; ++i)
    {
        float a = (-PI_F * 0.5f) + (((float)i / (float)LAUNCH_SECTOR_COUNT) * (2.0f * PI_F));
        s->launch_sector_x[i] = cx + (cosf(a) * rx);
        s->launch_sector_y[i] = cy + (sinf(a) * ry);
        s->launch_sector_stock[i] = per_sector;
    }
}

static int choose_launch_sector_for_target(const drone_hunter_scene_t *s, float tx, float ty)
{
    int pass;
    int used[LAUNCH_SECTOR_COUNT] = {0};

    for (pass = 0; pass < LAUNCH_SECTOR_COUNT; ++pass)
    {
        int i;
        int best = -1;
        float best_d2 = 1.0e30f;
        for (i = 0; i < LAUNCH_SECTOR_COUNT; ++i)
        {
            float d2;
            if (used[i])
            {
                continue;
            }
            d2 = dist2(tx, ty, s->launch_sector_x[i], s->launch_sector_y[i]);
            if (d2 < best_d2)
            {
                best_d2 = d2;
                best = i;
            }
        }
        if (best < 0)
        {
            break;
        }
        used[best] = 1;
        if (s->launch_sector_stock[best] > 0)
        {
            return best;
        }
    }
    return -1;
}

static float hunter_regroup_x(const drone_hunter_scene_t *s, int h, float core_x)
{
    int sec = s->h_launch_sector[h];
    if ((sec >= 0) && (sec < LAUNCH_SECTOR_COUNT))
    {
        return s->launch_sector_x[sec];
    }
    return core_x + ((h == 0) ? -48.0f : 48.0f);
}

static float hunter_regroup_y(const drone_hunter_scene_t *s, int h)
{
    int sec = s->h_launch_sector[h];
    if ((sec >= 0) && (sec < LAUNCH_SECTOR_COUNT))
    {
        return s->launch_sector_y[sec];
    }
    return (float)s->arena_y + (float)s->arena_h - 22.0f;
}

static float hunter_capability_score(hunter_type_t t)
{
    const hunter_profile_t *p = &g_hunter_profiles[(int)t];
    return (p->speed * 0.75f) + (p->lead_gain * 0.85f) + (p->kill_radius * 0.05f);
}

static float hunter_range_km(hunter_type_t t)
{
    switch (t)
    {
        case HUNTER_STING_II:       return 25.0f;
        case HUNTER_BAGNET:         return 15.0f;
        case HUNTER_SKYFALL_P1:     return 23.0f;
        case HUNTER_OCTOPUS_100:    return 30.0f;
        case HUNTER_ODIN_WIN_HIT:   return 18.0f;
        case HUNTER_VB140_FLAMINGO: return 50.0f;
        case HUNTER_TYTAN:          return 35.0f;
        case HUNTER_MEROPS:         return 30.0f;
        default:                    return 20.0f;
    }
}

static int hunter_is_low_alt_mismatch(hunter_type_t h, target_type_t tt, float altitude_norm)
{
    if (tt != TARGET_FPV)
    {
        return 0;
    }
    if (altitude_norm > 0.26f)
    {
        return 0;
    }
    return (h == HUNTER_VB140_FLAMINGO) || (h == HUNTER_TYTAN) || (h == HUNTER_MEROPS);
}

static float hunter_turn_rate_limit(hunter_type_t h, controller_t ctrl)
{
    float base = 0.045f;
    switch (h)
    {
        case HUNTER_STING_II:
        case HUNTER_SKYFALL_P1:
            base = 0.068f;
            break;
        case HUNTER_OCTOPUS_100:
        case HUNTER_ODIN_WIN_HIT:
            base = 0.058f;
            break;
        case HUNTER_BAGNET:
        case HUNTER_MEROPS:
            base = 0.050f;
            break;
        case HUNTER_TYTAN:
            base = 0.046f;
            break;
        case HUNTER_VB140_FLAMINGO:
        default:
            base = 0.040f;
            break;
    }
    if (ctrl == CTRL_EDGEAI)
    {
        base += 0.010f;
    }
    return clampf(base, 0.030f, 0.090f);
}

static float hunter_guidance_speed_mult(hunter_type_t h, controller_t ctrl)
{
    float base = (ctrl == CTRL_EDGEAI) ? 1.90f : 1.72f;
    switch (h)
    {
        case HUNTER_STING_II:
        case HUNTER_SKYFALL_P1:
            base += 0.08f;
            break;
        case HUNTER_BAGNET:
        case HUNTER_MEROPS:
            base += 0.02f;
            break;
        case HUNTER_VB140_FLAMINGO:
            base -= 0.10f;
            break;
        default:
            break;
    }
    return clampf(base, 1.52f, 2.05f);
}

static float hunter_guidance_lead_frames(hunter_type_t h, controller_t ctrl, float lead_gain)
{
    float lead = 8.0f + (11.0f * lead_gain);
    switch (h)
    {
        case HUNTER_STING_II:
        case HUNTER_SKYFALL_P1:
            lead += 1.8f;
            break;
        case HUNTER_VB140_FLAMINGO:
            lead -= 1.4f;
            break;
        default:
            break;
    }
    if (ctrl != CTRL_EDGEAI)
    {
        lead *= 0.62f;
    }
    return clampf(lead, 4.0f, 26.0f);
}

static hunter_flight_model_t hunter_flight_model(hunter_type_t h)
{
    switch (h)
    {
        case HUNTER_BAGNET:
        case HUNTER_ODIN_WIN_HIT:
            return HUNTER_FLIGHT_VERTICAL;
        case HUNTER_OCTOPUS_100:
            /* Keep Octopus on stable fixed-wing steering to avoid visible weave. */
            return HUNTER_FLIGHT_PLANE;
        case HUNTER_STING_II:
        case HUNTER_SKYFALL_P1:
        case HUNTER_VB140_FLAMINGO:
        case HUNTER_TYTAN:
        case HUNTER_MEROPS:
        default:
            return HUNTER_FLIGHT_PLANE;
    }
}

static void steer_hunter_toward_target(drone_hunter_scene_t *s, int h, int target, const hunter_profile_t *p)
{
    float tx;
    float ty;
    float dist_to_pred;
    float desired;
    float current;
    float delta;
    float turn_limit = hunter_turn_rate_limit(s->h_type[h], s->team_ctrl[h]);
    float speed_now = sqrtf((s->hvx[h] * s->hvx[h]) + (s->hvy[h] * s->hvy[h]));
    float speed_goal = p->speed *
                       hunter_guidance_speed_mult(s->h_type[h], s->team_ctrl[h]) *
                       difficulty_defense_mult(s) *
                       speed_pp_mult(s);
    float speed;
    hunter_flight_model_t fm = hunter_flight_model(s->h_type[h]);
    float lead_frames = hunter_guidance_lead_frames(s->h_type[h], s->team_ctrl[h], p->lead_gain);
    tx = s->kx[target] + (s->kvx[target] * lead_frames);
    ty = s->ky[target] + (s->kvy[target] * lead_frames);
    dist_to_pred = sqrtf(dist2(s->hx[h], s->hy[h], tx, ty));
    desired = atan2f(ty - s->hy[h], tx - s->hx[h]);

    if (fm == HUNTER_FLIGHT_VERTICAL)
    {
        float nx = tx - s->hx[h];
        float ny = ty - s->hy[h];
        float accel = 0.34f;
        float speed_cap = speed_goal * 1.04f;
        float speed_floor = 0.22f;

        if ((nx * nx) + (ny * ny) > 0.0001f)
        {
            (void)normalize_vec2(&nx, &ny);
        }
        if (dist_to_pred < (p->kill_radius * 4.2f))
        {
            speed_cap *= (dist_to_pred < (p->kill_radius * 2.6f)) ? 0.80f : 0.90f;
            accel = 0.40f;
        }
        s->hvx[h] = (s->hvx[h] * (1.0f - accel)) + (nx * speed_cap * accel);
        s->hvy[h] = (s->hvy[h] * (1.0f - accel)) + (ny * speed_cap * accel);
        speed = sqrtf((s->hvx[h] * s->hvx[h]) + (s->hvy[h] * s->hvy[h]));
        if (speed > speed_cap)
        {
            float scale = speed_cap / clampf(speed, 0.001f, 1000.0f);
            s->hvx[h] *= scale;
            s->hvy[h] *= scale;
        }
        else if (speed < speed_floor)
        {
            s->hvx[h] += nx * 0.08f;
            s->hvy[h] += ny * 0.08f;
        }
        return;
    }

    current = ((speed_now > 0.02f) ? atan2f(s->hvy[h], s->hvx[h]) : s->h_heading[h]);
    delta = wrap_angle_pi(desired - current);
    if (fm == HUNTER_FLIGHT_PLANE)
    {
        turn_limit *= 0.86f;
    }
    else if ((fm == HUNTER_FLIGHT_HYBRID) && (dist_to_pred < (p->kill_radius * 5.5f)))
    {
        float nx = tx - s->hx[h];
        float ny = ty - s->hy[h];
        float blend = 0.28f;
        float h_speed = speed_goal * 0.90f;
        if ((nx * nx) + (ny * ny) > 0.0001f)
        {
            (void)normalize_vec2(&nx, &ny);
        }
        s->hvx[h] = (s->hvx[h] * (1.0f - blend)) + (nx * h_speed * blend);
        s->hvy[h] = (s->hvy[h] * (1.0f - blend)) + (ny * h_speed * blend);
        return;
    }
    delta = clampf(delta, -turn_limit, turn_limit);
    speed = speed_now + ((speed_goal - speed_now) * 0.20f);
    if (dist_to_pred < (p->kill_radius * 5.0f))
    {
        /* Reduce terminal overshoot while preserving turn authority near intercept. */
        speed *= (dist_to_pred < (p->kill_radius * 3.0f)) ? 0.82f : 0.90f;
    }
    if (fm == HUNTER_FLIGHT_PLANE)
    {
        speed = clampf(speed, speed_goal * 0.55f, speed_goal * 1.08f);
    }
    else
    {
        speed = clampf(speed, 0.35f, speed_goal * 1.08f);
    }
    current += delta;
    s->hvx[h] = cosf(current) * speed;
    s->hvy[h] = sinf(current) * speed;
}

static int choose_hunter_reacquire_target(const drone_hunter_scene_t *s, int h, int previous_target)
{
    int k;
    int best = -1;
    float best_score = -1.0e30f;
    for (k = 0; k < KILLER_COUNT; ++k)
    {
        float d2;
        float proximity_q;
        float score;
        if ((k == previous_target) || !s->killer_active[k] || s->k_dying[k])
        {
            continue;
        }
        if (target_has_assigned_hunter(s, k))
        {
            continue;
        }
        d2 = dist2(s->hx[h], s->hy[h], s->kx[k], s->ky[k]);
        if (d2 > ((float)s->arena_w * 0.65f * (float)s->arena_w * 0.65f))
        {
            continue;
        }
        proximity_q = 1.0f - clampf(sqrtf(d2) / clampf((float)s->arena_w, 20.0f, 4000.0f), 0.0f, 1.0f);
        score = (s->k_threat_score[k] * 0.68f) +
                (proximity_q * 0.24f) +
                (s->k_commit_conf[k] * 0.08f);
        if (score > best_score)
        {
            best_score = score;
            best = k;
        }
    }
    return best;
}

static int choose_hunter_opportunistic_target(const drone_hunter_scene_t *s, int h, int committed, const hunter_profile_t *p)
{
    int k;
    int best = -1;
    float best_score = -1.0e30f;
    controller_t ctrl = s->team_ctrl[h];
    float speed = sqrtf((s->hvx[h] * s->hvx[h]) + (s->hvy[h] * s->hvy[h]));
    float heading = (speed > 0.05f) ? atan2f(s->hvy[h], s->hvx[h]) : s->h_heading[h];
    float lookahead = clampf(speed * 18.0f, 12.0f, 56.0f);
    float seg_bx = s->hx[h] + (cosf(heading) * lookahead);
    float seg_by = s->hy[h] + (sinf(heading) * lookahead);
    float eta_locked = 999.0f;
    float cap = hunter_capability_score(s->h_type[h]);
    float req_locked = threat_required_score(s, committed);
    float p_locked = clampf(0.34f + ((cap - req_locked) * 0.22f) - s->h_launch_penalty[h], 0.05f, 0.95f);
    float path_gate = p->kill_radius * ((ctrl == CTRL_EDGEAI) ? 2.7f : 2.2f);
    float easy_gate = (ctrl == CTRL_EDGEAI) ? 0.60f : 0.72f;
    float eta_gain_gate = (ctrl == CTRL_EDGEAI) ? -0.10f : 0.25f;
    float commit_urgency = s->k_eta_to_goal[committed];

    if ((committed < 0) || !s->killer_active[committed] || s->k_dying[committed])
    {
        return -1;
    }
    speed = clampf(speed, 0.45f, 4.0f);
    eta_locked = sqrtf(dist2(s->hx[h], s->hy[h], s->kx[committed], s->ky[committed])) / speed;

    for (k = 0; k < KILLER_COUNT; ++k)
    {
        float eta_alt;
        float eta_gain;
        float req_alt;
        float p_alt;
        float d2_path;
        float ang_alt;
        float turn_need;
        float score;
        int in_path;
        int easy;
        int low_abandon_risk;
        if ((k == committed) || !s->killer_active[k] || s->k_dying[k])
        {
            continue;
        }
        if (target_has_assigned_hunter(s, k))
        {
            continue;
        }
        eta_alt = sqrtf(dist2(s->hx[h], s->hy[h], s->kx[k], s->ky[k])) / speed;
        eta_gain = eta_locked - eta_alt;
        req_alt = threat_required_score(s, k);
        p_alt = clampf(0.34f + ((cap - req_alt) * 0.22f) - s->h_launch_penalty[h], 0.05f, 0.95f);
        d2_path = segment_point_dist2(s->hx[h], s->hy[h], seg_bx, seg_by, s->kx[k], s->ky[k]);
        in_path = (d2_path < (path_gate * path_gate));
        easy = (p_alt >= easy_gate);
        low_abandon_risk = (commit_urgency > 2.0f) || in_path || (p_alt > (p_locked + 0.12f));
        if (!low_abandon_risk)
        {
            continue;
        }
        if (!in_path && !(easy && (eta_gain > eta_gain_gate)))
        {
            continue;
        }
        ang_alt = atan2f(s->ky[k] - s->hy[h], s->kx[k] - s->hx[h]);
        turn_need = fabsf(wrap_angle_pi(ang_alt - heading));
        score = 0.0f;
        score += (in_path ? 1.25f : 0.0f);
        score += (easy ? 0.68f : 0.0f);
        score += eta_gain * 0.52f;
        score += (p_alt - p_locked) * 1.20f;
        score += s->k_threat_score[k] * 0.26f;
        score -= turn_need * 0.55f;
        if (score > best_score)
        {
            best_score = score;
            best = k;
        }
    }
    return best;
}

static void note_failure(drone_hunter_scene_t *s, const char *txt, int *counter)
{
    if (counter != NULL)
    {
        (*counter)++;
    }
    s->fail_reason_ttl = 3.6f;
    (void)snprintf(s->fail_reason_text, sizeof(s->fail_reason_text), "%s", txt);
}

static float attack_speed_score(const drone_hunter_scene_t *s, int k)
{
    float speed;
    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        /* Shahed: 150-190 class, Strike-Prop: 100-150 class. */
        speed = (s->threat_faction == THREAT_FACTION_RUSSIA) ? 1.60f : 1.35f;
    }
    else
    {
        /* Strike-X: lower speed but highly evasive near terrain. */
        speed = 1.30f;
    }
    return speed + (0.12f * (float)s->k_tier[k]);
}

static float threat_required_score(const drone_hunter_scene_t *s, int k)
{
    float base = 1.40f + (attack_speed_score(s, k) * 0.90f) + ((float)s->k_tier[k] * 0.28f);
    float conf_penalty = 1.0f - clampf((s->k_detect_conf[k] + s->k_class_conf[k]) * 0.5f, 0.0f, 1.0f);
    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        base += (s->threat_faction == THREAT_FACTION_RUSSIA) ? 0.24f : 0.12f;
    }
    else
    {
        base += 0.26f; /* Evasion burden for Strike-X swarms. */
    }
    base += (s->k_threat_score[k] * 0.22f);
    base += (conf_penalty * 0.35f);
    return base;
}

static const char *target_type_name(const drone_hunter_scene_t *s, int k)
{
    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        return (s->threat_faction == THREAT_FACTION_RUSSIA) ? "Shahed" : "Strike-Prop";
    }
    return "Strike-X";
}

static int target_is_shahed(const drone_hunter_scene_t *s, int k)
{
    return (s->ktype[k] == TARGET_FIXED_WING) && (s->threat_faction == THREAT_FACTION_RUSSIA);
}

static hunter_type_t recommended_hunter_for_target(const drone_hunter_scene_t *s, int k)
{
    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        if (s->threat_faction == THREAT_FACTION_RUSSIA)
        {
            /* Shahed class */
            return HUNTER_SKYFALL_P1;
        }
        /* Strike-Prop class */
        return HUNTER_MEROPS;
    }
    return HUNTER_STING_II;
}

static int hunter_is_high_speed_tier(hunter_type_t h)
{
    return (h == HUNTER_SKYFALL_P1) ||
           (h == HUNTER_STING_II) ||
           (h == HUNTER_ODIN_WIN_HIT) ||
           (h == HUNTER_OCTOPUS_100);
}

static int target_has_assigned_hunter(const drone_hunter_scene_t *s, int target)
{
    int h;
    for (h = 0; h < HUNTER_COUNT; ++h)
    {
        if (s->hunter_loaded[h] && (s->h_target_idx[h] == target))
        {
            return 1;
        }
    }
    return 0;
}

static int has_unassigned_higher_threat(const drone_hunter_scene_t *s, int target)
{
    int k;
    float t = s->k_threat_score[target];
    for (k = 0; k < KILLER_COUNT; ++k)
    {
        if ((k == target) || !s->killer_active[k])
        {
            continue;
        }
        if (target_has_assigned_hunter(s, k))
        {
            continue;
        }
        if (s->k_threat_score[k] > (t + 0.22f))
        {
            return 1;
        }
    }
    return 0;
}

static hunter_type_t best_stocked_hunter_type(const drone_hunter_scene_t *s)
{
    int i;
    int best_stock = 0;
    hunter_type_t best = HUNTER_STING_II;
    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        if (s->hunter_stock[i] > best_stock)
        {
            best_stock = s->hunter_stock[i];
            best = (hunter_type_t)i;
        }
    }
    return best;
}

static float hunter_pairing_bias(const drone_hunter_scene_t *s, hunter_type_t h, int k)
{
    float b = 0.0f;
    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        if (s->threat_faction == THREAT_FACTION_RUSSIA)
        {
            /* Shahed: Sting/ODIN/Skyfall/Bagnet/Octopus best; Flamingo poor terminal. */
            if ((h == HUNTER_SKYFALL_P1) || (h == HUNTER_STING_II) || (h == HUNTER_ODIN_WIN_HIT))
            {
                b += 0.58f;
            }
            else if ((h == HUNTER_BAGNET) || (h == HUNTER_OCTOPUS_100))
            {
                b += 0.36f;
            }
            else if (h == HUNTER_VB140_FLAMINGO)
            {
                b -= 0.60f;
            }
            else if (h == HUNTER_TYTAN)
            {
                b -= 0.18f;
            }
        }
        else
        {
            /* Strike-Prop: Octopus/Merops/Tytan/Flamingo preferred. */
            if ((h == HUNTER_OCTOPUS_100) || (h == HUNTER_MEROPS))
            {
                b += 0.52f;
            }
            else if ((h == HUNTER_TYTAN) || (h == HUNTER_VB140_FLAMINGO))
            {
                b += 0.38f;
            }
            else if (h == HUNTER_BAGNET)
            {
                b -= 0.16f;
            }
        }
    }
    else
    {
        /* Strike-X: Sting + Octopus best; fixed-wing terminal and Bagnet weaker. */
        if (h == HUNTER_STING_II)
        {
            b += 0.64f;
        }
        else if (h == HUNTER_OCTOPUS_100)
        {
            b += 0.50f;
        }
        else if (h == HUNTER_ODIN_WIN_HIT)
        {
            b += 0.12f;
        }
        else if (h == HUNTER_BAGNET)
        {
            b -= 0.32f;
        }
        else if ((h == HUNTER_VB140_FLAMINGO) || (h == HUNTER_TYTAN) || (h == HUNTER_MEROPS))
        {
            b -= 0.48f;
        }
    }

    /* Altitude mismatch penalty for fixed-wing vs very low erratic targets. */
    if ((s->ktype[k] == TARGET_FPV) &&
        (s->k_altitude_norm[k] < 0.25f) &&
        ((h == HUNTER_VB140_FLAMINGO) || (h == HUNTER_TYTAN) || (h == HUNTER_MEROPS)))
    {
        b -= 0.24f;
    }

    return b;
}

static float attack_speed_kmh_est(const drone_hunter_scene_t *s, int k)
{
    float base;
    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        base = (s->threat_faction == THREAT_FACTION_RUSSIA) ? 175.0f : 130.0f;
    }
    else
    {
        base = 95.0f;
    }
    return base + ((float)s->k_tier[k] * 18.0f);
}

static void refresh_target_metrics(drone_hunter_scene_t *s, float core_x, float core_y)
{
    int i;
    int k;

    for (i = 0; i < LANE_SITE_COUNT; ++i)
    {
        s->lane_pressure[i] = clampf((s->lane_pressure[i] * 0.985f) - 0.004f, 0.0f, 8.0f);
    }
    for (k = 0; k < KILLER_COUNT; ++k)
    {
        if (s->killer_active[k] && !s->k_dying[k])
        {
            int site = s->k_spawn_site[k];
            int edge;
            int lane;
            float add;

            if (site < 0 || site >= LANE_SITE_COUNT)
            {
                site = choose_spawn_site(s, k, s->k_goal_x[k], s->k_goal_y[k]);
            }
            edge = lane_site_edge(site);
            lane = lane_site_lane(site);
            add = 0.12f + (s->k_payload_score[k] * 0.16f) + (s->k_survivability[k] * 0.10f);
            s->lane_pressure[site] = clampf(s->lane_pressure[site] + add, 0.0f, 8.0f);
            if (lane > 0)
            {
                int left_site = (edge * 4) + (lane - 1);
                s->lane_pressure[left_site] = clampf(s->lane_pressure[left_site] + (add * 0.32f), 0.0f, 8.0f);
            }
            if (lane < 3)
            {
                int right_site = (edge * 4) + (lane + 1);
                s->lane_pressure[right_site] = clampf(s->lane_pressure[right_site] + (add * 0.32f), 0.0f, 8.0f);
            }
        }
    }

    for (k = 0; k < KILLER_COUNT; ++k)
    {
        if (!s->killer_active[k] || s->k_dying[k])
        {
            s->k_speed_est[k] = 0.0f;
            s->k_altitude_norm[k] = 0.0f;
            s->k_range_to_core[k] = 0.0f;
            s->k_eta_to_goal[k] = 0.0f;
            s->k_priority_rank[k] = 0;
            s->k_target_value_mod[k] = 0.0f;
            s->k_lane_pressure[k] = 0.0f;
            s->k_recommended_counter[k] = HUNTER_STING_II;
            s->k_detect_ok[k] = 0;
            s->k_class_ok[k] = 0;
            s->k_commit_ok[k] = 0;
            s->k_corridor_ok[k] = 0;
            s->k_los_ok[k] = 0;
            s->k_commit_conf[k] = 0.0f;
            s->k_track_history[k] = 0.0f;
            s->k_noise[k] = 0.0f;
            s->k_commit_reason[k] = COMMIT_REASON_NO_TARGET;
            continue;
        }

        s->k_speed_est[k] = attack_speed_kmh_est(s, k);
        s->k_altitude_norm[k] = 1.0f - clampf((s->ky[k] - (float)s->arena_y) / clampf((float)s->arena_h, 1.0f, 4000.0f), 0.0f, 1.0f);
        s->k_range_to_core[k] = sqrtf(dist2(s->kx[k], s->ky[k], core_x, core_y));
        s->k_eta_to_goal[k] = sqrtf(dist2(s->kx[k], s->ky[k], s->k_goal_x[k], s->k_goal_y[k])) /
                              clampf(attack_speed_score(s, k), 0.3f, 4.0f);
        s->k_recommended_counter[k] = recommended_hunter_for_target(s, k);
        s->k_target_value_mod[k] = target_value_modifier(s, s->k_goal_x[k], s->k_goal_y[k], core_x, core_y);
        if (s->k_spawn_site[k] >= 0 && s->k_spawn_site[k] < LANE_SITE_COUNT)
        {
            s->k_lane_pressure[k] = s->lane_pressure[s->k_spawn_site[k]];
        }
        else
        {
            s->k_lane_pressure[k] = 0.0f;
        }
        s->k_priority_rank[k] = 1;
        {
            float far_grid = 1.0f - clampf((s->ky[k] - (float)s->arena_y) / clampf((float)s->arena_h, 1.0f, 4000.0f), 0.0f, 1.0f);
            float low_alt = 1.0f - s->k_altitude_norm[k];
            float clutter = (s->ktype[k] == TARGET_FPV) ? (low_alt * 0.28f) : (far_grid * 0.14f);
            float noise = clampf(0.22f + (0.16f * fabsf(sinf((s->t * 0.73f) + (float)(k * 1.7f) + ((float)s->wave_idx * 0.21f)))) + clutter, 0.0f, 0.95f);
            float los_score = clampf(0.92f - (far_grid * low_alt * 0.80f) - (noise * 0.35f), 0.0f, 1.0f);
            float detect_target = clampf(0.76f + ((1.0f - clutter) * 0.16f) - (noise * 0.22f) + (s->k_track_history[k] * 0.12f), 0.05f, 0.98f);
            float class_target = clampf(detect_target - ((s->ktype[k] == TARGET_FPV) ? 0.16f : 0.08f) - (noise * 0.12f) + (s->k_track_history[k] * 0.10f), 0.05f, 0.95f);

            s->k_noise[k] = (s->k_noise[k] * 0.76f) + (noise * 0.24f);
            s->k_detect_conf[k] = (s->k_detect_conf[k] * 0.78f) + (detect_target * 0.22f);
            s->k_class_conf[k] = (s->k_class_conf[k] * 0.76f) + (class_target * 0.24f);
            s->k_track_history[k] = clampf((s->k_track_history[k] * 0.84f) +
                                           (((s->k_detect_conf[k] + s->k_class_conf[k]) * 0.5f - s->k_noise[k] * 0.20f) * 0.16f),
                                           0.0f, 1.0f);
            s->k_los_ok[k] = (los_score >= 0.42f) ? 1 : 0;
        }
        s->k_detect_ok[k] = (s->k_detect_conf[k] >= 0.35f) ? 1 : 0;
        s->k_class_ok[k] = (s->k_class_conf[k] >= 0.32f) ? 1 : 0;
        s->k_corridor_ok[k] = 1;
        {
            int emergency = (s->k_eta_to_goal[k] <= 1.2f);
            float urgency = clampf((2.8f - s->k_eta_to_goal[k]) / 2.8f, 0.0f, 1.0f);
            float confidence = clampf((s->k_detect_conf[k] + s->k_class_conf[k]) * 0.5f, 0.10f, 1.0f);
            float lane_mult = 1.0f + (clampf(s->k_lane_pressure[k], 0.0f, 4.0f) * 0.12f);

            s->k_threat_score[k] = s->k_payload_score[k] *
                                   (1.0f + urgency * 1.30f) *
                                   s->k_survivability[k] *
                                   confidence *
                                   s->k_target_value_mod[k] *
                                   lane_mult;

            s->k_commit_conf[k] = clampf((s->k_detect_conf[k] * 0.40f) +
                                         (s->k_class_conf[k] * 0.35f) +
                                         (s->k_track_history[k] * 0.15f) +
                                         (urgency * 0.10f) -
                                         (s->k_noise[k] * 0.14f),
                                         0.0f, 1.0f);
            s->k_commit_ok[k] = ((s->k_detect_ok[k] && s->k_class_ok[k] && s->k_corridor_ok[k] && s->k_los_ok[k] && (s->k_commit_conf[k] >= 0.42f)) || emergency) ? 1 : 0;
            if (!s->k_detect_ok[k])
            {
                s->k_commit_reason[k] = COMMIT_REASON_DETECT_LOW;
            }
            else if (!s->k_class_ok[k])
            {
                s->k_commit_reason[k] = COMMIT_REASON_CLASS_LOW;
            }
            else if (!s->k_los_ok[k])
            {
                s->k_commit_reason[k] = COMMIT_REASON_LOS_BLOCK;
            }
            else if (!s->k_commit_ok[k])
            {
                s->k_commit_reason[k] = COMMIT_REASON_COMMIT_LOW;
            }
            else
            {
                s->k_commit_reason[k] = COMMIT_REASON_READY;
            }
        }
    }

    if (s->killer_active[0] && !s->k_dying[0] &&
        s->killer_active[1] && !s->k_dying[1])
    {
        int active_idx[KILLER_COUNT];
        int n = 0;
        for (k = 0; k < KILLER_COUNT; ++k)
        {
            if (s->killer_active[k])
            {
                active_idx[n++] = k;
            }
        }
        for (i = 0; i < n; ++i)
        {
            int j;
            int best = i;
            for (j = i + 1; j < n; ++j)
            {
                if (s->k_threat_score[active_idx[j]] > s->k_threat_score[active_idx[best]])
                {
                    best = j;
                }
            }
            if (best != i)
            {
                int tmp = active_idx[i];
                active_idx[i] = active_idx[best];
                active_idx[best] = tmp;
            }
        }
        for (i = 0; i < n; ++i)
        {
            s->k_priority_rank[active_idx[i]] = i + 1;
        }

        /* Corridor/deconfliction: if both threats are nearly co-linear from core, hold lower-priority lane. */
        {
            int keep = (s->k_priority_rank[0] <= s->k_priority_rank[1]) ? 0 : 1;
            int hold = (keep == 0) ? 1 : 0;
            float a0 = atan2f(s->ky[0] - core_y, s->kx[0] - core_x);
            float a1 = atan2f(s->ky[1] - core_y, s->kx[1] - core_x);
            float da = wrap_angle_pi(a0 - a1);
            float r0 = sqrtf(dist2(s->kx[0], s->ky[0], core_x, core_y));
            float r1 = sqrtf(dist2(s->kx[1], s->ky[1], core_x, core_y));

            if ((fabsf(da) < 0.24f) && (fabsf(r0 - r1) < 52.0f))
            {
                s->k_corridor_ok[hold] = 0;
                s->k_commit_ok[hold] = 0;
                s->k_commit_reason[hold] = COMMIT_REASON_CORRIDOR;
            }
        }
    }
}

static hunter_type_t choose_conservative_hunter(drone_hunter_scene_t *s, int target, int launcher_idx, float *out_score)
{
    int i;
    float req = threat_required_score(s, target);
    hunter_type_t best_pick = HUNTER_STING_II;
    float best_pick_score = -9999.0f;
    hunter_type_t best_any = HUNTER_STING_II;
    int best_any_stock = -1;

    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        float cap;
        float pairing;
        float fit;
        float scarcity_penalty;
        float overkill_penalty = 0.0f;
        float score;
        int stock_i = s->hunter_stock[i];
        hunter_type_t ht = (hunter_type_t)i;
        int other_loaded_same_type = 0;
        if (s->hunter_stock[i] <= 0)
        {
            continue;
        }

        cap = hunter_capability_score(ht);
        pairing = hunter_pairing_bias(s, ht, target);
        fit = -fabsf(cap - req) * 0.36f;
        if (cap < req)
        {
            fit -= (req - cap) * 0.54f;
        }
        scarcity_penalty = (stock_i <= 1) ? 0.28f : ((stock_i == 2) ? 0.12f : 0.0f);

        if ((launcher_idx == 0) && s->hunter_loaded[1] && (s->h_type[1] == ht))
        {
            other_loaded_same_type = 1;
        }
        else if ((launcher_idx == 1) && s->hunter_loaded[0] && (s->h_type[0] == ht))
        {
            other_loaded_same_type = 1;
        }

        if (other_loaded_same_type && (s->k_threat_score[target] < 1.55f))
        {
            /* Avoid overkill: don't stack same hunter type on low-threat target. */
            overkill_penalty += 0.32f;
        }
        if (hunter_is_high_speed_tier(ht) &&
            (s->k_threat_score[target] < 1.40f) &&
            has_unassigned_higher_threat(s, target))
        {
            /* Preserve premium hunters when a higher-priority unassigned threat exists. */
            overkill_penalty += 0.40f;
        }

        score = pairing + fit - scarcity_penalty - overkill_penalty;
        if (score > best_pick_score)
        {
            best_pick_score = score;
            best_pick = ht;
        }
        if (s->hunter_stock[i] > best_any_stock)
        {
            best_any_stock = s->hunter_stock[i];
            best_any = ht;
        }
    }

    if (out_score != NULL)
    {
        *out_score = best_pick_score;
    }
    if (best_pick_score > -9998.0f)
    {
        return best_pick;
    }
    return best_any;
}

/* Stability guard: limit runtime launch sprites to the known-stable set. */
static hunter_type_t sanitize_hunter_pick(hunter_type_t pick)
{
    if (((int)pick < 0) || ((int)pick >= HUNTER_TYPE_COUNT))
    {
        return HUNTER_STING_II;
    }
    return pick;
}

static void apply_hunter_profile(drone_hunter_scene_t *s, int h)
{
    const hunter_profile_t *p = &g_hunter_profiles[(int)s->h_type[h]];
    uint16_t zoom = (uint16_t)clampf(188.0f + (p->speed * 22.0f), 180.0f, 296.0f);
    if (s->h_type[h] == HUNTER_STING_II)
    {
        zoom = (uint16_t)clampf((float)zoom * 1.05f, 170.0f, 260.0f);
    }
    lv_image_set_src(s->hunters[h], hunter_image_src(s->h_type[h]));
    s->h_base_zoom[h] = zoom;
    lv_obj_set_style_transform_zoom(s->hunters[h], s->h_base_zoom[h], 0);
    lv_obj_clear_flag(s->hunters[h], LV_OBJ_FLAG_HIDDEN);
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
        /* Mix human start timing into RNG so each boot has varied opening sequences. */
        s->rng_state ^= (runtime_entropy32() ^ (lv_tick_get() * 2246822519u));
        (void)scene_rng_next(s);
        (void)scene_rng_next(s);
        lv_obj_add_flag(s->splash, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(s->arena, LV_OBJ_FLAG_HIDDEN);
        if (s->hud_diag_stage != NULL)
        {
            lv_obj_clear_flag(s->hud_diag_stage, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_width(s->hud_diag_stage, s->arena_w - 8);
            lv_obj_align(s->hud_diag_stage, LV_ALIGN_TOP_MID, 0, 2);
            lv_obj_move_foreground(s->hud_diag_stage);
        }
        /* Reset at match start so opening wave uses fresh entropy (not boot-time deterministic state). */
        reset_round(s);
        set_diag_stage(s, "DBG:START_MATCH");
    }
}

static float wrap_angle(float a)
{
    if (!isfinite(a))
    {
        return 0.0f;
    }
    a = remainderf(a, 2.0f * PI_F);
    if (!isfinite(a))
    {
        return 0.0f;
    }
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
    static const float drone_scale[SPLASH_DRONE_COUNT] = {0.70f, 1.00f, 0.78f, 1.55f};

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
    lv_indev_t *indev = lv_indev_get_act();
    lv_point_t p = {0, 0};

    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        if (indev != NULL)
        {
            lv_indev_get_point(indev, &p);
        }
        s->rng_state ^= (uint32_t)((p.x << 16) ^ (p.y << 1) ^ (int32_t)runtime_entropy32());
        begin_arena_from_splash(s);
    }
}

static void splash_show_title(drone_hunter_scene_t *s)
{
#if BOOT_FLAME_GALLERY_MODE
    lv_obj_add_flag(s->lineup_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->lineup_desc, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->splash_start_btn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->splash_hero_odin, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s->splash_hero_skyfall, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s->splash_hero, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_transform_width(s->splash_hero, 0, 0);
    lv_obj_set_style_transform_height(s->splash_hero, 0, 0);
    lv_obj_set_style_opa(s->splash_hero, LV_OPA_COVER, 0);
    lv_obj_align(s->splash_hero, LV_ALIGN_CENTER, 0, 22);
    lv_obj_set_style_transform_zoom(s->splash_hero, 440, 0);
    s->splash_center_idx = 0;
    s->splash_phase_sec = 0.0f;
    s->splash_last_center_idx = -1;
    lv_label_set_text(s->splash_title, "Flame Gallery");
    lv_label_set_text(s->splash_subtitle, "Boot demo loop: each flame profile every 5 seconds");
    lv_obj_clear_flag(s->splash_title, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s->splash_subtitle, LV_OBJ_FLAG_HIDDEN);
    return;
#endif

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

#if BOOT_FLAME_GALLERY_MODE
    {
        int profile;
        static int bright_idx = 0;
        static int dark_idx = 0;
        static int slot_idx = 0;
        const lv_image_dsc_t **frames;
        float fps;
        int frame_idx;
        uint16_t zoom = 440;
        char subtitle[96];
        s->splash_phase_sec += DT_SEC;
        if (s->splash_phase_sec >= BOOT_FLAME_PROFILE_SEC)
        {
            s->splash_phase_sec = 0.0f;
            slot_idx = (slot_idx + 1) & 0x3;
            if (slot_idx == 3)
            {
                profile = g_city_fire_dark_profiles[dark_idx % 8];
                dark_idx++;
            }
            else
            {
                profile = g_city_fire_bright_profiles[bright_idx % 8];
                bright_idx++;
            }
            s->splash_center_idx = profile % CITY_FIRE_PROFILE_COUNT;
            s->splash_last_center_idx = -1;
        }
        profile = s->splash_center_idx % CITY_FIRE_PROFILE_COUNT;
        frames = city_fire_profile_frames(profile);
        fps = city_fire_profile_fps(profile);
        frame_idx = ((int)(((float)lv_tick_get() * 0.001f) * fps)) % FLAME_SPRITE_FRAME_COUNT;
        if ((FLAME_SPRITE_FRAME_COUNT > 1) && (frame_idx == 0))
        {
            frame_idx = 1;
        }
        if ((profile == CITY_FIRE_PROFILE_SMOKE) || (profile == CITY_FIRE_PROFILE_GROUND))
        {
            zoom = 480;
        }
        else if ((profile == CITY_FIRE_PROFILE_BURST) || (profile == CITY_FIRE_PROFILE_BRIGHT_RED_HOT))
        {
            zoom = 420;
        }
        if (s->splash_last_center_idx != ((profile * 100) + frame_idx))
        {
            lv_image_set_src(s->splash_hero, frames[frame_idx]);
            lv_obj_set_style_transform_zoom(s->splash_hero, zoom, 0);
            if (city_fire_is_dark_profile(profile))
            {
                lv_obj_set_style_image_recolor(s->splash_hero, city_fire_profile_preview_tint(profile), 0);
                lv_obj_set_style_image_recolor_opa(s->splash_hero, city_fire_profile_preview_tint_opa(profile), 0);
            }
            else
            {
                lv_obj_set_style_image_recolor(s->splash_hero, city_fire_bright_demo_tint(bright_idx), 0);
                lv_obj_set_style_image_recolor_opa(s->splash_hero, LV_OPA_COVER, 0);
            }
            s->splash_last_center_idx = (profile * 100) + frame_idx;
        }
        (void)snprintf(subtitle, sizeof(subtitle),
                       "Profile %02d/%02d: %s",
                       profile + 1, CITY_FIRE_PROFILE_COUNT, city_fire_profile_name(profile));
        lv_label_set_text(s->splash_subtitle, subtitle);
    }
    return;
#endif

    (void)s;
}

static void assign_mode(drone_hunter_scene_t *s, match_mode_t m)


{
    if (m == MODE_ALGO_VS_ALGO)
    {
        s->attacker_mode_sel = CTRL_ALGO;
        s->defender_mode_sel = DEFENDER_MODE_ALGO;
    }
    else if (m == MODE_EDGEAI_VS_EDGEAI)
    {
        s->attacker_mode_sel = CTRL_EDGEAI;
        s->defender_mode_sel = DEFENDER_MODE_EDGEAI;
    }
    else
    {
        s->attacker_mode_sel = CTRL_EDGEAI;
        s->defender_mode_sel = DEFENDER_MODE_ALGO;
    }
    apply_control_settings(s);
    update_mode_button_label(s);
}

static int attack_variant_idx(const drone_hunter_scene_t *s, int k)
{
    int variant = (s->k_serial[k] + k + s->k_variant_seed[k]) % 3;
    if (variant < 0)
    {
        variant += 3;
    }
    return variant;
}

static int target_is_shahed_visual(const drone_hunter_scene_t *s, int k)
{
    return (s->ktype[k] == TARGET_FIXED_WING) && (s->threat_faction == THREAT_FACTION_RUSSIA);
}

static int target_blast_style_for_visual(const drone_hunter_scene_t *s, int k)
{
    if ((s->ktype[k] == TARGET_FIXED_WING) && (s->threat_faction == THREAT_FACTION_RUSSIA))
    {
        /* Shahed */
        return BLAST_STYLE_GIANT_ORANGE;
    }
    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        /* Red fixed-wing */
        return BLAST_STYLE_MEDIUM_RED;
    }
    /* X-wing DJI */
    return BLAST_STYLE_SMALL_WHITE;
}

static float attack_zoom_scale(const drone_hunter_scene_t *s, int k)
{
    if ((s->ktype[k] == TARGET_FIXED_WING) && (s->threat_faction == THREAT_FACTION_RUSSIA))
    {
        return 2.0f;  /* Shahed: largest */
    }
    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        return 1.0f;  /* Red fixed-wing: medium */
    }
    return 0.5f;      /* X-wing DJI: small */
}

static const lv_image_dsc_t *attack_image_src(const drone_hunter_scene_t *s, int k)
{
#if ATTACK_SPRITE_STABLE_ONLY
    (void)s;
    (void)k;
    return &attack_shahed_yellow;
#else
    if ((s->ktype[k] == TARGET_FIXED_WING) && (s->threat_faction == THREAT_FACTION_RUSSIA))
    {
        return &attack_shahed_yellow;
    }
    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        return &attack_vb140_like_red;
    }
    return &attack_dji_x_orange;
#endif
}

static void style_target(drone_hunter_scene_t *s, int k)
{
    const lv_image_dsc_t *src;
    uint16_t zoom = (s->k_tier[k] >= 2) ? 205 : ((s->k_tier[k] == 1) ? 188 : 172);
    float zscale = attack_zoom_scale(s, k);

#if ATTACK_SPRITE_STABLE_ONLY
    /* Stability guard: keep a known-good attack sprite source. */
    src = attack_image_src(s, k);
    zoom = 176;
#else
    src = attack_image_src(s, k);
#endif

    if (s->k_last_src[k] != src)
    {
        lv_image_set_src(s->killers[k], src);
        s->k_last_src[k] = src;
    }
    {
        int32_t iw = lv_obj_get_width(s->killers[k]);
        int32_t ih = lv_obj_get_height(s->killers[k]);
        if ((iw <= 0) || (ih <= 0) || (iw > 220) || (ih > 220))
        {
            /* Defensive fallback for corrupted/unsupported image descriptors. */
            src = attack_image_src(s, k);
            lv_image_set_src(s->killers[k], src);
            s->k_last_src[k] = src;
            zoom = 170;
        }
    }
    s->k_base_zoom[k] = zoom;
    s->k_base_zoom[k] = (uint16_t)clampf((float)s->k_base_zoom[k] * zscale, 80.0f, 900.0f);
    lv_obj_set_style_transform_zoom(s->killers[k], s->k_base_zoom[k], 0);
    lv_obj_set_style_opa(s->killers[k], LV_OPA_COVER, 0);
    lv_obj_set_style_image_opa(s->killers[k], LV_OPA_COVER, 0);
    lv_obj_set_style_image_recolor_opa(s->killers[k], LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(s->killers[k], LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(s->killers[k], 0, 0);
    lv_obj_set_style_outline_width(s->killers[k], 0, 0);
    lv_obj_set_style_shadow_width(s->killers[k], 0, 0);
    lv_obj_set_style_shadow_opa(s->killers[k], LV_OPA_TRANSP, 0);
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
        lv_obj_set_style_transform_angle(s->killers[k], SPRITE_NOSE_FWD_TENTH, 0);
    }
}

static int wave_target_count(int wave_idx, wave_archetype_t archetype)
{
    float multiplier = 1.0f;
    int milestone = mission_milestone_from_wave(wave_idx);
    int total = ATTACK_POOL_BASE + ((wave_idx - 1) * ATTACK_POOL_GROWTH);
    switch (archetype)
    {
        case WAVE_ARCHETYPE_SHAHED_HEAVY:
            multiplier = 1.06f;
            break;
        case WAVE_ARCHETYPE_STRIKE_X_SWARM:
            multiplier = 1.18f;
            break;
        case WAVE_ARCHETYPE_MIXED_DECEPTION:
            multiplier = 1.10f;
            break;
        case WAVE_ARCHETYPE_TERMINAL_SATURATION:
        default:
            multiplier = 1.24f;
            break;
    }
    multiplier *= mission_milestone_spawn_mult(milestone);
    total = (int)((float)total * multiplier);
    if (total > ATTACK_POOL_MAX)
    {
        total = ATTACK_POOL_MAX;
    }
    return total;
}

static attack_strategy_t adaptive_attacker_strategy(drone_hunter_scene_t *s, attack_strategy_t base)
{
    int delta_pts = s->attacker_points - s->hunter_points;
    int leaks = s->attack_leaked;
    int kills = s->attack_destroyed;
    int total = leaks + kills;
    float leak_ratio = (total > 0) ? ((float)leaks / (float)total) : 0.0f;

    if ((delta_pts <= -6) || (leak_ratio < 0.24f && total >= 10))
    {
        /* Defender is suppressing attacks: escalate pressure to keep attacker viable. */
        if (base == ATTACK_STRATEGY_FLANK_PRESSURE)
        {
            return ATTACK_STRATEGY_MIXED_LURE_STRIKE;
        }
        return ATTACK_STRATEGY_TERMINAL_SATURATION;
    }
    if ((delta_pts >= 8) || (leak_ratio > 0.58f && total >= 8))
    {
        /* Attacker dominating too hard: reduce predictability/overpowering saturation. */
        if (base == ATTACK_STRATEGY_TERMINAL_SATURATION)
        {
            return ATTACK_STRATEGY_MIXED_LURE_STRIKE;
        }
        return ATTACK_STRATEGY_FLANK_PRESSURE;
    }
    return base;
}

static void maybe_apply_mid_wave_shift(drone_hunter_scene_t *s)
{
    int spawned;
    int trigger_primary;
    int trigger_late;
    int leak_trigger;
    if ((s->wave_shift_applied >= 2) || (s->attack_strategy_select != ATTACK_STRATEGY_AUTO))
    {
        return;
    }
    spawned = s->wave_target_total - s->attack_remaining_to_spawn;
    trigger_primary = (s->wave_target_total * 55) / 100;
    trigger_late = (s->wave_target_total * 82) / 100;
    leak_trigger = (s->wave_target_total * 8) / 100;
    if (leak_trigger < 1)
    {
        leak_trigger = 1;
    }
    if ((s->wave_shift_applied == 0) && (spawned >= trigger_primary))
    {
        attack_strategy_t base = attack_strategy_shift_for_archetype(s->wave_archetype);
        s->attack_strategy_live = adaptive_attacker_strategy(s, base);
        s->wave_shift_applied = 1;
        note_failure(s, "Wave shift: attacker doctrine changed mid-wave", NULL);
        return;
    }
    if (s->wave_shift_applied != 1)
    {
        return;
    }
    if ((spawned < trigger_late) && (s->attack_leaked < leak_trigger))
    {
        return;
    }
    {
        attack_strategy_t base = attack_strategy_late_shift_for_archetype(s->wave_archetype);
        s->attack_strategy_live = adaptive_attacker_strategy(s, base);
    }
    s->wave_shift_applied = 2;
    note_failure(s, "Late-wave shift: terminal pressure adaptation engaged", NULL);
}

static void update_hunter_deck_ui(drone_hunter_scene_t *s)
{
    int i;
    static int cache_ready = 0;
    static int prev_stock[HUNTER_TYPE_COUNT];
    static int prev_name_live[HUNTER_TYPE_COUNT];
    static int prev_count_live[HUNTER_TYPE_COUNT];
    static int prev_icon_live[HUNTER_TYPE_COUNT];
    static int prev_in_use[HUNTER_TYPE_COUNT];
    static int prev_selected[HUNTER_TYPE_COUNT];
    static int prev_ciws_left = -1;
    static int prev_ciws_right = -1;
    static int prev_ciws_live = -1;
    static uint32_t last_ciws_label_tick = 0U;

    if (!cache_ready)
    {
        for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
        {
            prev_stock[i] = -1;
            prev_name_live[i] = -1;
            prev_count_live[i] = -1;
            prev_icon_live[i] = -1;
            prev_in_use[i] = -1;
            prev_selected[i] = -1;
        }
        cache_ready = 1;
    }

    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        int in_use = (s->h_type[0] == (hunter_type_t)i) || (s->h_type[1] == (hunter_type_t)i);
        int selected = (s->manual_selected_hunter == i);
        int stock_live = (s->hunter_stock[i] > 0) ? 1 : 0;
        if (prev_icon_live[i] != stock_live)
        {
            /* Keep deck icons visually stable; avoid periodic opacity flicker. */
            lv_obj_set_style_opa(s->deck_icon[i], LV_OPA_COVER, 0);
            prev_icon_live[i] = stock_live;
        }
        if (prev_in_use[i] != in_use)
        {
            lv_obj_set_style_border_width(s->deck_icon[i], in_use ? 2 : 0, 0);
            lv_obj_set_style_border_color(s->deck_icon[i], lv_color_hex(0x22D3EE), 0);
            lv_obj_set_style_radius(s->deck_icon[i], 3, 0);
            prev_in_use[i] = in_use;
        }
        if (prev_selected[i] != selected)
        {
            lv_obj_set_style_outline_width(s->deck_icon[i], selected ? 2 : 0, 0);
            lv_obj_set_style_outline_color(s->deck_icon[i], lv_color_hex(0xFBBF24), 0);
            prev_selected[i] = selected;
        }
        if (prev_stock[i] != s->hunter_stock[i])
        {
            lv_label_set_text_fmt(s->deck_count[i], "x%d", s->hunter_stock[i]);
            prev_stock[i] = s->hunter_stock[i];
        }
        if (prev_count_live[i] != stock_live)
        {
            lv_obj_set_style_text_color(s->deck_count[i],
                                        stock_live ? lv_color_hex(0xFDE68A) : lv_color_hex(0x6B7280),
                                        0);
            prev_count_live[i] = stock_live;
        }
        if (prev_name_live[i] != stock_live)
        {
            lv_obj_set_style_text_color(s->deck_name[i],
                                        stock_live ? lv_color_hex(0xE5E7EB) : lv_color_hex(0x6B7280),
                                        0);
            prev_name_live[i] = stock_live;
        }
    }

    if (s->deck_ciws_count != NULL)
    {
        int total_ammo = s->ciws_ammo_left + s->ciws_ammo_right;
        int ciws_live = (total_ammo > 0) ? 1 : 0;
        uint32_t now_tick = lv_tick_get();
        int ciws_changed = (prev_ciws_left != s->ciws_ammo_left) || (prev_ciws_right != s->ciws_ammo_right);
        int label_due = ((uint32_t)(now_tick - last_ciws_label_tick) >= 180U) || (total_ammo <= 0);
        if (ciws_changed && label_due)
        {
            lv_label_set_text_fmt(s->deck_ciws_count, "L%d/R%d", s->ciws_ammo_left, s->ciws_ammo_right);
            prev_ciws_left = s->ciws_ammo_left;
            prev_ciws_right = s->ciws_ammo_right;
            last_ciws_label_tick = now_tick;
        }
        if (prev_ciws_live != ciws_live)
        {
            lv_obj_set_style_text_color(s->deck_ciws_count,
                                        ciws_live ? lv_color_hex(0x93C5FD) : lv_color_hex(0x6B7280),
                                        0);
            lv_obj_set_style_text_color(s->deck_ciws_name,
                                        ciws_live ? lv_color_hex(0xE5E7EB) : lv_color_hex(0x6B7280),
                                        0);
            lv_obj_set_style_opa(s->deck_ciws_icon, ciws_live ? LV_OPA_COVER : LV_OPA_70, 0);
            prev_ciws_live = ciws_live;
        }
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
    s->k_dying[k] = 0;
    s->k_dying_t[k] = 0.0f;
    lv_obj_clear_flag(s->killers[k], LV_OBJ_FLAG_HIDDEN);
}

static void respawn_killer(drone_hunter_scene_t *s, int k, int side)
{
    float min_x = (float)s->arena_x + 20.0f;
    float max_x = (float)(s->arena_x + s->arena_w - 20);
    float min_y = (float)s->arena_y + 20.0f;
    float max_y = combat_floor_y(s);
    float span_x = (max_x - min_x > 1.0f) ? (max_x - min_x) : 1.0f;
    float span_y = (max_y - min_y > 1.0f) ? (max_y - min_y) : 1.0f;
    float pos_x = min_x;
    float pos_y = min_y;
    float goal_min_x = (float)s->arena_x + 14.0f;
    float goal_max_x = (float)(s->arena_x + s->arena_w - 14);
    float goal_min_y = (float)s->arena_y + ((float)s->arena_h * 0.16f);
    float goal_max_y = (float)s->arena_y + ((float)s->arena_h * 0.84f);
    int edge;
    int lane;
    float lane_t;
    int phase = arena_phase(s);
    int spawned_so_far;
    float wave_progress;

    set_diag_stage(s, "DBG:RESPAWN_ENTER");
    s->rng_state ^= (runtime_entropy32() + (uint32_t)(k * 747796405u) + (uint32_t)(s->killer_spawn_tick * 2891336453u));
    (void)scene_rng_next(s);

    if (s->attack_remaining_to_spawn <= 0)
    {
        set_killer_hidden(s, k);
        return;
    }

    set_killer_visible(s, k);
    s->attack_remaining_to_spawn--;
    s->killer_spawn_tick++;
    maybe_apply_mid_wave_shift(s);
    s->k_serial[k]++;
    spawned_so_far = s->wave_target_total - s->attack_remaining_to_spawn;
    wave_progress = (float)spawned_so_far / clampf((float)s->wave_target_total, 1.0f, 20000.0f);

    {
        /* Strategic stochastic composition: doctrine shapes probabilities, RNG decides exact sequence. */
        float fixed_bias = 0.50f;
        float tier2_bias = 0.22f;
        float tier1_bias = 0.52f;
        float type_roll = (float)scene_rng_range(s, 1000) / 1000.0f;
        float tier_roll = (float)scene_rng_range(s, 1000) / 1000.0f;

        switch (s->wave_archetype)
        {
            case WAVE_ARCHETYPE_SHAHED_HEAVY:
                fixed_bias = 0.78f;
                tier2_bias = 0.30f;
                tier1_bias = 0.46f;
                break;
            case WAVE_ARCHETYPE_STRIKE_X_SWARM:
                fixed_bias = 0.20f;
                tier2_bias = 0.16f;
                tier1_bias = 0.50f;
                break;
            case WAVE_ARCHETYPE_MIXED_DECEPTION:
                fixed_bias = (wave_progress < 0.52f) ? 0.58f : 0.44f;
                tier2_bias = 0.24f;
                tier1_bias = 0.50f;
                break;
            case WAVE_ARCHETYPE_TERMINAL_SATURATION:
            default:
                fixed_bias = (wave_progress < 0.45f) ? 0.62f : 0.36f;
                tier2_bias = (wave_progress < 0.45f) ? 0.20f : 0.34f;
                tier1_bias = 0.50f;
                break;
        }

        if (s->attack_strategy_live == ATTACK_STRATEGY_CENTER_PRESSURE)
        {
            fixed_bias += 0.07f;
        }
        else if (s->attack_strategy_live == ATTACK_STRATEGY_FLANK_PRESSURE)
        {
            fixed_bias -= 0.07f;
        }
        else if (s->attack_strategy_live == ATTACK_STRATEGY_TERMINAL_SATURATION)
        {
            tier2_bias += 0.08f;
        }

        if ((s->attack_leaked > s->attack_destroyed) || (wave_progress > 0.68f))
        {
            tier2_bias += 0.08f;
            tier1_bias -= 0.04f;
        }
        if (s->wave_idx <= 2)
        {
            tier2_bias -= 0.06f;
            tier1_bias += 0.04f;
        }
        if ((s->wave_idx == 1) || (wave_progress < 0.20f))
        {
            fixed_bias -= 0.08f;
            tier2_bias -= 0.06f;
        }

        fixed_bias = clampf(fixed_bias, 0.12f, 0.90f);
        tier2_bias = clampf(tier2_bias, 0.08f, 0.62f);
        tier1_bias = clampf(tier1_bias, 0.20f, 0.74f);

        s->ktype[k] = (type_roll < fixed_bias) ? TARGET_FIXED_WING : TARGET_FPV;

        if (tier_roll < tier2_bias)
        {
            s->k_tier[k] = 2;
        }
        else if (tier_roll < (tier2_bias + tier1_bias))
        {
            s->k_tier[k] = 1;
        }
        else
        {
            s->k_tier[k] = 0;
        }
    }

    if (phase == 0 && s->ktype[k] == TARGET_FIXED_WING)
    {
        /* Keep early timeline somewhat lighter before escalation ramps. */
        s->k_tier[k] = (s->k_tier[k] > 0) ? (s->k_tier[k] - 1) : 0;
    }
    if (s->ktype[k] == TARGET_FIXED_WING)
    {
        s->k_payload_score[k] = (s->threat_faction == THREAT_FACTION_RUSSIA) ? 1.80f : 1.35f;
        s->k_survivability[k] = 1.10f + ((float)s->k_tier[k] * 0.10f);
    }
    else
    {
        s->k_payload_score[k] = 1.05f;
        s->k_survivability[k] = 1.20f + ((float)s->k_tier[k] * 0.14f);
    }
    s->k_detect_conf[k] = 0.62f;
    s->k_class_conf[k] = 0.58f;
    s->k_threat_score[k] = 1.0f;
    s->k_speed_est[k] = attack_speed_kmh_est(s, k);
    s->k_altitude_norm[k] = 0.50f;
    s->k_range_to_core[k] = 0.0f;
    s->k_eta_to_goal[k] = 0.0f;
    s->k_priority_rank[k] = 1;
    s->k_recommended_counter[k] = recommended_hunter_for_target(s, k);
    s->k_detect_ok[k] = 1;
    s->k_class_ok[k] = 1;
    s->k_commit_ok[k] = 1;
    s->k_corridor_ok[k] = 1;
    s->k_los_ok[k] = 1;
    s->k_track_history[k] = 0.52f;
    s->k_noise[k] = 0.08f;
    s->k_commit_conf[k] = 0.55f;
    s->k_commit_reason[k] = COMMIT_REASON_READY;

    style_target(s, k);

    choose_attack_goal(s, k, goal_min_x, goal_max_x, goal_min_y, goal_max_y);

    {
        int site = (side >= 0 && side < 16) ? side : choose_spawn_site(s, k, s->k_goal_x[k], s->k_goal_y[k]);
        edge = (site / 4) & 0x3;
        lane = site & 0x3;
        s->k_spawn_site[k] = site;
    }
    lane_t = ((float)lane + 0.5f) * 0.25f; /* 0.125, 0.375, 0.625, 0.875 */

    switch (edge)
    {
        case 0: /* left edge */
            pos_x = (float)s->arena_x - 18.0f;
            pos_y = min_y + (lane_t * span_y);
            break;
        case 1: /* right edge */
            pos_x = (float)(s->arena_x + s->arena_w) + 18.0f;
            pos_y = min_y + (lane_t * span_y);
            break;
        case 2: /* top edge */
            pos_x = min_x + (lane_t * span_x);
            pos_y = (float)s->arena_y - 18.0f;
            break;
        default: /* bottom edge */
            pos_x = min_x + (lane_t * span_x);
            pos_y = (float)(s->arena_y + s->arena_h) + 18.0f;
            break;
    }

    s->kx[k] = pos_x;
    s->ky[k] = pos_y;
    s->kvx[k] = 0.0f;
    s->kvy[k] = 0.0f;
    s->k_heading[k] = atan2f(s->k_goal_y[k] - s->ky[k], s->k_goal_x[k] - s->kx[k]);

    s->fx_spawn_t[k] = FX_SPAWN_SEC;
    s->fx_spawn_x[k] = s->kx[k];
    s->fx_spawn_y[k] = s->ky[k];
    s->k_missed_by_hunter[k] = 0;
    s->k_variant_seed[k] = scene_rng_range(s, 3);

}

static void start_wave(drone_hunter_scene_t *s, int wave_idx)
{
    s->wave_archetype = wave_archetype_from_wave(wave_idx);
    if (wave_idx == 1)
    {
        /* Opening wave starts with random doctrine/archetype to avoid reboot replay patterns. */
        s->wave_archetype = (wave_archetype_t)scene_rng_range(s, 4);
    }
    else if (s->attack_strategy_select == ATTACK_STRATEGY_AUTO)
    {
        /* Small per-wave mutation keeps attacker plan dynamic over long runs. */
        if (scene_rng_range(s, 100) < 24)
        {
            s->wave_archetype = (wave_archetype_t)((((int)s->wave_archetype) + scene_rng_range(s, 4)) & 0x3);
        }
    }
    s->wave_idx = wave_idx;
    s->mission_milestone = mission_milestone_from_wave(wave_idx);
    s->wave_target_total = wave_target_count(wave_idx, s->wave_archetype);
    s->attack_remaining_to_spawn = s->wave_target_total;
    s->attack_destroyed = 0;
    s->attack_leaked = 0;
    s->wave_shift_applied = 0;
    s->threat_faction = threat_faction_for_archetype(s->wave_archetype, wave_idx);
    if (s->attack_strategy_select == ATTACK_STRATEGY_AUTO)
    {
        attack_strategy_t base = attack_strategy_for_archetype(s->wave_archetype);
        s->attack_strategy_live = adaptive_attacker_strategy(s, base);
    }
    else
    {
        s->attack_strategy_live = s->attack_strategy_select;
    }

    respawn_killer(s, 0, -1);
    respawn_killer(s, 1, -1);
}

static void reset_hunters(drone_hunter_scene_t *s)
{
    int i;
    s->h_launch_sector[0] = 5; /* bottom-left */
    s->h_launch_sector[1] = 3; /* bottom-right */
    s->hx[0] = hunter_regroup_x(s, 0, (float)(s->arena_x + (s->arena_w / 2)));
    s->hy[0] = hunter_regroup_y(s, 0);
    s->hx[1] = hunter_regroup_x(s, 1, (float)(s->arena_x + (s->arena_w / 2)));
    s->hy[1] = hunter_regroup_y(s, 1);

    for (i = 0; i < HUNTER_COUNT; ++i)
    {
        s->hvx[i] = 0.0f;
        s->hvy[i] = 0.0f;
        s->h_heading[i] = -PI_F * 0.5f;
        s->h_type[i] = HUNTER_STING_II;
        s->h_reselect_sec[i] = 0.0f;
        s->hunter_loaded[i] = 0;
        s->h_target_idx[i] = -1;
        s->h_target_serial[i] = 0;
        s->h_falling[i] = 0;
        s->h_launch_penalty[i] = 0.0f;
        s->h_last_target_d2[i] = 0.0f;
        s->h_overshoot_cooldown[i] = 0.0f;
        s->h_lock_persist_t[i] = 0.0f;
        s->h_switch_cooldown_t[i] = 0.0f;
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

static int defense_layer_remaining(const drone_hunter_scene_t *s)
{
    int k;
    int total_stock = 0;
    int hunters_air = 0;
    for (k = 0; k < HUNTER_TYPE_COUNT; ++k)
    {
        total_stock += s->hunter_stock[k];
    }
    for (k = 0; k < HUNTER_COUNT; ++k)
    {
        if (s->hunter_loaded[k])
        {
            hunters_air++;
        }
    }
    return (total_stock > 0) || (hunters_air > 0) || (s->ciws_ammo_left > 0) || (s->ciws_ammo_right > 0);
}

static void hide_quick_menu(drone_hunter_scene_t *s)
{
    if (s->quick_menu != NULL)
    {
        s->quick_menu_settings_active = 0;
        lv_obj_add_flag(s->quick_menu, LV_OBJ_FLAG_HIDDEN);
    }
}

static void refresh_settings_menu(drone_hunter_scene_t *s)
{
    char body[448];
    const char *attacker = ctrl_name_compact(s->attacker_mode_sel);
    const char *defender = (s->defender_mode_sel == DEFENDER_MODE_HUMAN) ? "HUMAN" :
                           ((s->defender_mode_sel == DEFENDER_MODE_EDGEAI) ? "EDGEAI" : "ALGO");
    (void)snprintf(body, sizeof(body),
                   "[Tap row to cycle]\n"
                   "ATTACKER: %s\n"
                   "DEFENDER: %s\n"
                   "NPU: %s\n"
                   "DIFFICULTY: %s\n"
                   "SPEED++: %s",
                   attacker,
                   defender,
                   s->npu_enabled ? "ON" : "OFF",
                   difficulty_name(s->difficulty_sel),
                   speed_pp_name(s->speed_pp_idx));
    show_quick_menu(s, "Settings", body);
    s->quick_menu_settings_active = 1;
}

static void cycle_setting_row(drone_hunter_scene_t *s, int row)
{
    switch (row)
    {
        case 0:
            s->attacker_mode_sel = (s->attacker_mode_sel == CTRL_ALGO) ? CTRL_EDGEAI : CTRL_ALGO;
            break;
        case 1:
            s->defender_mode_sel = (defender_mode_t)(((int)s->defender_mode_sel + 1) % 3);
            break;
        case 2:
            s->npu_enabled = s->npu_enabled ? 0 : 1;
            break;
        case 3:
            s->difficulty_sel = (difficulty_t)(((int)s->difficulty_sel + 1) % 3);
            break;
        case 4:
            s->speed_pp_idx = (s->speed_pp_idx + 1) % 3;
            break;
        default:
            break;
    }
    apply_control_settings(s);
    update_mode_button_label(s);
    refresh_settings_menu(s);
}

static void show_quick_menu(drone_hunter_scene_t *s, const char *title, const char *body)
{
    if (s->quick_menu == NULL)
    {
        return;
    }
    lv_label_set_text(s->quick_menu_title, title);
    lv_label_set_text(s->quick_menu_body, body);
    lv_obj_clear_flag(s->quick_menu, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(s->quick_menu);
}

static void quick_menu_close_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
    {
        return;
    }
    hide_quick_menu(s);
}

static void quick_menu_body_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t *indev;
    lv_point_t p;
    lv_area_t a;
    int row_h;
    int y_rel;
    int row;

    if ((code != LV_EVENT_CLICKED) || !s->quick_menu_settings_active)
    {
        return;
    }

    indev = lv_indev_get_act();
    if (indev == NULL)
    {
        return;
    }
    lv_indev_get_point(indev, &p);
    lv_obj_get_coords(s->quick_menu_body, &a);
    if ((p.x < a.x1) || (p.x > a.x2) || (p.y < a.y1) || (p.y > a.y2))
    {
        return;
    }

    row_h = lv_font_get_line_height(&lv_font_montserrat_12) + 4;
    y_rel = p.y - a.y1;
    row = (y_rel / row_h) - 1; /* first visible line is hint text */
    if ((row < 0) || (row >= SETTINGS_ROW_COUNT))
    {
        return;
    }
    cycle_setting_row(s, row);
}

static void hud_attack_card_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    if ((s == NULL) || (lv_event_get_code(e) != LV_EVENT_CLICKED))
    {
        return;
    }
    s->attacker_mode_sel = (s->attacker_mode_sel == CTRL_ALGO) ? CTRL_EDGEAI : CTRL_ALGO;
    apply_control_settings(s);
    update_mode_button_label(s);
    if (s->quick_menu_settings_active)
    {
        refresh_settings_menu(s);
    }
    update_hud(s);
}

static void hud_defend_card_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    if ((s == NULL) || (lv_event_get_code(e) != LV_EVENT_CLICKED))
    {
        return;
    }
    s->defender_mode_sel = (defender_mode_t)(((int)s->defender_mode_sel + 1) % 3);
    apply_control_settings(s);
    update_mode_button_label(s);
    if (s->quick_menu_settings_active)
    {
        refresh_settings_menu(s);
    }
    update_hud(s);
}

static void reset_round(drone_hunter_scene_t *s)
{
    int i;

    if ((s->attack_strategy_select < ATTACK_STRATEGY_AUTO) || (s->attack_strategy_select >= ATTACK_STRATEGY_COUNT))
    {
        s->attack_strategy_select = ATTACK_STRATEGY_AUTO;
    }
    s->attack_strategy_live = ATTACK_STRATEGY_CENTER_PRESSURE;

    s->team_score[0] = 0;
    s->team_score[1] = 0;
    s->iff_degraded = 0;
    s->iff_merged_tracks = 0;
    s->iff_recovery_ttl = 0.0f;
    s->manual_selected_hunter = -1;
    s->manual_selected_target = -1;
    s->manual_select_ttl = 0.0f;
    s->core_hp = CORE_HP_START;
    s->round_time_sec = ROUND_TIME_SEC;
    s->round_over = 0;
    s->round_start_tick_ms = lv_tick_get();
    s->hud_refresh_t = HUD_REFRESH_SEC;
    s->t = 0.0f;
    s->rng_state = (uint32_t)(lv_tick_get() ^ (uint32_t)(uintptr_t)s ^ ((uint32_t)s->wave_idx * 2654435761u) ^ runtime_entropy32());
    if (s->rng_state == 0U)
    {
        s->rng_state = 0x5F3759DFu;
    }
    {
        int warm = 8 + (int)(runtime_entropy32() & 0x1Fu);
        int w;
        for (w = 0; w < warm; ++w)
        {
            (void)scene_rng_next(s);
        }
    }
    s->strategy_replan_tick = 0;
    s->killer_spawn_tick = scene_rng_range(s, 97);
    s->fx_core_hit_t = 0.0f;
    s->ciws_cooldown_sec = 0.0f;
    s->ciws_cooldown_left_sec = 0.0f;
    s->ciws_sweep_right_rad = -PI_F;
    s->ciws_sweep_left_rad = -PI_F * 0.5f;
    s->ciws_sweep_right_pitch = 0.04f;
    s->ciws_sweep_left_pitch = -0.04f;
    s->ciws_sweep_right_dir = 1.0f;
    s->ciws_sweep_left_dir = 1.0f;
    s->ciws_sweep_right_pitch_dir = 1.0f;
    s->ciws_sweep_left_pitch_dir = -1.0f;
    s->ciws_heat_right = 0.0f;
    s->ciws_heat_left = 0.0f;
    s->ciws_lock_right = 0.0f;
    s->ciws_lock_left = 0.0f;
    s->defense_kills = 0;
    s->defense_misses = 0;
    s->hunter_points = 0;
    s->attacker_points = 0;
    s->ciws_ammo_right = CIWS_AMMO_PER_GUN;
    s->ciws_ammo_left = CIWS_AMMO_PER_GUN;
    s->ciws_shots = 0;
    s->ciws_kills = 0;
    s->commit_attempts = 0;
    s->commit_launched = 0;
    s->commit_hold_detect = 0;
    s->commit_hold_class = 0;
    s->commit_hold_conf = 0;
    s->commit_hold_corridor = 0;
    s->commit_hold_los = 0;
    s->h_swept_hit_events = 0;
    s->h_reacquire_events = 0;
    s->h_overshoot_events = 0;
    s->h_opportunistic_switch_events = 0;
    s->attacker_evasion_events = 0;
    s->attacker_goal_detonations = 0;
    s->attacker_algo_ticks = 0;
    s->attacker_edgeai_overrides = 0;
    s->attacker_edgeai_fallbacks = 0;
    s->fail_range_mismatch = 0;
    s->fail_altitude_mismatch = 0;
    s->fail_overkill = 0;
    s->fail_ciws_misuse = 0;
    s->fail_manual_override_low_conf = 0;
    s->iff_ff_events = 0;
    s->iff_collateral_events = 0;
    s->fail_reason_ttl = 0.0f;
    s->fail_reason_text[0] = '\0';
    s->ciws_tracer_head = 0;
    for (i = 0; i < CIWS_TRACER_COUNT; ++i)
    {
        s->ciws_tracer_t[i] = 0.0f;
        s->ciws_tracer_g[i] = 0.0f;
        s->ciws_tracer_scale[i] = 1.0f;
    }
    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        s->hunter_stock[i] = HUNTER_STOCK_PER_TYPE;
    }
    init_launch_sectors(s);

    s->city_fire_count = 0;
    s->city_fire_head = 0;
    for (i = 0; i < CITY_FIRE_MAX; ++i)
    {
        s->city_fire_x[i] = 0.0f;
        s->city_fire_y[i] = 0.0f;
        s->city_fire_style[i] = 0;
        s->city_fire_intensity[i] = CITY_FIRE_INTENSITY_SMALL;
        s->city_fire_phase[i] = 0.0f;
        s->city_fire_last_profile[i] = 0xFFu;
        s->city_fire_last_frame[i] = 0xFFu;
    }

    for (i = 0; i < KILLER_COUNT; ++i)
    {
        s->fx_intercept_t[i] = 0.0f;
        s->fx_spawn_t[i] = 0.0f;
        s->fx_kill_t[i] = 0.0f;
        s->fx_blast_style[i] = BLAST_STYLE_SMALL_WHITE;
        s->k_dying_t[i] = 0.0f;
        s->killer_active[i] = 0;
        s->k_dying[i] = 0;
        s->k_tier[i] = 0;
        s->k_missed_by_hunter[i] = 0;
        s->k_serial[i] = 0;
        s->k_goal_x[i] = 0.0f;
        s->k_goal_y[i] = 0.0f;
        s->k_detect_conf[i] = 0.0f;
        s->k_class_conf[i] = 0.0f;
        s->k_threat_score[i] = 0.0f;
        s->k_payload_score[i] = 0.0f;
        s->k_survivability[i] = 0.0f;
        s->k_speed_est[i] = 0.0f;
        s->k_altitude_norm[i] = 0.0f;
        s->k_range_to_core[i] = 0.0f;
        s->k_eta_to_goal[i] = 0.0f;
        s->k_priority_rank[i] = 0;
        s->k_spawn_site[i] = 0;
        s->k_target_value_mod[i] = 0.0f;
        s->k_lane_pressure[i] = 0.0f;
        s->k_recommended_counter[i] = HUNTER_STING_II;
        s->k_detect_ok[i] = 0;
        s->k_class_ok[i] = 0;
        s->k_commit_ok[i] = 0;
        s->k_corridor_ok[i] = 0;
        s->k_los_ok[i] = 0;
        s->k_track_history[i] = 0.0f;
        s->k_noise[i] = 0.0f;
        s->k_commit_conf[i] = 0.0f;
        s->k_commit_reason[i] = COMMIT_REASON_NO_TARGET;
        s->k_last_src[i] = NULL;
        s->k_variant_seed[i] = 0;
    }
    for (i = 0; i < LANE_SITE_COUNT; ++i)
    {
        s->lane_pressure[i] = 0.0f;
    }

    reset_hunters(s);
    start_wave(s, 1);
    update_hunter_deck_ui(s);
    update_mode_button_label(s);

    hide_overlay(s);
    hide_quick_menu(s);
    set_diag_stage(s, "DBG:ROUND_RESET");
}

static void restart_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    reset_round(s);
}

static void mode_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_indev_t *indev;
    lv_point_t p;
    lv_area_t a;
    int left_half = 1;
    char body[320];

    if (code != LV_EVENT_CLICKED)
    {
        return;
    }

    lv_obj_get_coords(btn, &a);
    indev = lv_indev_get_act();
    if (indev != NULL)
    {
        lv_indev_get_point(indev, &p);
        left_half = (p.x <= ((a.x1 + a.x2) / 2));
    }

    if (left_half)
    {
        refresh_settings_menu(s);
    }
    else
    {
        (void)snprintf(body, sizeof(body),
                       "Left half: open Settings\n"
                       "Right half: open Help\n"
                       "Tap drone icon: select hunter\n"
                       "Tap attack drone: set priority target\n"
                       "Press START ARENA to run gameplay.");
        show_quick_menu(s, "Help", body);
        s->quick_menu_settings_active = 0;
    }
}

static void deck_pick_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    lv_obj_t *target;
    int i;

    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
    {
        return;
    }
    target = lv_event_get_target(e);
    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        if (target == s->deck_icon[i])
        {
            if (s->hunter_stock[i] <= 0)
            {
                note_failure(s, "Manual pick blocked: selected hunter out of stock", NULL);
                return;
            }
            s->manual_selected_hunter = i;
            s->manual_select_ttl = 5.0f;
            note_failure(s, "Manual hunter selected from icon bar", NULL);
            update_hunter_deck_ui(s);
            return;
        }
    }
}

static void target_pick_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    lv_obj_t *target;
    int k;

    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
    {
        return;
    }
    target = lv_event_get_target(e);
    for (k = 0; k < KILLER_COUNT; ++k)
    {
        if ((target == s->killers[k]) && s->killer_active[k])
        {
            s->manual_selected_target = k;
            s->manual_select_ttl = 5.0f;
            note_failure(s, "Manual target priority selected", NULL);
            return;
        }
    }
}

static void iff_toggle_cb(lv_event_t *e)
{
    drone_hunter_scene_t *s = (drone_hunter_scene_t *)lv_event_get_user_data(e);
    if (lv_event_get_code(e) != LV_EVENT_LONG_PRESSED)
    {
        return;
    }
    s->iff_advanced_mode = !s->iff_advanced_mode;
    if (!s->iff_advanced_mode)
    {
        s->iff_degraded = 0;
        s->iff_merged_tracks = 0;
        s->iff_recovery_ttl = 0.0f;
    }
    note_failure(s,
                 s->iff_advanced_mode ? "IFF advanced mode enabled" : "IFF advanced mode disabled",
                 NULL);
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
    float ciws_block_top_y = (float)s->arena_y + ((float)s->arena_h * (1.0f - CIWS_TOP_GRID_BLOCK_FRAC));
    float ciws_max_vertical = (float)s->arena_h * CIWS_MAX_VERTICAL_FRAC;
    float floor_y = combat_floor_y(s);
    float sweep_step = CIWS_SWEEP_SPEED_RAD * DT_SEC;
    float speed_mult = speed_pp_mult(s) * difficulty_attack_mult(s);

    phase_speed_gain *= speed_mult;

    s->ciws_cooldown_sec = clampf(s->ciws_cooldown_sec - DT_SEC, 0.0f, 2.0f);
    s->ciws_cooldown_left_sec = clampf(s->ciws_cooldown_left_sec - DT_SEC, 0.0f, 2.0f);
    s->ciws_heat_right = clampf(s->ciws_heat_right - (0.16f * DT_SEC), 0.0f, 1.0f);
    s->ciws_heat_left = clampf(s->ciws_heat_left - (0.16f * DT_SEC), 0.0f, 1.0f);
    s->ciws_lock_right = clampf(s->ciws_lock_right * 0.92f, 0.0f, 1.0f);
    s->ciws_lock_left = clampf(s->ciws_lock_left * 0.92f, 0.0f, 1.0f);
    s->ciws_sweep_right_rad += s->ciws_sweep_right_dir * sweep_step;
    s->ciws_sweep_left_rad += s->ciws_sweep_left_dir * sweep_step;
    s->ciws_sweep_right_pitch += s->ciws_sweep_right_pitch_dir * (CIWS_SWEEP_PITCH_SPEED * DT_SEC);
    s->ciws_sweep_left_pitch += s->ciws_sweep_left_pitch_dir * (CIWS_SWEEP_PITCH_SPEED * DT_SEC);

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

    if (s->ciws_sweep_right_pitch > CIWS_SWEEP_PITCH_MAX)
    {
        s->ciws_sweep_right_pitch = CIWS_SWEEP_PITCH_MAX;
        s->ciws_sweep_right_pitch_dir = -1.0f;
    }
    else if (s->ciws_sweep_right_pitch < -CIWS_SWEEP_PITCH_MAX)
    {
        s->ciws_sweep_right_pitch = -CIWS_SWEEP_PITCH_MAX;
        s->ciws_sweep_right_pitch_dir = 1.0f;
    }

    if (s->ciws_sweep_left_pitch > CIWS_SWEEP_PITCH_MAX)
    {
        s->ciws_sweep_left_pitch = CIWS_SWEEP_PITCH_MAX;
        s->ciws_sweep_left_pitch_dir = -1.0f;
    }
    else if (s->ciws_sweep_left_pitch < -CIWS_SWEEP_PITCH_MAX)
    {
        s->ciws_sweep_left_pitch = -CIWS_SWEEP_PITCH_MAX;
        s->ciws_sweep_left_pitch_dir = 1.0f;
    }

    for (k = 0; k < KILLER_COUNT; ++k)
    {
        if (!s->killer_active[k])
        {
            continue;
        }
        if (!isfinite(s->kx[k]) || !isfinite(s->ky[k]) || !isfinite(s->kvx[k]) || !isfinite(s->kvy[k]) || !isfinite(s->k_heading[k]))
        {
            set_killer_hidden(s, k);
            respawn_killer(s, k, -1);
            continue;
        }
        if (s->k_dying[k])
        {
            s->k_dying_t[k] = clampf(s->k_dying_t[k] - DT_SEC, 0.0f, FX_SHAHED_DEATH_SEC);
            if (s->k_dying_t[k] <= 0.0f)
            {
                s->k_dying[k] = 0;
                set_killer_hidden(s, k);
                respawn_killer(s, k, -1);
            }
            continue;
        }

        float dir_x = s->k_goal_x[k] - s->kx[k];
        float dir_y = s->k_goal_y[k] - s->ky[k];
        float d = sqrtf((dir_x * dir_x) + (dir_y * dir_y));
        float noise_x = sinf(s->t * (2.0f + 0.4f * (float)k) + (float)k);
        float noise_y = cosf(s->t * (1.7f + 0.2f * (float)k) + (float)k);
        float evade_x = 0.0f;
        float evade_y = 0.0f;
        float goal_x = 0.0f;
        float goal_y = 0.0f;
        float algo_x;
        float algo_y;
        float move_x;
        float move_y;
        float evade_weight = 0.0f;
        int hh;
        int use_edge = (s->team_ctrl[1] == CTRL_EDGEAI) && s->npu_enabled;
        float evade_r = clampf((float)s->arena_w * ATTACK_EVADE_RADIUS_FRAC, 26.0f, 130.0f);
        float evade_r2 = evade_r * evade_r;

        if (d > 1.0f)
        {
            dir_x /= d;
            dir_y /= d;
        }
        goal_x = dir_x;
        goal_y = dir_y;
        (void)normalize_vec2(&goal_x, &goal_y);

        for (hh = 0; hh < HUNTER_COUNT; ++hh)
        {
            float hx;
            float hy;
            float dx;
            float dy;
            float hd2;
            float hd;
            float away_x;
            float away_y;
            float pressure;
            if (!s->hunter_loaded[hh] || (s->h_falling[hh] != 0))
            {
                continue;
            }
            hx = s->hx[hh];
            hy = s->hy[hh];
            dx = s->kx[k] - hx;
            dy = s->ky[k] - hy;
            hd2 = (dx * dx) + (dy * dy);
            if (hd2 > evade_r2)
            {
                continue;
            }
            hd = sqrtf(clampf(hd2, 0.0001f, 1.0e9f));
            away_x = dx / hd;
            away_y = dy / hd;
            pressure = 1.0f - clampf(hd / evade_r, 0.0f, 1.0f);
            evade_x += away_x * (0.88f + (pressure * 0.62f));
            evade_y += away_y * (0.88f + (pressure * 0.62f));
            if (use_edge)
            {
                float hx_future = hx + (s->hvx[hh] * 12.0f);
                float hy_future = hy + (s->hvy[hh] * 12.0f);
                float fx = s->kx[k] - hx_future;
                float fy = s->ky[k] - hy_future;
                float fd2 = (fx * fx) + (fy * fy);
                float fd = sqrtf(clampf(fd2, 0.0001f, 1.0e9f));
                evade_x += (fx / fd) * (pressure * 0.55f);
                evade_y += (fy / fd) * (pressure * 0.55f);
            }
            evade_weight += pressure;
        }
        if (normalize_vec2(&evade_x, &evade_y))
        {
            float evasive_mix = clampf(0.35f + (evade_weight * 0.95f), 0.0f, 1.45f);
            algo_x = (goal_x * (1.15f - (evasive_mix * 0.25f))) + (evade_x * evasive_mix);
            algo_y = (goal_y * (1.15f - (evasive_mix * 0.25f))) + (evade_y * evasive_mix);
            if (!normalize_vec2(&algo_x, &algo_y))
            {
                algo_x = goal_x;
                algo_y = goal_y;
            }
            if (evade_weight > 0.45f)
            {
                s->attacker_evasion_events++;
            }
        }
        else
        {
            algo_x = goal_x;
            algo_y = goal_y;
        }
        move_x = algo_x;
        move_y = algo_y;
        s->attacker_algo_ticks++;
        if (use_edge)
        {
            float edge_x = algo_x;
            float edge_y = algo_y;
            float lane_mix = (s->attack_strategy_live == ATTACK_STRATEGY_FLANK_PRESSURE) ? 0.20f : 0.10f;
            float lane_bias = (((k + s->wave_idx) & 0x1) == 0) ? -1.0f : 1.0f;
            edge_x += -algo_y * lane_mix * lane_bias;
            edge_y += algo_x * lane_mix * lane_bias;
            edge_x += evade_x * clampf(evade_weight * 0.62f, 0.0f, 1.10f);
            edge_y += evade_y * clampf(evade_weight * 0.62f, 0.0f, 1.10f);
            if (normalize_vec2(&edge_x, &edge_y))
            {
                move_x = edge_x;
                move_y = edge_y;
                s->attacker_edgeai_overrides++;
            }
            else
            {
                s->attacker_edgeai_fallbacks++;
            }
        }

        if (s->ktype[k] == TARGET_FIXED_WING)
        {
            float desired = atan2f(move_y, move_x) + (noise_y * 0.35f);
            float delta;
            float turn_limit = (phase == 2) ? 0.038f : 0.030f;
            float speed = attack_speed_score(s, k) * phase_speed_gain;
            float goal_heading = atan2f(goal_y, goal_x);
            float goal_delta = desired - goal_heading;
            float vx;
            float vy;
            float progress;
            float min_progress = speed * 0.16f;

            delta = wrap_angle_pi(desired - s->k_heading[k]);

            /* Fixed-wing rule: allow evasive banking but avoid commanding a reverse-away heading. */
            goal_delta = wrap_angle_pi(goal_delta);
            if (fabsf(goal_delta) > 1.28f)
            {
                desired = goal_heading + clampf(goal_delta, -1.28f, 1.28f);
                delta = wrap_angle_pi(desired - s->k_heading[k]);
            }

            delta = clampf(delta, -turn_limit, turn_limit);
            s->k_heading[k] += delta;
            vx = (cosf(s->k_heading[k]) * speed) + (noise_x * 0.12f);
            vy = (sinf(s->k_heading[k]) * speed) + (noise_y * 0.12f);
            progress = (vx * goal_x) + (vy * goal_y);
            if (progress < min_progress)
            {
                float steer_goal = wrap_angle_pi(goal_heading - s->k_heading[k]);
                s->k_heading[k] += clampf(steer_goal, -turn_limit, turn_limit);
                vx = cosf(s->k_heading[k]) * speed;
                vy = sinf(s->k_heading[k]) * speed;
            }
            s->kvx[k] = vx;
            s->kvy[k] = vy;
        }
        else
        {
            float speed = (attack_speed_score(s, k) * 0.72f) * phase_speed_gain;
            s->kvx[k] = (s->kvx[k] * 0.84f) + ((move_x * speed) * 0.14f) + (noise_x * 0.05f);
            s->kvy[k] = (s->kvy[k] * 0.84f) + ((move_y * speed) * 0.14f) + (noise_y * 0.05f);
        }

        s->kx[k] += s->kvx[k];
        s->ky[k] += s->kvy[k];

        s->kx[k] = clampf(s->kx[k], 6.0f, (float)lv_obj_get_width(s->screen) - 6.0f);
        s->ky[k] = clampf(s->ky[k], (float)s->arena_y + 6.0f, floor_y);

        {
            /* Detect -> classify -> commit confidence model feeding threat score. */
            float max_range = sqrtf(dist2((float)s->arena_x, (float)s->arena_y,
                                          (float)(s->arena_x + s->arena_w), (float)(s->arena_y + s->arena_h)));
            float range_now = sqrtf(dist2(s->kx[k], s->ky[k], core_x, core_y));
            float proximity = 1.0f + (2.0f * (1.0f - clampf(range_now / clampf(max_range, 1.0f, 5000.0f), 0.0f, 1.0f)));
            float altitude_norm = clampf((s->ky[k] - (float)s->arena_y) / clampf((float)s->arena_h, 1.0f, 4000.0f), 0.0f, 1.0f);
            float clutter = (s->ktype[k] == TARGET_FPV) ? (1.0f - altitude_norm) * 0.26f : altitude_norm * 0.12f;
            float detect = clampf(0.72f + (proximity * 0.11f) - clutter, 0.10f, 0.98f);
            float classify = clampf(detect - ((s->ktype[k] == TARGET_FPV) ? 0.15f : 0.08f), 0.08f, 0.95f);

            s->k_detect_conf[k] = (s->k_detect_conf[k] * 0.78f) + (detect * 0.22f);
            s->k_class_conf[k] = (s->k_class_conf[k] * 0.76f) + (classify * 0.24f);
            s->k_threat_score[k] = s->k_payload_score[k] *
                                   proximity *
                                   s->k_survivability[k] *
                                   clampf((s->k_detect_conf[k] + s->k_class_conf[k]) * 0.5f, 0.1f, 1.0f) *
                                   ((s->ktype[k] == TARGET_FIXED_WING) ? 1.15f : 1.00f);
        }

        if ((s->ciws_ammo_right > 0) &&
            (s->ciws_cooldown_sec <= 0.0f) &&
            (s->ky[k] >= ciws_block_top_y) &&
            ((ciws_y - s->ky[k]) <= ciws_max_vertical) &&
            ciws_target_in_sweep(s, ciws_x, ciws_y, s->ciws_sweep_right_rad, s->ciws_sweep_right_pitch, ciws_range, s->kx[k], s->ky[k]))
        {
            float lock_q = ciws_lock_quality(s, k, ciws_x, ciws_y, s->ciws_sweep_right_rad, s->ciws_sweep_right_pitch, ciws_range);
            if (lock_q > s->ciws_lock_right)
            {
                s->ciws_lock_right = lock_q;
            }
            if (ciws_fire_at(s, k, ciws_x, ciws_y, s->ciws_sweep_right_rad, s->ciws_sweep_right_pitch, ciws_range, lock_q, &s->ciws_cooldown_sec, &s->ciws_ammo_right))
            {
                continue;
            }
            s->ciws_heat_right = clampf(s->ciws_heat_right + (0.010f + ((1.0f - lock_q) * 0.020f)), 0.0f, 1.0f);
            s->ciws_cooldown_sec = clampf(s->ciws_cooldown_sec + (s->ciws_heat_right * 0.040f), 0.0f, 2.0f);
        }
        if ((s->ciws_ammo_left > 0) &&
            (s->ciws_cooldown_left_sec <= 0.0f) &&
            (s->ky[k] >= ciws_block_top_y) &&
            ((ciws_left_y - s->ky[k]) <= ciws_max_vertical) &&
            ciws_target_in_sweep(s, ciws_left_x, ciws_left_y, s->ciws_sweep_left_rad, s->ciws_sweep_left_pitch, ciws_range, s->kx[k], s->ky[k]))
        {
            float lock_q = ciws_lock_quality(s, k, ciws_left_x, ciws_left_y, s->ciws_sweep_left_rad, s->ciws_sweep_left_pitch, ciws_range);
            if (lock_q > s->ciws_lock_left)
            {
                s->ciws_lock_left = lock_q;
            }
            if (ciws_fire_at(s, k, ciws_left_x, ciws_left_y, s->ciws_sweep_left_rad, s->ciws_sweep_left_pitch, ciws_range, lock_q, &s->ciws_cooldown_left_sec, &s->ciws_ammo_left))
            {
                continue;
            }
            s->ciws_heat_left = clampf(s->ciws_heat_left + (0.010f + ((1.0f - lock_q) * 0.020f)), 0.0f, 1.0f);
            s->ciws_cooldown_left_sec = clampf(s->ciws_cooldown_left_sec + (s->ciws_heat_left * 0.040f), 0.0f, 2.0f);
        }

        if (dist2(s->kx[k], s->ky[k], s->k_goal_x[k], s->k_goal_y[k]) < 324.0f)
        {
            set_diag_stage(s, "DBG:ATTACKER_GOAL_HIT");
            s->attack_leaked++;
            s->attacker_points++;
            s->attacker_goal_detonations++;
            s->core_hp = (s->core_hp > 0) ? (s->core_hp - 1) : 0;
            s->fx_core_hit_t = FX_CORE_HIT_SEC;
            s->fx_intercept_t[k] = FX_INTERCEPT_SEC;
            s->fx_intercept_x[k] = s->k_goal_x[k];
            s->fx_intercept_y[k] = s->k_goal_y[k];
            add_city_fire(s, s->k_goal_x[k], s->k_goal_y[k], CITY_FIRE_INTENSITY_BIG);
            set_diag_stage(s, "DBG:ATTACKER_RESPAWN");
            respawn_killer(s, k, -1);
        }
    }

    refresh_target_metrics(s, core_x, core_y);
    if (s->iff_advanced_mode)
    {
        float noise_avg = 0.0f;
        int active_n = 0;
        for (k = 0; k < KILLER_COUNT; ++k)
        {
            if (s->killer_active[k])
            {
                noise_avg += s->k_noise[k];
                active_n++;
            }
        }
        if (active_n > 0)
        {
            noise_avg /= (float)active_n;
        }
        s->iff_merged_tracks = iff_tracks_merged(s, core_x, core_y);
        if ((noise_avg > 0.42f) || (s->fail_ciws_misuse > 0) || (s->iff_recovery_ttl > 0.0f))
        {
            s->iff_degraded = 1;
        }
        else
        {
            s->iff_degraded = 0;
        }
    }
    else
    {
        s->iff_merged_tracks = 0;
        s->iff_degraded = 0;
    }
}

static void update_hunter(drone_hunter_scene_t *s, int h, float core_x, float core_y)
{
    int target = -1;
    float ground_y = combat_floor_y(s);
    float horizon_y = (float)s->arena_y + 4.0f;
    float offscreen_bottom_y = (float)lv_obj_get_height(s->screen) + 20.0f;
    const hunter_profile_t *p;
    float prev_hx = s->hx[h];
    float prev_hy = s->hy[h];

    if (!isfinite(s->hx[h]) || !isfinite(s->hy[h]) || !isfinite(s->hvx[h]) || !isfinite(s->hvy[h]) || !isfinite(s->h_heading[h]))
    {
        s->hx[h] = hunter_regroup_x(s, h, core_x);
        s->hy[h] = hunter_regroup_y(s, h);
        s->hvx[h] = 0.0f;
        s->hvy[h] = 0.0f;
        s->h_heading[h] = -PI_F * 0.5f;
        s->h_falling[h] = 0;
        s->h_target_idx[h] = -1;
        s->h_target_serial[h] = 0;
    }

    if (!s->hunter_loaded[h] && !any_hunter_stock_remaining(s))
    {
        return;
    }

    if (s->killer_active[0] && s->killer_active[1])
    {
        if (s->team_ctrl[h] == CTRL_EDGEAI)
        {
            float eta0 = sqrtf(dist2(s->kx[0], s->ky[0], core_x, core_y)) / clampf(attack_speed_score(s, 0), 0.9f, 3.4f);
            float eta1 = sqrtf(dist2(s->kx[1], s->ky[1], core_x, core_y)) / clampf(attack_speed_score(s, 1), 0.9f, 3.4f);
            target = (eta1 < eta0) ? 1 : 0;
        }
        else
        {
            float d0 = dist2(s->hx[h], s->hy[h], s->kx[0], s->ky[0]);
            float d1 = dist2(s->hx[h], s->hy[h], s->kx[1], s->ky[1]);
            target = (d1 < d0) ? 1 : 0;
        }
    }
    else if ((s->killer_active[0] && !s->k_dying[0]) ||
             (s->killer_active[1] && !s->k_dying[1]))
    {
        target = (s->killer_active[0] && !s->k_dying[0]) ? 0 : 1;
    }
    else
    {
        if (s->hunter_loaded[h])
        {
            /* Egress to horizon instead of pinning at deck edge. */
            s->h_falling[h] = 2;
            s->hvy[h] = -1.15f;
            s->hvx[h] *= 0.60f;
        }
        else
        {
            float regroup_x = hunter_regroup_x(s, h, core_x);
            float regroup_y = hunter_regroup_y(s, h);
            s->hvx[h] = (s->hvx[h] * 0.82f) + ((regroup_x - s->hx[h]) * 0.018f);
            s->hvy[h] = (s->hvy[h] * 0.82f) + ((regroup_y - s->hy[h]) * 0.018f);
            s->hx[h] = clampf(s->hx[h] + (s->hvx[h] * 1.2f), (float)s->arena_x + 10.0f, (float)(s->arena_x + s->arena_w - 10));
            s->hy[h] = clampf(s->hy[h] + (s->hvy[h] * 1.2f), (float)s->arena_y + 10.0f, ground_y);
        }
        return;
    }

    if ((h == 0) &&
        (s->manual_selected_target >= 0) &&
        (s->manual_selected_target < KILLER_COUNT) &&
        s->killer_active[s->manual_selected_target] &&
        !s->k_dying[s->manual_selected_target])
    {
        target = s->manual_selected_target;
    }

    /* Commit gate: launch only when staged pipeline marks target as committable. */
    if (!s->hunter_loaded[h])
    {
        s->commit_attempts++;
        if (!s->k_commit_ok[target])
        {
            switch (s->k_commit_reason[target])
            {
                case COMMIT_REASON_DETECT_LOW:
                    s->commit_hold_detect++;
                    break;
                case COMMIT_REASON_CLASS_LOW:
                    s->commit_hold_class++;
                    break;
                case COMMIT_REASON_CORRIDOR:
                    s->commit_hold_corridor++;
                    break;
                case COMMIT_REASON_LOS_BLOCK:
                    s->commit_hold_los++;
                    break;
                case COMMIT_REASON_COMMIT_LOW:
                default:
                    s->commit_hold_conf++;
                    break;
            }
            /* Keep demo playable: launch anyway for urgent or high-threat tracks. */
            if ((s->k_eta_to_goal[target] > 3.2f) && (s->k_threat_score[target] < 1.25f))
            {
                return;
            }
        }
        s->commit_launched++;
    }

    if (!s->hunter_loaded[h])
    {
        float tx;
        float ty;
        float dx;
        float dy;
        float d;
        float launch_speed;
        int launch_sector;
        hunter_type_t pick;
        int fallback_reason = 0;
        float auto_pick_score = -9999.0f;
        int pick_manual = 0;

        if ((h == 0) &&
            (s->manual_selected_hunter >= 0) &&
            (s->manual_selected_hunter < HUNTER_TYPE_COUNT) &&
            (s->hunter_stock[s->manual_selected_hunter] > 0))
        {
            pick = sanitize_hunter_pick((hunter_type_t)s->manual_selected_hunter);
            pick_manual = 1;
        }
        else
        {
            pick = sanitize_hunter_pick(choose_conservative_hunter(s, target, h, &auto_pick_score));
        }
        if (!pick_manual && (auto_pick_score < -0.30f))
        {
            hunter_type_t rec = sanitize_hunter_pick(recommended_hunter_for_target(s, target));
            if (s->hunter_stock[(int)rec] > 0)
            {
                pick = rec;
                fallback_reason = 1;
            }
            else
            {
                hunter_type_t stocked = best_stocked_hunter_type(s);
                if (s->hunter_stock[(int)stocked] > 0)
                {
                    pick = stocked;
                    fallback_reason = 2;
                }
            }
        }
        if (s->hunter_stock[(int)pick] <= 0)
        {
            hunter_type_t rec = sanitize_hunter_pick(recommended_hunter_for_target(s, target));
            if (s->hunter_stock[(int)rec] > 0)
            {
                pick = rec;
                fallback_reason = 3;
            }
            else
            {
                hunter_type_t stocked = best_stocked_hunter_type(s);
                if (s->hunter_stock[(int)stocked] > 0)
                {
                    pick = stocked;
                    fallback_reason = 4;
                }
            }
        }
        if (s->hunter_stock[(int)pick] <= 0)
        {
            note_failure(s, "No-stock fallback failed: all hunter classes depleted", NULL);
            return;
        }
        launch_sector = choose_launch_sector_for_target(s, s->kx[target], s->ky[target]);
        if (launch_sector < 0)
        {
            note_failure(s, "Launch hold: no launch sector has remaining stock", NULL);
            return;
        }
        if (fallback_reason == 1)
        {
            note_failure(s, "No-fit fallback: switched to recommended counter", NULL);
        }
        else if (fallback_reason == 2)
        {
            note_failure(s, "No-fit fallback: switched to highest-stock hunter", NULL);
        }
        else if (fallback_reason == 3)
        {
            note_failure(s, "No-stock fallback: switched to recommended counter", NULL);
        }
        else if (fallback_reason == 4)
        {
            note_failure(s, "No-stock fallback: switched to highest-stock hunter", NULL);
        }
        s->h_type[h] = pick;
        s->hunter_stock[(int)pick]--;
        s->launch_sector_stock[launch_sector]--;
        s->h_launch_sector[h] = launch_sector;
        s->hunter_loaded[h] = 1;
        s->h_falling[h] = 0;
        s->h_target_idx[h] = target;
        s->h_target_serial[h] = s->k_serial[target];
        s->h_reselect_sec[h] = 0.20f; /* minimum visible flight time before intercept resolve */
        s->h_lock_persist_t[h] = (s->team_ctrl[h] == CTRL_EDGEAI) ? H_LOCK_PERSIST_EDGE_SEC : H_LOCK_PERSIST_ALGO_SEC;
        s->h_switch_cooldown_t[h] = 0.0f;
        s->h_launch_penalty[h] = 0.0f;
        s->h_last_target_d2[h] = 0.0f;
        s->h_overshoot_cooldown[h] = 0.0f;
        s->hx[h] = s->launch_sector_x[launch_sector];
        s->hy[h] = s->launch_sector_y[launch_sector];
        update_hunter_deck_ui(s);
        apply_hunter_profile(s, h);
        {
            float km_per_px = MAP_SIZE_KM / clampf((float)s->arena_w, 20.0f, 4000.0f);
            float launch_range_km = sqrtf(dist2(s->hx[h], s->hy[h], s->kx[target], s->ky[target])) * km_per_px;
            float range_cap_km = hunter_range_km(s->h_type[h]);
            int overkill_pick = hunter_is_high_speed_tier(s->h_type[h]) &&
                                (s->k_threat_score[target] < 1.40f) &&
                                has_unassigned_higher_threat(s, target);
            int low_conf_override = (!s->k_commit_ok[target]) &&
                                    (s->k_commit_conf[target] < 0.48f) &&
                                    ((s->team_ctrl[h] == CTRL_ALGO) ||
                                     ((h == 0) && (s->manual_selected_hunter >= 0)));

            if (launch_range_km > (range_cap_km * 1.05f))
            {
                s->h_launch_penalty[h] += 0.22f;
                note_failure(s, "Range mismatch: selected hunter out of ideal envelope", &s->fail_range_mismatch);
            }
            if (hunter_is_low_alt_mismatch(s->h_type[h], s->ktype[target], s->k_altitude_norm[target]))
            {
                s->h_launch_penalty[h] += 0.20f;
                note_failure(s, "Altitude mismatch: fixed-wing on low erratic target", &s->fail_altitude_mismatch);
            }
            if (overkill_pick)
            {
                s->h_launch_penalty[h] += 0.14f;
                note_failure(s, "Overkill allocation: premium hunter burned on low threat", &s->fail_overkill);
            }
            if (low_conf_override)
            {
                s->h_launch_penalty[h] += 0.12f;
                note_failure(s, "Low-confidence override: risky launch committed", &s->fail_manual_override_low_conf);
            }
            s->h_launch_penalty[h] = clampf(s->h_launch_penalty[h], 0.0f, 0.54f);
        }

        p = &g_hunter_profiles[(int)s->h_type[h]];
        if (s->team_ctrl[h] == CTRL_EDGEAI)
        {
            float lead_frames = hunter_guidance_lead_frames(s->h_type[h], s->team_ctrl[h], p->lead_gain);
            tx = s->kx[target] + (s->kvx[target] * lead_frames);
            ty = s->ky[target] + (s->kvy[target] * lead_frames);
            launch_speed = p->speed * hunter_guidance_speed_mult(s->h_type[h], s->team_ctrl[h]);
        }
        else
        {
            float lead_frames = hunter_guidance_lead_frames(s->h_type[h], s->team_ctrl[h], p->lead_gain);
            tx = s->kx[target] + (s->kvx[target] * lead_frames);
            ty = s->ky[target] + (s->kvy[target] * lead_frames);
            launch_speed = p->speed * hunter_guidance_speed_mult(s->h_type[h], s->team_ctrl[h]);
        }
        launch_speed *= difficulty_defense_mult(s) * speed_pp_mult(s);
        dx = tx - s->hx[h];
        dy = ty - s->hy[h];
        d = sqrtf((dx * dx) + (dy * dy));
        if (d < 1.0f)
        {
            d = 1.0f;
        }
        s->hvx[h] = (dx / d) * launch_speed;
        s->hvy[h] = (dy / d) * launch_speed;
    }

    p = &g_hunter_profiles[(int)s->h_type[h]];
    if (s->h_lock_persist_t[h] > 0.0f)
    {
        s->h_lock_persist_t[h] = clampf(s->h_lock_persist_t[h] - DT_SEC, 0.0f, 2.0f);
    }
    if (s->h_switch_cooldown_t[h] > 0.0f)
    {
        s->h_switch_cooldown_t[h] = clampf(s->h_switch_cooldown_t[h] - DT_SEC, 0.0f, 2.0f);
    }
    if (s->h_overshoot_cooldown[h] > 0.0f)
    {
        s->h_overshoot_cooldown[h] = clampf(s->h_overshoot_cooldown[h] - DT_SEC, 0.0f, 2.0f);
    }
    if (s->h_falling[h] == 1)
    {
        s->hvy[h] += 0.22f;
        s->hvx[h] *= 0.992f;
    }
    else if (s->h_falling[h] == 2)
    {
        float center_pull = (core_x - s->hx[h]) * 0.012f;
        s->hvx[h] = (s->hvx[h] * 0.96f) + center_pull;
        s->hvy[h] = (s->hvy[h] * 0.95f) - 0.13f;
        if (s->hvy[h] < -2.2f)
        {
            s->hvy[h] = -2.2f;
        }
    }
    else
    {
        int committed_pre = s->h_target_idx[h];
        if ((committed_pre >= 0) &&
            s->killer_active[committed_pre] &&
            !s->k_dying[committed_pre] &&
            (s->k_serial[committed_pre] == s->h_target_serial[h]))
        {
            if ((s->h_reselect_sec[h] <= 0.0f) &&
                (s->h_lock_persist_t[h] <= 0.0f) &&
                (s->h_switch_cooldown_t[h] <= 0.0f))
            {
                int opportunistic = choose_hunter_opportunistic_target(s, h, committed_pre, p);
                if (opportunistic >= 0)
                {
                    s->h_target_idx[h] = opportunistic;
                    s->h_target_serial[h] = s->k_serial[opportunistic];
                    s->h_reselect_sec[h] = 0.10f;
                    s->h_switch_cooldown_t[h] = (s->team_ctrl[h] == CTRL_EDGEAI) ? H_SWITCH_COOLDOWN_EDGE : H_SWITCH_COOLDOWN_ALGO;
                    s->h_lock_persist_t[h] = (s->team_ctrl[h] == CTRL_EDGEAI) ? H_LOCK_PERSIST_EDGE_SEC : H_LOCK_PERSIST_ALGO_SEC;
                    s->h_opportunistic_switch_events++;
                    committed_pre = opportunistic;
                }
            }
            steer_hunter_toward_target(s, h, committed_pre, p);
        }
    }

    s->hx[h] += s->hvx[h];
    s->hy[h] += s->hvy[h];

    /* Prevent top-edge skating: if a hunter reaches the top arena boundary while climbing,
     * force horizon egress so it shrinks/fades out instead of sliding along the line. */
    if ((s->h_falling[h] == 0) &&
        (s->hy[h] <= ((float)s->arena_y + 8.0f)) &&
        (s->hvy[h] < -0.05f))
    {
        s->h_falling[h] = 2;
        s->h_target_idx[h] = -1;
        s->h_target_serial[h] = 0;
        s->h_reselect_sec[h] = 0.0f;
        s->hvx[h] *= 0.28f;
        s->hvy[h] = -1.35f;
    }
    s->hx[h] = clampf(s->hx[h], (float)s->arena_x + 6.0f, (float)(s->arena_x + s->arena_w - 6));
    if (s->h_falling[h] == 2)
    {
        s->hy[h] = clampf(s->hy[h], (float)s->arena_y - 24.0f, ground_y + 6.0f);
    }
    else if (s->h_falling[h] == 1)
    {
        s->hy[h] = clampf(s->hy[h], (float)s->arena_y + 6.0f, offscreen_bottom_y + 8.0f);
    }
    else
    {
        s->hy[h] = clampf(s->hy[h], (float)s->arena_y + 6.0f, ground_y);
    }
    if (s->h_reselect_sec[h] > 0.0f)
    {
        s->h_reselect_sec[h] = clampf(s->h_reselect_sec[h] - DT_SEC, 0.0f, 1.0f);
    }

    if ((s->hvx[h] * s->hvx[h]) + (s->hvy[h] * s->hvy[h]) > 0.006f)
    {
        s->h_heading[h] = atan2f(s->hvy[h], s->hvx[h]);
    }

    if (!s->h_falling[h])
    {
        int committed = s->h_target_idx[h];
        int target_lost = (committed < 0) ||
                          (!s->killer_active[committed]) ||
                          (s->k_serial[committed] != s->h_target_serial[h]);

        if (target_lost)
        {
            int reacquire = choose_hunter_reacquire_target(s, h, committed);
            if (reacquire >= 0)
            {
                s->h_target_idx[h] = reacquire;
                s->h_target_serial[h] = s->k_serial[reacquire];
                s->h_reselect_sec[h] = 0.08f;
                s->h_lock_persist_t[h] = (s->team_ctrl[h] == CTRL_EDGEAI) ? H_LOCK_PERSIST_EDGE_SEC : H_LOCK_PERSIST_ALGO_SEC;
                s->h_switch_cooldown_t[h] = (s->team_ctrl[h] == CTRL_EDGEAI) ? H_SWITCH_COOLDOWN_EDGE : H_SWITCH_COOLDOWN_ALGO;
                s->h_last_target_d2[h] = 0.0f;
                s->h_reacquire_events++;
                note_failure(s, "Hunter reacquire: switched to nearby active threat", NULL);
            }
            else
            {
                s->defense_misses++;
                note_failure(s, "Hunter miss: target lost and no viable reacquire", NULL);
                s->h_falling[h] = 1;
                s->h_last_target_d2[h] = 0.0f;
                if (s->hvy[h] < 0.0f)
                {
                    s->hvy[h] *= 0.35f;
                }
                s->hvy[h] += 0.18f;
            }
        }
        else
        {
            float r2 = p->kill_radius * p->kill_radius;
            float d2_now = dist2(s->hx[h], s->hy[h], s->kx[committed], s->ky[committed]);
            float d2_swept = segment_point_dist2(prev_hx, prev_hy, s->hx[h], s->hy[h], s->kx[committed], s->ky[committed]);
            int in_kill_window = ((d2_now < r2) || (d2_swept < r2)) ? 1 : 0;

            if ((s->h_reselect_sec[h] <= 0.0f) &&
                (s->h_last_target_d2[h] > 0.0f) &&
                (d2_now > s->h_last_target_d2[h]) &&
                (s->h_last_target_d2[h] < (r2 * 1.80f)) &&
                (s->h_overshoot_cooldown[h] <= 0.0f))
            {
                s->h_overshoot_events++;
                s->h_overshoot_cooldown[h] = 0.70f;
            }
            s->h_last_target_d2[h] = d2_now;

            if ((s->h_reselect_sec[h] <= 0.0f) && in_kill_window)
            {
                float cap = hunter_capability_score(s->h_type[h]);
                float req = threat_required_score(s, committed);
                float p_kill = clampf(0.34f + ((cap - req) * 0.22f), 0.12f, 0.93f);
                float roll = 0.5f + (0.5f * sinf((s->t * 2.3f) + (float)(h * 7 + committed * 11)));
                int manual_override = ((h == 0) && ((s->manual_selected_hunter >= 0) || (s->manual_selected_target >= 0))) ? 1 : 0;
                int low_conf = (s->k_commit_conf[committed] < 0.45f) ? 1 : 0;
                int ff_gate = (s->iff_advanced_mode && s->iff_degraded && s->iff_merged_tracks && manual_override && low_conf) ? 1 : 0;
                p_kill = clampf(p_kill - s->h_launch_penalty[h], 0.05f, 0.95f);

                if ((d2_now >= r2) && (d2_swept < r2))
                {
                    s->h_swept_hit_events++;
                }

                if (ff_gate)
                {
                    float ff_roll = 0.5f + (0.5f * cosf((s->t * 2.9f) + (float)(h * 13 + committed * 3)));
                    if (ff_roll < 0.36f)
                    {
                        float hx = s->hx[h];
                        float hy = s->hy[h];
                        s->iff_ff_events++;
                        s->iff_collateral_events++;
                        s->defense_misses++;
                        s->attacker_points++;
                        s->core_hp = (s->core_hp > 0) ? (s->core_hp - 1) : 0;
                        s->iff_recovery_ttl = 4.0f;
                        add_city_fire(s, hx, hy, CITY_FIRE_INTENSITY_SMALL);
                        note_failure(s, "IFF failure: blue-on-blue event, recovery in progress", NULL);

                        s->hunter_loaded[h] = 0;
                        s->h_falling[h] = 0;
                        s->h_target_idx[h] = -1;
                        s->h_target_serial[h] = 0;
                        s->h_reselect_sec[h] = 0.10f;
                        s->h_last_target_d2[h] = 0.0f;
                        s->h_lock_persist_t[h] = 0.0f;
                        s->h_switch_cooldown_t[h] = 0.0f;
                        s->hvx[h] = 0.0f;
                        s->hvy[h] = 0.0f;
                        s->hx[h] = hunter_regroup_x(s, h, core_x);
                        s->hy[h] = hunter_regroup_y(s, h);
                        lv_obj_add_flag(s->hunters[h], LV_OBJ_FLAG_HIDDEN);
                        return;
                    }
                }

                if (roll <= p_kill)
                {
                    int pts = (s->ktype[committed] == TARGET_FIXED_WING) ? p->points_fixed : p->points_fpv;
                    float hit_x;
                    float hit_y;
                    int shahed_kill = target_is_shahed_visual(s, committed);
                    int blast_style = target_blast_style_for_visual(s, committed);
                    set_diag_stage(s, shahed_kill ? "DBG:HUNTER_KILL_SHAHED" : "DBG:HUNTER_KILL_OTHER");

                    /* Anchor FX to the rendered target center (post-transform). */
                    get_obj_visual_center_in_parent(s->killers[committed], s->arena, &hit_x, &hit_y);
                    s->team_score[h] += pts;
                    s->attack_destroyed++;
                    s->defense_kills++;
                    s->hunter_points++;

                    s->fx_blast_style[committed] = blast_style;
                    s->fx_kill_t[committed] = shahed_kill ? (FX_KILL_SEC * 1.40f) : FX_KILL_SEC;
                    s->fx_kill_x[committed] = hit_x;
                    s->fx_kill_y[committed] = hit_y;
                    if (shahed_kill)
                    {
                        /* Shahed kills should visibly erupt before despawn. */
                        s->fx_intercept_t[committed] = FX_INTERCEPT_SEC;
                        s->fx_intercept_x[committed] = hit_x;
                        s->fx_intercept_y[committed] = hit_y;
                        s->k_dying[committed] = 1;
                        s->k_dying_t[committed] = FX_SHAHED_DEATH_SEC;
                        s->k_threat_score[committed] = 0.0f;
                        s->k_commit_ok[committed] = 0;
                        s->k_detect_ok[committed] = 0;
                        s->k_class_ok[committed] = 0;
                        s->k_corridor_ok[committed] = 0;
                        s->k_los_ok[committed] = 0;
                        s->kvx[committed] = 0.0f;
                        s->kvy[committed] = 0.0f;
                        s->k_heading[committed] = -PI_F * 0.5f;
                    }
                    else
                    {
                        set_killer_hidden(s, committed);
                    }
                    s->hunter_loaded[h] = 0;
                    s->h_falling[h] = 0;
                    s->h_target_idx[h] = -1;
                    s->h_target_serial[h] = 0;
                    s->h_reselect_sec[h] = 0.10f;
                    s->h_last_target_d2[h] = 0.0f;
                    s->h_lock_persist_t[h] = 0.0f;
                    s->h_switch_cooldown_t[h] = 0.0f;
                    s->hvx[h] = 0.0f;
                    s->hvy[h] = 0.0f;
                    s->hx[h] = hunter_regroup_x(s, h, core_x);
                    s->hy[h] = hunter_regroup_y(s, h);
                    lv_obj_add_flag(s->hunters[h], LV_OBJ_FLAG_HIDDEN);
                    if (!shahed_kill)
                    {
                        set_diag_stage(s, "DBG:HUNTER_RESPAWN");
                        respawn_killer(s, committed, -1);
                    }
                }
                else
                {
                    s->defense_misses++;
                    s->k_missed_by_hunter[committed] = 1;
                    if ((s->defense_misses % 4) == 0)
                    {
                        note_failure(s, "Hunter miss: target evaded in terminal window", NULL);
                    }
                    s->h_falling[h] = 1;
                    s->h_last_target_d2[h] = 0.0f;
                    s->h_lock_persist_t[h] = 0.0f;
                    s->h_switch_cooldown_t[h] = 0.0f;
                    if (s->hvy[h] < 0.0f)
                    {
                        s->hvy[h] *= 0.30f;
                    }
                    s->hvy[h] += 0.20f;
                }
            }
        }
    }

    if ((s->h_falling[h] == 2) && (s->hy[h] <= horizon_y))
    {
        float regroup_x = hunter_regroup_x(s, h, core_x);
        float regroup_y = hunter_regroup_y(s, h);
        s->hunter_loaded[h] = 0;
        s->h_falling[h] = 0;
        s->h_target_idx[h] = -1;
        s->h_target_serial[h] = 0;
        s->h_reselect_sec[h] = 0.0f;
        s->h_last_target_d2[h] = 0.0f;
        s->h_overshoot_cooldown[h] = 0.0f;
        s->h_lock_persist_t[h] = 0.0f;
        s->h_switch_cooldown_t[h] = 0.0f;
        s->hx[h] = regroup_x;
        s->hy[h] = regroup_y;
        s->hvx[h] = 0.0f;
        s->hvy[h] = 0.0f;
        lv_obj_add_flag(s->hunters[h], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if ((s->h_falling[h] == 1) && (s->hy[h] >= offscreen_bottom_y))
    {
        float regroup_x = hunter_regroup_x(s, h, core_x);
        float regroup_y = hunter_regroup_y(s, h);
        s->hunter_loaded[h] = 0;
        s->h_falling[h] = 0;
        s->h_target_idx[h] = -1;
        s->h_target_serial[h] = 0;
        s->h_reselect_sec[h] = 0.0f;
        s->h_last_target_d2[h] = 0.0f;
        s->h_overshoot_cooldown[h] = 0.0f;
        s->h_lock_persist_t[h] = 0.0f;
        s->h_switch_cooldown_t[h] = 0.0f;
        s->hx[h] = regroup_x;
        s->hy[h] = regroup_y;
        s->hvx[h] = 0.0f;
        s->hvy[h] = 0.0f;
        lv_obj_add_flag(s->hunters[h], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if ((s->hy[h] >= ground_y - 1.0f) && (s->h_falling[h] == 0))
    {
        float regroup_x = hunter_regroup_x(s, h, core_x);
        float regroup_y = hunter_regroup_y(s, h);
        s->hunter_loaded[h] = 0;
        s->h_falling[h] = 0;
        s->h_target_idx[h] = -1;
        s->h_target_serial[h] = 0;
        s->h_reselect_sec[h] = 0.0f;
        s->h_last_target_d2[h] = 0.0f;
        s->h_overshoot_cooldown[h] = 0.0f;
        s->h_lock_persist_t[h] = 0.0f;
        s->h_switch_cooldown_t[h] = 0.0f;
        s->hx[h] = regroup_x;
        s->hy[h] = regroup_y;
        s->hvx[h] = 0.0f;
        s->hvy[h] = 0.0f;
    }
}

static void update_effects(drone_hunter_scene_t *s, float core_x, float core_y)
{
    int k;
    (void)core_x;
    (void)core_y;

    for (k = 0; k < KILLER_COUNT; ++k)
    {
        if (s->fx_intercept_t[k] > 0.0f)
        {
            int blast_style = s->fx_blast_style[k];
            float life = clampf(s->fx_intercept_t[k] / FX_INTERCEPT_SEC, 0.0f, 1.0f);
            float grow = 1.0f - life;
            float depth_scale = clampf(depth_zoom_factor_for_y(s, s->fx_intercept_y[k]), 0.62f, 1.28f);
            int32_t size;
            lv_opa_t fill_opa;
            lv_opa_t border_opa;
            lv_color_t fill_color;
            lv_color_t border_color;
            int border_w;

            if (blast_style == BLAST_STYLE_GIANT_ORANGE)
            {
                size = (int32_t)((36.0f + (grow * 112.0f)) * depth_scale);
                fill_opa = (lv_opa_t)(90 + (int32_t)(life * 145.0f));
                border_opa = (lv_opa_t)(120 + (int32_t)(life * 135.0f));
                fill_color = lv_color_hex(0xF97316);
                border_color = lv_color_hex(0xFDE047);
                border_w = 5;
            }
            else if (blast_style == BLAST_STYLE_MEDIUM_RED)
            {
                size = (int32_t)((22.0f + (grow * 72.0f)) * depth_scale);
                fill_opa = (lv_opa_t)(55 + (int32_t)(life * 95.0f));
                border_opa = (lv_opa_t)(120 + (int32_t)(life * 125.0f));
                fill_color = lv_color_hex(0xC62828);
                border_color = lv_color_hex(0xFF6B6B);
                border_w = 4;
            }
            else
            {
                size = (int32_t)((8.0f + (grow * 34.0f)) * depth_scale);
                fill_opa = (lv_opa_t)(45 + (int32_t)(life * 110.0f));
                border_opa = (lv_opa_t)(150 + (int32_t)(life * 105.0f));
                fill_color = lv_color_hex(0xFFFFFF);
                border_color = lv_color_hex(0xFFFFFF);
                border_w = 3;
            }

            lv_obj_clear_flag(s->fx_intercept[k], LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(s->fx_intercept[k], size, size);
            lv_obj_set_style_radius(s->fx_intercept[k], LV_RADIUS_CIRCLE, 0);
            lv_obj_set_style_bg_color(s->fx_intercept[k], fill_color, 0);
            lv_obj_set_style_bg_opa(s->fx_intercept[k], fill_opa, 0);
            lv_obj_set_style_border_width(s->fx_intercept[k], border_w, 0);
            lv_obj_set_style_border_color(s->fx_intercept[k], border_color, 0);
            lv_obj_set_style_border_opa(s->fx_intercept[k], border_opa, 0);
            set_obj_center(s->fx_intercept[k], s->fx_intercept_x[k], s->fx_intercept_y[k]);
        }
        else
        {
            lv_obj_add_flag(s->fx_intercept[k], LV_OBJ_FLAG_HIDDEN);
        }
        if (s->fx_spawn_t[k] > 0.0f)
        {
            float life = clampf(s->fx_spawn_t[k] / FX_SPAWN_SEC, 0.0f, 1.0f);
            float grow = 1.0f - life;
            float depth_scale = clampf(depth_zoom_factor_for_y(s, s->fx_spawn_y[k]), 0.62f, 1.28f);
            int32_t size = (int32_t)((20.0f + (grow * 50.0f)) * depth_scale);
            lv_opa_t ring_opa = (lv_opa_t)(70 + (int32_t)(life * 165.0f));

            lv_obj_clear_flag(s->fx_spawn[k], LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(s->fx_spawn[k], size, size);
            lv_obj_set_style_radius(s->fx_spawn[k], LV_RADIUS_CIRCLE, 0);
            lv_obj_set_style_bg_opa(s->fx_spawn[k], LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_width(s->fx_spawn[k], 3, 0);
            lv_obj_set_style_border_color(s->fx_spawn[k], lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_border_opa(s->fx_spawn[k], ring_opa, 0);
            set_obj_center(s->fx_spawn[k], s->fx_spawn_x[k], s->fx_spawn_y[k]);
        }
        else
        {
            lv_obj_add_flag(s->fx_spawn[k], LV_OBJ_FLAG_HIDDEN);
        }
        if (s->fx_kill_t[k] > 0.0f)
        {
            int blast_style = s->fx_blast_style[k];
            float life = clampf(s->fx_kill_t[k] / FX_KILL_SEC, 0.0f, 1.0f);
            float grow = 1.0f - life;
            float depth_scale = clampf(depth_zoom_factor_for_y(s, s->fx_kill_y[k]), 0.62f, 1.28f);
            int32_t size;
            lv_opa_t ring_opa;
            lv_opa_t fill_opa;
            lv_color_t fill_color;
            lv_color_t border_color;
            int border_w;

            if (blast_style == BLAST_STYLE_GIANT_ORANGE)
            {
                size = (int32_t)((52.0f + (grow * 126.0f)) * depth_scale);
                ring_opa = (lv_opa_t)(120 + (int32_t)(life * 135.0f));
                fill_opa = (lv_opa_t)(50 + (int32_t)(life * 105.0f));
                fill_color = lv_color_hex(0xF97316);
                border_color = lv_color_hex(0xFDE68A);
                border_w = 6;
            }
            else if (blast_style == BLAST_STYLE_MEDIUM_RED)
            {
                size = (int32_t)((30.0f + (grow * 86.0f)) * depth_scale);
                ring_opa = (lv_opa_t)(125 + (int32_t)(life * 120.0f));
                fill_opa = (lv_opa_t)(30 + (int32_t)(life * 85.0f));
                fill_color = lv_color_hex(0xC62828);
                border_color = lv_color_hex(0xFF6B6B);
                border_w = 4;
            }
            else
            {
                size = (int32_t)((14.0f + (grow * 40.0f)) * depth_scale);
                ring_opa = (lv_opa_t)(145 + (int32_t)(life * 105.0f));
                fill_opa = (lv_opa_t)(35 + (int32_t)(life * 80.0f));
                fill_color = lv_color_hex(0xFFFFFF);
                border_color = lv_color_hex(0xFFFFFF);
                border_w = 4;
            }

            lv_obj_clear_flag(s->fx_kill[k], LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(s->fx_kill[k], size, size);
            lv_obj_set_style_radius(s->fx_kill[k], LV_RADIUS_CIRCLE, 0);
            lv_obj_set_style_bg_color(s->fx_kill[k], fill_color, 0);
            lv_obj_set_style_bg_opa(s->fx_kill[k], fill_opa, 0);
            lv_obj_set_style_border_width(s->fx_kill[k], border_w, 0);
            lv_obj_set_style_border_color(s->fx_kill[k], border_color, 0);
            lv_obj_set_style_border_opa(s->fx_kill[k], ring_opa, 0);
            set_obj_center(s->fx_kill[k], s->fx_kill_x[k], s->fx_kill_y[k]);
        }
        else
        {
            lv_obj_add_flag(s->fx_kill[k], LV_OBJ_FLAG_HIDDEN);
        }
    }

    lv_obj_add_flag(s->core, LV_OBJ_FLAG_HIDDEN);

    {
#if RENDER_STABILITY_SAFE_MODE
        int visible_fire = s->attacker_goal_detonations;
        if (visible_fire > CITY_FIRE_RENDER_MAX)
        {
            visible_fire = CITY_FIRE_RENDER_MAX;
        }
        if (visible_fire < 0)
        {
            visible_fire = 0;
        }
        for (k = 0; k < CITY_FIRE_MAX; ++k)
        {
            if (k < visible_fire)
            {
                /* Safe-mode fallback: static, low-cost persistent fires, one representative per successful strike. */
                int src_idx = (k * 4);
                int frame_idx = (k % (FLAME_SPRITE_FRAME_COUNT - 1)) + 1;
                float fx;
                float fy;
                int32_t zoom;
                lv_opa_t opa;
                lv_color_t tint;
                lv_opa_t tint_opa;
                int just_shown = lv_obj_has_flag(s->city_fire[k], LV_OBJ_FLAG_HIDDEN);
                if (s->city_fire_count > 0)
                {
                    src_idx %= s->city_fire_count;
                    fx = clampf(s->city_fire_x[src_idx], (float)s->arena_x + 10.0f, (float)(s->arena_x + s->arena_w - 10));
                    fy = clampf(s->city_fire_y[src_idx], (float)s->arena_y + 10.0f, combat_floor_y(s) - 6.0f);
                }
                else
                {
                    float spread = (float)(k + 1);
                    fx = (float)s->arena_x + 14.0f + fmodf((spread * 37.0f), (float)s->arena_w - 28.0f);
                    fy = (float)s->arena_y + ((float)s->arena_h * 0.32f) + fmodf((spread * 23.0f), (float)s->arena_h * 0.54f);
                    fy = clampf(fy, (float)s->arena_y + 10.0f, combat_floor_y(s) - 6.0f);
                }

                {
                    float depth = clampf(depth_zoom_factor_for_y(s, fy), 0.74f, 1.30f);
                    zoom = (int32_t)clampf(256.0f * depth * 1.06f, 180.0f, 410.0f);
                    opa = (lv_opa_t)clampf(220.0f + (depth * 24.0f), 205.0f, 255.0f);
                }

                if ((k % 3) == 0)
                {
                    tint = lv_color_hex(0xFF4D1E);   /* bright red/orange */
                    tint_opa = (lv_opa_t)186;
                }
                else if ((k % 3) == 1)
                {
                    tint = lv_color_hex(0xFF9A00);   /* bright orange */
                    tint_opa = (lv_opa_t)196;
                }
                else
                {
                    tint = lv_color_hex(0xFF3A2A);   /* bright red */
                    tint_opa = (lv_opa_t)178;
                }

                lv_obj_clear_flag(s->city_fire[k], LV_OBJ_FLAG_HIDDEN);
                if (just_shown || (s->city_fire_last_frame[k] != (uint8_t)frame_idx))
                {
                    lv_image_set_src(s->city_fire[k], g_flame_profile_torch[frame_idx]);
                    s->city_fire_last_profile[k] = 0u;
                    s->city_fire_last_frame[k] = (uint8_t)frame_idx;
                }
                if (just_shown)
                {
                    lv_obj_set_style_transform_zoom(s->city_fire[k], zoom, 0);
                    lv_obj_set_style_opa(s->city_fire[k], opa, 0);
                    lv_obj_set_style_image_recolor(s->city_fire[k], tint, 0);
                    lv_obj_set_style_image_recolor_opa(s->city_fire[k], tint_opa, 0);
                    set_obj_center(s->city_fire[k], fx, fy - (((float)lv_obj_get_height(s->city_fire[k]) * (float)zoom) / 512.0f));
                }
            }
            else
            {
                lv_obj_add_flag(s->city_fire[k], LV_OBJ_FLAG_HIDDEN);
            }
        }
#else
        int target_fire_count = s->attacker_goal_detonations;
        int guard;
        /* Keep post-impact fires persistent for the round; do not decay by score drift. */
        if (target_fire_count < s->city_fire_count)
        {
            target_fire_count = s->city_fire_count;
        }
        if (target_fire_count > CITY_FIRE_RENDER_MAX)
        {
            target_fire_count = CITY_FIRE_RENDER_MAX;
        }
        if (target_fire_count < 0)
        {
            target_fire_count = 0;
        }
        (void)guard;
        {
            uint32_t now_tick = lv_tick_get();
            static uint32_t fire_anim_tick = 0U;
            int fire_anim_step = 0;
            if ((uint32_t)(now_tick - fire_anim_tick) >= 160U)
            {
                fire_anim_tick = now_tick;
                fire_anim_step = 1;
            }
            for (k = 0; k < CITY_FIRE_MAX; ++k)
            {
                if (k < s->city_fire_count)
                {
                    int force_anim = fire_anim_step || lv_obj_has_flag(s->city_fire[k], LV_OBJ_FLAG_HIDDEN);
                    if (!force_anim)
                    {
                        continue;
                    }
                    int profile = (int)s->city_fire_style[k] % CITY_FIRE_PROFILE_COUNT;
                    const lv_image_dsc_t **frames = city_fire_profile_frames(profile);
                    float fps = city_fire_profile_fps(profile);
                    float phase = s->city_fire_phase[k];
                    float frame_pos;
                    int frame_idx;
                    float depth;
                    float jitter;
                    float style_var;
                    float profile_scale = 1.0f;
                    float profile_wobble = 2.0f;
                    float base_y;
                    float lift = 0.0f;
                    int intense;
                    lv_opa_t opa = LV_OPA_COVER;
                    lv_color_t tint = lv_color_hex(0xFFFFFF);
                    lv_opa_t tint_opa = LV_OPA_TRANSP;
                    int32_t img_h = 0;
                    int32_t zoom = 256;
                    float wobble = 0.0f;

                    if (!isfinite(phase))
                    {
                        phase = 0.0f;
                    }
                    frame_pos = ((s->t + phase) * fps);
                    frame_idx = ((int)frame_pos) % FLAME_SPRITE_FRAME_COUNT;
                    if ((FLAME_SPRITE_FRAME_COUNT > 1) && (frame_idx == 0))
                    {
                        frame_idx = 1;
                    }
                    depth = clampf(depth_zoom_factor_for_y(s, s->city_fire_y[k]), 0.70f, 1.34f);
                    jitter = 0.88f + (0.30f * sinf((s->t * 1.3f) + phase + ((float)k * 0.41f)));
                    style_var = 0.82f + (0.42f * fabsf(sinf((phase * 0.73f) + ((float)k * 1.37f))));
                    base_y = clampf(s->city_fire_y[k], (float)s->arena_y + 8.0f, combat_floor_y(s) - 6.0f);
                    intense = (s->city_fire_intensity[k] >= CITY_FIRE_INTENSITY_BIG) ? 1 : 0;
                    switch (profile)
                    {
                        case CITY_FIRE_PROFILE_TORCH:
                            profile_scale = intense ? 1.36f : 1.18f;
                            profile_wobble = 3.8f;
                            lift = 3.5f + (3.2f * sinf((s->t * 4.8f) + phase));
                            opa = (lv_opa_t)(intense ? 252 : 238);
                            tint = lv_color_hex(0xFFA640);
                            tint_opa = (lv_opa_t)44;
                            break;
                        case CITY_FIRE_PROFILE_BLUE_JET:
                            profile_scale = intense ? 1.28f : 1.12f;
                            profile_wobble = 3.4f;
                            lift = 2.2f + (2.8f * sinf((s->t * 4.2f) + phase));
                            opa = (lv_opa_t)(intense ? 236 : 222);
                            tint = lv_color_hex(0x58C8FF);
                            tint_opa = (lv_opa_t)96;
                            break;
                        case CITY_FIRE_PROFILE_FLICKER:
                            profile_scale = intense ? 1.42f : 1.18f;
                            profile_wobble = 4.2f;
                            lift = 2.6f + (3.6f * sinf((s->t * 5.4f) + phase));
                            opa = (lv_opa_t)(intense ? 248 : 234);
                            tint = lv_color_hex(0x8CDD55);
                            tint_opa = (lv_opa_t)84;
                            break;
                        case CITY_FIRE_PROFILE_SPIRAL:
                            profile_scale = intense ? 1.24f : 1.08f;
                            profile_wobble = 3.9f;
                            lift = 1.8f + (2.7f * sinf((s->t * 3.4f) + phase));
                            opa = (lv_opa_t)(intense ? 234 : 220);
                            tint = lv_color_hex(0xB56AF5);
                            tint_opa = (lv_opa_t)92;
                            break;
                        case CITY_FIRE_PROFILE_BURST:
                            profile_scale = intense ? 1.54f : 1.24f;
                            profile_wobble = 4.6f;
                            lift = 2.0f + (4.4f * sinf((s->t * 6.0f) + phase));
                            opa = (lv_opa_t)(intense ? 255 : 240);
                            if ((((int)(phase * 37.0f) + k) & 0x7) == 0)
                            {
                                tint = lv_color_hex(0x9B2C24);
                                tint_opa = (lv_opa_t)132;
                            }
                            else
                            {
                                tint = lv_color_hex(0xFF6B2E);
                                tint_opa = (lv_opa_t)64;
                            }
                            break;
                        case CITY_FIRE_PROFILE_PULSE:
                            profile_scale = intense ? 1.28f : 1.06f;
                            profile_wobble = 2.8f;
                            lift = 1.2f + (2.4f * sinf((s->t * 2.6f) + phase));
                            opa = (lv_opa_t)(intense ? 236 : 222);
                            tint = lv_color_hex(0xFFF2E5);
                            tint_opa = (lv_opa_t)40;
                            break;
                        case CITY_FIRE_PROFILE_SMOKE:
                            profile_scale = intense ? 1.36f : 1.20f;
                            profile_wobble = 2.3f;
                            lift = intense ? (7.0f + (4.2f * sinf((s->t * 1.7f) + phase)))
                                           : (3.8f + (1.9f * sinf((s->t * 1.9f) + phase)));
                            opa = intense ? 208 : 194;
                            tint = lv_color_hex(0x4A4038);
                            tint_opa = (lv_opa_t)160;
                            break;
                        case CITY_FIRE_PROFILE_DUAL_JET:
                            profile_scale = intense ? 1.34f : 1.14f;
                            profile_wobble = 3.8f;
                            lift = 2.4f + (3.2f * sinf((s->t * 4.1f) + phase));
                            opa = (lv_opa_t)(intense ? 240 : 226);
                            tint = lv_color_hex(0x55D4C7);
                            tint_opa = (lv_opa_t)96;
                            break;
                        case CITY_FIRE_PROFILE_WHIP:
                            profile_scale = intense ? 1.38f : 1.18f;
                            profile_wobble = 4.1f;
                            lift = 2.2f + (3.4f * sinf((s->t * 4.7f) + phase));
                            opa = (lv_opa_t)(intense ? 244 : 230);
                            tint = lv_color_hex(0xC756F0);
                            tint_opa = (lv_opa_t)102;
                            break;
                        case CITY_FIRE_PROFILE_WIDE_TORCH:
                            profile_scale = intense ? 1.42f : 1.22f;
                            profile_wobble = 3.8f;
                            lift = 2.3f + (3.0f * sinf((s->t * 3.7f) + phase));
                            opa = (lv_opa_t)(intense ? 246 : 232);
                            tint = lv_color_hex(0xF3B23F);
                            tint_opa = (lv_opa_t)56;
                            break;
                        case CITY_FIRE_PROFILE_SPUTTER:
                            profile_scale = intense ? 1.34f : 1.10f;
                            profile_wobble = 4.0f;
                            lift = 1.8f + (3.1f * sinf((s->t * 4.0f) + phase));
                            opa = (lv_opa_t)(intense ? 238 : 224);
                            tint = lv_color_hex(0x9BCC48);
                            tint_opa = (lv_opa_t)110;
                            break;
                        case CITY_FIRE_PROFILE_RED_ORANGE:
                            profile_scale = intense ? 1.46f : 1.20f;
                            profile_wobble = 4.3f;
                            lift = 2.3f + (3.9f * sinf((s->t * 5.2f) + phase));
                            opa = (lv_opa_t)(intense ? 255 : 246);
                            if ((((k + (int)(phase * 31.0f)) & 0x1) == 0))
                            {
                                tint = lv_color_hex(0xFF4B21);
                                tint_opa = (lv_opa_t)255;
                            }
                            else
                            {
                                tint = lv_color_hex(0xFFB020);
                                tint_opa = (lv_opa_t)242;
                            }
                            break;
                        case CITY_FIRE_PROFILE_ORANGE_WHITE:
                            profile_scale = intense ? 1.40f : 1.18f;
                            profile_wobble = 3.7f;
                            lift = 2.0f + (3.4f * sinf((s->t * 4.6f) + phase));
                            opa = (lv_opa_t)(intense ? 255 : 248);
                            if ((((k + (int)(phase * 29.0f)) & 0x1) == 0))
                            {
                                tint = lv_color_hex(0xFFFFFF);
                                tint_opa = (lv_opa_t)232;
                            }
                            else
                            {
                                tint = lv_color_hex(0xFFB000);
                                tint_opa = (lv_opa_t)255;
                            }
                            break;
                        case CITY_FIRE_PROFILE_BRIGHT_RED:
                            profile_scale = intense ? 1.48f : 1.16f;
                            profile_wobble = 4.4f;
                            lift = 2.5f + (4.1f * sinf((s->t * 5.6f) + phase));
                            opa = (lv_opa_t)(intense ? 255 : 246);
                            tint = lv_color_hex(0xFF2C2C);
                            tint_opa = (lv_opa_t)255;
                            break;
                        case CITY_FIRE_PROFILE_BRIGHT_ORANGE:
                            profile_scale = intense ? 1.46f : 1.18f;
                            profile_wobble = 4.1f;
                            lift = 2.2f + (3.8f * sinf((s->t * 5.0f) + phase));
                            opa = (lv_opa_t)(intense ? 255 : 246);
                            tint = lv_color_hex(0xFF9C00);
                            tint_opa = (lv_opa_t)255;
                            break;
                        case CITY_FIRE_PROFILE_BRIGHT_RED_HOT:
                            profile_scale = intense ? 1.50f : 1.17f;
                            profile_wobble = 4.5f;
                            lift = 2.6f + (4.0f * sinf((s->t * 5.5f) + phase));
                            opa = (lv_opa_t)(intense ? 255 : 240);
                            tint = lv_color_hex(0xFF442A);
                            tint_opa = (lv_opa_t)255;
                            break;
                        case CITY_FIRE_PROFILE_BRIGHT_YELLOW_ORANGE:
                            profile_scale = intense ? 1.44f : 1.19f;
                            profile_wobble = 4.0f;
                            lift = 2.1f + (3.6f * sinf((s->t * 4.9f) + phase));
                            opa = (lv_opa_t)(intense ? 255 : 246);
                            if ((((k + (int)(phase * 35.0f)) & 0x1) == 0))
                            {
                                tint = lv_color_hex(0xFFC21A);
                                tint_opa = (lv_opa_t)255;
                            }
                            else
                            {
                                tint = lv_color_hex(0xFF8A00);
                                tint_opa = (lv_opa_t)255;
                            }
                            break;
                        case CITY_FIRE_PROFILE_GROUND:
                        default:
                            profile_scale = intense ? 1.08f : 0.98f;
                            profile_wobble = 2.2f;
                            lift = 0.4f + (1.0f * sinf((s->t * 1.8f) + phase));
                            opa = intense ? 222 : 214;
                            if ((((k + (int)(phase * 19.0f)) & 0x3) == 0))
                            {
                                tint = lv_color_hex(0x7A201A);
                                tint_opa = (lv_opa_t)138;
                            }
                            else
                            {
                                tint = lv_color_hex(0xA43A2A);
                                tint_opa = (lv_opa_t)104;
                            }
                            break;
                    }
                    if (lift < 0.0f)
                    {
                        lift = 0.0f;
                    }
                    lift = clampf(lift * 0.22f, 0.0f, 2.8f);
                    profile_scale *= style_var;
                    profile_wobble *= (0.38f + (0.18f * style_var));
                    zoom = (int32_t)(256.0f * depth * jitter * profile_scale);
                    wobble = sinf((s->t * (1.5f + (0.5f * style_var))) + phase) * profile_wobble;
                    if ((profile == CITY_FIRE_PROFILE_SMOKE) || (profile == CITY_FIRE_PROFILE_GROUND))
                    {
                        wobble = 0.0f;
                    }

                    if (k < CITY_FIRE_RENDER_MAX)
                    {
                        lv_obj_clear_flag(s->city_fire[k], LV_OBJ_FLAG_HIDDEN);
                        if ((s->city_fire_last_profile[k] != (uint8_t)profile) ||
                            (s->city_fire_last_frame[k] != (uint8_t)frame_idx))
                        {
                            lv_image_set_src(s->city_fire[k], frames[frame_idx]);
                            s->city_fire_last_profile[k] = (uint8_t)profile;
                            s->city_fire_last_frame[k] = (uint8_t)frame_idx;
                        }
                        lv_obj_set_style_transform_zoom(s->city_fire[k], zoom, 0);
                        lv_obj_set_style_opa(s->city_fire[k], opa, 0);
                        lv_obj_set_style_image_recolor(s->city_fire[k], tint, 0);
                        lv_obj_set_style_image_recolor_opa(s->city_fire[k], tint_opa, 0);
                        img_h = lv_obj_get_height(s->city_fire[k]);
                        {
                            float scaled_half_h = ((float)img_h * (float)zoom) / 512.0f;
                            set_obj_center(s->city_fire[k], s->city_fire_x[k] + wobble, base_y - lift - scaled_half_h);
                        }
                    }
                    else
                    {
                        lv_obj_add_flag(s->city_fire[k], LV_OBJ_FLAG_HIDDEN);
                    }
                }
                else
                {
                    lv_obj_add_flag(s->city_fire[k], LV_OBJ_FLAG_HIDDEN);
                }
            }
        }
#endif
    }

    for (k = 0; k < CIWS_TRACER_COUNT; ++k)
    {
        if (s->ciws_tracer_t[k] > 0.0f)
        {
            float life_ratio = clampf(s->ciws_tracer_t[k] / CIWS_TRACER_LIFE_SEC, 0.0f, 1.0f);
            float depth = clampf(depth_zoom_factor_for_y(s, s->ciws_tracer_y0[k]), 0.58f, 1.34f);
            float stream_scale = clampf((s->ciws_tracer_scale[k] * 0.55f) + (depth * 0.45f), 0.52f, 1.35f);
            lv_opa_t opa = (lv_opa_t)clampf((40.0f + (life_ratio * 205.0f)) * stream_scale, 30.0f, 255.0f);
            int32_t base_dot = (life_ratio > 0.74f) ? 10 : ((life_ratio > 0.38f) ? 8 : 6);
            int32_t dot = (int32_t)clampf((float)base_dot * stream_scale, 3.0f, 14.0f);

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
    float floor_y = combat_floor_y(s);
    float ciws_x = (float)(s->arena_x + s->arena_w - 8);
    float ciws_y = (float)(s->arena_y + s->arena_h - 49);
    float ciws_left_x = (float)(s->arena_x + 18);
    float ciws_left_y = (float)(s->arena_y + s->arena_h - 49);

    for (h = 0; h < HUNTER_COUNT; ++h)
    {
        int hunter_visible = s->hunter_loaded[h] || (s->h_falling[h] != 0);
        int16_t angle_tenth = (int16_t)(s->h_heading[h] * (1800.0f / PI_F)) + SPRITE_NOSE_FWD_TENTH;
        float depth = depth_zoom_factor_for_y(s, s->hy[h]);
        uint16_t base_zoom = (uint16_t)clampf((float)s->h_base_zoom[h], 150.0f, 340.0f);
        uint16_t zoom = (uint16_t)clampf((float)base_zoom * depth, 140.0f, 310.0f);
        lv_opa_t hunter_opa = LV_OPA_COVER;

        int32_t hw = lv_obj_get_width(s->hunters[h]);
        int32_t hh = lv_obj_get_height(s->hunters[h]);
        if ((hw <= 0) || (hh <= 0) || (hw > 240) || (hh > 240))
        {
            s->h_type[h] = HUNTER_STING_II;
            lv_image_set_src(s->hunters[h], hunter_image_src(HUNTER_STING_II));
            s->h_base_zoom[h] = 210;
            hw = lv_obj_get_width(s->hunters[h]);
            hh = lv_obj_get_height(s->hunters[h]);
            if ((hw <= 0) || (hh <= 0) || (hw > 240) || (hh > 240))
            {
                hw = 96;
                hh = 96;
                s->h_base_zoom[h] = 180;
            }
        }
        if (s->h_falling[h] == 2)
        {
            float retreat = clampf((s->hy[h] - ((float)s->arena_y - 10.0f)) /
                                   ((floor_y) - ((float)s->arena_y - 10.0f)),
                                   0.0f, 1.0f);
            zoom = (uint16_t)clampf((float)zoom * (0.52f + (retreat * 0.48f)), 80.0f, 310.0f);
            hunter_opa = (lv_opa_t)(50 + (int32_t)(retreat * 205.0f));
            s->hy[h] = clampf(s->hy[h], (float)s->arena_y - 24.0f, floor_y);
        }
        else if (s->h_falling[h] == 1)
        {
            float screen_h = (float)lv_obj_get_height(s->screen);
            s->hy[h] = clampf(s->hy[h], (float)s->arena_y + 6.0f, screen_h + 20.0f);
        }
        else
        {
            s->hy[h] = clampf(s->hy[h], (float)s->arena_y + 6.0f, floor_y);
        }
        s->hx[h] = clampf(s->hx[h], (float)s->arena_x + 6.0f, (float)(s->arena_x + s->arena_w - 6));

        if (!hunter_visible)
        {
            /* Avoid hide/show flag thrash; drive invisibility through alpha for stable rendering. */
            lv_obj_set_style_opa(s->hunters[h], LV_OPA_TRANSP, 0);
            lv_obj_set_style_image_opa(s->hunters[h], LV_OPA_TRANSP, 0);
            lv_obj_add_flag(s->hunter_tail[h], LV_OBJ_FLAG_HIDDEN);
            continue;
        }

        lv_obj_set_style_transform_zoom(s->hunters[h], zoom, 0);
        lv_obj_set_style_transform_width(s->hunters[h], 0, 0);
        lv_obj_set_style_transform_height(s->hunters[h], 0, 0);
        lv_obj_set_style_opa(s->hunters[h], hunter_opa, 0);
        lv_obj_set_style_image_opa(s->hunters[h], hunter_opa, 0);
        lv_obj_clear_flag(s->hunters[h], LV_OBJ_FLAG_HIDDEN);
        set_obj_center(s->hunters[h], s->hx[h], s->hy[h]);
        lv_obj_set_style_transform_pivot_x(s->hunters[h], (int32_t)(hw / 2), 0);
        lv_obj_set_style_transform_pivot_y(s->hunters[h], (int32_t)(hh / 2), 0);
        lv_obj_set_style_transform_angle(s->hunters[h], angle_tenth, 0);
        lv_obj_add_flag(s->hunter_tail[h], LV_OBJ_FLAG_HIDDEN);
    }

    for (k = 0; k < KILLER_COUNT; ++k)
    {
        const lv_image_dsc_t *src = attack_image_src(s, k);

        if (!s->killer_active[k])
        {
            lv_obj_add_flag(s->killers[k], LV_OBJ_FLAG_HIDDEN);
            continue;
        }
        lv_image_set_src(s->killers[k], src);
        if (s->ktype[k] == TARGET_FIXED_WING)
        {
            update_fixed_wing_orientation(s, k);
        }
        else
        {
            int16_t k_angle_tenth = (int16_t)(s->k_heading[k] * (1800.0f / PI_F)) + SPRITE_NOSE_FWD_TENTH;
            lv_obj_set_style_transform_angle(s->killers[k], k_angle_tenth, 0);
        }
        {
            float depth = depth_zoom_factor_for_y(s, s->ky[k]);
            uint16_t base_zoom = (uint16_t)clampf((float)s->k_base_zoom[k], 80.0f, 900.0f);
            uint16_t zoom = (uint16_t)clampf((float)base_zoom * depth, 70.0f, 920.0f);
            lv_opa_t killer_opa = LV_OPA_COVER;
            int32_t kw = lv_obj_get_width(s->killers[k]);
            int32_t kh = lv_obj_get_height(s->killers[k]);
            if ((kw <= 0) || (kh <= 0) || (kw > 240) || (kh > 240))
            {
                if (s->k_last_src[k] != src)
                {
                    lv_image_set_src(s->killers[k], src);
                    s->k_last_src[k] = src;
                }
                s->k_base_zoom[k] = (uint16_t)clampf(176.0f * attack_zoom_scale(s, k), 80.0f, 900.0f);
                zoom = (uint16_t)clampf((float)s->k_base_zoom[k] * depth, 70.0f, 920.0f);
            }
            if (s->k_dying[k])
            {
                float life = clampf(s->k_dying_t[k] / FX_SHAHED_DEATH_SEC, 0.0f, 1.0f);
                zoom = (uint16_t)clampf((float)zoom * (1.0f + ((1.0f - life) * 0.22f)), 70.0f, 980.0f);
                killer_opa = (lv_opa_t)(80 + (int32_t)(life * 175.0f));
            }
            lv_obj_set_style_transform_zoom(s->killers[k], zoom, 0);
            lv_obj_set_style_opa(s->killers[k], killer_opa, 0);
            lv_obj_set_style_image_opa(s->killers[k], killer_opa, 0);
        }
        s->ky[k] = clampf(s->ky[k], (float)s->arena_y + 6.0f, floor_y);
        s->kx[k] = clampf(s->kx[k], (float)s->arena_x + 6.0f, (float)(s->arena_x + s->arena_w - 6));
        lv_obj_clear_flag(s->killers[k], LV_OBJ_FLAG_HIDDEN);
        set_obj_center(s->killers[k], s->kx[k], s->ky[k]);
    }

    set_obj_center(s->ciws, ciws_x, ciws_y);
    set_obj_center(s->ciws_left, ciws_left_x, ciws_left_y);
}

static void update_hud(drone_hunter_scene_t *s)
{
    int phase = arena_phase(s);
    uint32_t elapsed_ms = lv_tick_get() - s->round_start_tick_ms;
    int elapsed = (int)(elapsed_ms / 1000U);
    int elapsed_mm = elapsed / 60;
    int elapsed_ss = elapsed % 60;
    int lead_k = -1;
    float lead_score = -1.0f;
    int total_stock = 0;
    int hunters_air = 0;
    float lock_q = 0.0f;
    int def_endurance = 0;
    const char *env_fit = "N/A";
    const char *availability = "HOLD";
    char elapsed_line[192];
    int h;
    int k;
    const char *hunter_ctrl = ctrl_name_compact(s->team_ctrl[0]);
    const char *attacker_ctrl = ctrl_name_compact(s->team_ctrl[1]);
    const char *attacker_mode_text = (s->attacker_mode_sel == CTRL_EDGEAI) ? "EDGEAI" : "ALGO";
    const char *defender_mode_text = "ALGO";

    if ((s->hud_mode == NULL) &&
        (s->hud_attack_digits == NULL) &&
        (s->hud_defend_digits == NULL))
    {
        return;
    }

    if (s->defender_mode_sel == DEFENDER_MODE_HUMAN)
    {
        hunter_ctrl = "HUMAN";
        defender_mode_text = "HUMAN";
    }
    else if (s->defender_mode_sel == DEFENDER_MODE_EDGEAI)
    {
        defender_mode_text = "EDGEAI";
    }
    else
    {
        defender_mode_text = "ALGO";
    }

    for (k = 0; k < KILLER_COUNT; ++k)
    {
        if (s->killer_active[k] && !s->k_dying[k] && (s->k_threat_score[k] > lead_score))
        {
            lead_score = s->k_threat_score[k];
            lead_k = k;
        }
    }
    for (k = 0; k < HUNTER_TYPE_COUNT; ++k)
    {
        total_stock += s->hunter_stock[k];
    }
    for (h = 0; h < HUNTER_COUNT; ++h)
    {
        if (s->hunter_loaded[h])
        {
            hunters_air++;
        }
    }
    lock_q = clampf((s->ciws_lock_left > s->ciws_lock_right) ? s->ciws_lock_left : s->ciws_lock_right, 0.0f, 1.0f);
    def_endurance = total_stock + ((s->ciws_ammo_left + s->ciws_ammo_right) / 120);
    availability = ((total_stock > 0) || (hunters_air > 0) || (s->ciws_ammo_left > 0) || (s->ciws_ammo_right > 0)) ? "READY" : "EMPTY";

    if (s->hud_attack_digits != NULL)
    {
        lv_label_set_text_fmt(s->hud_attack_digits, "%04d", s->attacker_points);
    }
    if (s->hud_defend_digits != NULL)
    {
        lv_label_set_text_fmt(s->hud_defend_digits, "%04d", s->hunter_points);
    }
    if (s->hud_attack_mode_text != NULL)
    {
        lv_label_set_text_fmt(s->hud_attack_mode_text, "%s", attacker_mode_text);
        if (s->attacker_mode_sel == CTRL_EDGEAI)
        {
            lv_obj_set_style_text_color(s->hud_attack_mode_text, lv_color_hex(0x86EFAC), 0); /* EDGEAI */
        }
        else
        {
            lv_obj_set_style_text_color(s->hud_attack_mode_text, lv_color_hex(0x7DD3FC), 0); /* ALGO */
        }
    }
    if (s->hud_defend_mode_text != NULL)
    {
        lv_label_set_text_fmt(s->hud_defend_mode_text, "%s", defender_mode_text);
        if (s->defender_mode_sel == DEFENDER_MODE_HUMAN)
        {
            lv_obj_set_style_text_color(s->hud_defend_mode_text, lv_color_hex(0xFCD34D), 0); /* HUMAN */
        }
        else if (s->defender_mode_sel == DEFENDER_MODE_EDGEAI)
        {
            lv_obj_set_style_text_color(s->hud_defend_mode_text, lv_color_hex(0x86EFAC), 0); /* EDGEAI */
        }
        else
        {
            lv_obj_set_style_text_color(s->hud_defend_mode_text, lv_color_hex(0x7DD3FC), 0); /* ALGO */
        }
    }

    if (s->hud_mode == NULL)
    {
        /* Lightweight card-only HUD path to reduce update overhead and freeze risk. */
        return;
    }

    if (s->hud_mode != NULL)
    {
        lv_label_set_text_fmt(s->hud_mode, "MODE: %s  |  PHASE: %s  |  H:%04d A:%04d  |  IFF %s %s%s",
                              mode_name(s->mode), arena_phase_name(phase),
                              s->hunter_points, s->attacker_points,
                              s->iff_advanced_mode ? "ADV" : "LOCK",
                              s->iff_degraded ? "DEG" : "NOM",
                              s->iff_merged_tracks ? " MERGED" : "");
        lv_label_set_text_fmt(s->hud_hunter_score,
                              "HUNTER(%s): %04d",
                              hunter_ctrl, s->hunter_points);
        lv_label_set_text_fmt(s->hud_score, "CORE %03d", s->core_hp);
        lv_label_set_text_fmt(s->hud_attacker_score,
                              "ATTACKER(%s): %04d",
                              attacker_ctrl, s->attacker_points);
        lv_label_set_text_fmt(s->hud_wave,
                              "WAVE %d  |  MST %s  |  ARCH %s  |  STRAT %s%s%s  |  NEUT %d/%d  |  LEAK %d  |  REM %d",
                              s->wave_idx,
                              mission_milestone_short_name(s->mission_milestone),
                              wave_archetype_short_name(s->wave_archetype),
                              attack_strategy_short_name(s->attack_strategy_live),
                              (s->wave_shift_applied >= 1) ? "*" : "",
                              (s->wave_shift_applied >= 2) ? "+" : "",
                              s->attack_destroyed, s->wave_target_total,
                              s->attack_leaked,
                              s->attack_remaining_to_spawn);
        if (lead_k >= 0)
        {
            char info[256];
            float km_per_px = MAP_SIZE_KM / clampf((float)s->arena_w, 20.0f, 4000.0f);
            float need_km = s->k_range_to_core[lead_k] * km_per_px;
            float best_km = hunter_range_km(s->k_recommended_counter[lead_k]);
            env_fit = (best_km >= need_km) ? "GOOD" : "TIGHT";
            (void)snprintf(info, sizeof(info),
                           "TARGET:%s ETA%.1fs R%.1fkm REC:%s | F:R%d A%d O%d C%d M%d FF:%d COL:%d | SH:%d RQ:%d OS:%d",
                           target_type_name(s, lead_k),
                           s->k_eta_to_goal[lead_k],
                           s->k_range_to_core[lead_k] * km_per_px,
                           hunter_type_short_name(s->k_recommended_counter[lead_k]),
                           s->fail_range_mismatch,
                           s->fail_altitude_mismatch,
                           s->fail_overkill,
                           s->fail_ciws_misuse,
                           s->fail_manual_override_low_conf,
                           s->iff_ff_events,
                           s->iff_collateral_events,
                           s->h_swept_hit_events,
                           s->h_reacquire_events,
                           s->h_overshoot_events);
            lv_label_set_text(s->hud_info, info);
        }
        else
        {
            lv_label_set_text_fmt(s->hud_info,
                                  "No active target | DEF STK:%d AIR:%d | F:R%d A%d O%d C%d M%d FF:%d COL:%d SH:%d RQ:%d OS:%d",
                                  total_stock, hunters_air,
                                  s->fail_range_mismatch,
                                  s->fail_altitude_mismatch,
                                  s->fail_overkill,
                                  s->fail_ciws_misuse,
                                  s->fail_manual_override_low_conf,
                                  s->iff_ff_events,
                                  s->iff_collateral_events,
                                  s->h_swept_hit_events,
                                  s->h_reacquire_events,
                                  s->h_overshoot_events);
        }
        (void)snprintf(elapsed_line, sizeof(elapsed_line),
                       "EL %02d:%02d | DEF END:%d STK:%d AIR:%d AVL:%s | ENV:%s LOCK:%.2f CD %.2f/%.2f | FF:%s",
                       elapsed_mm, elapsed_ss,
                       def_endurance,
                       total_stock, hunters_air, availability,
                       env_fit,
                       lock_q,
                       s->ciws_cooldown_left_sec, s->ciws_cooldown_sec,
                       s->iff_advanced_mode ? "ADV" : "LOCK");
        if (s->fail_reason_ttl > 0.0f)
        {
            (void)snprintf(elapsed_line, sizeof(elapsed_line),
                           "ELAPSED %02d:%02d  |  WHY: %s",
                           elapsed_mm, elapsed_ss, s->fail_reason_text);
        }
        else if (s->iff_recovery_ttl > 0.0f)
        {
            (void)snprintf(elapsed_line, sizeof(elapsed_line),
                           "ELAPSED %02d:%02d  |  IFF RECOVERY %.1fs",
                           elapsed_mm, elapsed_ss, s->iff_recovery_ttl);
        }
        lv_label_set_text(s->hud_elapsed, elapsed_line);
    }
}

static void maybe_end_round(drone_hunter_scene_t *s)
{
    char subtitle[256];
    int total_stock = 0;
    int k;
    int defender_exhausted;
    int attacker_exhausted;

    if (s->round_over)
    {
        return;
    }
    for (k = 0; k < HUNTER_TYPE_COUNT; ++k)
    {
        total_stock += s->hunter_stock[k];
    }
    defender_exhausted = !defense_layer_remaining(s);
    attacker_exhausted = (s->wave_idx >= WIN_WAVE_TARGET) &&
                         (s->attack_remaining_to_spawn <= 0) &&
                         !s->killer_active[0] &&
                         !s->killer_active[1];

    if (defender_exhausted)
    {
        s->round_over = 1;
        (void)snprintf(subtitle, sizeof(subtitle),
                       "LOSS: defender inventory exhausted\nW%d CORE:%d LEAK:%d KILL:%d STOCK:%d CIWS:%d/%d",
                       s->wave_idx, s->core_hp, s->attack_leaked, s->attack_destroyed, total_stock,
                       s->ciws_ammo_left, s->ciws_ammo_right);
        show_overlay(s, "ROUND END", subtitle);
        return;
    }

    if (attacker_exhausted)
    {
        s->round_over = 1;
        (void)snprintf(subtitle, sizeof(subtitle),
                       "WIN: attacker inventory exhausted\nW%d CORE:%d LEAK:%d KILL:%d STOCK:%d CIWS:%d/%d",
                       s->wave_idx, s->core_hp, s->attack_leaked, s->attack_destroyed, total_stock,
                       s->ciws_ammo_left, s->ciws_ammo_right);
        show_overlay(s, "MISSION CLEAR", subtitle);
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

#if FREEZE_DIAG_MINIMAL_LOOP
    /* Extreme isolation mode: keep timer alive but bypass all arena updates. */
    s->t += 0.010f;
    return;
#endif

    s->t += 0.040f * speed_pp_mult(s);
    set_diag_stage(s, "DBG:ANIM_TICK");

    if (!s->round_over)
    {
        int k;

        s->round_time_sec -= DT_SEC;
#if FREEZE_DIAG_LOGIC_BYPASS
        /* Freeze isolation mode: bypass movement/engagement logic. */
        (void)core_x;
        (void)core_y;
#else
        set_diag_stage(s, "DBG:UPDATE_KILLERS");
        update_killers(s, core_x, core_y);

        set_diag_stage(s, "DBG:UPDATE_HUNTERS");
        for (h = 0; h < HUNTER_COUNT; ++h)
        {
            if (s->defender_mode_sel == DEFENDER_MODE_HUMAN)
            {
                int manual_gate = (h == 0) &&
                                  ((s->manual_selected_hunter >= 0) || (s->manual_selected_target >= 0));
                if (!s->hunter_loaded[h] && !manual_gate)
                {
                    continue;
                }
                if (h == 1 && !s->hunter_loaded[h])
                {
                    continue;
                }
            }
            update_hunter(s, h, core_x, core_y);
        }
#endif

        set_diag_stage(s, "DBG:DECAY_FX");
        for (k = 0; k < KILLER_COUNT; ++k)
        {
            s->fx_intercept_t[k] = clampf(s->fx_intercept_t[k] - DT_SEC, 0.0f, FX_INTERCEPT_SEC);
            s->fx_spawn_t[k] = clampf(s->fx_spawn_t[k] - DT_SEC, 0.0f, FX_SPAWN_SEC);
            s->fx_kill_t[k] = clampf(s->fx_kill_t[k] - DT_SEC, 0.0f, FX_KILL_SEC);
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
        s->fail_reason_ttl = clampf(s->fail_reason_ttl - DT_SEC, 0.0f, 10.0f);
        s->manual_select_ttl = clampf(s->manual_select_ttl - DT_SEC, 0.0f, 10.0f);
        s->iff_recovery_ttl = clampf(s->iff_recovery_ttl - DT_SEC, 0.0f, 10.0f);
        if ((s->manual_selected_target >= 0) &&
            ((s->manual_selected_target >= KILLER_COUNT) ||
             !s->killer_active[s->manual_selected_target] ||
             s->k_dying[s->manual_selected_target]))
        {
            s->manual_selected_target = -1;
        }

        if ((s->attack_remaining_to_spawn <= 0) && !s->killer_active[0] && !s->killer_active[1])
        {
            if (s->wave_idx < WIN_WAVE_TARGET)
            {
                start_wave(s, s->wave_idx + 1);
                for (h = 0; h < HUNTER_COUNT; ++h)
                {
                    s->h_reselect_sec[h] = 0.0f;
                }
            }
        }

        s->fx_core_hit_t = clampf(s->fx_core_hit_t - DT_SEC, 0.0f, FX_CORE_HIT_SEC);
#if !FREEZE_DIAG_LOGIC_BYPASS
        set_diag_stage(s, "DBG:END_ROUND_CHECK");
        maybe_end_round(s);
#endif
    }

#if !FREEZE_DIAG_LOGIC_BYPASS
    set_diag_stage(s, "DBG:UPDATE_POSITIONS");
    update_positions(s);
#endif
#if FREEZE_DIAG_EFFECTS_BYPASS
    /* Freeze isolation mode: bypass heavy FX render pass to keep game loop alive. */
#else
    set_diag_stage(s, "DBG:UPDATE_EFFECTS");
    update_effects(s, core_x, core_y);
#endif
    s->hud_refresh_t += DT_SEC;
    if (s->hud_refresh_t >= HUD_REFRESH_SEC)
    {
        s->hud_refresh_t = 0.0f;
        set_diag_stage(s, "DBG:UPDATE_HUD");
        update_hud(s);
    }
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
        lv_obj_set_style_transform_zoom(s->splash_hero, 204, 0);
        lv_obj_set_style_transform_width(s->splash_hero, 0, 0);
        lv_obj_set_style_transform_height(s->splash_hero, 0, 0);
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

    /* Top HUD text strip and SET|HELP button intentionally removed per UX request. */
    s->hud_mode = NULL;
    s->hud_hunter_score = NULL;
    s->hud_score = NULL;
    s->hud_attacker_score = NULL;
    s->hud_info = NULL;
    s->hud_wave = NULL;
    s->hud_elapsed = NULL;
    s->hud_attack_panel = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->hud_attack_panel);
    lv_obj_set_size(s->hud_attack_panel, 102, 88);
    lv_obj_align(s->hud_attack_panel, LV_ALIGN_TOP_LEFT, 10, 8);
    lv_obj_set_style_bg_color(s->hud_attack_panel, lv_color_hex(0x8FD3FF), 0);
    lv_obj_set_style_bg_opa(s->hud_attack_panel, (lv_opa_t)28, 0);
    lv_obj_set_style_border_color(s->hud_attack_panel, lv_color_hex(0xE6F4FF), 0);
    lv_obj_set_style_border_opa(s->hud_attack_panel, (lv_opa_t)70, 0);
    lv_obj_set_style_border_width(s->hud_attack_panel, 1, 0);
    lv_obj_set_style_radius(s->hud_attack_panel, 16, 0);
    lv_obj_set_style_shadow_color(s->hud_attack_panel, lv_color_hex(0x6BBEFF), 0);
    lv_obj_set_style_shadow_opa(s->hud_attack_panel, (lv_opa_t)38, 0);
    lv_obj_set_style_shadow_width(s->hud_attack_panel, 12, 0);
    lv_obj_add_flag(s->hud_attack_panel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(s->hud_attack_panel, hud_attack_card_cb, LV_EVENT_CLICKED, s);

    s->hud_defend_panel = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->hud_defend_panel);
    lv_obj_set_size(s->hud_defend_panel, 102, 88);
    lv_obj_align(s->hud_defend_panel, LV_ALIGN_TOP_RIGHT, -10, 8);
    lv_obj_set_style_bg_color(s->hud_defend_panel, lv_color_hex(0x8FD3FF), 0);
    lv_obj_set_style_bg_opa(s->hud_defend_panel, (lv_opa_t)28, 0);
    lv_obj_set_style_border_color(s->hud_defend_panel, lv_color_hex(0xE6F4FF), 0);
    lv_obj_set_style_border_opa(s->hud_defend_panel, (lv_opa_t)70, 0);
    lv_obj_set_style_border_width(s->hud_defend_panel, 1, 0);
    lv_obj_set_style_radius(s->hud_defend_panel, 16, 0);
    lv_obj_set_style_shadow_color(s->hud_defend_panel, lv_color_hex(0x6BBEFF), 0);
    lv_obj_set_style_shadow_opa(s->hud_defend_panel, (lv_opa_t)38, 0);
    lv_obj_set_style_shadow_width(s->hud_defend_panel, 12, 0);
    lv_obj_add_flag(s->hud_defend_panel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(s->hud_defend_panel, hud_defend_card_cb, LV_EVENT_CLICKED, s);

    s->hud_attack_label = lv_label_create(s->hud_attack_panel);
    lv_label_set_text(s->hud_attack_label, "ATTACK");
    lv_obj_set_style_text_font(s->hud_attack_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(s->hud_attack_label, lv_color_hex(0xFFB347), 0);
    lv_obj_set_style_text_letter_space(s->hud_attack_label, 2, 0);
    lv_obj_align(s->hud_attack_label, LV_ALIGN_TOP_MID, 0, 4);

    s->hud_defend_label = lv_label_create(s->hud_defend_panel);
    lv_label_set_text(s->hud_defend_label, "DEFEND");
    lv_obj_set_style_text_font(s->hud_defend_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(s->hud_defend_label, lv_color_hex(0x7DD3FC), 0);
    lv_obj_set_style_text_letter_space(s->hud_defend_label, 2, 0);
    lv_obj_align(s->hud_defend_label, LV_ALIGN_TOP_MID, 0, 4);

    s->hud_attack_digits = lv_label_create(s->hud_attack_panel);
    lv_label_set_text(s->hud_attack_digits, "0000");
    lv_obj_set_style_text_font(s->hud_attack_digits, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(s->hud_attack_digits, lv_color_hex(0xCFF2FF), 0);
    lv_obj_set_style_text_opa(s->hud_attack_digits, (lv_opa_t)234, 0);
    lv_obj_align(s->hud_attack_digits, LV_ALIGN_CENTER, 0, -2);

    s->hud_defend_digits = lv_label_create(s->hud_defend_panel);
    lv_label_set_text(s->hud_defend_digits, "0000");
    lv_obj_set_style_text_font(s->hud_defend_digits, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(s->hud_defend_digits, lv_color_hex(0xCFF2FF), 0);
    lv_obj_set_style_text_opa(s->hud_defend_digits, (lv_opa_t)234, 0);
    lv_obj_align(s->hud_defend_digits, LV_ALIGN_CENTER, 0, -2);

    s->hud_attack_mode_text = lv_label_create(s->hud_attack_panel);
    lv_label_set_text(s->hud_attack_mode_text, "ALGO");
    lv_obj_set_style_text_font(s->hud_attack_mode_text, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s->hud_attack_mode_text, lv_color_hex(0xD1ECFF), 0);
    lv_obj_align(s->hud_attack_mode_text, LV_ALIGN_BOTTOM_MID, 0, -4);

    s->hud_defend_mode_text = lv_label_create(s->hud_defend_panel);
    lv_label_set_text(s->hud_defend_mode_text, "ALGO");
    lv_obj_set_style_text_font(s->hud_defend_mode_text, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s->hud_defend_mode_text, lv_color_hex(0xD1ECFF), 0);
    lv_obj_align(s->hud_defend_mode_text, LV_ALIGN_BOTTOM_MID, 0, -4);

    /* Debug banner fully disabled in normal runtime UX. */
    s->hud_diag_stage = NULL;
    s->hud_diag_text[0] = '\0';

    s->mode_btn = NULL;
    s->mode_btn_label = NULL;

    s->deck_bar = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->deck_bar);
    lv_obj_set_size(s->deck_bar, sw - 16, DECK_H + 10);
    lv_obj_set_pos(s->deck_bar, 8, sh - (DECK_H + 10) - 6);
    lv_obj_set_style_bg_color(s->deck_bar, lv_color_hex(0x8FD3FF), 0);
    lv_obj_set_style_bg_opa(s->deck_bar, (lv_opa_t)34, 0);
    lv_obj_set_style_border_color(s->deck_bar, lv_color_hex(0xE6F4FF), 0);
    lv_obj_set_style_border_opa(s->deck_bar, (lv_opa_t)70, 0);
    lv_obj_set_style_border_width(s->deck_bar, 1, 0);
    lv_obj_set_style_radius(s->deck_bar, 16, 0);
    lv_obj_set_style_shadow_color(s->deck_bar, lv_color_hex(0x6BBEFF), 0);
    lv_obj_set_style_shadow_opa(s->deck_bar, (lv_opa_t)38, 0);
    lv_obj_set_style_shadow_width(s->deck_bar, 12, 0);
    lv_obj_add_flag(s->deck_bar, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    for (i = 0; i < HUNTER_TYPE_COUNT; ++i)
    {
        int32_t slot_w = (sw - 20) / (HUNTER_TYPE_COUNT + 1);
        int32_t x = 4 + (i * slot_w);
        int32_t iw;
        s->deck_icon[i] = lv_image_create(s->deck_bar);
        lv_image_set_src(s->deck_icon[i], hunter_icon_src((hunter_type_t)i));
        lv_obj_set_style_bg_opa(s->deck_icon[i], LV_OPA_0, 0);
        iw = lv_obj_get_width(s->deck_icon[i]);
        if (iw <= 0)
        {
            iw = 40;
        }
        {
            int32_t icon_y = 0;
            if ((i == HUNTER_BAGNET) || (i == HUNTER_ODIN_WIN_HIT))
            {
                icon_y += 8;
            }
            if (i == HUNTER_OCTOPUS_100)
            {
                icon_y += 5;
            }
            lv_obj_set_pos(s->deck_icon[i], x + ((slot_w - iw) / 2), icon_y);
        }
        lv_obj_set_style_transform_zoom(s->deck_icon[i], 320, 0);
        lv_obj_set_style_translate_y(s->deck_icon[i], 0, 0);
        lv_obj_set_style_transform_pivot_x(s->deck_icon[i], lv_obj_get_width(s->deck_icon[i]) / 2, 0);
        lv_obj_set_style_transform_pivot_y(s->deck_icon[i], lv_obj_get_height(s->deck_icon[i]) / 2, 0);
        lv_obj_set_style_outline_pad(s->deck_icon[i], 1, 0);
        lv_obj_add_flag(s->deck_icon[i], LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(s->deck_icon[i], deck_pick_cb, LV_EVENT_CLICKED, s);

        s->deck_name[i] = lv_label_create(s->deck_bar);
        lv_obj_set_style_text_font(s->deck_name[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(s->deck_name[i], lv_color_hex(0xE5E7EB), 0);
        lv_obj_set_style_text_align(s->deck_name[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(s->deck_name[i], slot_w);
        lv_label_set_text(s->deck_name[i], hunter_type_short_name((hunter_type_t)i));
        lv_label_set_long_mode(s->deck_name[i], LV_LABEL_LONG_CLIP);
        {
            int32_t name_y = 74;
            if ((i == HUNTER_STING_II) ||
                (i == HUNTER_BAGNET) ||
                (i == HUNTER_SKYFALL_P1) ||
                (i == HUNTER_OCTOPUS_100) ||
                (i == HUNTER_ODIN_WIN_HIT))
            {
                name_y += 6;
            }
            if ((i == HUNTER_VB140_FLAMINGO) || (i == HUNTER_TYTAN) || (i == HUNTER_MEROPS))
            {
                name_y += 6;
            }
            lv_obj_set_pos(s->deck_name[i], x, name_y);
        }

        s->deck_count[i] = lv_label_create(s->deck_bar);
        lv_obj_set_style_text_font(s->deck_count[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(s->deck_count[i], lv_color_hex(0xFDE68A), 0);
        lv_obj_set_style_text_align(s->deck_count[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(s->deck_count[i], slot_w);
        lv_label_set_text(s->deck_count[i], "0");
        {
            int32_t count_y = 88;
            if ((i == HUNTER_STING_II) ||
                (i == HUNTER_BAGNET) ||
                (i == HUNTER_SKYFALL_P1) ||
                (i == HUNTER_OCTOPUS_100) ||
                (i == HUNTER_ODIN_WIN_HIT))
            {
                count_y += 6;
            }
            if ((i == HUNTER_VB140_FLAMINGO) || (i == HUNTER_TYTAN) || (i == HUNTER_MEROPS))
            {
                count_y += 6;
            }
            lv_obj_set_pos(s->deck_count[i], x, count_y);
        }
    }
    {
        int32_t slot_w = (sw - 20) / (HUNTER_TYPE_COUNT + 1);
        int32_t x = 4 + (HUNTER_TYPE_COUNT * slot_w);
        int32_t iw;
        s->deck_ciws_icon = lv_image_create(s->deck_bar);
        lv_obj_set_style_bg_opa(s->deck_ciws_icon, LV_OPA_0, 0);
        lv_image_set_src(s->deck_ciws_icon, &img_hunter_ciws);
        iw = lv_obj_get_width(s->deck_ciws_icon);
        if (iw <= 0)
        {
            iw = 40;
        }
        lv_obj_set_pos(s->deck_ciws_icon, x + ((slot_w - iw) / 2), 12);
        lv_obj_set_style_transform_zoom(s->deck_ciws_icon, 150, 0);
        lv_obj_set_style_translate_y(s->deck_ciws_icon, 0, 0);
        lv_obj_add_flag(s->deck_ciws_icon, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(s->deck_ciws_icon, iff_toggle_cb, LV_EVENT_LONG_PRESSED, s);

        s->deck_ciws_name = lv_label_create(s->deck_bar);
        lv_obj_set_style_text_font(s->deck_ciws_name, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(s->deck_ciws_name, lv_color_hex(0xE5E7EB), 0);
        lv_obj_set_style_text_align(s->deck_ciws_name, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(s->deck_ciws_name, slot_w);
        lv_label_set_text(s->deck_ciws_name, "Phalanx");
        lv_label_set_long_mode(s->deck_ciws_name, LV_LABEL_LONG_CLIP);
        lv_obj_set_pos(s->deck_ciws_name, x, 80);
        lv_obj_add_flag(s->deck_ciws_name, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(s->deck_ciws_name, iff_toggle_cb, LV_EVENT_LONG_PRESSED, s);

        s->deck_ciws_count = lv_label_create(s->deck_bar);
        lv_obj_set_style_text_font(s->deck_ciws_count, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(s->deck_ciws_count, lv_color_hex(0x93C5FD), 0);
        lv_obj_set_style_text_align(s->deck_ciws_count, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(s->deck_ciws_count, slot_w);
        lv_label_set_text(s->deck_ciws_count, "x0");
        lv_obj_set_pos(s->deck_ciws_count, x, 94);
        lv_obj_add_flag(s->deck_ciws_count, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(s->deck_ciws_count, iff_toggle_cb, LV_EVENT_LONG_PRESSED, s);
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
        lv_obj_remove_style_all(s->killers[i]);
        lv_image_set_src(s->killers[i], &attack_shahed_yellow);
        s->k_last_src[i] = &attack_shahed_yellow;
        lv_obj_set_style_transform_zoom(s->killers[i], 185, 0);
        lv_obj_add_flag(s->killers[i], LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(s->killers[i], target_pick_cb, LV_EVENT_CLICKED, s);

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

        s->fx_kill[i] = lv_obj_create(s->arena);
        lv_obj_remove_style_all(s->fx_kill[i]);
        lv_obj_set_style_bg_opa(s->fx_kill[i], LV_OPA_0, 0);
        lv_obj_add_flag(s->fx_kill[i], LV_OBJ_FLAG_HIDDEN);
    }
    for (i = 0; i < CITY_FIRE_MAX; ++i)
    {
        s->city_fire[i] = lv_image_create(s->arena);
        lv_obj_remove_style_all(s->city_fire[i]);
        lv_image_set_src(s->city_fire[i], g_flame_profile_ground[0]);
        lv_image_set_pivot(s->city_fire[i], 0, 0);
        lv_obj_set_style_transform_zoom(s->city_fire[i], 256, 0);
        lv_obj_set_style_opa(s->city_fire[i], LV_OPA_COVER, 0);
        lv_obj_add_flag(s->city_fire[i], LV_OBJ_FLAG_HIDDEN);
        /* Keep fires above the background image but below drones/FX/HUD. */
        lv_obj_move_to_index(s->city_fire[i], 1);
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
        lv_image_set_src(s->hunters[i], hunter_image_src(HUNTER_STING_II));

        s->hunter_body[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_body[i]);
        lv_obj_set_size(s->hunter_body[i], 16, 7);
        lv_obj_set_pos(s->hunter_body[i], 11, 7);
        lv_obj_set_style_radius(s->hunter_body[i], 3, 0);
        lv_obj_set_style_bg_color(s->hunter_body[i], team_main, 0);
        lv_obj_set_style_bg_opa(s->hunter_body[i], LV_OPA_COVER, 0);
        lv_obj_add_flag(s->hunter_body[i], LV_OBJ_FLAG_HIDDEN);

        s->hunter_wing[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_wing[i]);
        lv_obj_set_size(s->hunter_wing[i], 14, 4);
        lv_obj_set_pos(s->hunter_wing[i], 8, 8);
        lv_obj_set_style_radius(s->hunter_wing[i], 2, 0);
        lv_obj_set_style_bg_color(s->hunter_wing[i], team_light, 0);
        lv_obj_set_style_bg_opa(s->hunter_wing[i], LV_OPA_COVER, 0);
        lv_obj_add_flag(s->hunter_wing[i], LV_OBJ_FLAG_HIDDEN);

        s->hunter_pod_upper[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_pod_upper[i]);
        lv_obj_set_size(s->hunter_pod_upper[i], 9, 5);
        lv_obj_set_pos(s->hunter_pod_upper[i], 5, 3);
        lv_obj_set_style_radius(s->hunter_pod_upper[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(s->hunter_pod_upper[i], team_light, 0);
        lv_obj_set_style_bg_opa(s->hunter_pod_upper[i], LV_OPA_COVER, 0);
        lv_obj_add_flag(s->hunter_pod_upper[i], LV_OBJ_FLAG_HIDDEN);

        s->hunter_pod_lower[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_pod_lower[i]);
        lv_obj_set_size(s->hunter_pod_lower[i], 9, 5);
        lv_obj_set_pos(s->hunter_pod_lower[i], 5, 12);
        lv_obj_set_style_radius(s->hunter_pod_lower[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(s->hunter_pod_lower[i], team_light, 0);
        lv_obj_set_style_bg_opa(s->hunter_pod_lower[i], LV_OPA_COVER, 0);
        lv_obj_add_flag(s->hunter_pod_lower[i], LV_OBJ_FLAG_HIDDEN);

        s->hunter_prop_upper[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_prop_upper[i]);
        lv_obj_set_size(s->hunter_prop_upper[i], 2, 8);
        lv_obj_set_pos(s->hunter_prop_upper[i], 4, 2);
        lv_obj_set_style_bg_color(s->hunter_prop_upper[i], lv_color_hex(0x111827), 0);
        lv_obj_set_style_bg_opa(s->hunter_prop_upper[i], LV_OPA_COVER, 0);
        lv_obj_add_flag(s->hunter_prop_upper[i], LV_OBJ_FLAG_HIDDEN);

        s->hunter_prop_lower[i] = lv_obj_create(s->hunters[i]);
        lv_obj_remove_style_all(s->hunter_prop_lower[i]);
        lv_obj_set_size(s->hunter_prop_lower[i], 2, 8);
        lv_obj_set_pos(s->hunter_prop_lower[i], 4, 10);
        lv_obj_set_style_bg_color(s->hunter_prop_lower[i], lv_color_hex(0x111827), 0);
        lv_obj_set_style_bg_opa(s->hunter_prop_lower[i], LV_OPA_COVER, 0);
        lv_obj_add_flag(s->hunter_prop_lower[i], LV_OBJ_FLAG_HIDDEN);
    }

    s->quick_menu = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->quick_menu);
    lv_obj_set_size(s->quick_menu, sw - 110, 150);
    lv_obj_align(s->quick_menu, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(s->quick_menu, lv_color_hex(0x0B1220), 0);
    lv_obj_set_style_bg_opa(s->quick_menu, LV_OPA_90, 0);
    lv_obj_set_style_border_color(s->quick_menu, lv_color_hex(0x22D3EE), 0);
    lv_obj_set_style_border_width(s->quick_menu, 2, 0);
    lv_obj_add_flag(s->quick_menu, LV_OBJ_FLAG_HIDDEN);

    s->quick_menu_title = lv_label_create(s->quick_menu);
    lv_obj_set_style_text_color(s->quick_menu_title, lv_color_hex(0xE2E8F0), 0);
    lv_obj_set_style_text_font(s->quick_menu_title, &lv_font_montserrat_16, 0);
    lv_obj_align(s->quick_menu_title, LV_ALIGN_TOP_LEFT, 12, 8);
    lv_label_set_text(s->quick_menu_title, "Menu");

    s->quick_menu_body = lv_label_create(s->quick_menu);
    lv_obj_set_width(s->quick_menu_body, sw - 150);
    lv_obj_set_style_text_color(s->quick_menu_body, lv_color_hex(0x93C5FD), 0);
    lv_obj_set_style_text_font(s->quick_menu_body, &lv_font_montserrat_12, 0);
    lv_label_set_long_mode(s->quick_menu_body, LV_LABEL_LONG_WRAP);
    lv_obj_align(s->quick_menu_body, LV_ALIGN_TOP_LEFT, 12, 36);
    lv_label_set_text(s->quick_menu_body, "");
    lv_obj_add_flag(s->quick_menu_body, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(s->quick_menu_body, quick_menu_body_cb, LV_EVENT_CLICKED, s);

    s->quick_menu_close = lv_btn_create(s->quick_menu);
    lv_obj_set_size(s->quick_menu_close, 70, 28);
    lv_obj_align(s->quick_menu_close, LV_ALIGN_TOP_RIGHT, -8, 8);
    lv_obj_set_style_bg_color(s->quick_menu_close, lv_color_hex(0x2563EB), 0);
    lv_obj_set_style_bg_opa(s->quick_menu_close, LV_OPA_COVER, 0);
    lv_obj_add_event_cb(s->quick_menu_close, quick_menu_close_cb, LV_EVENT_CLICKED, s);
    {
        lv_obj_t *close_lbl = lv_label_create(s->quick_menu_close);
        lv_label_set_text(close_lbl, "CLOSE");
        lv_obj_set_style_text_color(close_lbl, lv_color_hex(0xFFFFFF), 0);
        lv_obj_center(close_lbl);
    }

    s->flame_gallery = lv_obj_create(s->arena);
    lv_obj_remove_style_all(s->flame_gallery);
    lv_obj_set_size(s->flame_gallery, s->arena_w - 20, s->arena_h - 24);
    lv_obj_align(s->flame_gallery, LV_ALIGN_CENTER, 0, 8);
    lv_obj_set_style_bg_color(s->flame_gallery, lv_color_hex(0x030712), 0);
    lv_obj_set_style_bg_opa(s->flame_gallery, LV_OPA_90, 0);
    lv_obj_set_style_border_color(s->flame_gallery, lv_color_hex(0x67E8F9), 0);
    lv_obj_set_style_border_width(s->flame_gallery, 2, 0);
    lv_obj_set_style_radius(s->flame_gallery, 12, 0);
    lv_obj_add_flag(s->flame_gallery, LV_OBJ_FLAG_HIDDEN);
    s->flame_gallery_visible = 0;
    {
        lv_obj_t *title = lv_label_create(s->flame_gallery);
        lv_label_set_text(title, "Flame Gallery (tap panel to close)");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(title, lv_color_hex(0xE0F2FE), 0);
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 6);
    }
    for (i = 0; i < CITY_FIRE_PROFILE_COUNT; ++i)
    {
        int col = i % 3;
        int row = i / 3;
        int32_t pad_x = 8;
        int32_t top_y = 34;
        int32_t cell_w = (s->arena_w - 20 - (pad_x * 2)) / 3;
        int32_t cell_h = 52;
        lv_obj_t *cell = lv_obj_create(s->flame_gallery);
        lv_obj_t *img;
        lv_obj_t *lbl;
        const lv_image_dsc_t **frames = city_fire_profile_frames(i);
        lv_obj_remove_style_all(cell);
        lv_obj_set_size(cell, cell_w - 6, cell_h - 4);
        lv_obj_set_pos(cell, pad_x + (col * cell_w), top_y + (row * cell_h));
        lv_obj_set_style_bg_color(cell, lv_color_hex(0x0F172A), 0);
        lv_obj_set_style_bg_opa(cell, (lv_opa_t)175, 0);
        lv_obj_set_style_border_color(cell, lv_color_hex(0x1E293B), 0);
        lv_obj_set_style_border_width(cell, 1, 0);
        lv_obj_set_style_radius(cell, 8, 0);
        lv_obj_add_flag(cell, LV_OBJ_FLAG_EVENT_BUBBLE);

        img = lv_image_create(cell);
        lv_image_set_src(img, frames[2 % FLAME_SPRITE_FRAME_COUNT]);
        lv_obj_set_style_transform_zoom(img, 176, 0);
        lv_obj_set_style_image_recolor(img, city_fire_profile_preview_tint(i), 0);
        lv_obj_set_style_image_recolor_opa(img, city_fire_profile_preview_tint_opa(i), 0);
        lv_obj_align(img, LV_ALIGN_LEFT_MID, 8, 0);
        lv_obj_add_flag(img, LV_OBJ_FLAG_EVENT_BUBBLE);

        lbl = lv_label_create(cell);
        lv_label_set_text(lbl, city_fire_profile_name(i));
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(lbl, lv_color_hex(0xE2E8F0), 0);
        lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 56, -1);
        lv_obj_add_flag(lbl, LV_OBJ_FLAG_EVENT_BUBBLE);
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
    lv_obj_set_width(s->overlay_subtitle, sw - 132);
    lv_label_set_long_mode(s->overlay_subtitle, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(s->overlay_subtitle, LV_TEXT_ALIGN_CENTER, 0);
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

    s->attack_strategy_select = ATTACK_STRATEGY_AUTO;
    s->attack_strategy_live = ATTACK_STRATEGY_CENTER_PRESSURE;
    s->quick_menu_settings_active = 0;
    s->attacker_mode_sel = CTRL_EDGEAI;
    s->defender_mode_sel = DEFENDER_MODE_ALGO;
    s->difficulty_sel = DIFFICULTY_MEDIUM;
    s->npu_enabled = 1;
    s->speed_pp_idx = 0;
    assign_mode(s, MODE_ALGO_VS_EDGEAI);

    /* Keep bottom deck UI above arena action without per-frame z-order changes. */
    lv_obj_move_foreground(s->deck_bar);

    reset_round(s);

    s->anim_timer = lv_timer_create(anim_cb, TICK_MS, s);
    s->splash_timer = lv_timer_create(splash_tick_cb, TICK_MS, s);
}
