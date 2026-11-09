# Game design v2

> Sources: Kitty art bible v2, 2026-10-31; GDD v1, 2026-10-28; Review of GDD v1, 2026-10-28; GDD v2, 2026-10-28
> Raw: [Art bible v2](../../raw/game-design/2026-10-31-art-bible-v2.md); [GDD v1](../../raw/game-design/2026-10-28-gdd-v1.md); [GDD v1 review](../../raw/game-design/2026-10-28-gdd-v1-review.md); [GDD v2](../../raw/game-design/2026-10-28-gdd-v2.md)
> Updated: 2026-10-31

## Overview

GDD v2 is the authoritative design. It came out of a review of v1, and it supersedes v1, which remains as context only. Kitty, the init process, descends through five dormant CPU Cores. Each Core teaches one real concurrency idea as a puzzle verb, and the story ends in a warm reboot. The engine design treats v2 as binding: a fix that would contradict v2 becomes an owner question.

## 1.0 scope

| In 1.0 | Post-launch | Cut |
|---|---|---|
| 5 chapters, 8 verbs, ~60 rooms | 1.1 Contrib data packs | Network visits / shared clones |
| Hub (Workbench) with arcade cabinets; 3 mini-game disks | 1.2 Arcade expansion; 1.3 *Workbench Life*; 1.4 BOOPSI subclassing chapter | Online voting, leaderboards |
| Shell fast-travel, ENVARC settings, visible saves | FR/IT/SV catalogs | Falling-tetromino game, Pounce, Defrag |
| Replay/demo mode, Nine Lives rewind; EN + DE catalogs | Second-Kitty questline | ReBoot homage layer |

The rule: "nothing post-launch may be required by anything in 1.0."

## Verbs (v2 §5.1)

| Verb | Real concept | Unlocked |
|---|---|---|
| Move / Jump | — | Ch1 |
| FORBID | `Forbid()`/`Permit()`; hold fire on a keyboard node, starvation meter | Ch1 |
| NINE LIVES | death rewinds up to 9 s along the snapshot ring | Ch1 |
| REQUESTERS | doors are dialogs; `[Retry]` resets, `[Cancel]` removes the obstacle and closes something behind you | Ch1→ |
| FORK | a Fork Pad records a thread of up to 10 s; a clone replays it from the fork moment | Ch2 |
| WINDOWS | Intuition depth: Back = ghost, Front = solid | Ch2→ |
| JOIN | Join Gates open only when every live thread has arrived | Ch3 |
| MUTEX | Lock Pedestals hold one token | Ch4 |
| SEMAPHORE | Semaphore Gates display N | Ch5 |

The verb interaction rules (v2 §5.3) are normative, and each one gets a test. Rewind restores the whole world, clones included. Clones replay inputs, not positions. Retry kills every thread.

## Budgets (provisional until M0)

- 3,000 triangles per frame; 6 on-screen Kitties; ≤ 6 shaders; overdraw ≤ 2×.

> **Status: Outdated** (2026-10-31)
> Art bible v2 §0 records an owner decision that graphics quality improves chapter by chapter (look levels L1–L4). [GDD v3](gdd-v3.md) now carries it, overriding v2 §4's single 320×256 look and the ≤ 6 shader cap. The 3,000-triangle budget now applies to L1/L2 only; proposed totals are 6,280 (L3) and 17,000 (L4). See [art bible](art-bible.md).

- Frame time ≤ 40 ms on the baseline (lavapipe) and ≤ 20 ms on the enhanced tier (NVK).
- Resident memory ≤ 512 MB.
- The fallback order is: cut texture sampling, then drop to 320×200, then cap clones at 4, then do AROS-side work. Never fall back to a free camera.

## Milestones (v2 §11)

M0 feasibility spike on lavapipe → M1 deterministic core + greybox puzzles (Linux) → M2 cross-platform determinism → M3 Chapter 1 vertical slice (human gate) → M4 chapters 2–3 → M5 chapters 4–5 + finale → M6 polish and performance → M7 release.

## Owner decisions D1–D7

| # | Decision |
|---|---|
| D1 | Ship the Linux build publicly as a secondary platform |
| D2 | Fred Fish is not shown on-screen |
| D3 | Engine licence GPL-3.0-or-later (disputed by AGENTS.md; see [overview](../project/kitty-project-overview.md)) |
| D4 | Reference AROS hardware: QEMU/KVM x86-64, 2 vCPU, for the baseline; the RTX 5070 Ti card lane for the enhanced tier |
| D5 | Art and music owners must be named before M3 starts |
| D6 | Kitty's in-world pronoun is she |
| D7 | NVK ships at 1.0 only if its nouveau/PCI fixes are merged upstream and reach a distribution |

## See Also

- [Game design v3](gdd-v3.md)

- [Engine architecture](../engine/engine-architecture.md)
- [Room format](../engine/room-format.md)
