# Kitty: Core Awakening — project overview

> Sources: Kitty project AGENTS.md, 2026-10-30; GDD v2, 2026-10-28; Engine design rev 5, 2026-10-30; Engine design rev 5 critique, 2026-10-30
> Raw: [AGENTS.md](../../raw/project/2026-10-30-agents-md.md); [GDD v2](../../raw/game-design/2026-10-28-gdd-v2.md); [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [Rev 5 critique](../../raw/reviews/2026-10-30-engine-design-rev5-critique.md); [Rev 5 hand patches](../../raw/reviews/2026-10-30-rev5-hand-patches.md)
> Updated: 2026-10-30

## Overview

Kitty: Core Awakening is an AROS-only 3D isometric puzzle-platformer, to be built LLM-first on a small custom C++20 engine. As of 2026-10-30 the project is design-only. The local folder `~/Work/Kitty` is not a git repo. The upstream repo (https://github.com/Ashahell/Kitty) holds only `README.md` and an MIT `LICENSE`. The authoritative game design is GDD v2. The engine design document reached revision 5, which was accepted, and its remaining fixes were patched by hand on 2026-10-30. No engine code exists yet.

## Platform and toolchain stance

- AROS x86-64 is the only target. Development and CI run on Linux Vulkan first; AROS cross-compiling starts only after a headless Vulkan smoke test passes (AGENTS.md).
- Rendering goes through the AROS Vulkan loader chain on two tiers: Mesa **lavapipe** as the baseline and **NVK** as the enhanced tier. The Vulkan4Aros interpreter ICD is not a target. See [renderer](../engine/renderer.md).
- The engine is its own code, not a port of Godot/Bevy/Fyrox/O3DE.

## Where things live

| Thing | Location |
|---|---|
| GDD v1, review, v2 | `Docs/Development/kitty_core_awakening_design*.md`, `kitty_core_awakening_review.md` |
| Engine design (rev 5) + sources list | `Docs/Development/kitty_engine_design.md`, `kitty_engine_design_sources.md` |
| Critiques rev 1–5 | `Docs/Development/kitty_engine_design*_critique.md` |
| opencode dispatch prompts rev 1–5 | `Docs/Development/dispatch/` |
| Platform evidence | `~/Work/projects/Vulkan4Aros` (read-only for this project) |

## Working rules

- One milestone per session (M0 triangle-on-Vulkan → M7 contribution pipeline), each with automated acceptance tests. Commit after every milestone.
- Fork/Join (clone replay) is the riskiest subsystem and is built at M2 with determinism guarantees (AGENTS.md). The engine design puts the sim and snapshot core at M1 and cross-platform hash equality at M2. See [determinism](../engine/determinism-and-snapshots.md).
- Licensing is an architectural constraint: Kitty and AROS are trademarks, so all official art and names are swappable "skins" from day one.
- Never make write or destructive git calls in the Vulkan4Aros `src/abi/v1/AROS*` tree.

## Licence and stack conflicts between AGENTS.md and v2

> **Status: Disputed**
> AGENTS.md says the top-level MIT licence covers the engine layer. GDD v2 decision D3 says "Engine license | GPL-3.0-or-later (aligns with contributed-content options)". Nobody has reconciled the two.

> **Status: Outdated** (2026-10-28)
> AGENTS.md lists the deps "per GDD §8" as including volk/Vulkan-Hpp, miniaudio and libopenmpt, with a "~10–20k lines" engine. GDD v2 and the engine design reject volk and Vulkan-Hpp, and use an own MOD replayer instead of miniaudio/libopenmpt. On 2026-10-29 the owner raised the engine cap to 23k. See [engine architecture](../engine/engine-architecture.md).

## See Also

- [Game design v2](../game-design/gdd-v2.md)
- [Engine design revision history](../process/engine-design-revision-history.md)
- [Open questions](../process/open-questions.md)
