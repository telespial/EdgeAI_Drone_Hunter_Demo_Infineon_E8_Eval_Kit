# EdgeAI Drone Hunter - Detailed Implementation Roadmap

Date: 2026-03-27
Canonical rules source: `rules.md`
Primary code target: `firmware_kit_epc2/proj_cm55/app/drone_hunter/drone_hunter_arena.c`

## 0. Current Baseline (already in code)
- Splash + lineup + `START ARENA` flow is active.
- Hunter vs Attacker scoring is active.
- Attack goals are distributed across playable city area (not bottom-clustered).
- Leak impacts create persistent, flame-like fire patches.
- CIWS is finite-ammo and constrained to near-front envelope (cannot reliably hit far/top grid).
- Initial rules-engine pass added:
  - detect/classify/commit confidence signals,
  - dynamic threat score seed,
  - commit gate on low-confidence engagements,
  - recommended counter surfaced in HUD.
- Code-quality cleanup pass completed:
  - removed dead/unused helper functions from arena runtime file,
  - hardened HUD info line formatting to avoid truncation-prone composition,
  - restored code to normal baseline behavior after isolation experiments.
- Runtime stability hotfix completed:
  - fixed oversized launch sprite corruption,
  - fixed arena flashing spikes,
  - fixed gameplay freeze occurring around ~12s.

## 1. Architecture Goal
Implement `rules.md` end-to-end as an explainable strategy game loop where every launch decision is based on:
- time-to-intercept,
- effective envelope,
- target class match,
- confidence,
- opportunity cost (what gap is opened elsewhere).

---

## Phase 1 - Data Model Completion
Status: Complete

### Objectives
- Expand runtime target model so all rule dimensions are represented explicitly.

### Tasks
- Add per-target fields for:
  - `type`, `speed`, `altitude_band`, `range_to_core`, `eta`,
  - `payload`, `survivability`, `threat_score`,
  - `detect_conf`, `class_conf`, `commit_conf`,
  - `recommended_counter`, `priority_rank`.
- Add defender state fields for:
  - launch readiness, cooldown, envelope fit, confidence fit,
  - CIWS per-gun ammo + heat + lock-quality.
- Normalize unit constants into one section (hunter/attacker/CIWS).

### Exit criteria
- No “implicit-only” threat logic; all key rule dimensions are tracked as explicit variables.

---

## Phase 2 - Detection / Classification / Commit Pipeline
Status: Complete

### Objectives
- Make engagement decisions pass through explicit 3-stage confidence pipeline.

### Tasks
- Detection stage:
  - Include clutter, altitude, speed, range weighting.
- Classification stage:
  - Distinguish Shahed vs Strike-Prop vs Strike-X confidence.
- Commit stage:
  - Allow/deny launch based on confidence + urgency + corridor + LOS checks.
- Add optional degraded-confidence event hooks (future IFF/friendly-fire mode).
- Add commit hold reason accounting:
  - detect hold, class hold, confidence hold, corridor hold, LOS hold.

### Exit criteria
- Every autonomous launch has traceable detect/classify/commit reason codes.

---

## Phase 3 - Threat Score + Prioritization Engine
Status: Complete

### Objectives
- Implement full weighted threat scoring from `rules.md`.

### Tasks
- Compute per-target dynamic threat:
  - `Payload x Proximity x Survivability x Confidence x TargetValueModifier`.
- Add priority queue over all active hostile tracks.
- Add lane pressure term so swarms are not treated as isolated targets.
- Add target value modifiers for strategic assets/critical zones.

### Exit criteria
- Target choice is deterministic and explainable from visible metrics.

---

## Phase 4 - Hunter Assignment + Matchup Matrix
Status: Complete

### Objectives
- Make defender selection follow matchup doctrine and envelope constraints.

### Tasks
- Encode best/poor pairings from `rules.md`.
- Penalize bad matches:
  - Flamingo vs terminal Shahed,
  - Phalanx vs far zone,
  - TYTAN vs low-alt erratic Strike-X, etc.
- Add overkill prevention:
  - avoid spending multiple high-value hunters on low-value threats.
- Add no-stock and no-fit fallback logic with explicit reason reporting.

### Exit criteria
- Launch selections visibly align with matchup doctrine.

---

## Phase 5 - Attacker Strategy Layer (16 Launch Sites)
Status: Complete

### Objectives
- Turn attack spawning into explicit strategic pressure.

### Tasks
- Keep 4 edges x 4 lanes (16-site model).
- Add attacker strategy profiles:
  - center pressure,
  - flank pressure,
  - mixed lure-and-strike,
  - terminal saturation.
- Add player-selectable attacker lane profile (manual mode).

### Exit criteria
- Attacker behavior is visibly strategic and selectable.

---

## Phase 6 - CIWS Doctrine Finalization
Status: Complete

### Objectives
- Fully model CIWS as last-ditch finite terminal layer.

### Tasks
- Keep effective + hard cutoff range.
- Strengthen misuse consequences:
  - ammo waste,
  - heat growth,
  - temporary reaction-delay penalties,
  - degraded hit probability under poor geometry.
- Add on-screen CIWS state:
  - ammo L/R,
  - heat L/R,
  - lock quality.
- Include constrained CIWS fratricide strategy rule in advanced/failure windows:
  - hunter crossing + degraded IFF + poor lock/track merge can produce accidental hunter kill.

### Exit criteria
- Player can clearly see why CIWS succeeds/fails and when to avoid firing.

---

## Phase 7 - HUD/UX Completion (Rules Section 13)
Status: Complete

### Objectives
- Make decision support complete and readable.

### Tasks
- Per hostile track display:
  - type, speed, altitude, range, ETA, threat score, recommended counter.
- Defender panel display:
  - remaining stock/ammo/endurance,
  - intercept envelope fit,
  - lock confidence,
  - cooldown/reload availability,
  - lockout/friendly-fire status (for advanced mode).
- Keep text stable and non-overlapping across rows.

### Exit criteria
- User can make informed choices without hidden state.

---

## Phase 8 - Wrong-Choice Consequence System
Status: Complete

### Objectives
- Make strategic mistakes produce predictable losses (not random chaos).

### Tasks
- Implement explicit penalties for:
  - range mismatch,
  - altitude mismatch,
  - overkill allocation,
  - CIWS misuse,
  - manual override under low confidence.
- Add explainability cues in HUD/log text for each failure type.

### Exit criteria
- Every major loss has a clear “why this failed” indicator.

---

## Phase 9 - Friendly-Fire / IFF Advanced Mode (Optional Toggle)
Status: Complete

### Objectives
- Add constrained blue-on-blue risk only under combined failure conditions.

### Tasks
- Add advanced mode toggle.
- Enable friendly-fire only if multi-condition gate is met:
  - IFF degraded + merged tracks + manual override + low confidence.
- Extend friendly-fire model to include CIWS-on-hunter accidental kills:
  - only when hunter crosses active CIWS fire lane under degraded/merged low-lock conditions.
  - penalty model:
    - hunter supply is decremented (wasted friendly interceptor),
    - attacking drone may or may not be destroyed in the same burst event.
- Add collateral outcomes and recovery messaging.

### Exit criteria
- Friendly-fire exists only as intentional high-risk advanced behavior.

---

## Phase 10 - Mission/Wave Structure and Difficulty
Status: Complete

### Objectives
- Build coherent campaign pacing from hundreds to thousands of threats.

### Tasks
- Define wave archetypes:
  - Shahed-heavy,
  - Strike-X swarm,
  - mixed deception waves,
  - terminal saturation.
- Scale by composition, not just count.
- Add mission milestones and mid-wave strategic shifts.

### Exit criteria
- Difficulty escalates through meaningful tactical variety.

---

## Phase 11 - Win/Loss and Collateral Rules
Status: Complete

### Objectives
- Align completion conditions with `rules.md`.

### Tasks
- Defender win:
  - key asset survives,
  - at least one defense layer remains,
  - collateral below threshold.
- Defender loss:
  - key asset destroyed,
  - CIWS exhausted too early and terminal leaks occur,
  - critical node destroyed.
- Add round-end summary panel with causal metrics.

### Exit criteria
- End conditions are strategic and interpretable.

---

## Phase 12 - Verification + Calibration + Restore Governance
Status: Complete

### Objectives
- Keep iteration safe and reproducible.

### Tasks
- Maintain on-device checklist after each flash:
  - target distribution,
  - CIWS envelope behavior,
  - fire persistence,
  - score correctness,
  - HUD integrity.
- Track balancing deltas in `docs/COMMAND_LOG.md`.
- Promote golden/failsafe only after verified board behavior.

### Exit criteria
- Stable, explainable, tagged milestones with reproducible build/flash commands.

---

## Phase 13 - Hunter Guidance + Intercept Geometry Hardening
Status: Complete

### Objectives
- Eliminate straight-line fly-past behavior by making hunter intercepts continuously guided and hit-safe at frame boundaries.

### Tasks
- Add continuous terminal homing while hunter is committed:
  - re-steer velocity each tick toward predicted intercept point (not one-time launch vector only),
  - cap turn rate per hunter class to preserve role differentiation.
- Add swept-hit detection to prevent tunneling:
  - evaluate line-segment movement against target kill radius each tick,
  - preserve existing probability gate and failure accounting.
- Harden target handoff/loss handling:
  - reacquire when valid replacement exists inside envelope,
  - otherwise force explicit miss state with clear WHY reason.
- Add telemetry for validation:
  - log overshoot count, swept-hit count, reacquire count.
- Rebalance per-class speed/kill-radius only after guidance is stable.

### Progress update (2026-03-28)
- Implemented in `drone_hunter_arena.c`:
  - continuous hunter re-steering while committed,
  - turn-rate limiting in steering helpers,
  - swept-hit segment-vs-radius collision gate,
  - reacquire path for lost targets before miss/fall,
  - telemetry counters surfaced in HUD (`SH`, `RQ`, `OS`),
  - explicit WHY messaging added for target-loss/no-reacquire and terminal-evade misses,
  - guidance calibration pass completed:
    - class-aware lead/speed multipliers tuned,
    - terminal overshoot damping added near intercept,
    - per-class kill-radius values rebalanced.
  - hardware validation pass completed on board with updated guidance baseline.

### Exit criteria
- Hunters no longer pass through viable targets due to one-frame overshoot.
- Intercepts are visually centered on actual target position with no off-track despawns.
- Hardware validation confirms reduced miss-rate without breaking Phase 8 consequence model.

---

## Phase 14 - 360 Movement Doctrine + Dynamic Intercept Decisions
Status: Planned

### Objectives
- Enable full-direction movement for both hunter and attack drones with explicit behavior doctrine:
  - attack drones evade hunters but only detonate at their preset launch target coordinates,
  - hunter drones preserve primary lock while allowing controlled opportunistic intercepts on easier/path-conflict threats.

### Tasks
- Add all-direction steering model for both sides:
  - replace mostly ballistic movement with bounded acceleration/turn-rate steering,
  - allow positive/negative X/Y maneuver updates each tick (not lane-only drift).
- Implement attacker objective doctrine:
  - each attacker stores immutable launch target coordinates,
  - attacker uses evasive steering against nearby hunters,
  - attacker explodes only if it reaches target radius around its own preset coordinates,
  - attacker destroyed by hunter/CIWS does not count as city-impact detonation.
- Implement hunter objective doctrine:
  - keep primary locked target as default objective,
  - add opportunistic intercept gate for "easy/path-conflict" targets:
    - short time-to-intercept advantage,
    - low turn demand,
    - high kill confidence,
    - low risk of abandoning primary threat.
- Add controller-specific behavior profiles:
  - `ALGO` (baseline engine for both attacker and defender): conservative retargeting, stronger lock persistence, deterministic evasion/intercept heuristics.
  - `EDGEAI` (embedded intelligence layer): improves ALGO behavior using trained/adaptive reasoning for predictive retargeting, evasive pathing, and context-aware switching.
- Add explainability telemetry:
  - lock retained vs opportunistic switch counts,
  - attacker evade maneuvers count,
  - target-reached detonations vs in-flight kills.
- Implement control-core + NPU architecture:
  - `ALGO` and `EDGEAI` run on the same control core (single decision brain per side),
  - `EDGEAI` may offload heavy inference kernels to U55/NPU for performance,
  - define strict per-tick handoff contract for feature input + action output.
- Add runtime arbitration and fallback policy:
  - CPU ALGO always produces a valid action each tick,
  - if EDGEAI result arrives in time and passes sanity constraints, it can override/adjust ALGO output,
  - on timeout/error/invalid output, system stays ALGO-only for that tick (no stall).
- Define integration milestones:
  - M14.1: intra-core handoff schema + telemetry counters,
  - M14.2: attacker evasion model consuming ALGO decisions,
  - M14.3: hunter opportunistic-switch gate + lock persistence policy,
  - M14.4: EDGEAI NPU-accelerated path enabled with safe ALGO fallback,
  - M14.5: hardware profiling (latency, miss-rate, stability) and tuning.

### Exit criteria
- Attack drones maneuver in all directions but only detonate at their assigned target coordinates.
- Hunters can maneuver in all directions and make explainable opportunistic kills without random lock thrashing.
- Behavior differences between `ALGO` and `EDGEAI` are visible and stable in HUD/WHY telemetry.
- ALGO and EDGEAI remain co-located on one control core with deterministic fallback to ALGO on any EDGEAI/NPU failure.

---

## Immediate Next Sprint (execution order)
1. Start Phase 14 movement doctrine + dynamic intercept decision layer.
2. Re-profile memory/latency after Phase 14 implementation and retune thresholds.
3. Add Phase 14 hardware validation checklist and closure criteria in docs.
