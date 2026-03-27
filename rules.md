# EdgeAI Drone Hunter - Rules Specification

Date: 2026-03-27
Status: Canonical gameplay rules (design + implementation guidance)

## 1. Game concept
This is a two-player asymmetrical strategy-defense game:
- Attacker: launches swarms of attack drones toward city assets.
- Hunter (Defender): deploys Ukrainian interceptor drones and terminal CIWS to neutralize threats.

Core principle:
- Every interceptor is one-time-use. One hunter can only kill or miss one attack drone.
- A missed interceptor becomes unusable and falls harmlessly.
- A leaked attack drone reaches a city target and explodes, scoring for the Attacker.

## 2. Teams and unit classes

### 2.1 Defender units
Hunters:
1. Wild Hornets Sting-II
2. ODIN Win_Hit
3. Skyfall P1-SUN
4. Tenebris Bagnet
5. Octopus-100
6. VB140 Flamingo
7. Merops Interceptor
8. TYTAN

Gun:
- Phalanx CIWS

### 2.2 Attacker units
1. Shahed-class fixed-wing strike drone
2. Strike-Prop (Predator/VB140-like prop strike class)
3. Strike-X (DJI/camera-style X-wing quad class)

## 3. 3D map model and zones
Map model:
- Play area is a 3D defense box with X/Y ground plane and Z altitude.
- Suggested conceptual size: 120 km x 120 km x 0-6 km altitude.
- Defended city/core is centered.

Engagement rings:
1. Far Zone: 30-60 km
2. Intercept Zone: 8-30 km
3. Critical Zone: 2-8 km
4. Last-Ditch Zone: 0-2 km

Gameplay intent:
- Long-range interceptors should be used in far/intercept zones.
- Fast terminal hunters cover critical zone.
- Phalanx is primarily a last-ditch weapon.

## 4. Attack drone classes

### 4.1 Shahed class
- Speed: 150-190 km/h
- Altitude: 0.1-4 km
- Payload: high
- Maneuverability: low
- Threat score: very high
- Typical behavior: planned route, group/saturation attacks

### 4.2 Strike-Prop class
- Speed: 100-150 km/h
- Altitude: 0.5-5 km
- Payload: medium
- Maneuverability: medium
- Threat score: high
- Typical behavior: loiter, redirect, lane avoidance

### 4.3 Strike-X class
- Speed: 60-120 km/h
- Altitude: 0-0.5 km
- Payload: low-medium
- Maneuverability: very high
- Threat score: medium individually, high in swarms
- Typical behavior: erratic turns, low-altitude masking

## 5. Hunter drone performance model
These are game-balanced performance values aligned to open-source envelopes and role intent.

### 5.1 Speed ranking (fastest to slowest)
1. Skyfall P1-SUN (~400-450)
2. Wild Hornets Sting-II (~315-343)
3. ODIN Win_Hit (~280-300)
4. Octopus-100 (~300+ estimated operational envelope)
5. Merops (~280)
6. Tenebris Bagnet (~250)
7. TYTAN (~200-300 class)
8. VB140 Flamingo (~100-120)

### 5.2 Role tiers
High-speed interceptors:
- P1-SUN, Sting-II, Win_Hit, Octopus
- Best for fast terminal intercept and high closure speed

Endurance/patrol interceptors:
- Flamingo, Merops, TYTAN
- Best for distance, lane coverage, and sustained patrol

### 5.3 Per-hunter role guidance
- Sting-II: rapid intercept, bonus vs larger slower fixed-wing
- ODIN: short, aggressive intercept profile, high climb/direct approach
- Skyfall: highest dash speed, strong predictive intercept
- Bagnet: autonomous terminal guidance, medium-speed reliability
- Octopus-100: stable all-rounder for mixed threats
- VB140: long-range patrol/intercept, weak terminal chase
- Merops: aerodynamic medium-range fixed-wing interceptor
- TYTAN: balanced fixed-wing medium-speed interceptor

## 6. Phalanx CIWS rules

### 6.1 Core stats
- Caliber model: 20 mm CIWS class behavior
- Rate of fire model: very high (streaming tracers)
- Ammo: finite per gun (left and right tracked independently)
- Effective range: ~2 km
- Hard cutoff: ~3.5 km

### 6.2 Weapon doctrine
- Last-ditch terminal defense only.
- Strong inside effective range.
- Outside effective range but inside hard cutoff: ammo waste with near-zero success.
- Outside hard cutoff: no valid engagement.

### 6.3 Misuse penalties
Firing with poor conditions (too far, wrong arc, bad lead, masked LOS) should:
- deplete ammo,
- increase heat,
- increase reaction/cooldown delay,
- reduce future engagement efficiency.

## 7. Detection, classification, and commit pipeline
Each engagement should conceptually pass:
1. Detect
2. Classify
3. Commit

Detection quality factors:
- target size, altitude, speed,
- weather/clutter,
- distance

Classification quality factors:
- sensor type,
- line of sight,
- track history,
- electronic noise/spoofing

Failure outcomes:
- delayed launch,
- wrong class assignment,
- confidence collapse and poor targeting.

## 8. Friendly-fire policy
Default behavior:
- No friendly fire allowed.
- UI should lock out friendly target confirmation.

Exceptional failure window (optional advanced mode):
Friendly fire can occur only when several failure conditions overlap:
- IFF degraded/offline,
- merged/crossed tracks,
- manual override,
- low classification confidence,
- spoofing/debris confusion.

Consequences:
- blue-on-blue intercept,
- collateral damage,
- chain reaction near critical assets.

## 9. Intercept decision logic
For every potential launch, compute:
- time-to-intercept,
- closure speed,
- turn demand,
- altitude penalty,
- track confidence,
- remaining endurance/ammo,
- corridor deconfliction.

Good engagement requires:
- arrival before hostile reaches defended ring,
- positive closure,
- hunter-target compatibility,
- no corridor conflict,
- acceptable weather/sensor penalty.

## 10. Wrong-choice consequences
A. Phalanx misuse:
- burns finite ammo, raises heat, leaves terminal zone exposed.

B. Wrong hunter assignment:
- miss probability rises, launch slot wasted, other lanes leak.

C. Overkill trap:
- too many hunters on low-value target opens high-value lanes.

D. Altitude mismatch:
- low-agility unit against erratic low-alt targets causes misses/crashes.

E. Manual override under uncertainty:
- raises friendly-fire and collateral risk.

## 11. Threat scoring and prioritization
Use dynamic threat score:

ThreatScore = Payload x Proximity x Survivability x Confidence x TargetValueModifier

Suggested priority order:
1. Shahed in critical/last-ditch zone
2. Strike-Prop aligned with high-value infrastructure
3. Strike-X swarm near defended core
4. Distant low-payload targets

## 12. Recommended pairings

Best pairings:
- Shahed -> Sting-II, ODIN, P1-SUN, Bagnet, Octopus
- Strike-Prop -> Octopus, Merops, TYTAN, Flamingo
- Strike-X -> Sting-II, Octopus, Phalanx (only if very close)

Poor pairings:
- Flamingo vs terminal Shahed
- Bagnet vs near-ground erratic Strike-X swarm
- Phalanx vs far-zone targets
- TYTAN vs tight-turn low-altitude Strike-X

## 13. Win/loss conditions
Defender win:
- key asset survives,
- at least one defense layer remains,
- collateral below threshold.

Defender loss:
- key asset destroyed,
- Phalanx exhausted early and terminal wave leaks,
- friendly fire destroys critical defense,
- repeated low-priority engagements open decisive lanes.

## 14. UI requirements
For each hostile track, show:
- type,
- speed,
- altitude,
- range to base,
- ETA,
- threat score,
- recommended counter.

For each defender, show:
- remaining stock/ammo/endurance,
- intercept envelope,
- lock confidence,
- friendly-fire lockout status,
- cooldown/reload/availability.

## 15. Strategic launch model
Attack spawns must support strategic lane pressure:
- even distribution from top/bottom/left/right,
- four quadrants/lane choices per edge (16 launch site options total),
- target destinations distributed across the playable city grid (not pinned to one edge).

## 16. Visual and effects rules
- Intercept success: hunter and attack drone disappear immediately.
- Missed hunter: falls harmlessly and is consumed.
- Leaked attacker: impacts city target and explodes.
- Persistent city-fire markers remain burning after leaks.
- More fires appear as Attacker lead/loss pressure increases.

## 17. One governing strategic rule
Every launch decision must answer:

"Will this defender arrive in time, within its effective envelope, against the right target class, without opening a more dangerous gap elsewhere?"

If yes: launch.
If no: hold or choose a different defender.

## 18. Notes for ongoing implementation
- Keep behavior deterministic enough for explainable outcomes.
- Keep losses attributable to player choices (range, class mismatch, allocation, timing).
- Preserve finite-resource pressure (hunter stock + per-gun CIWS ammo).
- Maintain clear HUD feedback so players learn why engagements succeed or fail.
