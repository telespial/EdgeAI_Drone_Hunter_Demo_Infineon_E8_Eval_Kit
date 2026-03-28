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
Status: Not started

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
Status: Partially started (site selection exists, no player strategy UI yet)

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
Status: In progress

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

### Exit criteria
- Player can clearly see why CIWS succeeds/fails and when to avoid firing.

---

## Phase 7 - HUD/UX Completion (Rules Section 13)
Status: Partially started

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
- Add collateral outcomes and recovery messaging.

### Exit criteria
- Friendly-fire exists only as intentional high-risk advanced behavior.

---

## Phase 10 - Mission/Wave Structure and Difficulty
Status: In progress

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
Status: Partially started

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
Status: Ongoing

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

## Immediate Next Sprint (execution order)
1. Complete Phase 4 matchup matrix with explicit penalties and overkill prevention.
2. Complete Phase 6 CIWS heat/lock telemetry in HUD.
3. Add Phase 8 explainability reasons in round-time text.
4. Reflash and validate on hardware before next tag promotion.
