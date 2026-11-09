# Engine design rev 5: hand patches and re-verification

> Source: work record of the Claude Code review session (patches applied to Docs/Development/kitty_engine_design.md and kitty_engine_design_sources.md)
> Collected: 2026-10-30
> Published: 2026-10-30

On 2026-10-30, after the rev-5 critique, the following fixes were applied by hand to the engine design document and its sources file:

1. [S91] was rewritten to 21 verbatim line quotes from `PT-AHIPlay.s`, each checked against the file (line 415 is `divu.w d2,d1`). The §7 MOD sentences now cite [S93] instead of [S91][S93].
2. New source [S94] quotes the loader comment at `vk_aros_loader.c:418` and `:423`-`:424`. The §6.1 `VK_EXT_debug_utils` sentence now cites [S94] instead of [S15].
3. The example room's PAR line changed from `PAR 200 1 4` to `PAR 200 1 0`; the Hand-check line says "0 clones (FORK arrives in Ch2)".
4. A tempo clause was added to §7: "The CIA timer itself accepts 28–255 [S70][S93]; `modplay` follows the PT2.3A player, which raises a tempo below 32 to 32 (`MOVEQ #32,D0`) [S91]."
5. The `audio/` LB sentence was moved to the end of the §4.6 R bullet.
6. The Fxx sentence was rewritten in the source notation, in the doc, Appendix R row M1 and the rev-5 prompt: "`Fxx` with xx = 00–1F sets the speed (ticks per row), and xx = 00 is ignored; xx = 20–FF sets the tempo (32–255 BPM)." It cites [S93][S68].

Re-verification: all ten rev-5 acceptance scripts, extracted from the updated prompt, print PASS on the patched document. The "no source entry lost" check in script 7 was vacuous for this run, because the baseline was taken after the edits; the only source changes were the S91 rewrite and the new S94, and S1–S94 are contiguous.

Context: `/tmp` was wiped between the rev-5 opencode run and this review. This session's scratchpad was emptied too, which corroborates opencode's report that its script-0 baseline directory disappeared.
