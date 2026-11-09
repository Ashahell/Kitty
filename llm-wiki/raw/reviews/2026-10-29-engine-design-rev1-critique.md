# Engine design rev 1 critique

> Source: Work/Kitty/Docs/Development/kitty_engine_design_critique.md
> Collected: 2026-10-30
> Published: 2026-10-29

# Critique: `kitty_engine_design.md` — harness 2 (opencode, 2026-10-29)

**Verdict:** harness 1's critique is substantially correct and its findings are reproducible against the
document and against the Vulkan4Aros tree. Of its 27 findings, 24 are confirmed, 2 are refined (the
severity or the mechanism changes), and 1 is contested (finding 21, item 4 — "forced FIFO must not hang").
Harness 2 adds 18 further findings, of which 6 are blocking. The document is still not implementable as
written, but the gap is now small and well-defined: four decisions (audio API, CRT/allocator, room
grammar, result-code table) plus a `World` field list.

Severity: **B** = blocks implementation · **M** = wrong, fix before M0 · **m** = cleanup.

---

## Part A — harness 1 findings, validated

| # | h1 | h2 verdict | Evidence |
|---|---|---|---|
| 1 | M0 drops v2's NVK smoke run | **Confirmed** | §12 M0 table lists no NVK work; v2 §11.2 M0 row requires "one NVK smoke run of the same scene on the card lane" |
| 2 | Audio mixes two AHI APIs | **Confirmed** | `AHIA_PlayerFreq` is a tag of `AHI_AllocAudioA()` (library/hook API, src `src/abi/v11/AROS/workbench/devs/AHI/Device/audioctrl.c:394`); §4.6 D uses `ahir_Link`-chained `CMD_WRITE` (device API). Two different designs; the hook runs in AHI's mixer context, breaking §3 single-threaded |
| 3 | CRT/allocator undecided | **Confirmed** | §9 says `-nostartfiles … -ldos -lexec + libprintf/libm if stdio needed`; §4.2 permits `std::vector`; no `operator new` decision anywhere. AROS C library is renamed on build-pc (libcrt/libstdlib/libcrtprog) — [S51] |
| 4 | Flagship room example invalid | **Confirmed** | `ROOM ch1_03 16 12` declares 12 rows; GRID has 8; row 3 is 17 wide with a space; `#` is both comment and wall; `PAR 62 31 4` = 31 verb-commits in 1.24 s |
| 5 | Sim totality not delivered | **Confirmed** | `qdiv` has no divide-by-zero policy; §4.3 E claims "no error path at runtime". Also signed-overflow UB across host gcc vs AROS gcc-16 is a real divergence hole (no `-fwrapv` in §9) |
| 6 | Codegen flags differ | **Confirmed** | `-fno-strict-aliasing` is in Appendix A's AROS set but not in §9's common Linux line; `sim/`/`snapshot/` must use one set on both |
| 7 | Lavapipe oracle ignores LLVM | **Confirmed (M)** | [S50] oracle note compares host LLVM 22.1.8 vs guest LLVM 20; §2 C18/C19 and H4 pin only Mesa. Must add LLVM version + `LP_*` env to the pin stamp |
| 8 | NVK self-test bit-exact golden | **Confirmed** | §6.4 says "compare to stored golden"; cross-Turing→Blackwell bit-exactness is unrealistic; §6.4 also detects NVK by "name contains NVIDIA" — must use `vendorID 0x10DE` + `driverID == VK_DRIVER_ID_MESA_NVK` |
| 9 | `vkGetIcdWpaResult` is interpreter-internal | **Confirmed** | `icd_vkGetIcdWpaResult_internal` lives in `src/vulkan/library/icd_lifecycle.c` (interpreter ICD); the game targets lavapipe, which has no such getter. Need `ReadPixelArray`/QEMU HMP `screendump` instead |
| 10 | Palette quantize in FS is fill-hostile | **Confirmed (M)** | §6.3 "keeps fragment cost low" contradicts itself; llvmpipe is fill-bound [S17]. Palette-indexed vertex colours or a 32³ LUT or a 320×256 post pass |
| 11 | Instance extensions missing | **Confirmed** | §6.1 says "exactly one extension: `VK_KHR_swapchain`" — `VK_KHR_surface` (+ platform surface ext) is required |
| 12 | Frame sync underspecified | **Confirmed** | one semaphore + one fence + a 3-slot uniform ring for 1 frame in flight; classic present-semaphore VUID. Drop to N=1 frames, one render-finished semaphore per swapchain image, drop the 3-slot ring |
| 13 | Error policy treats every non-success as fatal | **Confirmed** | §4.5 E "any `VkResult != VK_SUCCESS` → fall back"; `VK_SUBOPTIMAL_KHR`/`OUT_OF_DATE`/`TIMEOUT`/`NOT_READY` are normal; §6.2 handles OUT_OF_DATE itself. No result-code table |
| 14 | Interpolation incoherent | **Confirmed** | "sub-tick interpolation accumulator (0..4 substeps)" vs "draws the latest fully-stepped state"; no previous-state or float-conversion spec. For flip-screen pixel art, no interpolation is the correct default |
| 15 | Input log contradicts itself | **Confirmed** | §5.1: 2 B/tick, pie/verbs derived from hold ticks, "on-node flag" in byte0. §4.7: 3-field `ChannelState{dir,fire,discrete}` + sparse events. The on-node flag is sim state, not input — recording it duplicates truth |
| 16 | Hashing schedule contradicts itself | **Confirmed** | §4.4 "rolling_hash updated every tick" vs §5.1 "(every 10 ticks … thinking-out-loud)" |
| 17 | Snapshot budget arithmetic wrong | **Confirmed** | 64×96 B = 6144 B already fills "≈ 6 KB" before clones/windows/requesters/grid (Cancel "closes path" makes the grid mutable state; 128×128 cells ≥ 16 KB). 20×25 = 500 ticks = 10 s, not "9.1 s" |
| 18 | Audio buffer numbers contradict | **Confirmed** | §4.6: 2×1024 ≈ 85 ms; §7: 2×2048 ≈ 85 ms "per side"; actual 1024 = 21 ms, 2048 = 43 ms; AHI natural pass ≈ 960 frames at 50 Hz [S33] |
| 19 | Atomic save via temp+Rename not atomic | **Confirmed (M)** | AmigaDOS `Rename` fails when dest exists → delete-then-rename has a no-save window. Two-slot `progress.a`/`progress.b` + generation counter |
| 20 | `LP_NUM_THREADS=1` not single-threaded | **Confirmed** | [S25] `=1` still spawns one worker pthread; `=0` renders in caller. Also how the app sets env before ICD reads it is unverified (AROS SetVar global vs local) |
| 21 | CI gates can't fail / wrong reason | **Refined** | (a) H7 `-ffp-contract=fast` sim code — sim has no floats, can't diverge → replace with uninitialized struct padding in `serialize` or `long`-size-dependent encoding. (b) §4.5 (b) "pipeline without rasterization state must NOT render" — `pRasterizationState` is required, null is invalid usage/UB, not a testable "does not render"; same for (a) swapped acquire/submit (validation layers catch it, goldens don't). (c) H5 `LVK_VALIDATE` is invented; use `VK_INSTANCE_LAYERS`/`VK_LOADER_LAYERS_ENABLE`; name a VUID layers actually report. (d) §4.1 "forced FIFO must not hang proves no WaitTOF" — **contested**: this does test the app's present path; keep it, but add the source lint + LVO scan the critique asks for as a separate gate (both are cheap and complementary). (e) H8 "BLOCKED is itself PASS-with-caveat" — **Confirmed**; BLOCKED must be its own state, never counted as PASS |
| 22 | Interfaces aren't shared | **Confirmed** | §4.2 promises "one header, two `.cpp`" but signatures take `PlatAROS&`; types drift (`RendCtx`/`RenderCtx`/`Allocator`/`Alloc`/`Plat`/`PlatAROS`/`HashSink`/`ByteBuf` undefined anywhere); `verb_commit` returns `bool` while §4.3 E says "result code" |
| 23 | Scope items have no module | **Confirmed** | §1 lists Shell fast-travel, ENVARC settings, Guru error screen, replay/demo mode, benchmark mode; none has an R/I/D/T/E/A/Test spec; `game/` is in the diagram with no spec. "13 modules × 7 fields" is true only because those were left out |
| 24 | No line budget | **Confirmed** | v2 sets ≤ 20k engine lines; no module carries an estimate |
| 25 | Citation padding | **Confirmed (m)** | "[S29 Delay unit]" on pie-menu hold (sim ticks aren't `Delay` ticks); "[C26 not; §10 flavor]" in §4.10 A is gibberish; §6.1's `device_generated_commands` "(the dispatch's 'render passes' phrase …)" is noise from misreading the dispatch |
| 26 | Small errors | **Confirmed (m)** | §4.2 has the Test line duplicated (lines 121–122, verified); §6.3 "≤ 5 pipelines" needs a real count (at least 6: flat, tex, dither, UI, plus cull/no-cull and depth variants); sources file says CRC-32C is for manifests, main doc uses it for RoomBin |
| 27 | Three hash algorithms | **Refined** | Three is fine, but the split (FNV state / CRC-32C room / SHA-256 pack) is never justified in one line; either justify or unify on one 64-bit hash |

---

## Part B — harness 2 new findings

**B1. [B] `qdiv(a,0)` and signed overflow are unspecified, and `§5.1.OV` does not exist.**
§4.3 I references "§5.1.OV" in its test line, but §5.1 has no OV subsection. `qdiv` has no divide-by-zero policy (UB + SIGFPE on x86) and `qmul` has no overflow policy beyond an assertion. Add `qdiv(a,0) → SAT_MAX` with a `SIM_DIV0` result code, and add `-fwrapv` (or `-ftrapv` in CI) to `sim/` on both platforms, plus UBSan in the Linux CI.

**B2. [B] Types referenced by interfaces are never defined.**
`PlatAROS`, `Plat`, `RendCtx`/`RenderCtx`, `Allocator`/`Alloc`, `HashSink`, `ByteBuf`, `TickUs`, `EventSink`, `UIRects`, `UICtx`, `UiSpec`, `Loc`, `Save`, `Reason`, `Progress`, `SnapSeed`, `AsMesh`, `MeshId`, `Levels`, `RoomBin`, `RoomId`, `InputLog`, `Input`, `Audio`, `ModBytes`, `Channel`, `Position`, `Assets`, `PathBuf`, `Buf`, `LogSink`, `TickId`, `TickRecord`, `SnapRing`, `ForkRec`, `World`, `InputLine`, `VerbId`, `Arg`, `HashState`, `ByteBuf`, `CharBuf`, `StringId`, `RendCtx`… none has a definition anywhere in the document. The "one header, two `.cpp`" promise (§4.2) cannot be implemented from this. Write `platform.h`, `sim_types.h`, `render_types.h` once, in the document, before any module spec.

**B3. [B] `World` has no field list, so the snapshot budget cannot be checked.**
§4.4 D says "≈ 6 KB" and §5.3 says 64×96 B = 6144 B, which already exhausts it before clones/windows/requesters/grid (and Cancel "closes path" makes the grid mutable state; 128×128 cells ≥ 16 KB). Re-derive `World` from a field list, then size the ring. (h1 #17 covers the arithmetic; this is the missing input.)

**B4. [B] M0 must include v2's NVK smoke run, and M0's scope is not "pure".**
§12 says "M0 stays a pure feasibility spike" but lists `platform/aros`, `render/`, `audio/`, `platform/linux` mirror and benchmark-lite, plus H5/H6/H7. That is not pure. And v2 §11.2 M0 explicitly requires "one NVK smoke run of the same scene on the card lane", which §12 M0 omits (h1 #1). Add the NVK smoke run (lane-gated, BLOCKED is a legal state) and relabel M0 honestly.

**B5. [B] Audio API decision is still open in the text.**
§7's "AHI-vs-SDL latency contract" still names `AHIA_PlayerFreq` (library/hook API) in prose while §4.6 I is device-API-shaped (`aud_open`/`aud_fill`). Pick the device API (main loop fills and `SendIO`s, keeping §3's single-threaded rule) and delete the hook references, or commit to the hook and specify the cross-thread hand-off. Same for SDL2: name `SDL_QueueAudio` (main thread, no lock) or specify the callback hand-off.

**B6. [B] Room grammar has no EBNF and no terminator.**
§8.1 gives an example with `GRID` and no end marker, mixed `#`/`;` comments, a 17-wide row in a 16-wide room, and `PAR 62 31 4`. The format is the contract for the compiler, solver, validator and 1.1 community packs. Write the EBNF, a terminator (`ENDGRID` or fixed row count from `ROOM … 16 12`), and a worked example that the grammar accepts. (h1 #4; the example itself is the blocker.)

**B7. [M] Pipeline count is understated.**
§6.3 says "≤ 5 pipelines once at startup" but enumerates 6 shaders plus cull/no-cull, depth, and dither variants. Count pipelines, not shaders, and budget init time against the uncached MCJIT finalize hazard [S48].

**B8. [M] `LP_NUM_THREADS` mechanism is unverified.**
§3 pins `LP_NUM_THREADS=1` "engine-set", but how an AROS app sets an env var before the ICD's `getenv` reads it is unverified (AROS `SetVar` global vs local visibility). Specify the mechanism (launcher `ENV:` file, or `SetVar` in `S:User-Startup` before the app), or ship it as a default in the ICD manifest. (h1 #20.)

**B9. [M] `HashSink* fnv` and the rolling-hash period are unspecified.**
§4.3 I takes `HashSink* fnv`; §4.4 says "rolling_hash updated every tick"; §5.1 thinks out loud ("every 10 ticks …? — no"). Pick: hash every tick (FNV-1a over 6 KB = negligible on host; measure on AROS at M0) or every keyframe (fork boundary). Delete the thinking-out-loud. (h1 #16.)

**B10. [M] `World` size and keyframe count.**
20 keyframes × 25 ticks = 500 ticks = 10 s, but Nine Lives rewinds 9 s (450 ticks) → 18 keyframes. Fix the count and the "9.1 s" figure. (h1 #17.)

**B11. [M] Tools count is wrong.**
§4.13 says "Six tools" but enumerates seven (`kitty-cc`, `kitty-solve`, `kitty-validate`, `kitty-pal`, `kitty-hash`, `kitty-shaders`, `kitty-lint`). Fix the count.

**B12. [M] `game/` has no spec and the line budget is absent.**
§1 lists Shell fast-travel, ENVARC settings, Guru error screen, replay/demo mode and benchmark mode; none has an R/I/D/T/E/A/Test. `game/` is in the diagram with no spec. v2 sets ≤ 20k engine lines; no module carries an estimate. (h1 #23, #24.)

**B13. [M] `kitty-lint` and `kitty-shaders` are tools but their rules are prose.**
§9's lint rules and §6.3's shader list are prose; the tools need a machine-readable rule file (regex table) so CI can run them. Otherwise "kitty-lint on a file containing `float` in `sim/` → exit ≠ 0" (§4.13 Test) is unimplementable as stated.

**B14. [M] `SEED` line and RNG are underspecified.**
§4.9/§8.1 use `SEED 0xC0FFEE`; §4.3 says "per-room xorshift64*". The variant, the seed-mixing (splitmix64 vs direct), and the consumption order are not specified — a determinism hole if two implementations diverge. Pin the variant and the mixing in §5.1.

**B15. [M] `ui/` is a state-mirror but has no input.**
§4.10 says "pure state-mirror; cannot mutate state". Requesters (`[Retry]`/`[Cancel]`) mutate world state via `sim/` verbs; the input routing (which `ui/` event becomes which verb commit) is not specified. Add the routing table.

**B16. [M] `assets/` skin swap is unspecified at the data level.**
§4.8 says "skin swap = discard+reload arena" but doesn't say whether swap is atomic (double-buffered) or whether a swap during a room transition is legal. State the rule.

**B17. [m] `§5.1.OV` dangling reference and `§5.1`/`§4.4` hash-period mismatch.**
Already in B1/B9. Also §4.4 says "rolling_hash updated every tick" while §5.1 says every 10 ticks — delete one.

**B18. [m] Citation hygiene.**
"[S29 Delay unit]" on the pie-menu hold (sim ticks are not `Delay` ticks); "[C26 not; §10 flavor]" in §4.10 A is gibberish; §6.1's `device_generated_commands` "(the dispatch's 'render passes' phrase …)" is noise. (h1 #25.)

---

## Part C — what's good (keep)

- C01–C28 constraints table with sources; the supersession list (Avail FLUSH, present modes, EClock, depth bias, r12 nuance) is the best part of the doc.
- One POD `World` serving snapshot/rewind/fork/save/hash as one system, with tools linking the real `sim/`.
- Lavapipe as shipping default, NVK self-test as machine-side enforcement of D7.
- Audio non-fatal, no flush/unload, every timing line carries a KVM/TCG stamp.
- Honest UNVERIFIED list; recording the VK-tree change as FAIL rather than hiding it.

---

## Part D — process notes

- h1's "verified by scan" (7 fields × 13 modules) checked marker presence, not content. h2 confirms the content gaps (B1–B6, B12, B15).
- The VK-status FAIL stands (concurrent stage-7 merge is real; confirm against `git -C VK log` for 434c5694/23aa69df).
- h2 did not modify `kitty_engine_design.md` or any source file; it wrote only this critique, per the dispatch authorization block (WRITE = this file; CODE none; VK read-only; no git writes; web read-only).

---

## Next step

One revision pass on findings B1–B6 (blocking) and B7–B16 (M), under the same authorization block, writing `kitty_engine_design.md` in place. Exit criteria (carried from h1, tightened):

1. Room grammar as EBNF, with an example that passes it (terminator, fixed width, no mixed comment markers).
2. One AHI API chosen and the other deleted; `SDL_QueueAudio` or the callback hand-off named.
3. CRT/allocator decided and written into §9 (no `new` on AROS + arena on `AllocVec` + global `operator new`, or the documented autoinit path).
4. A result-code table for `VkResult` (normal vs fatal), with `VK_SUBOPTIMAL_KHR`/`OUT_OF_DATE`/`TIMEOUT`/`NOT_READY` classified.
5. A `World` field list with a derived snapshot budget (keyframe count = 18 for 9 s at 25-tick period).
6. Per-module line budget against the 20k cap.
7. Every known-bad input able to produce the failure it names (H7/H8/H5 rewritten).
8. Type definitions (`platform.h`, `sim_types.h`, `render_types.h`) written once, before the module specs.
9. `LP_NUM_THREADS` mechanism specified (launcher `ENV:` or `S:User-Startup`).
10. `qdiv(a,0)` policy + `-fwrapv`/UBSan in CI.