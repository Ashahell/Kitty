# Engine design rev 4 critique

> Source: Work/Kitty/Docs/Development/kitty_engine_design_rev4_critique.md
> Collected: 2026-10-30
> Published: 2026-10-29

# Engine design rev 4 — critique

**Verdict: not accepted.** All ten scripts pass, and they also pass when re-run independently. The items the rev-3 critique listed are fixed. But the scripts checked form, not meaning: five blocking defects got through, and one of them crashes on AROS at M0.

## A. Blocking

| # | Defect | Where | Evidence |
|---|---|---|---|
| 1 | **The audio open crashes.** The doc opens `ahi.device` on `AHI_NO_UNIT` and then sends `CMD_WRITE`. For `AHI_NO_UNIT`, `io_Unit` is NULL, and `CMD_WRITE` does `PutMsg(&io_Unit->unit_MsgPort, …)`, a NULL dereference on an OS with no memory protection. `AHI_NO_UNIT` is for the low-level API only. The device API needs `AHI_DEFAULT_UNIT` (0). | §4.6 D, §7 | `VK/…/AHI/Device/device.c:202-205` (autodoc), `:325-333` (`io_Unit = NULL` for NO_UNIT); `devcommands.c:109` |
| 2 | **The InputLine wire format is defined three different ways.** (a) Fire is byte 0 bit 4 (§3.1 comment, §4.7 D, Appendix R14). (b) Fire is byte 1 bit 0 (§5.1 table, `struct InputLine {u8 dir; u8 fire;}`, §5.1 prose). (c) The §5.1 prose reserves byte 0 bits 5–7, but the table two lines below reserves bits 4–7. This is the replay, save and hash format. | §3.1 l.131, §4.7, §5.1 l.505–514, R14 | — |
| 3 | **Q11's default cut removes v2 1.0 features.** It cuts `shell/`, `replay/` and `benchmark/` to get under 20k. Shell fast-travel and Replay/demo mode are in v2's "In 1.0" table, and benchmark mode is an M6 deliverable in v2. This is a v2 change presented as an engine default, although §15 says v2 decisions are never re-opened. §1 still lists all three as in scope, and §12 still lands benchmark mode and the replay camera at M6. On top of that, the 1.2k audio budget is smaller than the ~1.5k the replayer alone is given (C16, Q05, v2 l.264). The budget table is internally impossible, and the "binding" cap is broken by the doc's own spec. **This one is the owner's decision:** raise the cap to about 22–23k, or cut real v2 scope through a v2 change. | §1, §4 LB table, §12, §15 Q11, C16 | v2 l.35–36, l.264, l.325, l.354 |
| 4 | **The only valid example room is an empty box.** It has no interior walls, so gates, panes and patrols block nothing, and a 0-verb walk reaches WIN. PAR declares 4 verb-commits, so by H3's own known-bad case ("solvable with fewer verbs than PAR declares") and by C22, the canonical example fails the solver gate. It was built to pass script 1. | §8.1 | Shortest path 22 = Manhattan distance, with no obstacles |
| 5 | **The FORK start snapshot has nowhere to live.** In v2, a FORK is an input slice plus a start snapshot. The world pauses, Kitty records up to 10 s, and then both Kitty and the clone continue from the fork moment. The doc stores inputs only. It never says where the fork-moment `World` is kept, and the 450-tick ring is shorter than a 500-tick recording. The BUDGET line has no slot for it (one more 10296 B keyframe). The §5.5 clone row also duplicates "input cursor" with the separate "per-clone recording cursors" row, and the "length" and "spawn position" that `ForkRec`'s comment and R8 place in `World` are not fields there. | §3.1 l.135, §4.4, §5.3, §5.5 | v2 l.160, l.196 |

## B. Major

- **The MOD spec is wrong, and contradicts its own source.**
  - "Tempo `Fxx` 17..64 BPM" disagrees with [S70], which says tempo is 28–255. In ProTracker, F01–F1F set the speed and F20–FF set the BPM (32–255).
  - "Period→frequency on the 50 Hz clock" is wrong: the conversion uses the Paula clock, and 50 Hz is the tick rate.
  - "`$`/`/` set-volume effects" means nothing; set volume is effect `Cxx`.
  - "Big-endian signed 8-bit → byte-swizzled" is meaningless, since 8-bit samples have no byte order.
  - Where: §4.6 D and §7.
- **§4.6 contradicts itself.** T says audio is "tick-independent (mixer runs on its own AHI timing)", while D says requests are "serviced in the tick loop".
- **R1's failure class is back for `VK_EXT_debug_utils`.** On AROS, debug builds request it unconditionally. If the ICD doesn't advertise it, instance creation fails with rc=-7, exactly like the surface extensions did. It must be requested only when enumerated. Where: §6.1 l.585.
- **Wrong cross-reference.** §4.5 E points at the "§6.5 table"; the VkResult table is in §6.2.
- **Two tick loops.** The §3 loop has five steps and no event step, while §5.1 has Step 0. One of them must defer to the other.

## C. Minor

- §4.12 D says "load failure of `current/` falls back to the other slot", but `current/` is not a slot.
- §5.1 says qdiv "cannot overflow given the arena limits", yet the code saturates and the harness tests `INT32_MIN / −1.0` overflowing.
- §5.6 says "first 8 bytes of the seed", which is undefined for `SEED 0xC0FFEE` (3 bytes). The byte order and padding rule are missing.
- The LB table includes a `tools/` row, but the bold "Engine total" (21.7) excludes it. The column sums to 23.7, and the row needs a label.
- Appendix R pointers R21 → §9 and B6 → §1 point at nothing that owns those decisions.

## D. What holds (verified)

- **Scripts:** my independent re-run passes all ten. Vulkan4Aros status is unchanged, and the protected files are intact.
- **§3 compiles:** 51 functions declared, and every `I:` name is declared there.
- **Fixed items:**
  - R1 is correct in §6.1.
  - R3 now uses process-local `SetVar` and is marked UNVERIFIED with a probe.
  - R10 and R12 are fixed.
  - R17's cited lines check out: `fat/ops.c:574` and `afs/filehandles2.c:503` contain `ERROR_OBJECT_EXISTS`, and `rom/dos/rename.c` has none.
  - [S88] is now accurate, and [S90] is good.
- **`ForkRec`:** `ForkRec = 500×2 B` matches v2's "up to 10 s" at 50 Hz. It was not padded to reach a round size.
- **`ReadPixelArray`:** LVO −160 is correct for the 8-byte stride (slot 20).
- **Report §8:** the VK-status delta it attributes to another session is plausible. `bf63c028` exists ("aros-v1-merge-fixes Stage 2"), and no delta shows now.
- **Report:** no false claims found. It was simply silent on everything above.
- **Not verified:** whether the sources file lost content going from 222 to 120 lines. There is no rev-3 copy to diff against. The current file has 90 entries, S1–S90, contiguous, all cited ids defined.

## E. Why the scripts passed (my defect)

- **Script 1** checks reachability and the PAR bound, but not that the room *needs* its verbs. It has no solver and no "0-verb path must not exist" check.
- **No script compares the InputLine layout across sections.** Only the REQ literals were section-pinned.
- **Script 3** accepted any cut that summed correctly. The prompt never said the cut must come from outside v2 1.0 scope, and never checked the LB table against C16.
- **The prompt fixed "Four chained `CMD_WRITE`" but not the unit.** No script checks API semantics against the AHI source.
- **Nothing scans for numeric claims** (BPM ranges, clocks) against the cited source's text.
