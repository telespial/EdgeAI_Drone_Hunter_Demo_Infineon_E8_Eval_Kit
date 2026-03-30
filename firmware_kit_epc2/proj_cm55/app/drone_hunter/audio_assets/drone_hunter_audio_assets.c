#include "drone_hunter_audio_assets.h"

#include <stddef.h>

#include "generated/dh_clip_attack_success.inc"
#include "generated/dh_clip_hunter_kill.inc"
#include "generated/dh_clip_ciws_kill.inc"
#include "generated/dh_clip_siren_a.inc"
#include "generated/dh_clip_siren_b.inc"
#include "generated/dh_clip_city_traffic.inc"
#include "generated/dh_clip_city_fire.inc"
#include "generated/dh_clip_drone_fixed.inc"
#include "generated/dh_clip_drone_fpv.inc"
#include "generated/dh_clip_gunfire1.inc"
#include "generated/dh_clip_gunfire2.inc"
#include "generated/dh_clip_ciws_fire.inc"

/* Keep in sync with dh_sound_event_t ordering in drone_hunter_arena.c. */
#define DH_AUDIO_EVENT_ATTACK_SUCCESS_LOUD   (0u)
#define DH_AUDIO_EVENT_HUNTER_KILL_MEDIUM    (1u)
#define DH_AUDIO_EVENT_CIWS_KILL_LIGHT       (2u)
#define DH_AUDIO_EVENT_CITY_SIREN_A          (3u)
#define DH_AUDIO_EVENT_CITY_SIREN_B          (4u)
#define DH_AUDIO_EVENT_CITY_TRAFFIC_AMBIENT  (5u)
#define DH_AUDIO_EVENT_CITY_FIRE_LOOP        (6u)
#define DH_AUDIO_EVENT_DRONE_FIXED_WING_LOOP (7u)
#define DH_AUDIO_EVENT_DRONE_FPV_LOOP        (8u)
#define DH_AUDIO_EVENT_GUNFIRE1              (9u)
#define DH_AUDIO_EVENT_GUNFIRE2              (10u)
#define DH_AUDIO_EVENT_CIWS_FIRE             (11u)

bool dh_audio_get_clip(uint32_t event_id, dh_audio_clip_t *out_clip)
{
    if (out_clip == NULL)
    {
        return false;
    }

    switch (event_id)
    {
        case DH_AUDIO_EVENT_ATTACK_SUCCESS_LOUD:
            out_clip->data = dh_clip_attack_success;
            out_clip->len_bytes = (uint32_t)dh_clip_attack_success_len;
            return true;
        case DH_AUDIO_EVENT_HUNTER_KILL_MEDIUM:
            out_clip->data = dh_clip_hunter_kill;
            out_clip->len_bytes = (uint32_t)dh_clip_hunter_kill_len;
            return true;
        case DH_AUDIO_EVENT_CIWS_KILL_LIGHT:
            out_clip->data = dh_clip_ciws_kill;
            out_clip->len_bytes = (uint32_t)dh_clip_ciws_kill_len;
            return true;
        case DH_AUDIO_EVENT_CITY_SIREN_A:
            out_clip->data = dh_clip_siren_a;
            out_clip->len_bytes = (uint32_t)dh_clip_siren_a_len;
            return true;
        case DH_AUDIO_EVENT_CITY_SIREN_B:
            out_clip->data = dh_clip_siren_b;
            out_clip->len_bytes = (uint32_t)dh_clip_siren_b_len;
            return true;
        case DH_AUDIO_EVENT_CITY_TRAFFIC_AMBIENT:
            out_clip->data = dh_clip_city_traffic;
            out_clip->len_bytes = (uint32_t)dh_clip_city_traffic_len;
            return true;
        case DH_AUDIO_EVENT_CITY_FIRE_LOOP:
            out_clip->data = dh_clip_city_fire;
            out_clip->len_bytes = (uint32_t)dh_clip_city_fire_len;
            return true;
        case DH_AUDIO_EVENT_DRONE_FIXED_WING_LOOP:
            out_clip->data = dh_clip_drone_fixed;
            out_clip->len_bytes = (uint32_t)dh_clip_drone_fixed_len;
            return true;
        case DH_AUDIO_EVENT_DRONE_FPV_LOOP:
            out_clip->data = dh_clip_drone_fpv;
            out_clip->len_bytes = (uint32_t)dh_clip_drone_fpv_len;
            return true;
        case DH_AUDIO_EVENT_GUNFIRE1:
            out_clip->data = dh_clip_gunfire1;
            out_clip->len_bytes = (uint32_t)dh_clip_gunfire1_len;
            return true;
        case DH_AUDIO_EVENT_GUNFIRE2:
            out_clip->data = dh_clip_gunfire2;
            out_clip->len_bytes = (uint32_t)dh_clip_gunfire2_len;
            return true;
        case DH_AUDIO_EVENT_CIWS_FIRE:
            out_clip->data = dh_clip_ciws_fire;
            out_clip->len_bytes = (uint32_t)dh_clip_ciws_fire_len;
            return true;
        default:
            out_clip->data = NULL;
            out_clip->len_bytes = 0u;
            return false;
    }
}
