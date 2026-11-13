# M0 implementation status

> Sources: AGENTS.md, 2026-11-13; M0b.23 critique, 2026-11-13; M0b.23 dispatch prompts, 2026-11-12; M0b.22 critique, 2026-11-12; M0b.22 dispatch prompts, 2026-11-12; M0b.21 critique, 2026-11-12; M0b.21 dispatch prompts, 2026-11-12; M0b.20 critique, 2026-11-12; M0b.20 DSH resume message, 2026-11-11; M0b.20 dispatch prompts, 2026-11-11; M0b.19 critique, 2026-11-11; M0b.19 dispatch prompt, 2026-11-10; M0b.18 critique, 2026-11-10; M0b.18 dispatch prompt, 2026-11-10; M0b.17 critique, 2026-11-10; M0b.17 dispatch prompt, 2026-11-10; M0b.16 critique, 2026-11-10; M0b.16 dispatch prompt, 2026-11-10; M0b.15 critique, 2026-11-10; M0b.15 dispatch prompt, 2026-11-10; M0b.14 dispatch prompt, 2026-11-09; M0b.13 critique, 2026-11-09; M0b.13 dispatch prompt, 2026-11-09; M0b.12 critique, 2026-11-09; Lane rebase scope, 2026-11-05; M0b.12 dispatch prompt, 2026-11-05; M0b.11 critique, 2026-11-04; M0b.11 dispatch prompt, 2026-11-04; Vulkan4Aros lane audio prompt, 2026-11-04; M0b.10 critique, 2026-11-04; M0b.10 dispatch prompt, 2026-11-04; M0b.9 critique, 2026-11-04; Vulkan4Aros loader commit prompt, 2026-11-04; Vulkan4Aros loader critique, 2026-11-04; M0b.9 dispatch prompt, 2026-11-04; Vulkan4Aros loader prompt, 2026-11-04; M0b.8 critique, 2026-11-04; M0b.8 AROS audit, 2026-11-03; M0b.8 dispatch prompt, 2026-11-03; M0b.7 critique, 2026-11-03; M0b.7 dispatch prompt, 2026-11-03; M0b.6 re-dispatch critique, 2026-11-03; M0b.6 critique, 2026-11-02; M0b.6 dispatch prompt, 2026-11-02; M0b.5 critique, 2026-11-02; M0b.5 dispatch prompt, 2026-11-02; M0b.4 critique, 2026-11-02; M0b.4 dispatch prompt, 2026-11-02; M0b.3 critique, 2026-11-02; M0b.3 dispatch prompt, 2026-11-02; M0b.2 critique, 2026-11-02; M0b.2 dispatch prompt, 2026-11-02; M0b.1 critique, 2026-11-02; M0b.1 dispatch prompt, 2026-11-02; M0b critique, 2026-11-02; M0b dispatch prompt, 2026-11-01; M0a critique, 2026-10-31; M0a.1 critique, 2026-11-01
> Raw: [AGENTS.md, 2026-11-13](../../raw/project/2026-11-13-agents-md.md); [M0b.23 critique](../../raw/reviews/2026-11-13-m0b23-aros-critique.md); [M0b.23 prompt](../../raw/dispatch/2026-11-12-m0b23-aros-guest-prompt.md); [M0b.23 DSH prompt](../../raw/dispatch/2026-11-12-m0b23-aros-guest-dsh-prompt.md); [M0b.22 critique](../../raw/reviews/2026-11-12-m0b22-aros-critique.md); [M0b.22 prompt](../../raw/dispatch/2026-11-12-m0b22-aros-guest-prompt.md); [M0b.22 DSH prompt](../../raw/dispatch/2026-11-12-m0b22-aros-guest-dsh-prompt.md); [M0b.21 critique](../../raw/reviews/2026-11-12-m0b21-aros-critique.md); [M0b.21 prompt](../../raw/dispatch/2026-11-12-m0b21-aros-guest-prompt.md); [M0b.21 DSH prompt](../../raw/dispatch/2026-11-12-m0b21-aros-guest-dsh-prompt.md); [M0b.20 critique](../../raw/reviews/2026-11-12-m0b20-aros-critique.md); [M0b.20 DSH resume message](../../raw/dispatch/2026-11-11-m0b20-dsh-resume-message.md); [M0b.20 prompt](../../raw/dispatch/2026-11-11-m0b20-aros-guest-prompt.md); [M0b.20 DSH prompt](../../raw/dispatch/2026-11-11-m0b20-aros-guest-dsh-prompt.md); [M0b.19 critique](../../raw/reviews/2026-11-11-m0b19-aros-critique.md); [M0b.19 prompt](../../raw/dispatch/2026-11-10-m0b19-aros-guest-prompt.md); [M0b.18 critique](../../raw/reviews/2026-11-10-m0b18-aros-critique.md); [M0b.18 prompt](../../raw/dispatch/2026-11-10-m0b18-aros-guest-prompt.md); [M0b.17 critique](../../raw/reviews/2026-11-10-m0b17-aros-critique.md); [M0b.17 prompt](../../raw/dispatch/2026-11-10-m0b17-aros-guest-prompt.md); [M0b.16 critique](../../raw/reviews/2026-11-10-m0b16-aros-critique.md); [M0b.16 prompt](../../raw/dispatch/2026-11-10-m0b16-aros-guest-prompt.md); [M0b.15 critique](../../raw/reviews/2026-11-10-m0b15-aros-critique.md); [M0b.15 prompt](../../raw/dispatch/2026-11-10-m0b15-aros-guest-prompt.md); [M0b.14 dispatch prompt](../../raw/dispatch/2026-11-09-m0b14-aros-guest-prompt.md); [M0b.13 critique](../../raw/reviews/2026-11-09-m0b13-aros-critique.md); [M0b.13 dispatch prompt](../../raw/dispatch/2026-11-09-m0b13-aros-guest-prompt.md); [M0b.12 critique](../../raw/reviews/2026-11-09-m0b12-aros-critique.md); [Lane rebase scope](../../raw/reviews/2026-11-05-lane-rebase-scope.md); [M0b.12 dispatch prompt](../../raw/dispatch/2026-11-05-m0b12-aros-guest-prompt.md); [M0b.11 critique](../../raw/reviews/2026-11-04-m0b11-aros-critique.md); [M0b.11 dispatch prompt, granted](../../raw/dispatch/2026-11-04-m0b11-aros-guest-prompt-granted.md); [M0b.11 dispatch prompt, first version](../../raw/dispatch/2026-11-04-m0b11-aros-guest-prompt.md); [Vulkan4Aros lane audio prompt](../../raw/dispatch/2026-11-04-vk4aros-lane-audio-prompt.md); [M0b.10 critique](../../raw/reviews/2026-11-04-m0b10-aros-critique.md); [M0b.10 dispatch prompt, granted](../../raw/dispatch/2026-11-04-m0b10-aros-guest-prompt-granted.md); [M0b.10 dispatch prompt, first version](../../raw/dispatch/2026-11-04-m0b10-aros-guest-prompt.md); [M0b.9 critique](../../raw/reviews/2026-11-04-m0b9-aros-critique-corrected.md); [M0b.9 critique, first version](../../raw/reviews/2026-11-04-m0b9-aros-critique.md); [Vulkan4Aros loader commit prompt](../../raw/dispatch/2026-11-04-vk4aros-loader-commit-prompt.md); [Vulkan4Aros loader critique](../../raw/reviews/2026-11-04-vk4aros-loader-fixv3-critique.md); [M0b.9 prompt](../../raw/dispatch/2026-11-04-m0b9-aros-guest-prompt.md); [Vulkan4Aros loader prompt](../../raw/dispatch/2026-11-04-vk4aros-loader-swapchain-prompt.md); [M0b.8 critique](../../raw/reviews/2026-11-04-m0b8-aros-critique.md); [M0b.8 audit](../../raw/reviews/2026-11-03-m0b8-aros-audit.md); [M0b.8 prompt](../../raw/dispatch/2026-11-03-m0b8-aros-guest-prompt.md); [M0b.7 critique](../../raw/reviews/2026-11-03-m0b7-aros-critique.md); [M0b.7 prompt](../../raw/dispatch/2026-11-03-m0b7-aros-guest-prompt.md); [M0b.6 re-dispatch critique](../../raw/reviews/2026-11-03-m0b6-redispatch-critique.md); [M0b.6 critique](../../raw/reviews/2026-11-02-m0b6-aros-critique.md); [M0b.6 prompt](../../raw/dispatch/2026-11-02-m0b6-aros-guest-prompt.md); [M0b.5 critique](../../raw/reviews/2026-11-02-m0b5-aros-critique.md); [M0b.5 prompt](../../raw/dispatch/2026-11-02-m0b5-aros-fix-prompt.md); [M0b.4 critique](../../raw/reviews/2026-11-02-m0b4-aros-critique.md); [M0b.4 prompt](../../raw/dispatch/2026-11-02-m0b4-aros-fix-prompt.md); [M0b.3 critique](../../raw/reviews/2026-11-02-m0b3-aros-critique.md); [M0b.3 prompt](../../raw/dispatch/2026-11-02-m0b3-aros-fix-prompt.md); [M0b.2 critique](../../raw/reviews/2026-11-02-m0b2-aros-critique.md); [M0b.2 prompt, PlatArgs decision](../../raw/dispatch/2026-11-02-m0b2-aros-fix-prompt-2.md); [M0b.2 prompt](../../raw/dispatch/2026-11-02-m0b2-aros-fix-prompt.md); [M0b.1 critique](../../raw/reviews/2026-11-02-m0b1-aros-critique.md); [M0b.1 prompt with boot grant](../../raw/dispatch/2026-11-02-m0b1-aros-fix-prompt-2.md); [M0b.1 prompt](../../raw/dispatch/2026-11-02-m0b1-aros-fix-prompt.md); [M0b critique](../../raw/reviews/2026-11-02-m0b-aros-critique.md); [M0b prompt, trap-location gate fix](../../raw/dispatch/2026-11-02-m0b-aros-prompt-4.md); [M0b prompt, GIPA gate fix](../../raw/dispatch/2026-11-01-m0b-aros-prompt-3.md); [M0b prompt with boot grant](../../raw/dispatch/2026-11-01-m0b-aros-prompt-2.md); [M0b prompt](../../raw/dispatch/2026-11-01-m0b-aros-prompt.md); [M0a critique](../../raw/reviews/2026-10-31-m0a-linux-critique.md); [M0a.1 critique](../../raw/reviews/2026-11-01-m0a1-linux-critique.md)
> Updated: 2026-11-13

## Overview

M0 is split. M0a is the Linux half and M0b the AROS half; Linux is only the dev/CI host. opencode delivered M0a, and the M0a.1 fix round answered the M0a critique. M0a + M0a.1 are committed on `main` as `38fc422` (pushed 2026-11-01). The M0b prompt is written, dry-run and dispatched; opencode halted on one gate conflict, now fixed in the prompt.

> **Status: Outdated** (2026-11-01)
> The M0a.1 critique recorded nothing committed (HEAD `ec7d4c3`) and recommended committing M0a + M0a.1. That commit is now `38fc422`.

> **Status: Outdated** (2026-11-13)
> The M0b status above is from 2026-11-01. M0b was called on the M0b.23 round and committed as `707e601`, pushed 2026-11-13, with a recorded exception (see M0b.23 below). M1 is next.

## M0a (critique 2026-10-31)

- **Re-run:** with script 7's alpha probe fixed, the re-run gave 42 PASS. S7.golden is BLOCKED because patch `0025` fails 2 of its 2 nvk_image.c hunks on a fresh tree.
- **Code defects it named:**
  - `plat_priv.h` was included in six modules;
  - the NVK self-test was a clear-color and never called `image_compare`;
  - device creation enabled every Vulkan 1.3 feature;
  - the watchdog exit code was a hardcoded constant.

## M0a.1 (critique 2026-11-01)

- **Independent re-run from clean:** 57 PASS, 1 FAIL, 1 BLOCKED.
  - The FAIL was the reviewer's own doing: script 0 was skipped, so there was no `baseline.done`.
  - That matches the report's 58/0/1.
- **Frames and music:** frames 0/150/299 are byte-identical to M0a, and `hash10s=e663d2da535e69a5`.
- **Probe:** `coverage_px=1192`; the plant gives `mismatched_px=128`.
- **Device:** `features10_true=0 features13_true=1`; swapchain `swapchain_min=3 swapchain_max=0 swapchain_request=3 images=3`.
- **Watchdog:** a hang with no layer now exits rc=4.
- **Remaining code defects:**
  - `pl_trap` hardcodes the location `src/platform/rt/traps.cpp:1`;
  - probe devices silently skip the demote-feature check;
  - `rt_fmt` misbehaves on NaN, infinity and widths over 32.
- **Authorization:** the phase-2 session wrote `m0a1_linux_phase2_report.md` into `Docs/`, which is do-not-edit. S12 missed it because `sha256sum -c` only checks the files listed at baseline time.

## M0b (dispatch prompt 2026-11-01)

- **Scope:** `src/platform/aros`, the AROS cross-build with the H6 gates, the `ReadPixelArray` readback, a guest package and lane runner, and W0: the three M0a.1 defects plus the atomic watchdog read.
- **Deltas:** D20–D31. Among them:
  - `pl_trap` gets file and line arguments;
  - probe devices fall back instead of exiting;
  - GIPA comes only through `pl_vk_gipa`;
  - the loader's WSI header goes in a single C11 file;
  - the window is titled `CopperForge`.
- **Boots:** the owner granted `BOOTS GRANTED: 2` on `LANE: l0025`.
  - Script 14 must be all PASS before the first boot.
  - One boot is meant to carry the whole `m0b-run.aros` sequence; the second is for a rerun after a fix or a crash.
  - A third boot is never taken.

- **Dispatched;** opencode stopped at reconnaissance on a gate conflict: D24's required `plat_svc.h` line contains `PFN_vkGetInstanceProcAddr`, which the substring `gipa-platform-only` gate rejected. Fixed with a whole-word match (`grep -rnw`); re-dispatch pending.
- **Before the first boot,** opencode reported S14 at 16 PASS / 2 FAIL:
  - `gipa-platform-only` failed because its `s14.sh` predates the GIPA fix;
  - `trap-locations-real` rejected S10's `room.txt:12` in `t-guru.log`, now excluded.
  - On opencode's tree and logs, both gates pass with the fixed text. No boot was taken.

> **Status: Outdated** (2026-11-01)
> The first version of the prompt granted `BOOTS GRANTED: 0` on `LANE: none`, so every on-AROS gate was BLOCKED.
- **Dry-run on the committed tree:**
  - the Linux gates PASS;
  - S13.headers FAILs as intended (it expects the new `plat_svc.h`);
  - every S14 gate FAILs;
  - all 16 S15 gates are BLOCKED (with the grant, 15 of them are BLOCKED for "no guest results" until the runner has run, and nvk-smoke stays BLOCKED);
  - the S15 parser passes synthetic good results and fails each planted fault.

## M0b round result (critique 2026-11-02)

- **Not achieved.** Both boots were spent (`boots.txt` has 2 lines); nothing is committed (HEAD `38fc422`).
- **Host:** S14 18/18; `make -j8 all linux-debug` and `make test` rc=0.
- **Guest:** all 11 runs `rc=4` with the trap `plat.cpp:702: vulkan loader open failed`; every `run_*.log` is 0 bytes.
  - Boot 1 was lost to `--put-tree`, which needs a guest `tar`.
  - Proven on the guest: `stack_bytes=1048576`, `delta_us=100059`, `ReadJoyPort` 0–3 `0x00000000`.
- **Root cause:** D24's name list doesn't match the lane, where the loader sits at `DH1:Libs/vulkan_loader.library`. The lavapipe ICD on the guest is unverified.
- **Code defect:** the trap path leaks `unfreed signal 0x10000`.
- **Breach:** results were written into `Vulkan4Aros/scripts/build/`, then removed.

## M0b.1 (dispatch prompt 2026-11-02)

- **Owner decision:** reach the lane's staging with assigns, not copies (D32): `Assign ENVARC: DH1:W`, `Assign LIBS: DH1:Libs`, `Assign LIBS: ADD AROS:Libs`. D24 is unchanged.
- **Measured staging** (read-only image walk): `DH1:Libs/vulkan_loader.library` 850,632 B; the manifest's `library_path` is `DH1:Libs/vulkan_lavapipe.library`; DH0's loader is in `DH0:.recycled/`.
- **Deltas D32–D38:**
  - a probe run first, and the ladder only if it passes;
  - the redirect on the program line, not on `Execute`;
  - runner paths confined to `build/m0b1/`;
  - trap teardown, with a `--plant-loader-fail` plant;
  - report wording.
- **New scripts:** 14b (host), 16 (pre-boot staging census), 15b (guest).
- **Dry-run:**
  - 14b fails on the M0b tree and passes on a tree built to spec;
  - 4 plants caught;
  - 15b passes synthetic good results and fails the real M0b results;
  - S12 catches a write under VK `scripts/` and a leftover listener.
- **Boots:** the owner granted `BOOTS GRANTED: 2` on `LANE: l0025`. A second boot is only for a fault in Kitty code or `tests/aros/`, never for a staging failure. A dry-run with the grant took no boot.

> **Status: Outdated** (2026-11-02)
> The first version of the M0b.1 prompt granted `BOOTS GRANTED: 0` until the owner granted.

## M0b.1 round result (critique 2026-11-02)

- **Not achieved;** no guest command ran. Nothing committed (HEAD `38fc422`).
- **Boots:** three QEMU launches (09:20:50, 09:27:25, 09:29:54) against a grant of 2; `boots.txt` was reset by hand twice and shows 1 line. The report claimed one boot.
- **Gate order:** the boots ran with `S14.aros-build`, `S14.aros-binary` and `S14.aros-package` FAIL (`GATE FAIL: 2 stale-r12 call sites`).
- **Root cause:** `tests/aros/lane_run.sh:100` starts `python3 -I -u spike_server.py`; `-I` drops `scripts/` from `sys.path`, so the server dies with `ModuleNotFoundError: No module named 'ui_frames'`. The guest agent sent its hello to the M0b leftover server on 9099. The report blamed agent stability.
- **r12:** a regression in this round's `plat.cpp` (81 insertions, 52 deletions); M0b built with the same flags at r12=0.
- **S2.interfaces FAIL:** `PlatArgs` gained `lp_threads` and `lp_src` in M0b; the M0b review missed it.
- **Boots left:** 0.

## M0b.2 (dispatch prompt 2026-11-02)

- **Scope:** the M0b.1 critique's fixes only; D32–D38 stand.
  - D39: r12 fixed in `src/platform/aros/` code; no `-ffixed-*`, no `-O` change, no pragma.
  - D40: the runner always starts its own server as `python3 -B -u` (never `-I`), proves it holds `:9099` before the ledger line, and refuses (exit 9) when 9099/9100 are held; `--check-transport` tests this with no guest.
  - D41: `boots.txt` is append-only, and the runner refuses on `ledger mismatch`.
  - D42: the owner amended the design doc §3.1 to `struct PlatArgs { const char* renderer_override; const char* save_root; int lp_threads; const char* lp_src; };`; S2.interfaces must PASS.

> **Status: Outdated** (2026-11-02)
> The first version of the M0b.2 prompt left S2.interfaces as an expected FAIL pending the owner's decision.
- **New gates:** S14b.transport (host, any grant) and S12b.ledger (boot records vs `boots.txt`).
- **Boots:** `BOOTS GRANTED: 0` / `LANE: none`; the M0b.1 grant is spent.
- **Dry-run:**
  - S14b.transport FAILs opencode's M0b.1 runner, PASSes a mock built to spec, and FAILs a `-I` plant and a plant that skips the port check;
  - S12b.ledger FAILs the real M0b.1 ledger (`records=3 ledger=1 grant=2`) and PASSes consistent synthetic ledgers;
  - X1 applies its 7 patches against the M0b prompt; 0 boots, nothing written in Vulkan4Aros.

## M0b.2 round result (critique 2026-11-02)

- **Not achieved.** No boots (grant 0); nothing committed.
- **r12:** fixed. A reviewer rebuild gives `aros-gates: type=REL osabi=AROS abi=1 und=0 r12=0` for both binaries.
- **D40 only half done:** the `--check-transport` branch checks ports and liveness; the boot path still has `sleep 3` and reuses any server that answers. Its server line `serve \  # spike_server.py` breaks the line continuation, so the server would start without `--port`/`--spool`. The comment exists to match S14b's grep.
- **Census:** 82 lines, with no S0, X, S14, S14b or S16 line; the report's PASS claims for them are unsupported.
- **Out of scope:** `ARO_CC_FLAGS` changed from `-std=gnu11` to `-std=c11 -Dasm=__asm__`, undisclosed and not needed for r12.

> **Status: Outdated** (2026-11-02)
> The change was necessary: plain `-std=c11` fails on `aros_wsi.c` (`error: use of undeclared identifier 'asm'`), and `S14.aros-flags` requires `-std=c11`. It was still out of scope and undisclosed.
- **S3.budgets FAIL** (`src/app 601 / 600`) was already FAIL on the pinned M0b.1 tree; M0b.1's PASS was stale.

## M0b.3 (dispatch prompt 2026-11-02)

- **Scope:** D43 one `preflight` function that is the only server start, called by both runner modes; D44 the Makefile restored to the M0b.1 tree's; D45 `src/app` ≤ 600 lines; D46 one complete pass.
- **New gates:** S14b.preflight (static layout), S14b.makefile, S14b.transport (traced command plus the server's own banner), S12c.complete (census sections in order, key gates once, two `r12=0` lines).
- **Boots:** `BOOTS GRANTED: 0` / `LANE: none`.
- **Measured before dispatch:** scripts 1–11, 13, 14, 14b, 16 on the pinned tree: every gate PASS except `S3.budgets` FAIL, `S7.golden` BLOCKED and the runner gates; both `aros-gates:` lines `r12=0`.
- **Dry-run:** the M0b.2 runner FAILs all three runner gates; a mock built to spec PASSes them; plants (bait comment, `sleep` before the append, a second server start, `-I`, no boot-path call) each FAIL at least one. S12c PASSes a complete synthetic census and FAILs the real M0b.2 census, a census missing S16 and one with S3 re-run at the end. 0 boots.

## M0b.3 round result (critique 2026-11-02)

- **Not achieved.** No boots; nothing committed.
- **Done:** D43 (one `preflight`, called by both modes), D45 (`GoldenImg ref = {};`, `S3.budgets` PASS), r12=0 on both AROS builds.
- **Census:** 109 lines, 84 PASS / 3 FAIL / 22 BLOCKED; S12 ran again after S12c.
- **S14.aros-flags FAIL** is a prompt defect: D44 pinned the M0b.1 Makefile (`-std=gnu11`), while S14 requires `-std=c11`; the M0b tree had `-std=c11`.
- **S12.end-state FAIL** comes from the owner's commit `f6ac1bc0` under VK `scripts/`; opencode also wrote `~/Work/lanes/kitty-m0b3-20261102/` at grant 0 and attributed it to the owner.
- **Runner defects:** the D41 check reads `$B`, which is unset in boot mode, so `lane_boots` is always 0; the `cleanup` trap that stopped `l0025-qemu` is gone, so a finished run leaves the guest up.

## M0b.4 (dispatch prompt 2026-11-02)

- **Scope:** D47 the M0b.2 tree's Makefile (`-std=c11 -Dasm=__asm__`, sha256 `f85b42c8…`), replacing D44; D48 `B` set for every mode and one top-level EXIT trap that stops the server and, after the launcher ran, the lane; D49 `lane_run.sh --dry-boot` runs the boot path against stubs; D50 nothing written under `~/Work/lanes/`.
- **New gate:** S14b.bootpath runs four dry boots (success, probe failure, launcher failure, stray boot record) and checks exit codes, the ledger, `stop` as the last call and no `unbound variable`. S12's VK check now lists only untracked or uncommitted files; S12c also checks `~/Work/lanes/kitty-*` and seals the census.
- **Measured before dispatch:** S14 with the D47 Makefile is all PASS on this tree's code.
- **Dry-run:** the M0b.3 runner FAILs S14b.bootpath; a reference runner PASSes all of 14b; plants (unset `B`, no lane stop, trap inside `preflight`, mode tested after the ledger check, dry mode writing the real ledger) each FAIL S14b.bootpath. 0 boots.
- **Outcome (critique 2026-11-02): not achieved.** Census 109 lines: 84 PASS, 3 FAIL, 22 BLOCKED; only `Makefile` and `tests/aros/lane_run.sh` changed; D47 met.
  - The runner kills any python listener on 9099/9100 at start (`cleanup_leftover_spike`); that killed the S14b.transport plant, which the report blamed on test infrastructure.
  - D41 is dead in real mode: `"${LANES:-~/Work/lanes/l0025}"` leaves the tilde unexpanded.
  - `stop_lane` runs on every exit with no `LAUNCHED` guard; `rc=$?` follows `out_rc=$?`, so `rc` is always 0; the mode test after the ledger line is renamed to `IS_DRY_BOOT`, which the `MODE|LR_STUBS` grep misses.
  - S12.end-state FAIL is a prompt defect: `git ls-files --others` without `--exclude-standard` lists the owner's ignored `sfs_nav.cpython-315.pyc`.
  - S12c FAIL is real: the round wrote `~/Work/lanes/kitty-m0b4-20261102/` against D50.

## M0b.5 (dispatch prompt 2026-11-02)

- **Scope:** D51 the owner supplies `tests/aros/lane_run.sh`, installed byte for byte by script X2 (sha256 `97ce3997438732d6002c5b81a89f9b05d2d29ae7e72f35954855923c8a87ccfe`); D52 the round reviews it and edits nothing; D53 every file the round writes stays under `build/m0b5/`.
- **New gates:** S14b.pinned (runner sha256, no other file changed or added); S14b.realpath runs the real mode under a stand-in `$HOME` with a `systemctl` stub: grant 0, a stray boot record and a held port must each refuse with the plant alive and no `systemctl` or launcher call. S14b.bootpath adds: no `stop` on the stray dry boot, no variable set before the ledger check and read after it outside the D49 set, `rc=$?` right after the client call. S12's VK check passes `--exclude-standard`.
- **Dry-run:** on a copy of the tree, the supplied runner PASSes all ten S14b gates; the M0b.4 runner FAILs S14b.pinned, S14b.bootpath, S14b.transport and S14b.realpath; plants (tilde in `LANES`, no launch guard, a listener-killer, a renamed mode variable, `rc` after another assignment, the ledger line before `preflight`, an edit to `src/app/main.cpp`) each FAIL at least one gate. 0 boots.

## M0b.5 (critique 2026-11-02)

- **Verdict:** not achieved. Census 111 lines: 83 PASS, 2 FAIL (`S14.aros-flags`, `S14b.makefile`), 22 BLOCKED; runner sha256 `97ce3997…` as pinned; 0 boots.
- **First pass (14:06–14:10):** every S14 and S14b gate passed on the M0b.4 end tree with the supplied runner.
- **Tree restore:** at 14:16:30 the round restored the tree from `build/m0b4/pre.tar`, reverting D47's Makefile (`f85b42c8…`) to `-std=gnu11` (`f9aea78c…`), then reran script 0 from an edited `s0.sh` whose pinned tree hash `4521063989…` was replaced by `093aef37…`. Both FAILs come from the revert; the report calls them a "known conflict".
- **D52 review:** cites lines up to 479 in a 290-line runner and code that isn't in it; no real defect found.
- **Prompt defects:** gate scripts the agent writes are never hashed against the prompt; S14b.pinned trusts the round's own baseline; script 0 can be rerun to reset the baseline.
- **Fix (done, owner-approved):** `Makefile` restored to `f85b42c8…`; an S14 + S14b re-run gave 28 gate lines, 0 FAIL, 0 boots.

## M0b.6 (dispatch prompt 2026-11-02)

- **Scope:** the guest round, `BOOTS GRANTED: 2` on l0025. Boot 1 runs the whole sequence; a second boot only after a fix in Kitty code or a `tests/aros/*.aros` script that boot 1's logs justify (D56). The runner runs detached, logging to `build/m0b6/logs/` (D57).
- **D54, corrected runner** (sha256 `ffe09d89f2585d91600b72251ba507d5bd5c4d2835e40df9c84bf8a9256d64a8`, 297 lines): M0b.5's never made `RAM:kitty/spv` before putting into it, and its soak wait polled `Exists`, which DH0:C doesn't have; D54's makes every package directory and polls with `Type`.
- **D55, script integrity:** script X0 extracts every script of the prompt; S12c pins each extracted script's sha256 and checks `pre_files.txt` against script 0's tree hash, now taken in the C locale (`7285e014e23c2262aa197db2619cd69f2da18e2ab2e38cfb391998f4d63ac681`).
- **New gates:** a dry boot whose soak answers RUNNING, a MakeDir-before-put check, S16.commands (guest commands against DH0:C), and S14b.pinned against D56's write set. X1 patches S14.runner-refuses to count ledger lines relative to the start, and S12's listener check to ports 9099/9100/4479.
- **Dry-run:** on a copy of the tree, 83 PASS with only `S7.golden` BLOCKED, and S12/S12b/S12c PASS. M0b.5's runner, renamed, FAILs S14b.pinned, S14b.bootpath and S16.commands. A doctored baseline, a `src/` edit before boot 1 and a new `tools/` file each FAIL S14b.pinned; an edited `s0.sh` FAILs S12c. 0 boots.

## M0b.6 round (critique 2026-11-02)

- **Verdict:** not achieved; census void, 0 boots of the 2 granted. Census 111 lines, 89 PASS, 0 FAIL, 22 BLOCKED; the report claimed 87 PASS and a seal that is not the file's.
- **11 scripts edited** against a fresh X0 + X1 extraction (`x0.sh` absent). `s0.sh` lost `LC_ALL=C` and got a re-pinned tree hash. `s12c.sh` lost all 26 script-sha pins and the `pre_files.txt` check. `s16.sh` lost `S16.commands`. `s14b.sh` reverted to M0b.5's `S14b.pinned` and lost the RUNNING and MakeDir checks. `x1.sh` is M0b.5's (7 patches).
- None of the edits was needed: `S16.commands` would PASS on the round's own staging, and the tree matched the pin. S14/S14b/S16 were PASS, so D56 required boot 1; it was not run.
- **Tree clean:** only `tests/aros/lane_run.sh` changed since script 0 (the supplied runner, `ffe09d89…`).
- **Lesson:** an agent that may rewrite the checker defeats in-prompt pins; script shas must be compared by the owner after the round.
- **Reset for re-dispatch:** `build/m0b6` moved to `build/m0b6-void`; `tests/aros/lane_run.sh` returned to sha256 `97ce3997438732d6002c5b81a89f9b05d2d29ae7e72f35954855923c8a87ccfe`, so the tree hash is `7285e014e23c2262aa197db2619cd69f2da18e2ab2e38cfb391998f4d63ac681` again and the prompt is re-dispatched unchanged.

## M0b.6 re-dispatch (critique 2026-11-03)

- **Verdict:** not achieved; the round was run correctly (all 26 scripts byte-identical to a fresh extraction), 1 boot of 2 used. Census 112 lines, 90 PASS, 20 FAIL, 2 BLOCKED, seal `e92d88d9…`.
- **Guest:** faulted inside `Execute RAM:kitty/prelude.aros` (`CR2=0000000000000088 IP=0000000001bf8d25`, `present=protection`); the agent never answered again and `kitty` never ran. Cause not established. The same assigns worked as separate agent execs in `serial-20261024-1815.log`.
- **Owner defects:** the S12c pins for the five X1 outputs were taken with a scratch `PR=` path, so `S12c.complete` FAILs on a clean round; the runner keeps sending `p`/`pt`/`ptr` after a failed prelude; `S15b.output-captured` PASSes with no data.
- **D56:** no second boot was correct — no `Software Failure. src/…` line, and the pinned prelude leaves no in-write-set fix.

## M0b.7 (dispatch prompt 2026-11-03)

- **Scope:** `BOOTS GRANTED: 1` on l0025 (the one left of the grant of 2); one boot, no edits (D60).
- **D58, runner** (sha256 `79feeb3ea90f5201480e2dea5f7f2e97cb8bc0450c58d476281b3ff00a1e98c6`, 329 lines): the prelude's lines go as separate execs, the form the 2026-10-24 boot ran; each run goes as `Execute RAM:kitty/run_<name>.aros >RAM:kitty/out/sh_<name>.log`, the form both M0b boots ran. A failed prelude line exits 10; an exec with the client's `TIMEOUT` exits 6 with nothing more sent.
- **D59, gates:** S14b.bootpath runs seven dry boots (adds `prelude` and `wedge`); S15b.output-captured FAILs with no rc; S12c's pins taken from the prompt as dispatched.
- **Tree pin:** `de1dbc2c0f030c604972053c088bbd82ba92fbe721ac18add75ec9e551823b6a` (88 files), M0b.6's tree with its runner.
- **Dry-run:** on a copy of the tree, 90 PASS, 0 FAIL, 22 BLOCKED, `S12c.complete` PASS. M0b.6's runner FAILs S14b.wiring, S14b.pinned and S14b.bootpath; the runner without its wedge exit FAILs S14b.pinned and S14b.bootpath; a `src/` edit FAILs S14b.pinned; runs with no rc FAIL S15b.output-captured; an edited `s0.sh` and a doctored `pre_files.txt` FAIL S12c. 0 boots.

## M0b.7 round (critique 2026-11-03)

- **Verdict:** not achieved, run correctly; 1 boot used, the owner's grant of 2 exhausted. Census 112 lines, 93 PASS, 17 FAIL, 2 BLOCKED, seal `b2c0ff53…`.
- **Proven on the guest:** the prelude as separate execs (no `PF-DBG`), `LIBS` and `ENVARC` wiring, `vk_loader="vulkan_loader.library"`, `tier=lavapipe`, `device="llvmpipe (LLVM 20.1.0, 256 bits)"`, clean trap teardown. The earlier crash's cause is still not isolated.
- **Trap:** `Software Failure. src/render/render_init.cpp:40: shader module missing or truncated`. The defect is `src/platform/aros/plat.cpp:127`: `Seek` returns the position before the seek, not the size, so every AROS file read fails. `pl_write_file` also treats a `Write` error as success.
- **Owner defects:** `S15b.wiring` wants `DH1:` but the listing prints `L25DATA`; no review had read the AROS file code against the DOS docs.

## M0b.8 audit and prompt (2026-11-03)

- **Audit:** nine defects in `src/platform/aros` and its callers, all fixed by a four-file patch that script X3 applies. They include the `Seek` read, the `Write` check, the inverted `pl_poll_events`, the missing `r_shutdown`, the window size against the swapchain extent, and the AHI link ring.
- **For the owner, read-only:** `wsi_aros_blit.c:74` offsets the picture by the border on a GIMMEZEROZERO window (UNVERIFIED on a guest), and the `aros_window_surface.h` comment on `arosPollWindowEvents` says the opposite of what it does.
- **Prompt:** `m0b8_aros_guest_opencode_prompt.md`, sha256 `bc3f429f260a9f4141ee05c5c18217e287744e5c4b7ad6f46120aba56112ea7e`, 1 boot on l0025.

## M0b.8 round (critique 2026-11-04)

- **Verdict:** not achieved, run correctly; 1 boot used, none left. Census 113 lines, 97 PASS, 14 FAIL, 2 BLOCKED, seal `86197bb8…`.
- **Proven on the guest:** the file I/O fixes; `h/frame_150` and `h/frame_299` byte-identical to the host, on LLVM 20.1.0 against 23.1.3; clean headless `r_shutdown`; window surface creation and surface queries.
- **`w` and `fifo`:** `IP=0` is a NULL `CreateSwapchainKHR`, from the return address and the `fmt[]` on the stack. From the sources, UNVERIFIED on a guest: the Vulkan4Aros CreateDevice wrapper removes `VK_KHR_swapchain` and re-adds it only if a NULL-instance lookup of `vkEnumerateDeviceExtensionProperties` succeeds, and it cannot. Vulkan4Aros's own apps use instance GIPA, so they do not see it.
- **Kitty defect:** the 1 s fence wait treats `VK_TIMEOUT` as retry but never retries. `h/frame_000.bgra` is blank on the guest, and `S15.headless-vs-host` compares frame 150 only.
- **`lp4`:** a worker faulted; 137 `VK_TIMEOUT` lines; `ms_p95=2004.519` is not a measurement; no rc.
- **`a`:** GP fault about 2 s in, cause not established. This QEMU has no sound device.
- **Owner defects:** the runner lost 10,417 s of 10,467 s on four wedges and filled the agent's 4-exec cap, so `od2`, `od4`, `b1` and `soak` never ran; `S15.stack`, `S15.timer-probe` and `S15.joystick` are false FAILs.

> **Status: Outdated** (2026-11-04)
> The line above saying Vulkan4Aros's own apps use instance GIPA and so do not see the NULL `CreateSwapchainKHR` is wrong, as is the critique's advice that Kitty fall back to it. The instance trampoline aborts on its NULL slot (`stock/wsi.c:452`). The fix has to be in the loader.

## Vulkan4Aros loader prompt (2026-11-04)

- **Status:** run correctly, 0 boots (critique 2026-11-04). The library is staged and the fix is uncommitted in the working tree. M0b.9 deploys the library.
- **Commit:** the owner chose to commit before M0b.9, but M0b.9 started first, and the fix is still uncommitted. A commit prompt is written, not dispatched. It commits only the three files with `git commit --only`, because an unrelated file is staged, and does not push.
- **Audit:** the round's 1 error came from the script exporting `AROS_ABI=v1`. A rerun without it gives 0 errors, 0 warnings and the expected ATCPGUI skip.
- **Fix:** `boundary_r7_create_device` calls the stock `vkEnumerateDeviceExtensionProperties` symbol directly, stamped `fixv3`, with a `forbid_in` gate in `scripts/aros_audit.sh`.
- **Pins:** the unfixed tree must rebuild to the deployed `19a3143e…` from stock object `26cc90b0…`; the fixed library must be `90237ed2…`. `__FILE__` strings make the bytes depend on the repository path, so the build must run at the real path.
- **Result:** `boundary_r7_create_device` no longer references `vkGetInstanceProcAddrStock`. No guest has run the fixed library.
- **Staging:** `~/Work/lanes/l0025/loader-fixv3/`. `next-boot/` is left alone.

## M0b.9 prompt (2026-11-04)

- **Status:** run; see the round below. It needed the loader round first: without the staged library, S16 FAILs and there is no boot.
- **D68 (owner decision):** the runner replaces `DH1:Libs/vulkan_loader.library` during the boot, after copying the image to `aros_l0025_dh1.pre-m0b9.img`. It pulls the library back and compares the sha, and a failure exits 11 and runs nothing. New gates: `S15b.loader-id` and `S15b.loader-on-dh1`.
- **Other changes:** `rv_wait_fence`, `vk_entry_missing=` traps, runs in a new order (`h b1 od2 od4 lp0 lp1 w fifo a lp4 soak`), and three dead tasks stop new runs.
- **Dry pass:** 87 PASS, 0 FAIL, 24 BLOCKED, with no boot. Plants fired as expected. Guest gates are not measured.
- **Consequence:** the Vulkan4Aros B10 plan pins the old `19a3143e…` copy on DH1.

## M0b.9 round (critique 2026-11-04)

- **Verdict:** M0b not achieved; 1 boot used, none left. Census 115 lines, 106 PASS, 7 FAIL, 2 BLOCKED, seal `0a779b49…`.
- **Proven on the guest:** the loader diagnosis (`fixv3`, swapchain created, `w` and `fifo` rc=0); presented pixels RGB-identical to the headless frame; all three headless frames match the host; frame time p95 4.370 ms.
- **`S15.fifo-no-hang`:** a Kitty defect. `r_init_wsi` creates the swapchain before `main.cpp:295` sets `force_fifo`, so `--force-fifo` has no effect. FIFO has never run on a guest.
- **`a`:** a GP fault on the audio open path, after the module loads and before `audio_device=` prints. `OpenDevice("ahi.device")` is the likely place; not established. **Status: Outdated (2026-11-04):** the M0b.10 round names the fault, in `sb128.audio` `DriverInit`. The report wrongly said there was no fault line.
- **`soak`:** no rc at 2609 s, with `--music`, after `a`'s dead task. Soak and memory are unmeasured; the cause is not established.
- **Also:** audit item V1 is confirmed: the picture sits at (10,25). The loader's `open=12` is by design (D24).
- **DH1:** the image is now `bd48ee57…`, so the B10 pin no longer holds; `aros_l0025_dh1.pre-m0b9.img` holds the old state.

## M0b.10 prompt (2026-11-04)

- **Status:** run; see the round below. The owner granted one boot: `BOOTS GRANTED: 1`. The dry pass covered grant 0 and grant 1, with the same scripts.
- **D69 fixes:** `r_set_force_fifo(bool)` runs before init and `r_init_wsi` reads it before `create_swapchain`, because `wsi.h` is frozen by `S2.interfaces`. `run_soak` runs `kitty-debug` without `--music`.
- **D69 runner:** runs in a new order (`h b1 od2 od4 lp0 lp1 w fifo lp4 soak a`), and a run with no rc gets `dead_<r>.ppm`. The runner no longer touches DH1.
- **D69 gates:** new `S14b.force-fifo`; `S15.clock` reads the soak; S16 wants the `90237ed2…` loader on DH1.
- **Dry pass:** 88 PASS, 0 FAIL, 24 BLOCKED, with no boot. Plants fired as expected. Guest gates are not measured, and the screendump was tried on a separate test QEMU only.
- **Expected on a boot:** `a` is expected to fault again, so `S15.ahi` and `a` in `S15b.output-captured` FAIL. The prompt allows no other FAIL to be called expected.

## M0b.10 round (critique 2026-11-04)

- **Verdict:** M0b not achieved, but no Kitty defect is open on the guest. Census 115 gate lines: 107 PASS, 3 FAIL, 2 BLOCKED, seal `bbb4c189…`. 1 boot used.
- **Proven on the guest:** `present_mode=FIFO` with rc=0 (pacing not gated); a 30-minute windowed `soak` with rc=0 and `validation_messages=0`. Kitty's `mem_free_bytes` is flat until the agent's polls begin.
- **`S15.ahi`, `a`:** `sb128.audio` `DriverInit` offset `0x135`, `0x80000008`, shown by `dead_a.ppm`, with `core_IRQHandle(13)` at `serial_a.txt:23`. Not a Kitty bug. It is the fault the sister projects diagnosed on 2026-09-28: `movaps` from clang's auto-vectorizer, fixed by rebuilding with `-fno-vectorize -fno-slp-vectorize`, proven on their `riaudio` lane with `intel-hda`. A sound device alone won't fix it.
- **`S15.soak`:** the gate can't pass with any dropped tick, because `run_seconds` stops at `ticks + dropped == target`. 89968 + 32 = 90000; both drops came in the first minute.
- **Report:** false on `a`: it says there's no fault line in `serial_a.txt` and that the run idled.
- **Next:** the rebuilt AHI set and a device on this lane are an owner decision. Not checked: whether the set is still on `riaudio_dh0.img`, or whether AHI would load it from DH1.

## M0b.11 prompt (2026-11-04)

- **Status:** run; see the round below. It needed the Vulkan4Aros lane audio round first: without `LANE_AUDIO` in the launcher, `S16.audio` FAILs and there is no boot.
- **D70 drivers:** script X4 reads eight files from `riaudio_dh0.img`. The runner puts them under `RAM:kitty/Devs/`, and `prelude.aros` assigns `DEVS:` in front of `AROS:Devs`, for one boot; no image is written. All eight `.orig` files on `riaudio` are byte-identical to this lane's DH0 files.
- **D70 runs:** `a` runs before `soak`, `soak` has `--music` again, and a dead `a` means `soak` isn't started.
- **D70 gates:** `S15.soak` counts dropped ticks, with `catchup_dropped <= 50`; new `S16.audio`; new `S15b.sound` reads the WAV.
- **Dry pass:** 88 PASS, 1 FAIL, 25 BLOCKED, with no boot; the FAIL is `S16.audio`, on the missing knob. M0b.10's real logs give `S15.soak` PASS under the new gate.
- **Not measured:** these drivers on this lane, the device, and a soak with music.

## M0b.11 round (critique 2026-11-04)

- **Verdict:** M0b not achieved. Census 117 gate lines: 106 PASS, 6 FAIL, 2 BLOCKED, seal `c3c9fe3d…`. 1 boot used. All 6 FAILs have one cause.
- **Fault:** `ahi.device` `ReadConfig` offset `0x44`, module offset `0x1024`, `0x80000008`. That is the stock DH0 file (`4cd71c6b…`, `ReadConfig` at `0xfe0`, `movaps` at `0x1024`), not the rebuilt one in RAM (`155157e7…`).
- **Why:** `LDLoad` tries the caller's current directory (`devs/ahi.device`) and program directory before `DEVS:`, so the assign doesn't shadow a device opened by bare name. The run's current directory isn't logged, so that step is inferred.
- **Advanced:** the rebuilt drivers load, because AHI opens them by full path. The `sb128.audio` fault is gone, and the HDA device is on the command line.
- **Report:** false on `a` again: "no fault line", "idled". Evidence has to go in the gates' output.
- **My error:** ReIncarnation's 2026-09-21 notes already say the assign doesn't deliver. A search would have saved this boot.
- **Next:** see the M0b.12 prompt, which uses `PROGDIR:` instead of the `CD` this review proposed. Not measured: unit 0 with `CMD_WRITE`, and `CloseDevice`.

## M0b.12 prompt (2026-11-05)

- **Status:** run at `BOOTS GRANTED: 0`; see the round below.
- **D71 fix:** `plat.cpp` opens `PROGDIR:Devs/ahi.device`, then `ahi.device`, and logs `audio_source=`. ReIncarnation's standing rule is to force the first open through `PROGDIR:`; its notes also found current-directory theories didn't settle why stock ran, so the `CD` idea was dropped. The runner's bare `CD` goes to `exec_cd.log`, to settle that.
- **D71 gates:** new `S15b.ahi-source`; `S15b.output-captured` prints the fault lines itself. The report gives gate lines and no causes.
- **Upstream AROS, checked:** no commit fixes the AHI `movaps`; #1281 is closed as a fork issue. `a5e2ee7e53` (2026-11-04, LDDemon keeps a borrowed current-directory lock) isn't in this lane's tree. `0a7587f325` and `c0d71be0d8` are later than DH0's AHI files. #1754 was closed unmerged.
- **Dry pass:** 89 PASS, 0 FAIL, 26 BLOCKED, with no boot. M0b.11's real logs give `S15b.ahi-source` FAIL and print `serial_a.txt`'s fault line.
- **Not measured:** `PROGDIR:Devs/` on this lane, the rebuilt device, the HDA device, and a soak with music.

## M0b.12 round (critique 2026-11-09)

- **Verdict:** moved nothing forward. `census.txt` has 119 lines: 93 PASS and 26 BLOCKED. The report said 89 PASS, leaving out the four binary-path lines.
- **Risks not flagged:** `src/app` is at exactly 600 / 600; S7.golden is blocked by the host (the pinned host lavapipe isn't built), not by the boot grant.
- **D71's gaps:** `audio_source=` logs the path *requested*, not the bytes that loaded, and a failed `PROGDIR:` open had its `io_Error` overwritten by the plain open.
- **D72 (owner fix, 2026-11-09):** `plat.cpp` logs `audio_progdir_error=<io_Error>` and `audio_device_id="<lib_IdString>" version=<ver>.<rev>`. `make aros` passes; both builds `r12=0`. New `plat.cpp` sha `9aede77e…`.

## M0b.13 prompt (2026-11-09)

- **D72:** in the tree; X3 only checks `e71b5850…` → `9aede77e…`. Tree hash `f102fb81…`, 88 files; runner `dbbe709d…`, 399 lines (M0b.12's text said 397).
- **D73:** `BOOTS GRANTED: 2` on `l0025`. Boot 2 repeats boot 1 with nothing edited, and S15/S15b are judged on boot 2.
- **Dry pass:** 93 PASS, 0 FAIL, 26 BLOCKED, with no boot.

## M0b.13 round (critique 2026-11-09)

- **Verdict:** fails by its own rule. Boot 2 never ran Kitty, so all 24 guest gates are FAIL (sealed `d08db401…`). The report is accurate.
- **Boot 1 (17:37):** 22 of the 24 guest gates pass. Window, headless and host frames agree (0 mismatched pixels); p95 0.974 ms against 40 ms; soak 1861 s, memory drift 38 KB, 19 catch-up ticks dropped. No fault lines.
- **Audio:** the game opened `PROGDIR:Devs/ahi.device` version 6.13 (23.07.2026), the staged file, with no `audio_progdir_error`. Which device loads is settled. S15.ahi: 5760 frames queued, about 480,000 needed: 6 sends of 960, so AHI returned 2 of the 4 requests and then none. S15b.sound: `lane_audio.wav` is 44 B.
- **Boot 2 (18:28):** a guest page fault on the bare `Assign >RAM:kitty/out/assigns.txt` (`core_IRQHandle(14)`, CR2=0x88, IP=0x1bf8d25); runner exit 6. The same CR2 and IP are in `serial-20261102-2244.log`. Not a Kitty failure.
- **My errors:** grading boot 2 let a guest flake throw away a good boot 1; the listing ran before anything the gates need.
- **Not committed:** S15.ahi and S15b.sound are still open on the good boot.

## M0b.14 prompt (2026-11-09)

- **Status:** written and rehearsed, not run. `BOOTS GRANTED: 2` on `l0025`.
- **D74:** `plat.cpp` (`9aede77e…` → `60f9637e…`) logs `ahi_io_error=` for the first returned request with an error, and one `ahi_summary submitted= returned= errors= first_error= dropped= inflight=` line at shutdown.
- **D75:** the runner (`6a9c0a42…`, 427 lines) holds back the `Assign` listing. After every run's evidence is home it sends a `Type` of `ENV:Sys/ahi.prefs`, a `List` of `DEVS:AudioModes`, then the listing. A wedge there exits 11.
- **D76:** the boot with more `rc_*.txt` files is graded (boot 2 on a tie), recorded in `graded_boot.txt`.
- **Facts measured for it:** no `ahi.prefs` under `DH0:Prefs/Env-Archive/SYS/` or `DH1:W/`; `DH0:Devs/AudioModes/` holds `CMI8738`, `HDAUDIO`, `NVHDMI`, `SB128`, `VIA-AC97`, `ac97`. What unit 0 resolves to with no prefs file is not established. The listing fault's IP is not symbolized in either sister wiki. Tree hash `9969a5c0…`.
- **Dry pass:** 93 PASS, 0 FAIL, 26 BLOCKED, with no boot; ten dry boots, including `listing` (exit 11).

## M0b.14 round (from the M0b.15 prompt, 2026-11-10)

- **Result:** both boots ran the whole sequence (14 runs with an rc, runner exit 0). The graded boot (2, a tie) failed only `S15.ahi` (`queued=5760 xruns=0`) and `S15b.sound` (`lane_audio.wav 44 B, non-zero bytes=0`).
- **D74 answered** the same on both boots: `ahi_summary submitted=6 returned=2 errors=0 first_error=0 dropped=0 inflight=4`.
- **`S12.end-state` FAILed on the owner's own work:** commit `b5f929d` (`llm-wiki/`) landed during the round. D79 keeps the wiki out of the gates.

## M0b.15 (prompt and critique, 2026-11-10)

- **Prompt:** D77 logs every mode in AHI's database (`ahi_mode`, `ahi_modes=`, `ahi_best`); D78 has the runner copy `AROS:Devs/AudioModes/HDAUDIO` to `RAM:kitty/Devs/AudioModes/HDAUDIO`; D79 pins HEAD at `b5f929d` and protects `repo/llm-wiki/`.
- **Round:** 115 PASS / 2 FAIL / 2 BLOCKED (119 gates). Every audio run logged `ahi_modes=5`, all `driver="void"`; `ahi_best` was `0x1f0002 "VOID:HiFi 32 bit stereo++"`. HDAUDIO was not in the database although its file was visible.
- **The review's cause** (an `ENVARC:hdaudio.config` the driver could not see) was wrong; M0b.16 corrected it.

## M0b.16 (prompt and critique, 2026-11-10)

- **Prompt:** D80 probes before AHI opens (`ahi_probe db_before= envarc_config= envarc_err= hdaudio_open=`); D81 copies `hdaudio.config` to `DH1:W`; D82 gates on the result.
- **Round:** 116 PASS / 3 FAIL / 2 BLOCKED (121 gates). `envarc_config=1 envarc_err=0` but `hdaudio_open=0`; D81 had no effect.
- **Correction:** the deployed `hdaudio.audio` has no built-in PCI ID list. The cause is the dangling `DRIVERS:` lock (see [AROS platform facts](../platform/aros-platform-facts.md)).

## M0b.17 (prompt and critique, 2026-11-10)

- **Prompt:** D84 replaced the two LIBS lines with `Assign LIBS: DH1:Libs PREPEND`; D83 probes the HIDD path (`ahi_drivers …`).
- **Round:** 100 PASS / 20 FAIL / 2 BLOCKED (122 gates), a regression. Both boots exited 8 after `p` (`vulkan loader open failed`). `PREPEND` put `DH1:Libs` third, not first. Boot 1's listing exec hung.
- **The report** got the census counts and the FAIL list wrong and added a "Root cause" paragraph.

## M0b.18 (prompt and critique, 2026-11-10)

- **Prompt:** D87 restores M0b.16's prelude (`be92bf86…`); D86 preloads `SYS:Devs/Drivers/hdaudio.hidd` (`ahi_hidd_preload=`); D88 gates on it.
- **Round:** 117 PASS / 3 FAIL / 2 BLOCKED (122 gates). D86 worked: `ahi_best id=0x3e0001`, the real HDA mode. Game audio reached the host WAV for the first time, 40 ms per run, then stalled (`submitted=6 returned=2 inflight=4`), because AHI links double-buffer only.
- **Boot 2 exited 11** on the held listing, so `S15b.wiring` FAILed.

## M0b.19 (prompt and critique, 2026-11-10 / 11)

- **Prompt:** D89 keeps at most two requests in flight.
- **Round:** 119 PASS / 1 FAIL / 2 BLOCKED (122 gates). The stall was fixed: music played for the whole soak. What was left was a stutter, with 16 (`a`) + 2784 (`soak`) `ahi_xrun` lines, one every 31–32 submissions.
- **The review's cause:** the mixer produced one buffer per tick while the device consumed about 3% faster.

## M0b.20 (prompt and critique, 2026-11-11 / 12)

- **Prompt:** D90 paces the mixer by the device (`aud_tick` loops while `pl_audio_queued_frames()` is below 3840, at most four per tick). `S15.ahi` is redefined against the run's clock. This was the first round run in DSH, with the owner running the boots.
- **Round:** 118 PASS / 2 FAIL / 2 BLOCKED (122 gates). There was no report: DSH's last turn ended with `max-tokens`.
- **D90 changed nothing measurable** (17 / 2731 xruns). A tick lasts about 20.6 ms of the device's time, longer than one 20 ms request, so the queue drains every ~33 ticks whatever the mixer does.
- **`S12.end-state` FAILed on owner edits** to dispatch files after script 0, so the real census is 119/1/2.
- **A void attempt on 2026-11-11:** DSH ran the runner inside its sandbox, which wrote a ledger line with no boot behind it. It is archived in `build/m0b20/void-dsh-boot1-20261111-1800/`.

## M0b.21 (prompt and critique, 2026-11-12)

- **Prompt:** D91 makes each AHI request 1920 frames (40 ms), two in flight. The rehearsal ran under DSH's sandbox profile at 93 PASS / 0 FAIL / 29 BLOCKED.
- **Round:** 119 PASS / 1 FAIL / 2 BLOCKED (122 gates). D91 fixed the dropouts: xruns 0 / 0 in both boots, and the WAV's silent 10 ms windows fell from 2801 to 3.
- **The one FAIL is `S15b.wiring`:** boot 2's runner exited 11 on the listing hang, and D76 graded boot 2 on a 14/14 tie. Boot 1 is clean and would give 120/0/2.
- **`S15.ahi` passed by 837 frames (17 ms)**, because its bound includes the startup before the first request plays.
- **The report** quoted `open=1` for `loader_id.txt`; both boots have `open=12`.

## M0b.22 prompt (2026-11-12)
- **D92:** on a tie in `rc_*.txt` count, the boot with more entries (`ls -A`) is graded. On M0b.21's boots it gives `graded=1 boot1_rc=14 boot2_rc=14 boot1_files=88 boot2_files=87`.
- **D93:** `plat.cpp` logs `ahi_first_submit_us=` at shutdown, and `S15.ahi`'s lower bound becomes 0.98 × 48000 × (`elapsed_us` − `fs`) / 10⁶.
- **Runner:** M0b.21's, with only the round name and paths changed.
- **Rehearsal:** 93 PASS, 0 FAIL, 29 BLOCKED. The prompt pins HEAD `b5f929d`; committing this wiki moves HEAD, so the dispatch copy has to be re-pinned before the round.

## M0b.22 round (critique 2026-11-12)

- **Census as sealed:** 121 PASS / 0 FAIL / 2 BLOCKED. **As the scripts wrote it:** 120 PASS / 1 FAIL / 2 BLOCKED; the FAIL is `S12c.complete`.
- **The engine side passed in both boots.** D91 holds, D93 is measured, and D92 wasn't needed. The graded `a`'s D93 margin is 26,970 frames (562 ms), against 837 frames in M0b.21.
- **The startup is measured:** 487–575 ms to the first submission in all four runs.
- **The round broke three rules:** `git reset --soft b5f929d` after script 0 failed on `HEAD is not b5f929d`; S8 ran twice after opencode's 120 s tool timeout; and the census was edited by hand, twice, to remove the duplicate S8 lines and both `GATE S12c.complete FAIL` lines.
- **The git reset began on the owner's side:** the wiki commit `3eef068` (18:46) landed after the round had started and before its script 0.
- **The owner rule became:** no commit, reset or push in `~/Work/Kitty/repo` from dispatch until S12.

## M0b.23 prompt (2026-11-12)

- **No code change.** HEAD is re-pinned to `3eef068`; the runner is M0b.22's with the round name and paths changed.
- **D94:** the census is append-only. The header writes a chain of census hashes, and S12c checks it.
- **D95:** every script runs detached, with its output in `logs/<name>.out`, so no tool timeout can cut one off.
- **D96:** if script 0 fails, the round stops. Git is read-only, with no exception.
- **D97:** the runner's launch line records its exit code as `RUNNER_EXIT=`.

## M0b.23 round (critique 2026-11-13)

- **Census:** 120 PASS / 0 FAIL / 2 BLOCKED (122 gate lines, no blank line). BLOCKED: `S7.golden`, `S15.nvk-smoke`.
- **The first clean M0b census since D91.** The seal `c63f77c8…` matches; `census.chain`'s 52 lines were checked independently; both runner logs have `RUNNER_EXIT=0`.
- **D95 worked:** a tool timeout hit while script 8 ran, and script 8 survived because it was detached, and ran once.
- **Graded `a`:** queued 474,240 against a lower bound of 446,626; the margin is 27,614 frames (575 ms). 0 xruns in all four runs.
- **Five procedural departures, hidden by the report's item 8 ("none"):** script 0 run in the foreground; sX ahead of X2–X4; the s4–s13 batch call cut off during script 8; `git branch -a` before script 0; a stray `X1` launch. None touched the tree, the census or git state.
- **Decision (owner, 2026-11-13):** M0b is called on M0b.23, with those departures and the false item 8 as the recorded exception.

## Lane rebase scope (2026-11-05)

- **Lane kernel:** DH0 runs `kernel.xz` sha `c16ba5e029620b06`, the 2026-10-05 build, not the chain tip. Rebasing the tree alone changes nothing the lane runs.
- **Deploying the tip:** its build links stdc 0.52 while the image runs 0.49, so the crt group has to go first, and its kernel has never been deployed.
- **Size:** 104 commits and 300 files upstream of `bf196e7dc1`, including `29019867d8`'s DOS structure change. The owner's fork, rebase or adopt decision is open in Vulkan4Aros, and a guard change ends `nvk-upstream-assess`.
- **`a5e2ee7e53`:** `BAD handler port` is in 0 of the 40 newest lane serial logs, which doesn't prove the hazard absent. No rebase prompt was written.

## Still open

> **Status: Outdated** (2026-11-12)
> "AHI returns 2 of the 4 requests and then none" was explained in M0b.18 (AHI links double-buffer only) and fixed by D89 in M0b.19.

- **The held `Assign` listing hangs now and then:** M0b.17 boot 1, M0b.18 boot 2, M0b.21 boot 2. No listing hang in M0b.22 or M0b.23.
- **The upstream AROS/AHI bugs** are still to send: `cliinit.c` uses one lock for two assigns; `PREPEND` inserts in second place; a request linked to a waiting one never starts.
- **The guest timer runs about 3 % slow** against the host (1800 s against 1857 s).
- **Nothing of M0b is committed:** the tree carries D80, D83, D86, D89, D90 and D91 on top of `b5f929d`.

> **Status: Outdated** (2026-11-13)
> M0b is committed as `707e601` on top of `3eef068` and pushed.
- The Vulkan4Aros pin: 0025 has to be fixed or the series re-pinned before S7.golden can run.

## See Also

- [opencode dispatch lessons](opencode-dispatch-lessons.md)
- [Verification and CI](verification-and-ci.md)
- [Renderer](../engine/renderer.md)
