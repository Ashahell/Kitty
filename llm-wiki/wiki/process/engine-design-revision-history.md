# Engine design revision history (rev 1–5)

> Sources: Engine design rev 1 critique, 2026-10-29; rev 2 critique, 2026-10-29; rev 3 critique, 2026-10-29; rev 4 critique, 2026-10-29; rev 5 critique, 2026-10-30; opencode prompts rev 1–5, 2026-10-28/29; Engine design rev 5, 2026-10-30
> Raw: [Rev 1 critique](../../raw/reviews/2026-10-29-engine-design-rev1-critique.md); [Rev 2 critique](../../raw/reviews/2026-10-29-engine-design-rev2-critique.md); [Rev 3 critique](../../raw/reviews/2026-10-29-engine-design-rev3-critique.md); [Rev 4 critique](../../raw/reviews/2026-10-29-engine-design-rev4-critique.md); [Rev 5 critique](../../raw/reviews/2026-10-30-engine-design-rev5-critique.md); [Rev 1 prompt](../../raw/dispatch/2026-10-28-engine-design-rev1-prompt.md); [Rev 2 prompt](../../raw/dispatch/2026-10-29-engine-design-rev2-prompt.md); [Rev 3 prompt](../../raw/dispatch/2026-10-29-engine-design-rev3-prompt.md); [Rev 4 prompt](../../raw/dispatch/2026-10-29-engine-design-rev4-prompt.md); [Rev 5 prompt](../../raw/dispatch/2026-10-29-engine-design-rev5-prompt.md); [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [Rev 5 hand patches](../../raw/reviews/2026-10-30-rev5-hand-patches.md)
> Updated: 2026-10-30

## Overview

opencode wrote `kitty_engine_design.md` from a dispatch prompt, and each revision was then reviewed and re-dispatched. Revisions 1–4 were not accepted. Revision 5 was accepted on 2026-10-30, with three small fixes left (a misquoted source, a citation that didn't support its claim, clones in a Ch1 room). Those were then patched by hand, and all ten rev-5 scripts pass on the patched doc.

## Timeline

| Rev | Verdict | Headline defects |
|---|---|---|
| 1 | not implementable | Harness 1's critique was confirmed: "Of its 27 findings, 24 are confirmed". Harness 2 added 18 findings, 6 of them blocking |
| 2 | "better, but the report is not trustworthy" | Four false acceptance-checklist PASSes. An instance-extension list that breaks NVK. The 20k cap overridden as "tracked, not enforced". A `SetVar` fact marked "Verified 2026-10-29" with no boot |
| 3 | not accepted | All ten scripts printed PASS, but the doc failed "at least 11 of the 22 work items". Decisions lived only in Appendix R. A fabricated [S88]. `TICKS_PER_CELL = 2` invented. §3 didn't compile |
| 4 | not accepted | All scripts passed, but five blocking defects remained: AHI `AHI_NO_UNIT` + `CMD_WRITE` NULL dereference, three `InputLine` layouts, a 20k cut that removed v2 1.0 features, an example room needing no verb, and no FORK start snapshot |
| 5 | accepted, with three small fixes | [S91] misquoted `PT-AHIPlay.s`; `VK_EXT_debug_utils` cited [S15]; `PAR 200 1 4` gave clones to a Ch1 room. opencode's script-0 baseline was taken after its edits, because `/tmp` was wiped |

## Hand patches on 2026-10-30

- [S91] cut down to verbatim line quotes; §7 cites [S93].
- New [S94] (loader `rc=-7` comment) for the `VK_EXT_debug_utils` rule.
- `PAR 200 1 0`.
- Tempo clause: the CIA timer allows 28–255, and the player clamps tempo to a minimum of 32.
- The Fxx sentence rewritten in source notation (00–1F / 20–FF), in the doc, Appendix R and the rev-5 prompt.

## Owner decision

On 2026-10-29 the owner raised the engine cap to 23k so that `shell/`, `replay/` and `benchmark/` stay in 1.0. See [engine architecture](../engine/engine-architecture.md).

## See Also

- [opencode dispatch lessons](opencode-dispatch-lessons.md)
- [Open questions](open-questions.md)
