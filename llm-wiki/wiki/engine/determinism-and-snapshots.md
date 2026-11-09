# Determinism and snapshots

> Sources: Engine design rev 5, 2026-10-30; Engine design rev 4 critique, 2026-10-29; GDD v2, 2026-10-28
> Raw: [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [Rev 4 critique](../../raw/reviews/2026-10-29-engine-design-rev4-critique.md); [GDD v2](../../raw/game-design/2026-10-28-gdd-v2.md)
> Updated: 2026-10-30

## Overview

The sim is integer and Q16.16 only, with no libm. The same input log must produce the same state hash on Linux and AROS (C27). One snapshot system serves six features: rewind, FORK record, replay, shareable file, save and CI hash (v2 §5.4). The whole `World` is one POD arena, so a snapshot is a `memcpy`.

## Fixed-point contract

- `qmul` rounds toward −∞ and saturates on overflow, recording `SIM_OVF`.
- `qdiv` truncates toward zero. "`qdiv` saturates and records `SIM_OVF` when the quotient leaves the `int32_t` range (e.g. `INT32_MIN / -1.0`)." Division by zero records `SIM_DIV0`.
- `sim/`, `snapshot/`, `level/` and `input/` compile with `-fwrapv -ffp-contract=off`; CI adds a UBSan job.
- Trig uses a 1025-entry quarter-period sine table, checked into the tree as data.

## Input log

"`InputLine` is two bytes: byte 0 bits 0–3 are the direction mask and bits 4–7 are reserved; byte 1 bit 0 is fire and bits 1–7 are reserved."

Pie and verb commits are derived from hold-ticks (a 25-tick threshold), not extra bytes. Every `World` mutation that doesn't come from an `InputLine` is a logged `Event` (shell `cd`, room transitions, requester choices), applied at Step 0 of its tick.

> **Status: Outdated** (2026-10-29)
> Revision 4 gave three layouts: fire in byte 0 bit 4 (§3.1 comment, §4.7, R14), fire in byte 1 bit 0 (§5.1 table and struct), and two different reserved-bit ranges. Rev 5 fixed one layout in §5.1.

## Snapshot budget

`World` is 10296 B (§5.5 field list). Keyframes are taken every 25 ticks, and the 9 s rewind needs 19 of them. "The fork-moment `World` is kept in one `fork_base` keyframe outside the ring; only one FORK recording is open at a time, so one slot is enough." "`ForkRec` holds 500 ticks because v2 caps a FORK recording at 10 s".

```
BUDGET keyframe=10296 B; ring=19*10296=195624 B; fork_base=10296 B; fork=1000 B; forks=16*1000=16000 B; total=195624+10296+16000=221920 B
```

Budgets stay provisional until M0 measures entity counts. Q07 defaults to full keyframes, with Braid-style delta compression only after a measured budget miss.

> **Status: Outdated** (2026-10-29)
> Revision 4 had no `fork_base`, and its total was 211624 B. v2 says FORK recordings are "an input slice + start snapshot", but rev 4 stored inputs only, and its 450-tick ring was shorter than a 500-tick recording. The rev-4 critique flagged this as blocking.

## State hash and RNG

- `hash_state` is FNV-1a 64-bit over `serialize(World)`, taken at every keyframe and at the end of a replay.
- The RNG is `xorshift64*` seeded via splitmix64. "The `SEED` hex value is zero-extended to 64 bits and used as the splitmix64 state." No RNG draw occurs in 1.0.

## See Also

- [Engine architecture](engine-architecture.md)
- [Room format](room-format.md)
