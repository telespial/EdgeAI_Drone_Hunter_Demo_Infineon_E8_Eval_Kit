#include "drone_hunter_audio_hal.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cy_pdl.h"
#include "cycfg_peripherals.h"
#include "cy_scb_i2c.h"
#include "audio_assets/drone_hunter_audio_assets.h"
/* Keep audio assets in this translation unit for fixed ninja graphs that don't auto-pick new .c files. */
#include "audio_assets/drone_hunter_audio_assets.c"

/* Shared I2C context from main.c used by display/touch controller path. */
extern cy_stc_scb_i2c_context_t disp_touch_i2c_controller_context;

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

#define DH_AUDIO_SAMPLE_RATE_HZ   (16000u)
#define DH_AUDIO_I2C_ADDR_CODEC   (0x18u)
#define DH_AUDIO_I2C_TIMEOUT_MS   (12u)
#define DH_AUDIO_I2C_RETRIES      (3u)
#define DH_AUDIO_EVENT_MAX        (12u)
#define DH_AUDIO_QUEUE_MASK       (31u)
#define DH_AUDIO_FADE_IN_MS       (120u)
#define DH_AUDIO_FADE_OUT_MS      (180u)

typedef enum
{
    DH_VOICE_BG_CITY = 0,
    DH_VOICE_BG_DRONE,
    DH_VOICE_SFX,
    DH_VOICE_COUNT
} dh_voice_id_t;

typedef struct
{
    const uint8_t *data;
    uint32_t len_bytes;
    uint32_t pos_bytes;
    uint32_t max_samples;
    bool active;
    bool loop;
    int32_t gain_cur_q12;
    int32_t gain_target_q12;
    int32_t gain_step_q12;
} dh_voice_t;

typedef struct
{
    bool init_ok;
    uint8_t q_event[32];
    uint16_t q_gain_q12[32];
    uint8_t q_head;
    uint8_t q_tail;
    uint16_t master_gain_q12;
    dh_voice_t voice[DH_VOICE_COUNT];
    uint8_t bg_drone_event;
    uint32_t rng_state;
} dh_audio_state_t;

static dh_audio_state_t s_audio;

static int16_t dh_audio_next_sample(void);
static void dh_audio_fill_tx_fifo(void);
static void dh_audio_tdm_tx_isr(void);

static uint32_t dh_rand_next(void)
{
    uint32_t x = s_audio.rng_state;
    if (x == 0u)
    {
        x = 0xA341316Cu;
    }
    x = (x * 1664525u) + 1013904223u;
    s_audio.rng_state = x;
    return x;
}

static uint16_t dh_gain_from_float(float gain)
{
    int32_t q12;
    if (gain < 0.0f)
    {
        gain = 0.0f;
    }
    if (gain > 1.0f)
    {
        gain = 1.0f;
    }
    q12 = (int32_t)(gain * 4096.0f);
    if (q12 < 0)
    {
        q12 = 0;
    }
    if (q12 > 4096)
    {
        q12 = 4096;
    }
    return (uint16_t)q12;
}

static int32_t dh_gain_step_q12(uint32_t fade_ms)
{
    uint32_t fade_samples = (DH_AUDIO_SAMPLE_RATE_HZ * fade_ms) / 1000u;
    if (fade_samples == 0u)
    {
        return 4096;
    }
    return (int32_t)(4096u / fade_samples) + 1;
}

static void dh_voice_apply_fade(dh_voice_t *v)
{
    if (v->gain_cur_q12 < v->gain_target_q12)
    {
        v->gain_cur_q12 += v->gain_step_q12;
        if (v->gain_cur_q12 > v->gain_target_q12)
        {
            v->gain_cur_q12 = v->gain_target_q12;
        }
    }
    else if (v->gain_cur_q12 > v->gain_target_q12)
    {
        v->gain_cur_q12 -= v->gain_step_q12;
        if (v->gain_cur_q12 < v->gain_target_q12)
        {
            v->gain_cur_q12 = v->gain_target_q12;
        }
    }
}

static void dh_voice_start(dh_voice_t *v, const dh_audio_clip_t *clip, bool loop, uint16_t gain_q12, uint32_t max_samples)
{
    v->data = clip->data;
    v->len_bytes = clip->len_bytes & ~1u;
    v->pos_bytes = 0u;
    v->max_samples = max_samples;
    v->active = (v->data != NULL) && (v->len_bytes >= 2u);
    v->loop = loop;
    v->gain_cur_q12 = 0;
    v->gain_target_q12 = (int32_t)gain_q12;
    v->gain_step_q12 = dh_gain_step_q12(DH_AUDIO_FADE_IN_MS);
}

static int16_t dh_voice_next_sample(dh_voice_t *v)
{
    int16_t out = 0;
    uint32_t played_samples;
    uint32_t clip_samples;

    if (!v->active)
    {
        return 0;
    }

    if ((v->data == NULL) || (v->len_bytes < 2u))
    {
        v->active = false;
        return 0;
    }

    played_samples = v->pos_bytes / 2u;
    clip_samples = v->len_bytes / 2u;

    if (v->max_samples > 0u && played_samples >= v->max_samples)
    {
        v->gain_target_q12 = 0;
        v->gain_step_q12 = dh_gain_step_q12(DH_AUDIO_FADE_OUT_MS);
    }

    if (v->pos_bytes + 1u >= v->len_bytes)
    {
        if (v->loop)
        {
            v->pos_bytes = 0u;
            played_samples = 0u;
        }
        else
        {
            v->gain_target_q12 = 0;
        }
    }

    if (!v->loop)
    {
        uint32_t remain_samples = (played_samples < clip_samples) ? (clip_samples - played_samples) : 0u;
        uint32_t fade_out_samples = (DH_AUDIO_SAMPLE_RATE_HZ * DH_AUDIO_FADE_OUT_MS) / 1000u;
        if (remain_samples <= fade_out_samples)
        {
            v->gain_target_q12 = 0;
            v->gain_step_q12 = dh_gain_step_q12(DH_AUDIO_FADE_OUT_MS);
        }
    }

    dh_voice_apply_fade(v);

    if (v->gain_cur_q12 == 0 && v->gain_target_q12 == 0 && !v->loop)
    {
        v->active = false;
        return 0;
    }

    if (v->pos_bytes + 1u < v->len_bytes)
    {
        uint32_t p = v->pos_bytes;
        int16_t raw = (int16_t)((uint16_t)v->data[p] | ((uint16_t)v->data[p + 1u] << 8));
        int32_t scaled = ((int32_t)raw * v->gain_cur_q12) / 4096;
        if (scaled > 32767)
        {
            scaled = 32767;
        }
        if (scaled < -32768)
        {
            scaled = -32768;
        }
        out = (int16_t)scaled;
        v->pos_bytes += 2u;
    }

    return out;
}

static bool dh_audio_codec_reg_write(uint8_t reg, uint8_t val)
{
    uint32_t r;
    for (r = 0u; r < DH_AUDIO_I2C_RETRIES; r++)
    {
        cy_en_scb_i2c_status_t rc = Cy_SCB_I2C_MasterSendStart(CYBSP_I2C_CONTROLLER_HW, DH_AUDIO_I2C_ADDR_CODEC,
                                                                CY_SCB_I2C_WRITE_XFER, DH_AUDIO_I2C_TIMEOUT_MS,
                                                                &disp_touch_i2c_controller_context);
        if (rc != CY_SCB_I2C_SUCCESS)
        {
            continue;
        }

        rc = Cy_SCB_I2C_MasterWriteByte(CYBSP_I2C_CONTROLLER_HW, reg, DH_AUDIO_I2C_TIMEOUT_MS,
                                        &disp_touch_i2c_controller_context);
        if (rc != CY_SCB_I2C_SUCCESS)
        {
            (void)Cy_SCB_I2C_MasterSendStop(CYBSP_I2C_CONTROLLER_HW, DH_AUDIO_I2C_TIMEOUT_MS,
                                            &disp_touch_i2c_controller_context);
            continue;
        }

        rc = Cy_SCB_I2C_MasterWriteByte(CYBSP_I2C_CONTROLLER_HW, val, DH_AUDIO_I2C_TIMEOUT_MS,
                                        &disp_touch_i2c_controller_context);
        (void)Cy_SCB_I2C_MasterSendStop(CYBSP_I2C_CONTROLLER_HW, DH_AUDIO_I2C_TIMEOUT_MS,
                                        &disp_touch_i2c_controller_context);
        if (rc == CY_SCB_I2C_SUCCESS)
        {
            return true;
        }
    }
    return false;
}

static bool dh_audio_codec_init_tlv320dac3100(void)
{
    bool ok = true;
    ok &= dh_audio_codec_reg_write(0x00u, 0x00u);
    ok &= dh_audio_codec_reg_write(0x01u, 0x01u);
    Cy_SysLib_Delay(2u);

    ok &= dh_audio_codec_reg_write(0x04u, 0x03u);
    ok &= dh_audio_codec_reg_write(0x06u, 0x08u);
    ok &= dh_audio_codec_reg_write(0x07u, 0x00u);
    ok &= dh_audio_codec_reg_write(0x08u, 0x00u);
    ok &= dh_audio_codec_reg_write(0x05u, 0x91u);
    Cy_SysLib_Delay(10u);

    ok &= dh_audio_codec_reg_write(0x0Bu, 0x88u);
    ok &= dh_audio_codec_reg_write(0x0Cu, 0x82u);
    ok &= dh_audio_codec_reg_write(0x0Du, 0x00u);
    ok &= dh_audio_codec_reg_write(0x0Eu, 0x80u);
    ok &= dh_audio_codec_reg_write(0x1Bu, 0x00u);
    ok &= dh_audio_codec_reg_write(0x3Cu, 0x0Bu);
    ok &= dh_audio_codec_reg_write(0x00u, 0x08u);
    ok &= dh_audio_codec_reg_write(0x01u, 0x04u);
    ok &= dh_audio_codec_reg_write(0x00u, 0x00u);
    ok &= dh_audio_codec_reg_write(0x74u, 0x00u);

    ok &= dh_audio_codec_reg_write(0x00u, 0x01u);
    ok &= dh_audio_codec_reg_write(0x1Fu, 0x04u);
    ok &= dh_audio_codec_reg_write(0x21u, 0x4Eu);
    ok &= dh_audio_codec_reg_write(0x23u, 0x44u);
    ok &= dh_audio_codec_reg_write(0x28u, 0x06u);
    ok &= dh_audio_codec_reg_write(0x29u, 0x06u);
    ok &= dh_audio_codec_reg_write(0x2Au, 0x1Cu);
    ok &= dh_audio_codec_reg_write(0x1Fu, 0xC2u);
    ok &= dh_audio_codec_reg_write(0x20u, 0x86u);
    ok &= dh_audio_codec_reg_write(0x24u, 0x92u);
    ok &= dh_audio_codec_reg_write(0x25u, 0x92u);
    ok &= dh_audio_codec_reg_write(0x26u, 0x92u);
    Cy_SysLib_Delay(60u);

    ok &= dh_audio_codec_reg_write(0x00u, 0x00u);
    ok &= dh_audio_codec_reg_write(0x3Fu, 0xD4u);
    ok &= dh_audio_codec_reg_write(0x41u, 0xD4u);
    ok &= dh_audio_codec_reg_write(0x42u, 0xD4u);
    ok &= dh_audio_codec_reg_write(0x40u, 0x00u);
    return ok;
}

static bool dh_audio_queue_push(uint8_t event_id, uint16_t gain_q12)
{
    uint8_t next_tail = (uint8_t)((s_audio.q_tail + 1u) & DH_AUDIO_QUEUE_MASK);
    if (next_tail == s_audio.q_head)
    {
        return false;
    }
    s_audio.q_event[s_audio.q_tail] = event_id;
    s_audio.q_gain_q12[s_audio.q_tail] = gain_q12;
    s_audio.q_tail = next_tail;
    return true;
}

static bool dh_audio_queue_pop(uint8_t *event_id, uint16_t *gain_q12)
{
    if (s_audio.q_head == s_audio.q_tail)
    {
        return false;
    }
    *event_id = s_audio.q_event[s_audio.q_head];
    *gain_q12 = s_audio.q_gain_q12[s_audio.q_head];
    s_audio.q_head = (uint8_t)((s_audio.q_head + 1u) & DH_AUDIO_QUEUE_MASK);
    return true;
}

static void dh_audio_set_bg_drone(uint8_t event_id, uint16_t gain_q12)
{
    dh_audio_clip_t clip;
    if (!dh_audio_get_clip(event_id, &clip))
    {
        return;
    }
    s_audio.bg_drone_event = event_id;
    dh_voice_start(&s_audio.voice[DH_VOICE_BG_DRONE], &clip, true, gain_q12, 0u);
}

static void dh_audio_start_random_city_segment(void)
{
    dh_audio_clip_t clip;
    uint8_t pick = (uint8_t)(dh_rand_next() % 3u);
    uint8_t ev = DH_AUDIO_EVENT_CITY_TRAFFIC_AMBIENT;
    uint32_t seg_sec = 3u + (dh_rand_next() % 4u); /* 3..6 sec */
    float gain = 0.10f;

    if (pick == 1u)
    {
        ev = DH_AUDIO_EVENT_GUNFIRE1;
        gain = 0.09f;
    }
    else if (pick == 2u)
    {
        ev = DH_AUDIO_EVENT_GUNFIRE2;
        gain = 0.09f;
    }
    else
    {
        ev = DH_AUDIO_EVENT_CITY_TRAFFIC_AMBIENT;
        gain = 0.11f;
    }

    if (!dh_audio_get_clip(ev, &clip))
    {
        return;
    }

    dh_voice_start(&s_audio.voice[DH_VOICE_BG_CITY], &clip, false, dh_gain_from_float(gain), seg_sec * DH_AUDIO_SAMPLE_RATE_HZ);
}

static bool dh_audio_init_once(void)
{
    cy_en_tdm_status_t st;
    if (s_audio.init_ok)
    {
        return true;
    }

    s_audio.rng_state = 0x1234ABCDu;

    (void)dh_audio_codec_init_tlv320dac3100();
    st = Cy_AudioTDM_Init(CYBSP_TDM_CONTROLLER_0_HW, &CYBSP_TDM_CONTROLLER_0_config);
    if (st != CY_TDM_SUCCESS)
    {
        s_audio.init_ok = false;
        return false;
    }

    Cy_AudioTDM_EnableTx(CYBSP_TDM_CONTROLLER_0_TX_HW);
    dh_audio_fill_tx_fifo();

    Cy_AudioTDM_ClearTxInterrupt(CYBSP_TDM_CONTROLLER_0_TX_HW,
                                 CY_TDM_INTR_TX_FIFO_TRIGGER | CY_TDM_INTR_TX_FIFO_OVERFLOW |
                                     CY_TDM_INTR_TX_FIFO_UNDERFLOW | CY_TDM_INTR_TX_IF_UNDERFLOW);
    Cy_AudioTDM_SetTxInterruptMask(CYBSP_TDM_CONTROLLER_0_TX_HW,
                                   CY_TDM_INTR_TX_FIFO_TRIGGER | CY_TDM_INTR_TX_FIFO_UNDERFLOW);
    NVIC_SetVector(CYBSP_TDM_CONTROLLER_0_TX_IRQ, (uint32_t)dh_audio_tdm_tx_isr);
    NVIC_ClearPendingIRQ(CYBSP_TDM_CONTROLLER_0_TX_IRQ);
    NVIC_EnableIRQ(CYBSP_TDM_CONTROLLER_0_TX_IRQ);
    Cy_AudioTDM_ActivateTx(CYBSP_TDM_CONTROLLER_0_TX_HW);

    s_audio.master_gain_q12 = 44010u; /* 3x louder than previous master setting. */
    s_audio.bg_drone_event = DH_AUDIO_EVENT_DRONE_FIXED_WING_LOOP;

    dh_audio_start_random_city_segment();
    dh_audio_set_bg_drone(DH_AUDIO_EVENT_DRONE_FIXED_WING_LOOP, dh_gain_from_float(0.42f));

    s_audio.init_ok = true;
    return true;
}

void drone_hunter_audio_play_event(uint32_t event_id, const char *asset_name, float gain, uint8_t looped)
{
    uint16_t gain_q12;
    (void)asset_name;

    if (!dh_audio_init_once())
    {
        return;
    }
    if (event_id >= DH_AUDIO_EVENT_MAX)
    {
        return;
    }

    gain_q12 = dh_gain_from_float(gain);

    if (looped)
    {
        if ((event_id == DH_AUDIO_EVENT_DRONE_FIXED_WING_LOOP) || (event_id == DH_AUDIO_EVENT_DRONE_FPV_LOOP))
        {
            float bg_gain = (event_id == DH_AUDIO_EVENT_DRONE_FPV_LOOP) ? 0.36f : 0.42f;
            if (s_audio.bg_drone_event != (uint8_t)event_id)
            {
                dh_audio_set_bg_drone((uint8_t)event_id, dh_gain_from_float(bg_gain));
            }
            return;
        }
        if (event_id == DH_AUDIO_EVENT_CITY_TRAFFIC_AMBIENT)
        {
            /* City bed is already random (traffic/gunfire1/gunfire2 segments). */
            return;
        }
    }

    (void)dh_audio_queue_push((uint8_t)event_id, gain_q12);
}

void drone_hunter_audio_heartbeat(void)
{
    if (!dh_audio_init_once())
    {
        return;
    }
    dh_audio_fill_tx_fifo();
}

static int16_t dh_audio_next_sample(void)
{
    int32_t mix = 0;

    if (!s_audio.voice[DH_VOICE_BG_CITY].active)
    {
        dh_audio_start_random_city_segment();
    }

    if (!s_audio.voice[DH_VOICE_SFX].active)
    {
        uint8_t ev;
        uint16_t g;
        if (dh_audio_queue_pop(&ev, &g))
        {
            dh_audio_clip_t clip;
            uint32_t max_samples = 0u;
            if (dh_audio_get_clip(ev, &clip))
            {
                if (ev == DH_AUDIO_EVENT_CIWS_FIRE)
                {
                    /* Keep CIWS fire as a short, punchy burst per trigger. */
                    max_samples = DH_AUDIO_SAMPLE_RATE_HZ / 2u; /* 0.5 s */
                }
                dh_voice_start(&s_audio.voice[DH_VOICE_SFX], &clip, false, g, max_samples);
            }
        }
    }

    mix += (int32_t)dh_voice_next_sample(&s_audio.voice[DH_VOICE_BG_CITY]);
    mix += (int32_t)dh_voice_next_sample(&s_audio.voice[DH_VOICE_BG_DRONE]);
    mix += (int32_t)dh_voice_next_sample(&s_audio.voice[DH_VOICE_SFX]);

    mix = (int32_t)(((int64_t)mix * (int64_t)s_audio.master_gain_q12) / 4096LL);
    if (mix > 32767)
    {
        mix = 32767;
    }
    if (mix < -32768)
    {
        mix = -32768;
    }

    return (int16_t)mix;
}

static void dh_audio_fill_tx_fifo(void)
{
    while (Cy_AudioTDM_GetNumInTxFifo(CYBSP_TDM_CONTROLLER_0_TX_HW) < 120u)
    {
        uint16_t u = (uint16_t)dh_audio_next_sample();
        Cy_AudioTDM_WriteTxData(CYBSP_TDM_CONTROLLER_0_TX_HW, (uint32_t)u);
        Cy_AudioTDM_WriteTxData(CYBSP_TDM_CONTROLLER_0_TX_HW, (uint32_t)u);
    }
}

static void dh_audio_tdm_tx_isr(void)
{
    uint32_t st = Cy_AudioTDM_GetTxInterruptStatusMasked(CYBSP_TDM_CONTROLLER_0_TX_HW);
    if (st != 0u)
    {
        Cy_AudioTDM_ClearTxInterrupt(CYBSP_TDM_CONTROLLER_0_TX_HW, st);
        dh_audio_fill_tx_fifo();
    }
}
