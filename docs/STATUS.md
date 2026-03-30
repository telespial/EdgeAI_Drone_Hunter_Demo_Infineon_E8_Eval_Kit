# STATUS

- Date: 2026-03-30
- Branch: `main`
- Runtime baseline: full clean rebuild and full 3-image flash validated on hardware.

## Phase Status
- Complete: `1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14`
- In progress: `15`
- Planned next: `16`

## Current Priority Queue (2026-03-30)
1. Replace speaker-test (`pong`) audio behavior with gameplay event soundscape:
   - explosions, city ambience, drone sounds, sirens, firetruck, ambulance,
   - event-timed layering and escalation rules.
2. Improve attacker + defender `ALGO` strategy depth and fun factor.
3. Add settings/help documentation files and wire them into workflow.
4. Schedule flame redraw phase (detailed redraw scope pending kickoff).
5. Investigate intermittent gameplay freeze and deliver hardened no-freeze baseline.

## Latest Validated Flash Results
- `wrote 32768 bytes`, `verified 30456 bytes`
- `wrote 12288 bytes`, `verified 8732 bytes`
- `wrote 3923968 bytes`, `verified 3920440 bytes`
- `** Resetting Target **`

## Current Runtime Baseline
- Opening attacker sequences now use strategic stochastic composition (not fixed replay):
  - splash start reseeds and resets the round,
  - doctrine biases are preserved while release order varies run-to-run.
- Top-center `DBG:*` stage banner is foregrounded for freeze localization.
- Settings popup controls are integrated and active:
  - attacker mode, defender mode, NPU toggle, difficulty, speed++.
- City + drone sound scheduling is active:
  - city ambience, fire loop, and emergency siren cadence remain event-driven,
  - drone flyby audio now triggers from active attacker composition (fixed-wing and FPV cadences).
- Actual audio output path is now active (sample playback):
  - short PCM clips are embedded from `sounds/` assets and streamed over TDM/DAC,
  - runtime now emits real sample audio per event instead of synthesized test tones.
- Audio mix and event routing pass applied:
  - all sounds now use fade in/out envelopes,
  - explosions were extended to ~2x duration with longer fade-out tails,
  - two explosion source recordings are actively mapped across explosion events,
  - ambulance clip was extended significantly,
  - successful attacks now schedule delayed ambulance response (8-15 seconds),
  - city traffic + random gunfire beds play in 3-6 second randomized segments,
  - drone + city ambience remain persistent low background layers,
  - CIWS firing now uses dedicated gun event routing with burst-style playback,
  - CIWS fire clip offset is forced to start beyond 3 seconds into source audio,
  - master output level is increased to latest requested loudness target.
- City background loop policy update:
  - city ambient clip is now max-fit for linker-safe runtime footprint,
  - city bed runs continuously in-game,
  - random city segment timing window is `40-55` seconds,
  - each segment boundary forces a crossfade return to clip start.
  - city/ambulance clip-size rebalance applied to remove audible micro-loop wobble while preserving fit.
- Top HUD text rows and `SET | HELP` button are removed.
- Explosion mapping is enforced by visual drone class:
  - Shahed: large orange, fixed-wing: red, X-wing DJI: small white.
- Fire palette now blends hot + vivid + smoke families with stronger per-fire variation.
- Bright fire visibility pass added evenly-selected bright profiles:
  - bright orange, bright red, bright red/orange.
- Skyfall steering set to plane model to remove hybrid wiggle behavior.
- Stinger steering set to plane model to remove wiggle behavior.
- Hunter labels normalized to: `Stinger`, `Pelican`, `TYTAN`.
- Additional freeze-hardening deployed:
  - city-fire render budget reduced,
  - flame sprite/style updates throttled.
- Render stability safe mode is enabled for city-fire drawing path.
- Additional runtime hardening for freeze risk:
  - per-tick hunter target-index sanitization before hunter updates.
- Latest engagement/targeting behavior updates:
  - hunters now persist in terminal intercept and retry close-range attacks before disengage,
  - long-range commit gate loosened so distant uncovered threats are engaged earlier,
  - lock boxes now use arena-local coordinates and correctly surround x-wing/small attack drones.
- Fire profile mapping is now centralized with weighted selection:
  - 75% bright bucket, 25% dark bucket.
- Hunter icon anti-flicker stabilization pass:
  - unloaded hunter sprites no longer hide/show thrash per frame,
  - deck hunter icons use stable opacity.
  - additional deck icon hardening:
    - icon hidden flags are force-cleared each UI refresh to prevent random disappear/reappear.
- Bottom deck liquid-glass bar opacity increased by ~20% for better readability.
- CIWS structures remain fixed while bullet streams target dynamically.
- CIWS audio reliability hardening:
  - CIWS fire sound is now emitted on every real burst trigger,
  - shared event cooldown suppression removed so left/right guns can both produce fire audio.
- Exhaustion-mode battle ending is active.
- Restore script path remains stable and validated.

## Memory Snapshot
- CM55 verified image size (external SMIF): `3,919,672 bytes`
- External SMIF capacity: `134,217,728 bytes`

## Restore Policy
- Golden: latest validated milestone baseline.
- Failsafe: latest validated direct-recovery baseline.
