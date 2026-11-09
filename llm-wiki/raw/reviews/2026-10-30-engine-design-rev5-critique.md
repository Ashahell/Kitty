# Engine design rev 5 critique

> Source: Work/Kitty/Docs/Development/kitty_engine_design_rev5_critique.md
> Collected: 2026-10-30
> Published: 2026-10-30

# Engine design rev 5 — critique

**Verdict: accepted, with three small fixes left.** I re-ran the scripts extracted from the rev-5 prompt, and all ten pass. Every rev-4 critique item is fixed in the body. None of the leftover defects is blocking. But two of the scripts' guarantees were void in this run (§B), and two citations don't support what they're cited for.

## A. Rev-4 items — status

| Item | Status | Note |
|---|---|---|
| A1 AHI unit | fixed | §4.6 and §7 open on `AHI_DEFAULT_UNIT`, citing [S92]. All seven cited S92 lines checked against the source (`ahi.h:354-355`, `device.c:202,331-332`, `devcommands.c:106,109`). |
| A2 InputLine | fixed | One layout in §5.1; §3.1 and §4.7 point at it; R14 updated. |
| A3 cap | fixed | 23k binding, 22.5k total, `audio/` 2.0k. No cut proposal remains. §12 is consistent with this. |
| A4 room | fixed | The wall at x=8 has one gap, held by `req1`. A requester gate is right for Ch1 per v2. |
| A5 fork_base | fixed | `fork_base` slot added and BUDGET = 221920 B. The clone row now names length and spawn position, and the duplicate cursor row is gone (total unchanged). |
| M1 MOD | fixed in the body | But see C1 on [S91]. |
| M2–M5, N1–N5 | fixed | — |

## B. Process failure (opencode)

- **Script 0 ran after the edits.** `/tmp` was wiped mid-session; my scratchpad was wiped too, which corroborates that. opencode didn't stop and report the lost baseline; it re-created `$R` and continued. Three guarantees are therefore void for this run:
  - script 7's "no source entry lost" check (it reported "changed: none" even though S68 and S70 changed);
  - the before/after line counts;
  - the protected-files hash check.
- **What I verified independently:**
  - **Protected files:** every protected file's mtime is older than the rev-5 prompt (2026-10-29 13:00), so none were touched.
  - **Sources:** S1–S93 are contiguous with no duplicates. The entries I had seen in rev 4 (S69, S81, S83–S85, S88–S90) are byte-identical.
  - **Not verifiable:** the content of S1–S67 and S71–S79. No rev-4 copy survives.
- **Report errors:**
  - Item 8 lists Rename as "UNVERIFIED … a save-path probe at M0/M6". The doc has no such marker or probe; the claim stays "inferred from the handler sources", which is correct.
  - Item 8 cites "§5.9", which doesn't exist (locale is §4.11).

## C. Remaining defects

1. **[S91] misquotes its source and draws conclusions.**
   - The entry quotes line 415 as `divu.w #3546895,d2,d1`, which isn't valid 68k. The file has `divu.w d2,d1`.
   - It puts `*` before `SetTempo` and `mt_VolumeChange`; the file has none.
   - It contains conclusions ("so Fxx speed is the parameter range 0x01–0x1F …"), against the evidence rule.
   - [S93] states every §7 MOD fact directly ("speed (00-1F) / tempo (20-FF)", "volume, 00-40", the period table).
   - **Fix:** cut S91 down to verbatim line quotes (or drop it from §7), and cite [S93] alone for the ranges.
2. **The §6.1 `VK_EXT_debug_utils` rule cites [S15]**, the advertised-capability contract page. That page says nothing about instance extensions or enumeration: the same defect class as rev 3's [S86].
   - **Fix:** cite the loader line that fails with rc=-7 on an unadvertised extension (`vk_aros_loader.c` ~406–420), or the Vulkan spec's `VK_ERROR_EXTENSION_NOT_PRESENT` rule for `vkCreateInstance`.
3. **The example room declares `PAR 200 1 4`**: up to 4 clones in a Chapter 1 room. FORK is the Ch2 verb (v2 §5.1), so a Ch1 room can't hold clones.
   - **Fix:** use `PAR 200 1 0`, and change "4 clones ≤ 16" in the Hand-check line to match.
4. **Minor:** [S70] and [S93] give the CIA tempo range as 28–255, while §7 says 32–255 (the clamp in the PT2.3A player). Both are true; one clause in §7 should say which one the engine follows.
5. **Minor:** the §4.6 LB sentence sits as a loose paragraph between the `R:` and `I:` bullets, breaking the field list.

## D. Why the scripts passed C1–C3

- **Script 7** checks only that a cited entry *contains* the term or range. It doesn't check that a quoted VK line matches the file. A per-line quote check would have caught S91's line 415.
- **No rule ties** the `VK_EXT_debug_utils` citation to a source that contains it. The term list covers `AHI_DEFAULT_UNIT`, `Cxx` and others, but not this one.
- **Script 1** caps clones-max at 16, and no rule ties it to the room's chapter.
- **Script 0's baseline** lived only in `/tmp`. A baseline outside `/tmp` (or a check that `src_start.md` predates the doc's mtime) would have made the vacuous run visible.
