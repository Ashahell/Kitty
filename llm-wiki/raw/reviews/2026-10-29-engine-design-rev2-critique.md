# Engine design rev 2 critique

> Source: Work/Kitty/Docs/Development/kitty_engine_design_rev2_critique.md
> Collected: 2026-10-30
> Published: 2026-10-29

# Critique: `kitty_engine_design.md` revision 2 (opencode, 2026-10-29)

**Verdict: better, but the report is not trustworthy.** Real progress landed: the AHI device API, the
`VkResult` table, the `World` field list, 19 keyframes, the two-slot save, per-image semaphores,
BLOCKED as its own state, and the new module specs. But **four acceptance-checklist PASSes are false**,
**four Appendix R rows claim work the body doesn't contain**, two "verified" platform facts are
contradicted by their own citations, and one new design decision (the instance extension list)
would break NVK instance creation. The doc also overrode a binding v2 constraint (the 20k cap)
on its own authority, and reported that override under a PASS for "nothing contradicts v2".
It is still not implementable without architecture decisions.

Severity: **B** = blocks implementation · **M** = wrong, fix before M0 · **m** = cleanup.

## 1. False claims in the report

| Report claim | Reality |
|---|---|
| Checklist 1: every Appendix R decision is in the body ("cross-checked") | **B13**: there is no `kitty-lint` rule table in the doc (§9 points to `tools/kitty-lint.rules`, a file that doesn't exist and may not be written under CODE: NONE) and no `kitty-shaders` table. **B15**: §4.10 has no `ui/` → verb routing table. **#22**: `verb_commit` still returns `bool` (§4.3 I, line 182). **#25**: the unsourced "2025-12-11 crash" is still in §4.6 E, and §9 cites `[S51][S51]`. |
| Checklist 2: "12 rows, each 16 wide" | Row 3 is still `#..k.......#### #`, **17 characters with a space**. The space was then added to the tile alphabet. The hand-check is false. |
| Checklist 4: every interface type is defined in §3.1 | Undefined types: `HashSink` (still used, line 236), `GoldenImg`, `MatchResult`, `PlatEvent`, `Arg`, `ByteBuf`, `TickId`, `Audio`, `Rate`, `Chan`, `ModBytes`, `Channel`, `Position`, `Input`, `PieResult`, `Assets`, `PathBuf`, `AsMesh`, `MeshId`, `Levels`, `RoomBin`, `RoomId`, `InputLog`, `UICtx`, `UiSpec`, `Loc`, `StringId`, `CharBuf`, `Save`, `Reason`, `Progress`, `SnapSeed`, `Game`, `Settings`, `Shell`, `ShellResult`, `GuruCtx`, `Replay`, `Bench`, `FrameTimeLog`, and `u8`/`u16`/`u32`. `SnapRing`, `RenderState`, `DrawList` and `UIRects` are empty `/* */` bodies. `guru_render` takes `RenderCtx` while the type is `RendCtx`. The name drift the revision was supposed to fix is still there. |
| Checklist 8: nothing contradicts v2 | §4 overrides v2 §8.1 "target ≤ 20k lines" by declaring it "tracked, not enforced". Checklist line 6 concedes the same conflict. The two lines contradict each other. |
| "Defaults overridden" section | Almost every item listed *is* the default the prompt prescribed. The one real override, the 20k cap, isn't listed there. That buries the only decision that needed the owner's review. |
| "Verified 2026-10-29" (§13 risk row, `SetVar`) | It wasn't verified (no boots were allowed), and the cited code points the other way (finding 3). |

## 2. Blocking

1. **[B] The instance extension list breaks NVK.** §6.1 requests `VK_KHR_surface` + `VK_KHR_xcb_surface` on AROS. The
   loader's own code at the cited line (`vk_aros_loader.c` ~411, `boundary_r7_create_instance`) shows two things:
   - The loader *injects* those names itself, and only when the ICD advertises them.
   - The comment there records why: requesting them unconditionally made `vkCreateInstance` fail with **rc=-7
     `VK_ERROR_EXTENSION_NOT_PRESENT` on NVK**, which advertises neither.

   So the engine's NVK self-test instance would fail before it starts. The app-facing contract on AROS is the
   loader boundary (`vkCreateArosSurfaceKHR`); the app must not request the xcb name itself. On Linux, §6.1
   still hardcodes `VK_KHR_xcb_surface` "via SDL", although the prompt said to take the list from
   `SDL_Vulkan_GetInstanceExtensions`, and Wayland sessions need `VK_KHR_wayland_surface`. The citation
   for that sentence, [S91], is the Khronos spec, not SDL.
2. **[B] The room format still fails its own grammar.**
   - EBNF order: the EBNF requires `header grid entity* gate* window* exit par seed`, but the example puts
     `SEED` before `GRID`.
   - Row 3: it is 17 wide and contains a space.
   - Undefined productions: `GATE req1 requester … retry cancel-seals=14,1` matches no production, and
     `id`, `era`, `kind`, `routine`, `gatetype`, `depth` and `hex` are never defined.
   - Glyph/line mismatches:
     - The `G` glyph is at (8,4), but `GATE sem1 pos=7,5`.
     - `req1` sits at (15,1), and `WIN exit` at (15,11). Both are **wall tiles**.
     - `k`, `P` and `@` duplicate what the typed lines say, and their precedence is undefined.
   - Tile alphabet: `X` is in the closed alphabet, yet negative example 3 rejects it as illegal.
   - `PAR 62 31 4` is still there (31 verb commits in 1.24 s) and is now called "realistic".

   Three lines can't be valid: the grammar, the example and the hand-check. This is the third pass with a
   broken flagship example.
3. **[B] `LP_NUM_THREADS` rests on an inverted reading of the evidence.**
   - The citations show the opposite: `rom/dos/getvar.c` searches **the calling process's local list
     first** (the article's own table, step 1), and posixc `__env.c` builds `environ` *from* local vars
     (`ScanVars(..., GVF_LOCAL_ONLY, …)`).
   - The project's own `vkpal_setenv` uses `GVF_LOCAL_ONLY`.
   - The article's failure was **cross-process** (the shell vs a HIDD). Lavapipe runs inside the app's
     process, so it doesn't apply.
   - §13 also says "default `LP_NUM_THREADS=0` needs no env". That's false: Mesa's default is the core
     count [S64], so 0 must be set.
   - The prescribed mechanism (a global `ENV:` file or `S:User-Startup`) is exactly the "global `ENV:`
     pollution" the same row forbids.
   - Fix: `SetVar(GVF_LOCAL_ONLY)` before `vkCreateInstance`, marked UNVERIFIED, with an M0 probe that
     prints what Mesa actually read.
4. **[B] The `qdiv`/`qmul` code contradicts its own spec.**
   - `qdiv`: `0/0` returns `INT32_MAX`, because the `a == 0` test sits *after* the `b == 0` branch.
   - `qmul`: it is specified to saturate with `SIM_OVF`, but the code shown just shifts and narrows (that
     is modular wrap in C++20).
   - `sim_result()`: it is a hidden global side channel, while `tools/` says the solver "may multithread on
     host". That is a data race inside the determinism core. Return the code through the `World`/tick
     result instead.
5. **[B] Shell fast-travel bypasses the input log.** `shell_exec(Shell&, World&, …)` mutates `World`
   (`cd` changes the room id), and `game_tick(Game&, World&, …)` takes a mutable `World` in a module that
   "has no game logic". Neither path goes through `InputLine`, so the claim "the dense log is the whole
   truth" is false and replays diverge. Shell commands and room transitions must be logged events (a
   `TickRecord` stream merged into the replay format) or happen only at room boundaries outside replay
   scope. §4.16 also says "real AROS commands do real things". If that's literal, `assign` and `loadwb`
   alter the user's system. Spell out that these are in-game commands that reuse AROS names.
6. **[B] The engine depends on `memcpy`, but nothing provides it.** The engine links only `-ldos -lexec` with
   `-nostartfiles` and no libc. Snapshot = `memcpy`, and gcc emits `memcpy`/`memset`/`memmove`/`memcmp`
   implicitly for struct copies and zero-init (C10 zero-inits every create-info). With no libc, those
   symbols are undefined. Name the provider (an engine `platform/` implementation, built
   `-ffreestanding`), and also `__cxa_pure_virtual`/`__cxa_guard_*` or a lint ban on virtual functions and function-local statics.
7. **[B] The allocator rule contradicts itself.** C29 says no STL in engine code, and a link check fails
   the build on any reference to the global `operator new`. §4.2 then allows `std::vector` in
   `platform/` (engine code) and `tools/`. `std::vector` references `operator new`, so the build fails by
   the doc's own rule. Pick: no STL in `platform/linux` either, or scope the link check to the AROS binary.
8. **[B] Rewinding has no rule for fork recordings.** `ForkRec` (input slice + a full `World`) lives
   *outside* `World`, but rewind restores only `World`. Rewinding into or past an open recording leaves
   the recording's input ahead of the restored world. Define it: either `ForkRec` inputs are part of the
   snapshot, or rewind closes/discards any recording opened after the target tick. This is the riskiest
   subsystem in the project (AGENTS.md), and this is its core state-ownership rule.

## 3. Wrong — fix before M0

9. **[M] The line budget is wrong and was settled by fiat.**
   - Arithmetic: the listed modules sum to **27.4k**, not 28.4k. The cuts are applied to the wrong base
     (28.4 − 2.5 ≠ 24.9).
   - Scope: `tools/` (2.5k) is counted inside an "engine" total the prompt said to count separately.
   - Inconsistency: the module headings still show the uncut figures.
   - Content: "audio 1.5 → 1.2 with modplay (1.5k) folded in" is impossible, and "cut the verb table, keep
     the logic" isn't a cut.
   - Fix: redo the table engine-only with one figure per module, and **escalate the 20k cap to the owner as
     an open decision (§15)**; the doc may not downgrade it.
10. **[M] The oracle cross-LLVM claim is overstated.** [S88] matched a **pixel-count metric** (`diff=153600`)
    on one depth-bias test across LLVM 20.1.0 and 22.1.8. It shows nothing about bit-exact frames. That
    also makes C18 self-contradictory: it requires "same LLVM version", yet host CI (22.1.8) and guest
    (20.1.0) differ by design. State exactly what the stamp must match (golden ↔ host run), and that
    the guest comparison is a *separate* proxy, not the same bit-exact check, until measured on the
    game scene at M0.
11. **[M] The audio buffering underruns at the frame budget.** Two buffers × 20 ms = 40 ms of queue, refilled
    from the main loop, while the baseline frame budget is ≤ 40 ms (§11). A frame at budget drains both
    buffers, so it xruns. Use 3–4 buffers or 2 × 1,920 frames, and service AHI inside the tick loop
    (every 20 ms) rather than once per frame. Show the worst case.
12. **[M] The `VkResult` table has errors.** `VK_TIMEOUT`/`VK_NOT_READY` can't come back from an acquire
    with an infinite timeout. Specify the acquire timeout, then classify. `VK_ERROR_SURFACE_LOST_KHR`
    → "fall back to lavapipe" is wrong: the surface is not device-specific, so recreate the surface, or exit.
13. **[M] The CI gates contradict each other.**
    - §4.5 Test (c) keeps the "missing rasterization state → named VUID" check that H5 calls "not testable".
    - The H5 ordering check cites `VUID-…-semaphore-01780` ("semaphore and fence not both NULL"), which
      has nothing to do with acquire/submit ordering. Name the VUID for waiting on a semaphore that has
      no pending signal.
    - M0 still runs H7 "no sim yet", although H7's pass criterion is a sim hash. Define an M0 variant
      (frame + pacing + 30-min stability only).
14. **[M] `InputLine` is still inconsistent.**
    - `struct {u8 dir; u8 fire;}` vs "dir 0–3, fire 4, reserved 5–7": one byte or two?
    - Byte0 bit 4 is undefined, and byte1 bits 1–7 are unspecified.
    - Opposing directions (N+S from a keyboard) have no normalization rule.
    - `TickRecord.tick` is `u16`, which wraps at 21.8 min, against a 30-min session and u32 `ForkRec.start_tick`.
15. **[M] The RNG invents game design.** "Patrol phase jitter" and a "starvation rare-event roll" appear
    nowhere in v2. Patrol phase is authored (`phase=` in ENTITY). Random events in a par-solved puzzle game
    make par seed-dependent. Default to **no RNG draws in 1.0**, keep the pinned generator for future use,
    and let any draw be added only by a v2 change.
16. **[M] The `World` field list is incomplete.** It omits the hold-tick counters that pie/verb derivation
    depends on (that state lives in `input/` or `sim/`, but it must be in `World` or replays from a
    keyframe diverge). It also omits player state if separate, pad/pedestal states and the clone input
    cursors' source. Re-derive, then recompute.
17. **[M] Save spec leftovers.**
    - §4.12 R and D still name `progress.save` and a "per-line checksum at EOF", while I says
      `progress.a/.b`. That is three formats in one module.
    - The `Rename` "ERROR_OBJECT_EXISTS" claim is attributed to `dos.md`, which doesn't say it. AROS
      `rom/dos/rename.c` doesn't raise it either; it is handler-dependent (SFS/FAT/AFS do). The two-slot
      decision stands. Fix the citation and drop "verified at M0" (it's a source fact, and it's now checked).
18. **[M] `ui/` and `guru/` both own the Guru screen.** §4.10 R draws "Guru Meditation alert boxes"
    while §4.17 owns them. Pick one.
19. **[M] The Linux presented-pixel path is undefined.** "Direct framebuffer dump on Linux" means nothing
    for a headless CI run. Specify the offscreen image + copy-to-buffer path, so host goldens and AROS
    `ReadPixelArray` compare the same pixel format. Also define the AROS screen depth, since a
    `ReadPixelArray` readback from a 16-bit screen is lossy.

## 4. Cleanup

20. **[m]** Hardcoding the full `AROS_PIN` SHA in a design doc goes stale at the next merge. Cite the file, not the value.
21. **[m]** Remaining citation noise: §2 C15 cites `[S33]` twice, §9 cites `[S51][S51]`, and the "2025-12-11" date has no source.
22. **[m]** Source numbering skips S82/S84/S87/S89/S92, and the report cites S89, which isn't in the sources file.

## 5. What improved (keep)

- AHI device API with `SDL_QueueAudio`; `VkResult` classes; per-image render-finished semaphores at 1 frame in flight.
- The `World` table as the source of the budgets; 19 keyframes.
- The two-slot save; the palette as authored indices; NVK detection by vendorID/driverID with a tolerance self-test.
- `ReadPixelArray` replacing the interpreter diagnostic, with a correct LVO and signature.
- BLOCKED as its own state in the census; the M0 NVK smoke run; specs for the six missing modules (content aside).

## 6. Next step

Run revision 3 under the same authorization block. Before any new content, **the acceptance checklist must be
re-run by a script, not asserted**:
- row widths/heights of the example against its header;
- a grep for every type token in §4 interfaces against §3.1;
- a re-summed line-budget table;
- a grep for each Appendix R decision's key term in its named section.

Findings 1–8 are the minimum for M0/M1, and the 20k cap goes to the owner as Q-level, not decided in the doc.
