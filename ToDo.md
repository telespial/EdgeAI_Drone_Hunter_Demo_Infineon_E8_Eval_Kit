# EdgeAI Drone Hunter - Build Roadmap (ToDo)

## Baseline (Done)
- Keep startup identity splash: `EdgeAI Drone Hunter`.
- Boot display and peripherals on E8 (`W4P3INCH_DISP`).
- Launch animated POV arena scene after splash.
- Show ALGO vs EdgeAI pursuit overlays and basic miss/hit telemetry.
- Build + flash verified on hardware (`PSE846GPS2DBZC4A`).

## Phase 1 - Playable Core Loop
- Add game states: `Search`, `Lock`, `Intercept`, `Confirm`, `Wave End`, `Game Over`.
- Add score, lives, mission timer, and wave counter.
- Define objective: protect core asset from incoming killers.
- Add fail conditions and clear victory conditions.
- Add restart flow from Game Over/Victory.

## Phase 2 - Player-Controlled POV Hunter
- Make player fly one hunter (FPV controls).
- Add control model: pitch/yaw/throttle/boost/brake.
- Keep two AI wingmen as support drones.
- Add lock-on reticle and intercept cueing.
- Add camera options: strict FPV and assisted chase-FPV.

## Phase 3 - Target Behaviors
- Implement two target classes: `Killer FPV` and `Fixed-Wing`.
- Give each class distinct speed, turn-rate, and escape logic.
- Add threat intent (what target is attacking).
- Add route variants and evasive maneuvers.
- Add wave composition rules (mixed target types).

## Phase 4 - ALGO vs EdgeAI Demonstration
- Add mode selector: `ALGO`, `EdgeAI`, `Head-to-Head`.
- Run deterministic scenario seeds for fair A/B comparisons.
- Track metrics: time-to-intercept, miss distance, kill rate, energy cost.
- Add confidence visualization for prediction quality.
- Add summary screen with per-wave and aggregate stats.

## Phase 5 - Combat and Feedback Polish
- Add lock, near-miss, hit, and kill-confirm audio cues.
- Add visual effects: trail, hit flash, shock ring, debris hints.
- Improve HUD readability and threat prioritization.
- Add minimap/radar and wingman status panel.
- Add accessibility options for HUD scale and contrast.

## Phase 6 - Arena and Mission Depth
- Replace simple circle with sector-based map zones.
- Add no-fly zones and risk-reward shortcuts.
- Add defendable assets and dynamic objective updates.
- Add difficulty scaling by player performance.
- Add mission modifiers (weather/jamming/low visibility).

## Phase 7 - Benchmark and Demo Packaging
- Add replay capture with scenario seed + results.
- Add benchmark mode for public demos.
- Add exportable run summary for ALGO vs EdgeAI comparisons.
- Add operator quick-start guide and controls reference.
- Freeze golden and failsafe restore tags for demo release.

## Next Build Target (Immediate)
- Implement Phase 1 fully and partial Phase 2.
- Deliver a playable single-wave intercept mission with explicit win/lose flow.
- Preserve current splash and arena visual language.
