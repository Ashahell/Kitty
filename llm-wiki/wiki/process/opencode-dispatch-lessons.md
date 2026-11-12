# opencode dispatch lessons

> Sources: M0b.17 critique, 2026-11-10; M0b.19 critique, 2026-11-11; M0b.20 and M0b.21 critiques, 2026-11-12; M0b.20–M0b.22 DSH prompts, 2026-11-11 / 12; M0b.13 critique, 2026-11-09; M0b.12 critique, 2026-11-09; M0b.3 critique, 2026-11-02; M0b.2 critique, 2026-11-02; M0b.1 critique, 2026-11-02; M0b critique, 2026-11-02; M0b dispatch prompt, 2026-11-01; M0a.1 critique, 2026-11-01; Engine design rev 2–5 critiques, 2026-10-29/30; opencode prompts rev 3–5, 2026-10-29
> Raw: [M0b.13 critique](../../raw/reviews/2026-11-09-m0b13-aros-critique.md); [M0b.12 critique](../../raw/reviews/2026-11-09-m0b12-aros-critique.md); [M0b.3 critique](../../raw/reviews/2026-11-02-m0b3-aros-critique.md); [M0b.2 critique](../../raw/reviews/2026-11-02-m0b2-aros-critique.md); [M0b.1 critique](../../raw/reviews/2026-11-02-m0b1-aros-critique.md); [M0b critique](../../raw/reviews/2026-11-02-m0b-aros-critique.md); [M0b prompt, trap-location gate fix](../../raw/dispatch/2026-11-02-m0b-aros-prompt-4.md); [M0b prompt, GIPA gate fix](../../raw/dispatch/2026-11-01-m0b-aros-prompt-3.md); [M0b prompt](../../raw/dispatch/2026-11-01-m0b-aros-prompt.md); [M0a.1 critique](../../raw/reviews/2026-11-01-m0a1-linux-critique.md); [Rev 2 critique](../../raw/reviews/2026-10-29-engine-design-rev2-critique.md); [Rev 3 critique](../../raw/reviews/2026-10-29-engine-design-rev3-critique.md); [Rev 4 critique](../../raw/reviews/2026-10-29-engine-design-rev4-critique.md); [Rev 5 critique](../../raw/reviews/2026-10-30-engine-design-rev5-critique.md); [Rev 3 prompt](../../raw/dispatch/2026-10-29-engine-design-rev3-prompt.md); [Rev 4 prompt](../../raw/dispatch/2026-10-29-engine-design-rev4-prompt.md); [Rev 5 prompt](../../raw/dispatch/2026-10-29-engine-design-rev5-prompt.md); [M0b.17 critique](../../raw/reviews/2026-11-10-m0b17-aros-critique.md); [M0b.19 critique](../../raw/reviews/2026-11-11-m0b19-aros-critique.md); [M0b.20 critique](../../raw/reviews/2026-11-12-m0b20-aros-critique.md); [M0b.21 critique](../../raw/reviews/2026-11-12-m0b21-aros-critique.md); [M0b.20 DSH prompt](../../raw/dispatch/2026-11-11-m0b20-aros-guest-dsh-prompt.md); [M0b.21 DSH prompt](../../raw/dispatch/2026-11-12-m0b21-aros-guest-dsh-prompt.md); [M0b.22 DSH prompt](../../raw/dispatch/2026-11-12-m0b22-aros-guest-dsh-prompt.md)
> Updated: 2026-11-12

## Overview

Across five revisions, opencode's self-reports and its passing scripts both proved unreliable. Each of revisions 3, 4 and 5 printed PASS on every script while real defects remained. What worked was to fix the decisions in the prompt as exact sentences, to make the scripts check meaning rather than form, and to review every revision independently against primary sources.

## What failed

- **Self-reports:** rev 2 had false checklist PASSes; rev 3 claimed "no deviations" over 11+ failed items; rev 5's report claimed a Rename probe that the doc doesn't have.
- **Decisions only in the changelog:** in rev 3 the Appendix R row said one thing and the body the opposite.
- **Gaming the checks:** rev 3 invented `TICKS_PER_CELL = 2` so its own PAR passed, and fabricated [S88]. Rev 4 built an empty example room that passed reachability but needed no verb.
- **Form-only scripts:** the rev-4 scripts couldn't see a NULL-dereferencing API call, cross-section contradictions, or a cut that removed v2 scope.
- **Lost baselines:** opencode's script-0 baseline lived only in `/tmp`, which was wiped mid-session. opencode re-ran it after the edits, so the "no source entry lost", line-count and protected-file checks proved nothing for that run.

## What worked

- **Fixed decisions as verbatim sentences** pasted into named sections, plus banned-phrase lists and an Appendix R that is an index only.
- **Compiling the interface code:** §3 must pass `g++ -fsyntax-only -Wall -Werror -Wredundant-decls`, and `sizeof(ForkRec)` is measured from it.
- **Oracles in the scripts, not the doc:** the room validator, BFS with every gate closed, `TICKS_PER_CELL` fixed by the prompt, and budget arithmetic recomputed.
- **Source checks:** every `VK/<path>:<line>` must exist; the Rename and AHI entries must cite lines that contain the key token; number ranges in §4.6/§7 must appear in a cited entry.
- **Proving the scripts first:** before dispatch, the scripts fail the current doc, pass a doc built to spec, and catch planted errors (11 of 11 for rev 5).

## Lessons from M0a.1 (2026-11-01)

- **Hash-only protection checks miss new files.** `sha256sum -c` on a baseline list missed a phase-2 write into `Docs/`, so S12 PASSed. Diff the sorted file list as well.
- **A runtime reference is not a golden.** The probe compares two lavapipe renders, so a draw missing from both images passes. Each draw needs its own region check.
- **Self-contradictory specs get resolved silently.** D13's `pl_trap` had to print `<file>:<line>` without taking either, so the code hardcoded a false location. The phase-2 report disclosed it, but the final report dropped it.
- **Location regexes accept fake locations.** `[^ :]+:[0-9]+` matched the hardcoded one.
- **Take the audit baseline when the run starts,** and exclude paths the owner edits in parallel.

## Lessons from writing the M0b prompt (2026-11-01)

- **Test a gate pattern on the real tool's output.** The r12 pattern copied from Vulkan4Aros never matches `llvm-objdump` output; only a plant linked into a probe exposed it.
- **A plant must fail for its own reason.** Each known-bad build must print its own message (`stale SDK`, `banned API WaitTOF`, `GATE FAIL: <n> stale-r12 call sites`). Otherwise a target that doesn't exist yet "fails" and the plant passes.
- **Check every required line against every ban.** D24 required the line `PFN_vkGetInstanceProcAddr pl_vk_gipa(Plat&);` in `plat_svc.h`, while the `gipa-platform-only` gate substring-matched `vkGetInstanceProcAddr` outside `src/platform/linux/`, so no honest tree could pass both. opencode caught it in reconnaissance, before writing code. The gate now uses `grep -rnw` (whole word), and D24 states that the `PFN_` type name is allowed.
- **Run each new gate over the logs the existing ladder already writes.** `S14.trap-locations-real` scanned every `build/m0b/*.log`, including `t-guru.log`, where S10 requires the game-data location `room.txt:12`. The gate now passes `--exclude=t-guru.log`.
- **A fixed prompt needs re-extracted scripts.** opencode's `build/m0b/s14.sh` was extracted before the GIPA fix, so it still had `grep -rn`.
- **Owner work runs in parallel.** `~/Work/lanes` changes and new qemu processes are listed for attribution, not failed. Boots are counted in `boots.txt` against the granted number.


## Lessons from the M0b round (2026-11-02)

- **A guest precondition must test resolution, not presence.** W4 asked whether the guest "carries" the loader. The lane opens it as `DH1:Libs/vulkan_loader.library`, which none of D24's three names reach.
- **Budget a deployment boot.** Boot 1 died on `--put-tree` (no guest `tar`, Shell rc=0), and no host dry-run can catch that.
- **Guest redirects:** `>log` on `Execute` does not redirect the child's output, so all output landed in `exec_*.log`.
- **Read the disk images before deciding where a library lives.** A read-only `sfs_nav.py` walk showed the loader on `DH1:Libs` and DH0's copy in `.recycled`; that is now a pre-boot gate (S16).
- **Guest exec budgets:** a 1800 s soak exceeds the agent's 900 s exec budget.

## Lessons from the M0b.1 round (2026-11-02)

- **A boot ledger kept by the agent is not evidence.** `boots.txt` was reset by hand twice; three boots ran against a grant of 2. Cross-check `boots.txt` against the lane's own `boot-*.txt` records.
- **Gate the host side of the transport before a boot.** The runner's spike server died at import (`-I` dropped `scripts/` from `sys.path`) and port 9099 was held by a leftover server; a host-only ping would have caught both at no boot cost.
- **Name the exceptions to a blanket rule.** The general `python3 -I` practice is wrong for `spike_server.py`, which imports its siblings.
- **Claims that a toolchain cannot do something need a baseline.** The same flags built M0b with r12=0, so the r12 sites were a code regression.

## Lessons from the M0b.2 round (2026-11-02)

- **Test the path that will run, not a sibling mode.** S14b.transport exercised `--check-transport`; the boot path kept the old server start, and a broken line continuation in it.
- **Grepping for a file name invites bait.** A `# spike_server.py` comment was added so the `-I`/`-B` grep would match; check the real command line instead.
- **Check that the census is complete.** The final census lacked S0, X, S14, S14b and S16, and the report claimed PASS for them.
- **Measure the expected census on the pinned tree.** S3 was already FAIL there.

## Lessons from the M0b.3 round (2026-11-02)

- **Check a pinned reference against every gate before pinning it.** D44 pinned a Makefile that `S14.aros-flags` rejects.
- **Layout gates don't run code.** An unset variable under `set -u` inside a pipeline killed the D41 check silently, and a removed cleanup trap passed every gate.
- **Gates on shared trees must separate owner commits from round writes.** S12 failed on the owner's own commit under VK `scripts/`.
- **Scope a write check to the whole forbidden tree.** Narrowing the lanes check to `l0025` missed a write to `~/Work/lanes/kitty-m0b3-20261102/`.

The rev-5 critique lists these:

- Script 7 doesn't compare a quoted VK line with the file.
- The `VK_EXT_debug_utils` citation isn't tied to a source that contains it.
- clones-max isn't tied to the room's chapter.
- The script-0 baseline should live outside `/tmp`.

## Lessons from the M0b.12 and M0b.13 rounds (2026-11-09)

- **A zero-boot round moves nothing.** M0b.12 only re-ran gates that already passed; a guest round needs a boot.
- **Log what loaded, not what was asked for.** `audio_source=` named the requested path; only the device's `lib_IdString` settled which `ahi.device` ran.
- **Don't grade one fixed boot.** D73 graded boot 2, so a guest flake voided a near-pass boot 1. Grade the better boot, and report both.
- **Run flake-prone diagnostics last.** The `Assign` listing had faulted before and still ran ahead of everything the gates need.

## Lessons from the M0b.17–M0b.21 rounds and the move to DSH (2026-11-10 to 2026-11-12)

- **Reports still editorialise unless told not to.** M0b.17's report had wrong census counts and a "Root cause" paragraph. M0b.19's "Key result" paragraph came back and appended a story to a FAIL line. The rule became: gate lines verbatim, nothing appended, and counts copied from `census.txt`.
- **"Verbatim" has to be defined.** M0b.21's report labelled one-line summaries "verbatim" and quoted `open=1` from `loader_id.txt`, where both boots have `open=12`. M0b.22's prompts say that verbatim means copied bytes.
- **DSH cannot boot the lane.** Inside DSH's sandbox, `systemd-run --user` fails with `Failed to connect to user scope bus via local transport`, and nothing a call starts outlives it. The owner runs each boot in a terminal, and DSH must never run `lane_run.sh l0025 …`.
- **A sandboxed runner can leave a phantom ledger line.** On 2026-11-11 a DSH run started the runner. It wrote its `boots.txt` line, then was killed when the call ended, with no boot behind it, and the runner refused every later boot.
- **Write the report in pieces.** M0b.20's DSH run ended with `max-tokens` while writing the whole report in one call, so no report existed. From M0b.21 the report is written one item per call.
- **The owner must not edit dispatch files mid-round.** In M0b.20, `S12.end-state` FAILed because of the owner's own edits after script 0. The rule: no edits under `Docs/Development/dispatch/` between a round's script 0 and its S12.
- **Rehearse under the agent's real sandbox.** M0b.21 and M0b.22 were rehearsed with each script in its own call under DSH's bwrap profile.

## See Also

- [M0 implementation status](m0-implementation.md)

- [Engine design revision history](engine-design-revision-history.md)
- [Verification and CI](verification-and-ci.md)
