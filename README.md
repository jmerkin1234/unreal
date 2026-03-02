# billiardgame_clean

Developed with Unreal Engine 5 — realistic 8-ball pool on Linux using Chaos Physics.

**Engine:** UE 5.7.3 (Linux) | **Assets:** Blender 4.3.2 → FBX | **Physics:** Chaos with manual spin/rolling overrides

---

## Development Progress

| Phase | Description | Status |
|-------|-------------|--------|
| 1 | Blender Asset Preparation | Pending |
| 2 | UE5 Project Setup | Complete |
| 3 | Blueprint Architecture (C++) | ✅ Complete |
| 4 | Aiming Aid System (C++) | ✅ Complete |
| 5 | Camera System | Not started |
| 6 | UI / HUD | Not started |
| 7 | Audio | Not started |
| 8 | Lighting | Not started |

Full guide: `billiards_game_guide.md`

---

## Source Overview (`Source/billiardgame/`)

| File | Description |
|------|-------------|
| `BilliardsTypes.h` | Core enums: player groups, game phases, foul reasons |
| `BilliardBallActor` | Base ball — physics setup, spin correction tick, cushion hit override |
| `BilliardCueBallActor` | Extends BilliardBallActor — `FireShot(Direction, Power, HitOffset)` |
| `BilliardCueActor` | Cue stick — aim direction, power, hit offset, shot animation |
| `BilliardAimingAidActor` | Ghost ball preview, trajectory spline, cushion reflection arrow |
| `BilliardGameMode8Ball` | Full 8-ball rules — fouls, group assignment, turn flow, win/loss |
| `BilliardTableActor` | Table with auto-bound pocket overlap triggers |
| `BilliardPocketBinderComponent` | Component-based alternative for pocket detection |

---

## Physics Constants

| Parameter | Value |
|-----------|-------|
| Ball diameter | 5.25 UE units (52.5mm) |
| Ball mass | 0.17 kg |
| Ball restitution (COR) | 0.96 |
| Cushion restitution | 0.75 |
| Rolling friction | 0.2 |
| Sliding friction | 0.4 |
| Max cue impulse | 500 cm/s |
| Stop threshold | 1.0 cm/s linear / 1.0 rad/s angular |
| CCD | Enabled on all balls |

---

## Architecture Notes

- Ball #0 is the cue ball (`ABilliardCueBallActor`, auto-detected by `ABilliardGameMode8Ball`)
- GameMode polls for all-balls-settled every 0.2s, then calls `EvaluateTurnResult()`
- Blueprint hooks for UI events: `ShowCue`, `ShowFoul`, `RespawnCueBall`, `OnGameEnded`
- Spin correction runs per-tick: sliding friction force + rolling constraint clamp
- Pocket detection via `BoxComponent` overlaps on `ABilliardTableActor`

---

## Linux Notes

- Requires Vulkan + proprietary GPU driver (NVIDIA 535+ or AMD RADV)
- C++ compilation requires clang 16+: `clang --version`
- UE5 asset paths are case-sensitive
- Live Coding works but is unstable; delete `Binaries/` + `Intermediate/` if stuck in crash loop
