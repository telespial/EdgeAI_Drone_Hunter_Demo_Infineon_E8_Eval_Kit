#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    const uint8_t *data;
    uint32_t len_bytes;
} dh_audio_clip_t;

bool dh_audio_get_clip(uint32_t event_id, dh_audio_clip_t *out_clip);
