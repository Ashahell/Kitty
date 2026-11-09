# Verification and CI plan

> Sources: Engine design rev 5, 2026-10-30; GDD v2, 2026-10-28
> Raw: [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [GDD v2](../../raw/game-design/2026-10-28-gdd-v2.md)
> Updated: 2026-10-30

## Overview

Every v2 §8.4 item maps to a harness H1–H8, each with a pass criterion and a known-bad input it must FAIL on: "a check that cannot fail is not a check" (C23). BLOCKED is a distinct state and is never counted as PASS. Every run prints a census of the gates that ran and each gate's return code.

## Harnesses

| # | What | Known-bad |
|---|---|---|
| H1 | Linux build + unit tests (with a UBSan job) | planted off-by-one in the `qdiv` negative test |
| H2 | State-hash replay suite (`kitty-hash`) | a log with one flipped bit |
| H3 | Puzzle solver on all rooms | walled-off goal; a room solvable with fewer verbs than PAR declares |
| H4 | Render CI on host lavapipe, golden compare | planted shader error; golden from a different pin |
| H5 | Validation layers over a 60 s demo | a named VUID, e.g. `VUID-vkCmdCopyImage-srcImageLayout-01917` |
| H6 | AROS cross-build + ABI gates | building against the stale SDK |
| H7 | On-AROS smoke run: hash == Linux, frame == golden, 30-min stability | forced `WaitTOF` path |
| H8 | NVK card-lane run | BLOCKED until the lane reaches present |

## Milestone gates

M0 runs H5, H6 and H7 (no sim yet). M1 runs H1, H2 (Linux side) and H3. M2 runs H7 hash equality on AROS. M3 adds H4 goldens and the first H8 attempt.

## See Also

- [Renderer](../engine/renderer.md)
- [Determinism and snapshots](../engine/determinism-and-snapshots.md)
