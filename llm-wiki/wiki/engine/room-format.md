# Room format

> Sources: Engine design rev 5, 2026-10-30; opencode prompt rev 5, 2026-10-29; Engine design rev 3 critique, 2026-10-29; Engine design rev 4 critique, 2026-10-29
> Raw: [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [Rev 5 prompt](../../raw/dispatch/2026-10-29-engine-design-rev5-prompt.md); [Rev 3 critique](../../raw/reviews/2026-10-29-engine-design-rev3-critique.md); [Rev 4 critique](../../raw/reviews/2026-10-29-engine-design-rev4-critique.md); [Rev 5 hand patches](../../raw/reviews/2026-10-30-rev5-hand-patches.md)
> Updated: 2026-10-30

## Overview

Rooms are written as text and compiled on the host. `kitty-cc` validates a room and compiles it to a binary `RoomBin`; at runtime the engine loads the binary and checks a CRC-32C, and never parses text. `kitty-solve` checks that every room can be solved within its PAR, has no cheese solutions, and uses exactly its intended verb set (C22). Every error prints the Guru shape `Software Failure. <file>:<line>: <message>`.

## Grammar essentials

- The field order is fixed: `ROOM id w h era`, `SEED 0x…`, `PAR ticks verb-commits clones-max`, `GRID` … `ENDGRID`, `SPAWN`, then `ENTITY`/`GATE`/`PANE` lines, then `WIN exit pos=`.
- Tiles are terrain only (`#` and `.`). Entities, gates and panes are lines, never tiles. `;` is the only comment marker.
- `TICKS_PER_CELL = 8`, a player speed of 6.25 cells/s. PAR ticks must be at least the shortest path × 8. Verb-commits × the 25-tick hold must fit in the PAR ticks, and clones-max is ≤ 16.

## Example room rules (rev 5)

- With every `GATE` cell treated as closed, WIN must be unreachable from SPAWN, so the example needs at least one verb.
- The shipped example `ch1_03` uses the wall at x=8 with a single gap held by the requester `req1`, and `PAR 200 1 0`. The shortest path is 22 cells × 8 = 176 ticks. It has 0 clones because FORK arrives in Ch2.
- There are five negative examples (row width, row count, illegal tile, out of bounds, semaphore n), each a complete room followed by its exact expected Guru line.

> **Status: Outdated** (2026-10-29)
> Revision 3 invented `TICKS_PER_CELL = 2` so that its own PAR would pass. Revision 4's example was an empty box that needed no verb, so it failed the doc's own solver gate (H3). Revision 5 first shipped `PAR 200 1 4`, clones in a Ch1 room; that was patched on 2026-10-30.

## See Also

- [Determinism and snapshots](determinism-and-snapshots.md)
- [Game design v2](../game-design/gdd-v2.md)
