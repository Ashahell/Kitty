# M0a.1 (M0a fix round) — critique of the opencode report (2026-11-01)

> Source: Work/Kitty/Docs/Development/m0a1_linux_critique.md
> Collected: 2026-11-01
> Published: 2026-11-01

# M0a.1 (M0a fix round) — critique of the opencode report (2026-11-01)

Verdict: **the fix round worked. Every M0a defect named in the M0a critique is fixed, the frames and the music are byte-identical to M0a, and the gate census matches the report.** Three code defects remain, all small, and one of them makes the logs state a false location. There was one write into a do-not-edit path. My own S12 script could not see it, and that is a defect in my prompt. The report is mostly accurate, but it misstates two things.

## Re-run (independent)

I ran scripts X, 1–11, 13 and 12 on a scratch copy of `~/Work/Kitty/repo`, building from clean (`build/linux*`, `tests*`, `tools`, `spv` deleted first):

- **Exit codes:** every script ended rc=0.
- **Census:** 57 PASS, 1 FAIL, 1 BLOCKED.
  - The FAIL is S5.build, and I caused it: I skipped script 0, so the scratch copy has no `baseline.done`. All 10 binaries were rebuilt fresh.
  - The BLOCKED is S7.golden: the pinned lavapipe still isn't loadable.
  - That matches the report's 58/0/1.
- **Frames and music:** headless frames 0/150/299 came out byte-identical to `build/m0a/r1`, and `hash10s=e663d2da535e69a5` was reproduced.
- **Probe:** `coverage_px=1192`, 9 colours, plant `mismatched_px=128`. Auto mode gives `candidate=none reason="no NVK device"`, and release `--tier-probe` gives rc=2.
- **Features and swapchain:** `features10_true=0 features13_true=1` on every log. The swapchain logs `swapchain_min=3 swapchain_max=0 swapchain_request=3 images=3`.
- **Watchdog:** a hang with no validation layer now exits rc=4 with `validation_messages=0`.
- **Probe draws:** I checked `probe_ref` box by box, and every one of the five draws is present (covered pixels: flat 360, nocull 160, tex 196, dither 158, ui 256).
- **`rt_fmt` fuzz:** 200,000 random integer cases came out byte-identical to `snprintf`. For floats it differs from glibc in 7,225 of 200,000 cases, which is by design: D14 rounds the double product half away from zero.
- **Git:** HEAD `ec7d4c3`, one branch, no stash, no tags, nothing committed.
- **Repo changes:** 4 new files and 16 changed files against the reviewed M0a tree. `build/m0a` (44 files) is unchanged.
- **Vulkan4Aros:** protected paths unchanged, 0 status lines.

`audit.sh` exits rc=1, because six files fail the protected-files check: `AGENTS.md`, `wiki/index.md`, `wiki/log.md`, `renderer.md`, `gdd-v2.md`, `open-questions.md`. **I made all six edits** (art bible v2, GDD v3, the CopperForge rename). The audit's baseline was taken at 06:32, before those edits, so they are not opencode's. The run's own baseline (12:06) includes them, so S12's PASS on that point is correct.

## Code defects

1. **`pl_trap` prints a false location.** `src/platform/linux/plat.cpp:341` hardcodes `"Software Failure. src/platform/rt/traps.cpp:1: %s"`.
   - Every trap prints that line: all six operators in `traps.cpp:4–9`, and both `rv_die` fallbacks (`render_init.cpp:16`, `:21`).
   - The root cause is my D13: the spec has `<file>:<line>` in the output, but the signature has no file or line argument.
   - Fix: `[[noreturn]] void pl_trap(const char* file, u32 line, const char* msg)`, plus a `PL_TRAP(msg)` macro that passes `__FILE__` and `__LINE__`.
   - The phase-2 report disclosed this as deviation 1. The final report dropped it and wrote the opposite in item 3.
2. **Probe devices silently skip D16.** `render_init.cpp:976–977` sets demote only `if (f13sup.shaderDemoteToHelperInvocation)`, and otherwise creates the device anyway.
   - `P_dither`'s `discard` lowers to `OpDemoteToHelperInvocation`. On a candidate that doesn't support the feature, the probe would then run invalid SPIR-V.
   - Report item 3 says D16's "same path [is] used by probe devices". It isn't.
   - Fix: when the feature is unsupported, set `res->ok=false` with `reason="shaderDemoteToHelperInvocation unsupported"`. Exit 4 is the wrong fix: a failed probe has to fall back to lavapipe (GDD §8.3). That makes D16's "exit 4" wording wrong for probes, which is also mine.
3. **`rt_fmt` misbehaves outside D14's input range.**
   - Fed NaN, `%.3f` prints `9223372036854775.808`: the cast from float to `u64` at `rt_fmt.cpp:75` is undefined behaviour.
   - Fed INFINITY, `%.0f` prints `0`.
   - A width over 32 is clamped to 32 (`%33u` prints 32 columns), when D14 says to copy any other `%` sequence verbatim.
   - Fix: when `!(av < 1e12)`, print `nan`/`inf`, or copy the sequence verbatim. When the width is over 32, copy the sequence verbatim.

## Notes for M0b (not defects now)

- **The debug messenger calls `pl_log` (`render_init.cpp:61`), possibly on a driver thread.** Linux stdio locks around each write, so that is safe today. On AROS, M0b must make `pl_log` safe across tasks, or have the callback only count and defer the text.
- **The watchdog reads the counter through `volatile`, with no atomic load.** That works on x86-64; use `__atomic_load_n` in M0b.
- **`guru_report` with a null `Plat` now logs nothing (`guru.cpp:28`).** Production always binds the Plat (`main.cpp:211`). The phase-2 report disclosed this.

## Authorization

- **The phase-2 session wrote `~/Work/Kitty/Docs/Development/m0a1_linux_phase2_report.md`** (8,373 B, 2026-10-31 16:41). `Docs/` is "DO NOT EDIT".
  - The final report does list it, but only as "awaits an owner's keep-or-purge decision", not as a breach.
  - The file is byte-identical to `~/Work/lanes/kitty-m0a1-fix-20261031/p2-report-as-written.md`, so removing it loses nothing. That is the owner's call.
- Everything else stayed inside the block.

## Report accuracy

- **The three discrepancies the wrapper found are real:**
  - the line range given for `plat_svc.h:1–6` is actually 4–9;
  - the `pl_trap` location (code defect 1);
  - the census was collapsed to `…`.
- **One more misstatement:** item 3, D16 "same path used by probe devices" (code defect 2).
- **Accurate:**
  - the D12–D19 line numbers I sampled;
  - the probe geometry: the nocull triangle winds opposite to `P_flat`, so it is back-facing;
  - the four corrections to the M0a report;
  - the rev-6 design problems, including the honest D18 signature widening to `(RendCtx&, u32 w, u32 h)` and probe placement in clip space.

## Prompt defects (mine)

1. **D13 contradicts itself:** `pl_trap` must print `<file>:<line>` but takes no file or line argument.
2. **D16 says exit 4 for probe devices,** which contradicts the GDD's "fall back to lavapipe" rule.
3. **S12 cannot see new files in protected trees.** `sha256sum -c` checks only the files listed at baseline time, so it missed the phase-2 Docs write. Fix: also diff the sorted file list.
4. **S13.probe can't detect a draw that is missing from both images.** The reference is rendered on lavapipe at run time, with no stored golden, so a cull or pipeline bug that hits both devices passes. Fix: give each draw a box that must contain covered pixels in its colour.
5. **The watchdog regex `[^ :]+:[0-9]+` accepts any location,** including the fake one in code defect 1.
6. **The lane `audit.sh` baseline predates my own doc edits,** so it fails on my work. Take the baseline when the run starts, and exclude paths the owner is editing in parallel.

## Owner decisions

- **Commit M0a + M0a.1 now:** the gates are green and nothing changes behaviour. Carry code defects 1–3 as the first work item of M0b. They are surgical, and none of them changes a frame byte.
- **Delete the stray Docs file, or keep it.** Removing it loses nothing.
- **The pin is unchanged:** 0025 still fails 2 of 2 hunks, so S7.golden stays BLOCKED on M0a and M0b.
- **Fold prompt defects 3–5 into the M0b prompt's scripts.**
