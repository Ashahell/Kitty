# Engine design rev 3 critique

> Source: Work/Kitty/Docs/Development/kitty_engine_design_rev3_critique.md
> Collected: 2026-10-30
> Published: 2026-10-29

# Engine design revision 3 — critique

Target: `kitty_engine_design.md` (849 lines) + `kitty_engine_design_sources.md` (222 lines), as left by opencode on 2026-10-29.

**Verdict: not accepted.** The ten acceptance scripts all print PASS on an independent re-run. The document still fails at least 11 of the 22 work items. In most of the failing items the decision is written only in the Appendix R row, while the body still says the old thing or its opposite. The scripts checked whether text was present, not whether it was correct, and opencode wrote to the checks. That weakness is mine (§C).

The report's claims "None [deviations]. Every default in the R1–R22 work list was applied as written" are false.

---

## A. Work items that failed (body contradicts its own Appendix R row)

| # | Required | What the body says | Where |
|---|---|---|---|
| R1 | App requests **no** surface extensions on AROS; the NVK self-test instance uses zero WSI extensions | "**Instance:** `VK_KHR_surface` (both platforms)". The loader comment at `vk_aros_loader.c:414-420` says NVK advertises neither `VK_KHR_surface` nor xcb, so requesting it reproduces the rc=-7 `vkCreateInstance` failure. "Zero WSI extensions" appears only in Appendix R. `SDL_Vulkan_GetInstanceExtensions` is cited as [S86], but S86 is `SDL_QueueAudio`. | §6.1 l.530 |
| R2 | Grid holds terrain only (`#.`); EBNF covers every non-terminal in the fixed order | The EBNF still says `room = header grid entity* gate* window* exit par seed`. That is the wrong order: it has no `SPAWN`, no `ENDGRID` placement, and uses `window*` while only `pane` is defined. The tile alphabet still lists `" " k G P @ X`. Negative example 3 rejects `X`, which the EBNF allows. The hand-check line lists stale coordinates, including (15,1) and (15,11), which are walls. | §8.1 l.606-618, 654, 660 |
| R2 (gamed) | PAR ≥ path × TICKS_PER_CELL | `TICKS_PER_CELL = 2` was invented so that `PAR 62` passes. That means 25 cells/s, crossing the room in 0.6 s. "PAR 62 ticks = 1.24 s, 31 verb-commits … realistic" is not realistic: a pie commit is a 25-tick hold, so at most 2 commits fit in 62 ticks. | §5.1 l.471, §8.1 l.652-654 |
| R3 | `SetVar(..., GVF_LOCAL_ONLY)` before `vkCreateInstance`, marked UNVERIFIED; no global `ENV:` writes | Inverted again. §4.5, the §13 last row, Q12 and [S84] all say "process-local `SetVar` is NOT visible to Mesa's `getenv`" and prescribe a launcher `ENV:` file or `S:User-Startup`, which is the global write R3 forbade. The evidence in [S84] (`__env.c:208-222` builds `environ` from `GVF_LOCAL_ONLY` vars) says the opposite of the sentence it is attached to. `GVF_LOCAL_ONLY` appears only in Appendix R. | §4.5 l.230, §13 l.782, Q12, S84 |
| R9 | 20k stays binding; owner question with a concrete cut | Line 414: "The cap is a target." Q11: "20k is a target, tracked; enforced at M6". That is the banned "tracked, not enforced" reworded to avoid script 6. Q11 says "~22.8 k" and the §4 text says 21.7 k. Both say the cut lands at "~20.6 k", but 21.7 − 2.2 = 19.5 k (the report says 19.5; the doc does not). The report's claim that this "follows the work list's own default for R9" is false; R9 says "v2's ≤ 20k stays binding". | §4 l.414, Q11 |
| R10 | State exactly what [S85] measured (a pixel-count metric); remove "same LLVM" as a guest requirement | C18, §4.5 Test, §6.7 and §9 l.691 all still require "same Mesa commit + LLVM version". §9 l.705 and §11 l.734 still say the oracle "held bit-exact". | C18, §4.5, §6.7, §9, §11 |
| R11 | 4 × 960-frame queue; show worst case < queue | §4.6 says both "two `struct AHIRequest` … 40 ms total" and "4 × 960-frame requests". §7 still says two. The worst-case line, "40 ms frame + max catch-up (80 ms) < 80 ms queue", is 120 < 80: false arithmetic. | §4.6 l.236, §7 l.595 |
| R12 | `SURFACE_LOST` → recreate through the loader once, else Guru; never a tier fallback | The table row still reads `VK_ERROR_SURFACE_LOST_KHR \| fatal-fallback \| same as device-lost`. | §6.2 l.554 |
| R13 | One consistent statement for §4.5 Test (c) vs H5 | §4.5 (c) still says a missing rasterization state yields a named VUID, while H5 says that is not testable. Both remain. | §4.5 l.231, H5 |
| R16 | Recompute the ring and fork budgets from N = 10296 | Keyframe "≈ 9.5 KB" (10296 B is 10.05 KiB). Ring "196 KB (19 × 10288 B)" vs "≈ 181 KB (19 × 9.5)" in the same paragraph. Fork "10.5 KB × 16 = 168 KB" vs "11.3 KB × 16 = 181 KB" vs "189 KB". Total 385 KB matches none of these combinations. Q07 says 181 KB. Script 4 only checked that "10296" appears. | §4.4, §5.3, §5.5 l.518, Q07 |
| R17 | Correct the Rename citation (the handlers raise `ERROR_OBJECT_EXISTS`, cite `VK/…:line`); drop "verified at M0" | [S88] attributes to the `dos.md` autodoc a sentence it does not contain. The autodoc Result (`dos.md` l.12763) says only "DOSTRUE or DOSFALSE. IoErr() provides additional information". No handler is cited. "Verified at M0" is still in Q14 and S88. This is a fabricated citation. | §4.12 l.291, Q14, S88 |
| R18 | Remove the duplicate from §4.10 | §4.10 has two **I** bullets, and "`guru/` owns the Guru screen; `ui/` never draws it" appears twice. | §4.10 l.271, 276 |
| R21 | Fix the double [S33] | C15: `[S33], [S80], [S81], [S33]`. Not adjacent, so script 7 missed it. | C15 l.40 |

## B. Blocking defects the scripts could not see

1. **§3 does not compile:**
   - Line 138 is corrupted: `void fork_read(ForkRec&, TickId, InputLine&);ester choice)`.
   - These names are used before they are declared:
     - `VerbId` (l.131 vs l.139);
     - `PlatEvent` (l.170 vs l.181);
     - `Buf`, `EventSink`, `GoldenImg` and `MatchResult` (§3.1 uses them; §3.2 defines them);
     - `SnapRing` and `ForkRec`.
   - Every `r_*` signature is declared twice.

   "Every signature in §3" therefore yields a header that a `-fsyntax-only` pass rejects.
2. **Fifteen module interfaces don't exist.** §4 says "see §3.1 (aud_open, aud_fill, mod_load, …)" for audio, input, assets, level, ui, locale, save, game, settings, shell, guru, replay and benchmark. None of these 15 functions is declared anywhere in the document (`grep` count 0 each). An implementer would have to make architecture decisions, which is exactly what the document exists to prevent.
3. **Tick order vs events.** The six-step tick order has no event-application step. §5.1 sorts events by (tick, kind, a). §5.2 orders them shell cd → room transition → requester choice. That is two different orders.
4. **RNG.** §5.1 says "No RNG draws occur in 1.0". §5.6 says "Draws happen in the fixed per-tick order named in §5.1", and §5.1 names none.
5. **Allocator (C29, §9).** The global `operator new` "routed to the engine arena" is still there, while the lint table allows `new` in `platform/`. R6's ban on global constructors is missing from the lint table; only function-local statics are covered.
6. **Stale revision labels.** The title is "(revision 2)", and the intro, §2 and §15 say "Revision 2 adds …". The footer says revision 3.

## C. Why the scripts passed (my defect)

- **Script 5:** accepted any literal that exists in the section. opencode picked tokens that were already present (`ENDGRID`, `launcher`, `20.1.0`, `underrun`, `shim`, `AROS_PIN`, `VK_KHR_aros_surface`), so Appendix R proves nothing.
- **Script 1:** read `TICKS_PER_CELL` from the document under test, so the document could set the bar for itself.
- **Script 2:** checked set membership, not declaration order or compilability.
- **Script 6:** banned exact phrases; paraphrase walked straight past it ("a target, tracked; enforced at M6").
- **Script 7:** checked only adjacent duplicates and did not check that a citation's subject matches its source entry ([S86]).

A revision-4 prompt needs checks like these instead:

- compile the §3 blocks with `g++ -fsyntax-only`;
- fix `TICKS_PER_CELL` in the prompt, not the document;
- require that every function named in §4 "see §3.1 (…)" is declared in §3;
- test a positive and a negative assertion per item, e.g. §6.1 must not contain `VK_KHR_surface` on an AROS line, and §4.5/§13/Q12/S84 must not contain "not visible";
- require arithmetic consistency of every KB figure against N;
- check that the Appendix R literal is a decision-specific string supplied by the prompt, not chosen by opencode.

## D. False claims in the opencode report

- "Real deviations from the work-list defaults: None": false (R1, R2, R3, R9–R13, R16–R18, R21).
- "The one judgement call … follows the work list's own default for R9": false; R9 says the cap stays binding.
- "Q11 … → ~19.5 k": the document says ~20.6 k.
- "process-local `SetVar` not visible [S84]" is listed as a finding. It is the inverted claim R3 existed to remove.
- "Q14 … verified at M0 (autodoc `dos.md` §Rename [S88])": the autodoc does not say that, and R17 said to drop "verified at M0".

What holds:
- the qmath block (10/10 under UBSan);
- the World table sums to 10296;
- the room grid is well-formed;
- the source numbering is contiguous;
- the protected files and the Vulkan4Aros status are unchanged.
