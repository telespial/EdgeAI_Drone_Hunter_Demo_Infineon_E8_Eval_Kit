#pragma once

#include <stdint.h>

/* Non-blocking service hook; call once per frame/tick. */
void drone_hunter_audio_heartbeat(void);

/* Event bridge consumed by drone_hunter_arena.c sound mapping layer. */
void drone_hunter_audio_play_event(uint32_t event_id, const char *asset_name, float gain, uint8_t looped);

/* Runtime global mute toggle from HUD control. */
void drone_hunter_audio_set_muted(uint8_t muted);
