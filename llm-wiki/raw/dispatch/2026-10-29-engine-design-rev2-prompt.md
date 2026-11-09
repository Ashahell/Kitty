# opencode prompt: engine design revision (rev 2)

> Source: Work/Kitty/Docs/Development/dispatch/engine_design_revision_opencode_prompt.md
> Collected: 2026-10-30
> Published: 2026-10-29

# opencode prompt — Kitty: Core Awakening, engine design **revision 2** (2026-10-29)

## Owner authorization (read first — opencode never edits this block)

    WRITE: Docs/Development/kitty_engine_design.md (edit in place)
           Docs/Development/kitty_engine_design_sources.md (edit in place)
    DO NOT EDIT: Docs/Development/kitty_engine_design_critique.md, kitty_core_awakening_design_v2.md,
                 kitty_core_awakening_review.md, anything under Docs/Development/dispatch/
    CODE: NONE — no source files, no build scripts, no prototypes, no test programs
    BUILDS / BOOTS / DEPLOYS: NONE
    VULKAN4AROS REPO: READ-ONLY (cat/grep/ls/git show/git log/git status only)
    GIT: no commit, no push, in any repository
    WEB: read-only research allowed; post nothing, sign up for nothing, download no binaries

Don't ask for, infer, widen or grant any authorization yourself. If the work seems to need
something outside this block, stop and list it in the final report.

## Goal

Revise `kitty_engine_design.md` so it closes critique findings **1–24**, plus the harness-2
additions listed under "Also required". After that, an implementer must be able to build M0–M1
**without making a single architecture decision**. This is a revision, not a rewrite. Keep
everything the critique's "What's good / keep" section names, especially C01–C28, the
supersession list, the risk register and the rejected alternatives. Change only what a finding
requires, or what a change forces.

## Inputs (read fully before editing)

- `~/Work/Kitty/Docs/Development/kitty_engine_design_critique.md`: the findings. Part A, rows
  1–24, is the work list. Parts B–D are context. Where a harness-2 verdict says "Refined" or
  "Contested", the resolution given below is binding.
- `~/Work/Kitty/Docs/Development/kitty_engine_design.md` and `kitty_engine_design_sources.md`: the files you edit.
- `~/Work/Kitty/Docs/Development/kitty_core_awakening_design_v2.md`: still authoritative. §4, §5,
  §8, §11, §13 and Appendix A are binding.
- `~/Work/Kitty/Docs/Development/dispatch/engine_design_opencode_prompt.md`: the original
  dispatch. Its required 15-section structure, evidence rules and VK-guard rules still apply.
- Vulkan4Aros (`~/Work/projects/Vulkan4Aros/`, READ-ONLY): the llm-wiki, plus **read-only**
  source where a finding needs a fact pinned:
  - the AHI device, in `src/abi/v11/AROS/workbench/devs/AHI/`;
  - cybergraphics `ReadPixelArray`;
  - the C library's `getenv`/`SetVar` behaviour;
  - the loader's surface extension name.
  Never run a write or destructive git command in `src/abi/v1/AROS*`. A guarded merge may be in flight there.
- **Fact correction (harness 2 got this backwards):** the build-pc AROS ships
  `libstdc`/`libstdcio`/`libposixc`. The *older* names `libcrt`/`libstdlib`/`libcrtprog` are what
  the cross-gcc `LIB_SPEC` expects (the `aros-development` skill's 2026-07-07 record). State the
  correct direction.

## Skills (load each, and say which ones you loaded)

| Skill | Use it for |
|---|---|
| `aros-development` | **Mandatory, first.** Every AROS fact you add (AHI, DOS `Rename`, CRT/startup, env vars, ABI gates) must agree with it. Where the skill and the wiki disagree, the newer dated wiki record wins: cite it and flag the conflict |
| `karpathy-guidelines` | Each decision is the simplest one that meets v2. No speculative options, and every decision gets a verifiable check |
| `karpathy-llm-wiki` | **Query mode only** over the Vulkan4Aros wiki. Don't ingest, lint-fix or edit it |
| `superpowers:writing-plans` (if present) | Structuring the revision changelog and the M0/M1 gate lists. Record any owner question in §15; never ask it |
| Web research | Primary sources only: the Khronos spec/registry (VUIDs, `VkResult` semantics, required structs), Mesa docs and source (`LP_NUM_THREADS`, the lavapipe/LLVM dependency), AmigaOS/AROS autodocs (`Rename`, AHI device vs library API, `ReadPixelArray`), and SDL2 docs (`SDL_QueueAudio`) |

## Work list: the decision each finding must end in

The defaults below are binding unless you find **cited evidence** that one is wrong. In that
case, choose differently, cite the evidence and flag it in the report. Never leave a finding as
"TBD" or as a choice for the implementer.

| # | Required outcome |
|---|---|
| 1 | Add v2's **NVK smoke run** to M0: the same scene, on the card lane, lane-gated. Its result is informational and never part of go/no-go. Relabel M0 honestly; don't call it "pure". |
| 2 | **One AHI API.** Default: the **device API** (`CMD_WRITE`, two requests chained with `ahir_Link`, filled and `SendIO`'d from the main loop). Delete every `AHIA_PlayerFreq`/`PlayerFunc`/"callback" reference. The measured xrun lessons [S33] were taken on the *library* API, so mark device-API xrun behaviour **UNVERIFIED** and add an M0 probe with a pass number. Linux: `SDL_QueueAudio` from the main thread, with no callback and no lock. §3 must stay true: "single-threaded end to end". |
| 3 | **CRT/allocator decided** in §9 and §4.1. Default: no STL containers and no exceptions anywhere in engine code. Fixed arrays plus per-room arenas. A global `operator new`/`delete` routed to the engine arena (`AllocVec` on AROS, `malloc` on Linux) exists only to catch strays: a link-time check fails the build if any engine object references it. Name the exact libraries linked under `-nostartfiles` + `__startup_entry`, each with the VK record that proves it links and runs ([S39][S40][S41][S52]). Spell out how logging formats text without libc stdio, or name the verified lib. |
| 4 | **Room grammar as EBNF** in §8.1, with these rules: `;` is the only comment marker; GRID is followed by exactly `h` rows of exactly `w` characters from a closed tile alphabet table, then `ENDGRID`; every keyword's fields are typed, with units. Define `PAR` fields with units and realistic values. Replace the example with a valid Chapter-1 room that the grammar accepts, and hand-check it (width, height, every referenced position in bounds). Add 5 negative examples, each with the exact Guru error line `kitty-cc` must print. |
| 5 | **Sim totality.** `qdiv(a,0)` returns sign(a)·`INT32_MAX` (and 0/0 → 0) and records `SIM_DIV0` in the tick result. Specify qmul's overflow policy (saturate or wrap, then record `SIM_OVF`), and make it identical in release and CI. Add `-fwrapv` to `sim/`, `snapshot/`, `level/` and `input/` on both platforms. CI adds a Linux UBSan job (`-fsanitize=undefined -fno-sanitize-recover`) over H1+H2. Fix the dangling `§5.1.OV` reference by creating that subsection. |
| 6 | **One flag file** for the determinism-critical modules, identical on both platforms (include `-fno-strict-aliasing -fwrapv -ffp-contract=off`, no `-ffast-math`). Pin the host compiler to the gcc-16 major. If that can't be pinned, H2+H7 hash equality is the stated proof, and a mismatch blocks M2. |
| 7 | **Pin stamp = Mesa commit + LLVM version + `LP_*` env + `AROS_PIN` hash.** Read [S50] and the lavapipe track and state, with a citation, which LLVM versions the bit-exact oracle match covered. If it held across LLVM 20 and 22, say so and keep both in the stamp. Update C18, C19, H4 and the risk register. |
| 8 | **NVK self-test** compares against a tolerance (per-channel ±ε plus a coverage-mask equality, with numbers) instead of a bit-exact golden. Detect NVK by `vendorID == 0x10DE` && `driverID == VK_DRIVER_ID_MESA_NVK`. |
| 9 | Remove `vkGetIcdWpaResult`. Presented-pixel proof on AROS: a `ReadPixelArray` readback from the window RastPort (verify it exists in AROS cybergraphics source, read-only, and cite file:line). On the lane, the QEMU HMP `screendump` is the external check. |
| 10 | **Palette:** vertex colours are authored as palette indices, so there is no runtime quantize. `kitty-pal` rejects off-palette art. Textures, if used, are pre-quantized at build time. Delete the per-fragment nearest-colour search. |
| 11 | **Complete extension list per platform**, at both instance and device level, including `VK_KHR_surface` and the AROS surface extension's exact name (cited from the loader/wiki). On Linux, take the list from `SDL_Vulkan_GetInstanceExtensions`. |
| 12 | **Frame sync:** 1 frame in flight; one acquire semaphore, one render-finished semaphore *per swapchain image*, and one fence. Delete the 3-slot uniform ring. Cite the VUID this avoids. |
| 13 | **`VkResult` table** (§6.2 or §4.5): every result the engine can receive from acquire, submit, present and create, each classified normal / recreate-swapchain / retry / fatal-fallback / fatal-exit. Define what "fallback" means once the device is already lavapipe (Guru screen + exit code, never a loop). |
| 14 | **No interpolation.** Render draws the latest fully-stepped `World`. At 25 fps the sim steps 2 ticks per frame. Add a max-catch-up rule (cap N ticks per frame, then drop wall-time debt, logged). Delete the "0..4 substeps" text. |
| 15 | **`InputLine` = inputs only.** Exact bit layout (direction 4 bits, fire 1 bit, keyboard enhancements if any, reserved bits zero and checked). Delete the on-node flag. Pie and verb commits stay derived from hold ticks. Make §4.7 and §5.1 agree byte-for-byte. |
| 16 | **One hash schedule:** FNV-1a over the canonical serialization at every keyframe and at end-of-replay. A host-only CI job may hash every tick. Delete the thinking-out-loud and every conflicting sentence. |
| 17 | **`World` field list** (name, type, count, bytes) covering entities, clones, windows, requesters, gates, mutable grid cells (a bitset or a sparse overlay; choose one and size it), the RNG and the tick counter. Derive the total. Keyframe count for a guaranteed 9 s rewind = ⌈450/25⌉+1 = **19**. Recompute every snapshot budget from that total. |
| 18 | **Audio buffer arithmetic shown.** Default: 2 × 960 frames (one AHI pass each, 20 ms) = 40 ms, at 48 kHz S16 stereo = 3,840 B per buffer. §4.6 and §7 must use the same numbers. M0 measures xruns. |
| 19 | **Two-slot save** (`progress.a`/`progress.b` + generation counter + checksum; load the newest valid slot). Cite the AmigaDOS `Rename` behaviour on an existing destination, from an autodoc or AROS source. |
| 20 | **`LP_NUM_THREADS`.** Default `0` (render in the caller's thread), subject to an M0 probe of `0` vs `1`. Specify the mechanism and verify it from AROS source (read-only): how Mesa's `getenv` resolves on AROS, whether a process-local `SetVar(GVF_LOCAL_ONLY)` before `vkCreateInstance` is visible to it, or else a launcher script. Don't pollute global `ENV:` silently. |
| 21 | **Rewrite the gates so each named known-bad input really produces its failure:** H7's known-bad = uninitialized padding reaching `serialize` (or a `long`-size-dependent field). Delete §4.5's (a) and (b); acquire/submit ordering goes to H5 with a named VUID. H5 uses the real env vars (`VK_INSTANCE_LAYERS` / `VK_LOADER_LAYERS_ENABLE`), and its known-bad must trigger a named VUID the layers actually report. Keep §4.1's forced-FIFO no-hang test (harness-2 contest accepted) **and** add a `WaitTOF` source lint plus an LVO/import scan. Add the CI state **BLOCKED** as distinct from PASS and FAIL; a BLOCKED gate never counts toward green. |
| 22 | **Types once, before the module specs:** `platform.h`, `sim_types.h` and `render_types.h` as code blocks defining every type the interfaces use (harness-2 B2 lists them). One `Plat` interface with two implementations; no `PlatAROS&` in a shared signature. Make all names consistent (`RenderCtx`, `Alloc`), and `verb_commit` returns a result code. |
| 23 | **Module specs (all 7 fields)** for `game/`, `settings/` (ENV/ENVARC), Shell fast-travel, the Guru error screen (one module shared by the runtime and the tools' output format), replay/demo mode and benchmark mode. Merge one into an existing module only when its spec says so explicitly. |
| 24 | **Line-budget column** per module. Engine total ≤ 20k (v2 §8.1); tools counted separately; the totals row shown. A module over budget names what it cuts. |

### Also required (harness-2 additions)

- **B7:** count pipelines (not shaders), and budget their uncached startup JIT time as an M0 gate.
- **B11:** tools count is seven.
- **B13:** a `kitty-lint` rule table (pattern, scope, rationale, must-fail fixture), and the `kitty-shaders` shader list as a table.
- **B14:** pin the RNG. Use `xorshift64*` with its exact constants, seeded from `SEED` via splitmix64, and draw in a fixed per-tick order (named in the tick-order list).
- **B15:** a `ui/` → verb routing table (requester `[Retry]`/`[Cancel]`, pie choices).
- **B16:** a skin-swap rule. Swaps are legal only at room boundaries and take effect atomically at the next room load.

### Cleanup (do while editing)

Critique findings 25–27 and B18:
- cut every citation that doesn't support its sentence;
- remove the duplicated §4.2 Test line;
- make the sources file and main doc agree on what CRC-32C covers, and justify the three hash choices in one line;
- remove the `device_generated_commands` "render passes" aside.

## Evidence rules (in addition to the original dispatch's)

- Every new platform fact cites a wiki path, an AROS source `file:line` (read-only), or a primary web source.
  Label anything you can't verify **UNVERIFIED:**, together with the M0 probe that settles it.
- Every number you add shows its arithmetic or cites a measurement.
- Don't re-litigate v2 §13 decisions D1–D7.

## Required changes to the document's structure

- Keep the 15 sections. Insert the type headers as §3.1. Add the new module specs inside §4.
- Add an **Appendix R — Revision 2 changelog**: one row per finding (1–24, B7/B11/B13–B16, cleanup), giving the section(s) changed and the one-line decision. A finding not in the table counts as not done.
- Add new sources to the sources file, numbered from `[S80]`. Don't renumber existing sources.

## Acceptance checklist (run it yourself; report each line PASS/FAIL)

1. Appendix R has a row for every item in the work list, and every row's decision is actually in the body.
2. The §8.1 example passes its own EBNF. Show the hand-check: the declared w×h against the actual rows and widths, and every position in bounds.
3. `grep` finds no `AHIA_PlayerFreq`, `PlayerFunc`, `vkGetIcdWpaResult`, `LVK_VALIDATE`, `substep`, "on-node flag" or "9.1 s" outside Appendix R and the sources file.
4. Every type named in a §4 interface is defined in §3.1 (list any that aren't, and the count must be 0).
5. Every CI gate names a known-bad input and the specific mechanism by which it fails. BLOCKED is defined and is never counted as PASS.
6. The line budgets sum to ≤ 20k for the engine; the totals row is shown.
7. The `World` field list sums to the total used in §4.4/§5.3, and the snapshot budgets are recomputed from it with 19 keyframes.
8. Nothing contradicts v2 §4/§5/§8/§11/§13/Appendix A, M0 included (the NVK smoke run is present).
9. Every `[S#]` and every `VK/`/`KD/` path resolves (`test -e`); new AROS source citations give `file:line`.
10. `git -C ~/Work/projects/Vulkan4Aros status --short`: record it at start and at end. If it differs, say whether this session caused it. Read commands only.
11. The critique, v2, the review and the dispatch files are unchanged (compare `sha256sum` at start and at end).

## Final report (reply with this only, no narration)

1. Files changed, with line counts before → after.
2. Skills loaded, and any that couldn't be loaded.
3. The acceptance checklist, each line PASS/FAIL, with one line of evidence each.
4. Defaults you overrode: the finding, the new decision and the citation.
5. New UNVERIFIED items, each with its M0 probe.
6. Anything needed outside the authorization block.
