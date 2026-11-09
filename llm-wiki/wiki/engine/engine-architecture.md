# Engine architecture

> Sources: Engine design rev 5, 2026-10-30; opencode prompt rev 5, 2026-10-29; GDD v2, 2026-10-28
> Raw: [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [Rev 5 prompt](../../raw/dispatch/2026-10-29-engine-design-rev5-prompt.md); [GDD v2](../../raw/game-design/2026-10-28-gdd-v2.md)
> Updated: 2026-10-30

## Overview

The engine is a small custom C++20 subset with `-fno-exceptions -fno-rtti` and no STL containers. It is single-threaded end to end. All OS access goes through one `platform/` interface with two implementations, `platform/aros` and `platform/linux`. The simulation is a deterministic 50 Hz Q16.16 core, and rendering is Vulkan through the AROS loader chain. Revision 5 fixes the module line budgets, under a binding 23k engine cap.

## Line budget and the cap

v2 §8.1 targets at most 20k lines. On 2026-10-29 the owner raised the engine cap to 23k, so that `shell/`, `replay/` and `benchmark/` stay in 1.0. "The 23k engine cap is binding."

| Module | LB (k) | Module | LB (k) |
|---|---|---|---|
| `platform/aros` | 1.5 | `locale/` | 0.8 |
| `platform/linux` | 1.2 | `save/` | 1.0 |
| `sim/` | 3.5 | `game/` | 1.0 |
| `snapshot/` | 1.2 | `settings/` | 0.6 |
| `render/` | 2.5 | `shell/` | 0.8 |
| `audio/` | 2.0 | `guru/` | 0.6 |
| `input/` | 1.2 | `replay/` | 0.6 |
| `assets/` | 1.0 | `benchmark/` | 0.8 |
| `level/` | 1.2 | `tools/` (host, excluded) | 2.0 |
| `ui/` | 1.0 | **Engine total (excl. `tools/`)** | **22.5** |

`audio/` is 2.0k: `modplay` at 1.5k, plus 0.5k for the AHI/SDL stream and mixer (C16).

> **Status: Outdated** (2026-10-29)
> Revision 4 kept "The 20k engine cap is binding." and proposed cutting `shell/`+`benchmark/`+`replay/` (2.2 k) to reach ~19.5 k, with `audio/` at 1.2k. The rev-4 critique showed that this cut removed v2 1.0 features, and that 1.2k was smaller than the ~1.5k replayer alone. The owner then raised the cap.

## Loops and ordering

- **Frame loop** (§3): input → sim tick(s) → snapshot → render → audio/ui. "This list is the frame loop; the per-tick order is §5.1's, starting with Step 0."
- **Tick order** (§5.1): Step 0 applies the `Event`s queued for this tick, sorted by (kind, a). The steps that follow are player verb resolution, the FORBID task-switch fiction, clone input replay, entities in fixed ID order, locks/gates/window depth, then win/lose.
- **Catch-up:** at most N ticks per frame (default 4), after which wall-time debt is dropped and logged. There is no interpolation; at 25 fps the sim steps 2 ticks per frame (C30).
- **Threading:** the only threads are lavapipe's rasterizer workers inside the ICD. The engine pins `LP_NUM_THREADS=0`, subject to an M0 probe. See [AROS platform facts](../platform/aros-platform-facts.md).

## Allocation and C++ subset (C29)

- Fixed arrays plus per-room arenas.
- Global `operator new`/`delete` are defined only as traps that call `guru_report` and exit; a link-time check fails the build if any engine object references them.
- `kitty-lint` enforces the subset: banned headers, no floats or libm in `sim/`, no namespace-scope objects with non-trivial constructors.

## Interfaces

§3 declares every module interface once, in C++ that must compile with `g++ -std=c++20 -fsyntax-only -Wall -Werror -Wredundant-decls`. In rev 5 that is 51 functions. Every module spec in §4 has exactly one each of the R/I/D/T/E/A/Test fields.

## See Also

- [Determinism and snapshots](determinism-and-snapshots.md)
- [Renderer](renderer.md)
- [Audio](audio.md)
- [Room format](room-format.md)
- [Saves and settings](saves-and-settings.md)
