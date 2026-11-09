# opencode prompt: engine design (rev 1)

> Source: Work/Kitty/Docs/Development/dispatch/engine_design_opencode_prompt.md
> Collected: 2026-10-30
> Published: 2026-10-28

# opencode prompt — Kitty: Core Awakening, game-engine design document (2026-10-28)

## Owner authorization (read first — opencode never edits this block)

    WRITE: Docs/Development/kitty_engine_design.md (new) + Docs/Development/kitty_engine_design_sources.md (new)
    CODE: NONE — no source files, no build scripts, no prototypes
    BUILDS / BOOTS / DEPLOYS: NONE
    VULKAN4AROS REPO: READ-ONLY
    GIT: no commit, no push, in any repository
    WEB: read-only research allowed; post nothing, sign up for nothing, download no binaries

Don't ask for, infer, widen or grant any authorization yourself. If the work seems to need
something outside this block, stop and list it in the final report.

## Goal

Write the **engine design document** for *Kitty: Core Awakening*. It must be detailed
enough that a later session can turn it into implementation milestones without making
architecture decisions of its own. It designs the engine. It doesn't design the game,
which is already fixed by the inputs below.

## Inputs (read all of these fully before writing anything)

**Game design (authoritative):**
- `~/Work/Kitty/Docs/Development/kitty_core_awakening_design_v2.md`. The engine must
  serve **this** document. §4 presentation, §5 mechanics and interaction rules, §8 engine
  & stack, §11 milestones, §13 open decisions and Appendix A are binding constraints.
- `~/Work/Kitty/Docs/Development/kitty_core_awakening_review.md`. It explains *why* v2
  says what it says. Don't reintroduce anything the review rejected unless you give
  evidence the rejection was wrong.
- `~/Work/Kitty/Docs/Development/kitty_core_awakening_design.md` (v1) is context only.
  It's superseded.

**Platform knowledge (Vulkan4Aros, READ-ONLY):** `~/Work/projects/Vulkan4Aros/`
- Start at `llm-wiki/index.md`, `llm-wiki/project-status.md` and `AGENTS.md`.
- Must-read pages:
  - `llm-wiki/lavapipe-track.md`: the baseline tier, its state and open walls.
  - `llm-wiki/nvk-card-lane.md` and `llm-wiki/aros-nouveau-drm-stack.md`: the enhanced tier.
  - `llm-wiki/raw/articles/2026-10-28-nouveau-ours-vs-upstream-and-the-fork-tax.md`: why
    NVK is lane-only and what gates it.
  - `llm-wiki/loader-icd-pattern.md`, `llm-wiki/phase-4-wsi-integration.md` and
    `llm-wiki/aros-writepixelarray-blit.md`: loader, WSI and present path.
  - `llm-wiki/aros-v1-elf-loader-large-modules.md`, `llm-wiki/aros-resident-library-reload.md`
    and the l0025 Expunge/`LDFlush` records: library residency and the force-expunge crash.
  - `llm-wiki/advertised-capability-contract.md` and `llm-wiki/advertised-vs-implemented-inventory.md`.
  - `llm-wiki/software-rasterizer-performance.md` and any lavapipe performance measurements.
  - `llm-wiki/deqp-session-abort-classes.md`, `llm-wiki/probe-evidence-discipline.md`, `llm-wiki/audit-gate-design.md`.
- Search further with `grep -ril` across `llm-wiki/`. Use these terms: pthread, SMP,
  condvar, timer.device, WaitTOF, AHI, lowlevel, joystick, IDCMP, `vkPollArosEventsKHR`,
  `vkCreateArosSurfaceKHR`, swapchain, present mode, stdio, constructors, exceptions,
  libstdc++, `-ffixed-r12`, memory, MEMF, Avail FLUSH.
- **Never** run a write or destructive git command in `src/abi/v1/AROS*` (no checkout,
  reset, clean, restore or stash). A merge is in flight there and the tree is guarded.
  Read files with `cat`/`grep`/`git show`/`git log` only.

## Skills (load each one before the work it covers, and say in the report which you loaded)

Load each skill with the skill tool. If the tool can't load one, read its `SKILL.md`
directly from `~/.claude/skills/<name>/` or `~/.agents/skills/<name>/`.

| Skill | Use it for |
|---|---|
| `aros-development` | **Mandatory, first.** AROS ABI and toolchain facts: mandatory compiler flags, library-base calling convention, no fork/mmap, vfork, stack limits, startup/constructor pitfalls, stdio-in-libraries, `bug()`/`RawPutChar`, never `Avail FLUSH`, reboot after library rebuilds, the Vulkan struct strictness rules. Every AROS claim in the document must be consistent with it. Where the skill and the wiki disagree, **the wiki's newer dated record wins**: cite it and flag the conflict |
| `karpathy-guidelines` | Design discipline: the simplest design that meets v2, no speculative generality, verifiable success criteria for every component |
| `karpathy-llm-wiki` | **Query mode only.** Answer platform questions from the Vulkan4Aros wiki with citations. Don't ingest, lint-fix or edit that wiki |
| `superpowers:brainstorming`, `superpowers:writing-plans` (if the superpowers plugin is present) | Structuring alternatives and the final document. Run brainstorming **without** asking the owner questions: record each question you would have asked in §Open questions instead |
| Web research (fetch/search tools) | External facts the wiki doesn't hold (see the research list). Prefer primary sources: Khronos spec and registry, Mesa docs and source, AmigaOS/AROS autodocs, format specs, library licences |

## Research list (answer each, or mark it UNRESOLVED with what you tried)

1. **Vulkan feature floor for both tiers.** Pick the exact Vulkan version, features and
   extensions the engine may use: dynamic rendering vs render passes, synchronization2,
   timeline semaphores, push descriptors, descriptor indexing. Every choice must be
   supported by **both** Mesa 26.2.2 lavapipe and NVK on Turing+. Sources: Mesa docs and
   `features.txt`, the gpuinfo.org database, the wiki's measured capability probes on AROS.
   Where AROS lavapipe measures differently from Linux lavapipe (for example zero depth
   bias), the AROS measurement wins.
2. **Lavapipe performance model.** Thread count control (`LP_NUM_THREADS`), how
   tiling and binning scale with resolution and overdraw, the costs of fragment shading,
   texturing and blending, and pipeline-compile (JIT) cost and caching. That last one
   includes the wiki's decision that the Mesa shader cache stays off on AROS
   (`decisions/2026-10-14-mesa-shader-cache-stays-off-until-writes-have-a-destination.md`),
   so plan for compiling every pipeline at load time and say what that costs.
3. **NVK on AROS specifics.** Memory types and heaps, present modes, GSP behaviour at
   shutdown, and what the lane says must be avoided.
4. **Present path on AROS.** Swapchain, present modes, Intuition window ownership through
   the loader, input via `vkPollArosEventsKHR`, and resize and screen-mode behaviour.
   Explain how one window serves both rendering and IDCMP input.
5. **Timing.** `timer.device` units (MICROHZ/ECLOCK) for a 50 Hz fixed step with render
   interpolation, why `WaitTOF()` is banned, and pacing under QEMU.
6. **Audio.** AHI API for low-latency streaming of a software mixer, plus the SDL2 audio
   alternative on Linux. ProTracker MOD format (primary spec), with per-channel muting,
   tempo/BPM effects and `Fxx`, and how to make a replayer deterministic. Licences of the
   candidate reference replayers, if any are studied.
7. **Input.** `lowlevel.library` `ReadJoyPort` vs keyboard IDCMP, mapping one-button
   joysticks plus pads, and the v2 §5.2 pie menu.
8. **Deterministic simulation.** Q16.16 fixed-point patterns: multiply/divide rounding,
   overflow policy, and trig via tables. Snapshot ring buffers and input logs (rollback
   literature such as GGPO-style designs), state hashing, and avoiding floats and libm in
   `sim/`. How to prove identical state hashes on gcc/glibc and on the AROS gcc-16 newlib-based crt.
9. **C++20 subset on AROS gcc-16.** Confirm from the wiki and skill whether global
   constructors and destructors run in **executables** (not libraries), and the status of
   exceptions, RTTI, `thread_local`, `std::atomic`, pthreads and libstdc++ containers.
   Then decide what the engine may use. Put the measured facts first and mark anything
   unmeasured.
10. **Isometric rendering and puzzle readability.** Dimetric projection, depth sorting vs
    a depth buffer, ordered-dither transparency, and palette quantization per chapter
    (v2 §3.2).
11. **Level/data formats.** A text-first, schema-validated room format the puzzle solver,
    the validator and the Guru-Meditation error reporter can all read. Look at prior art
    in puzzle-game formats (Sokoban `.xsb`, PuzzleScript), JSON vs a custom text format,
    and the line-number reporting requirement.
12. **Puzzle solver.** A state-space search over the deterministic sim at grid
    abstraction (BFS/A*/IDA*), state-explosion control with clones in play, and how to
    prove "no shortcut" (v2 §8.4 item 3).
13. **Localization.** AROS `locale.library` catalog formats (`.cd`/`.ct`/`.catalog`) and
    how the Linux build reads the same source strings.

## Required document structure (`Docs/Development/kitty_engine_design.md`)

1. **Scope and non-goals.** What the engine does for v2 1.0 only, and what it explicitly won't do.
2. **Constraints table.** Every binding constraint from v2 and the platform, each with its source (v2 section, wiki path or URL).
3. **Architecture overview.** Modules, dependency direction (an ASCII diagram is fine), and threading model (one-core-correct; any extra threads must be optional).
4. **Module specs.** For each of `platform/{aros,linux}`, `sim/`, `snapshot/`, `render/`, `audio/` (with `modplay`), `input/`, `assets/` and `level/`, `ui/` (pie menu, requesters), `locale/`, `save/`, `tools/` (level compiler, solver, validator, palette quantizer, state-hash checker), give:
   - responsibility
   - public interface (C++ declarations or signatures)
   - data structures and memory ownership
   - threading
   - error handling (no exceptions)
   - the AROS vs Linux difference
   - test strategy
5. **Simulation and determinism spec.** Fixed-point type, tick order, RNG, the input-log format, snapshot contents and size budget, and the state-hash definition. Include the v2 §5.3 interaction matrix mapped to concrete sim rules.
6. **Renderer spec.** Vulkan feature floor, frame graph (acquire → record → submit → present), pipelines (≤ 6 shaders), resource lifetimes, tier selection and the startup self-test, the dither and palette passes, the golden-image hooks, and the library-residency rule (open once, never expunge).
7. **Audio spec.** Mixer, MOD replayer, channel-mute API for dynamic music, buffering and latency on AHI vs SDL2.
8. **Data formats.** Room format with a full example, manifest, save tree layout, replay file format, catalog strings. Give versioning rules for each.
9. **Build and toolchain.** Linux and AROS cross builds, the mandatory flag set (v2 Appendix A plus the skill), the lint rules for the C++ subset, and the gate commands.
10. **Verification and CI.** Map every item of v2 §8.4 to a concrete harness, its input and its pass criterion, with the known-bad input each gate must fail on ("a check that cannot fail is not a check").
11. **Performance budgets.** Per-tier frame, memory and CPU breakdown, set against v2 §4.1. Say what M0 must measure to confirm each number.
12. **Milestone mapping.** Which modules and which gates land in v2's M0–M7, so that M0 stays a pure feasibility spike.
13. **Risks.** Platform risks the game doesn't control (force-expunge crash, NVK lane-only, SMP experimental, Mesa pin drift), each with likelihood, impact, detection and fallback.
14. **Rejected alternatives.** At least: an existing engine port, Vulkan-Hpp/volk, float simulation, a free camera, SDL2 on AROS for windowing, and native-code mods. Give the reason for each, with sources.
15. **Open questions for the owner.** Numbered, each with a recommended default. Don't re-open v2 §13 decisions; reference them.

`Docs/Development/kitty_engine_design_sources.md` is a numbered source list (wiki paths
with dates, URLs with access date). The main document cites it as `[S12]` and so on.

## Evidence rules

- Every platform or third-party claim carries a citation. A claim you couldn't verify is
  written **UNVERIFIED:**, with the check that would settle it.
- Measured facts beat documented ones, and newer dated wiki records beat older ones and
  the skill. When sources conflict, show both and say which you chose and why.
- Don't copy copyrighted text beyond short quotes. Summarize specs in your own words.
- Don't invent numbers. Any budget not derived from a cited measurement is labelled
  "provisional — set by M0".
- Write plainly: tables over prose, no filler.

## Acceptance checklist (run it yourself before reporting; report each line PASS/FAIL)

- [ ] All 15 sections are present. Each module in §4 has all 7 fields.
- [ ] Each of research items 1–13 is answered with a citation, or marked UNRESOLVED.
- [ ] Nothing contradicts v2 §4, §5, §8, §11, §13 or Appendix A. If something must, it is listed in §Open questions with evidence.
- [ ] Nothing requires NVK, networking, `Avail FLUSH`, `WaitTOF()`, exceptions, or floats in `sim/`.
- [ ] Every CI gate in §10 names its known-bad input.
- [ ] Every link and path in both files resolves (check paths with `test -e`).
- [ ] `git -C ~/Work/projects/Vulkan4Aros status --short` matches what it showed before you started. Run it at start and at end and diff the two.

## Final report (reply with this only, no narration)

1. The files written, with line counts.
2. Skills loaded (names).
3. The acceptance checklist with PASS/FAIL per line.
4. UNRESOLVED and UNVERIFIED items, one line each.
5. Any conflict found between the wiki, the skill and v2.
6. Anything the work needed that falls outside the authorization block.
