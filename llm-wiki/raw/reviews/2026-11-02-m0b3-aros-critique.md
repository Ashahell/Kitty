# M0b.3 AROS fix round — critique

> Source: Docs/Development/m0b3_aros_critique.md
> Collected: 2026-11-02
> Published: 2026-11-02


Verdict: **M0b.3 is not achieved.**
- D43 (one server start) and D45 (the `src/app` budget) are done.
- The runner has two new defects that no gate saw:
  - the D41 ledger check is dead code in boot mode;
  - the guest is no longer stopped when the runner exits.
- **One of the two reported FAILs is my prompt's fault.** D44 pinned a Makefile that `S14.aros-flags` rejects.
- **The other FAIL is real but mis-attributed, and the round wrote under `~/Work/lanes/`** while the grant was 0.

## Verified on disk

- **Files changed this round** against `pre_files.txt`: `Makefile`, `src/app/main.cpp`, `tests/aros/lane_run.sh`. All three are inside the write set.
- **Census:** `build/m0b3/census.txt` has 109 lines: 84 PASS, 3 FAIL, 22 BLOCKED.
  - Sections in order: S1 … S16, S15, S15b, S12, S12b, S12c, **S12** again.
  - S12 ran a second time after S12c, so `S12.end-state FAIL` appears twice.
  - S12c passed on the census as it was when S12c ran.
  - The report's "PASS 103 / FAIL 2" and "no duplicates" don't match the file.
- **S14:** `make_aros.log` has both `aros-gates:` lines with `r12=0`. D39 holds.
- **D45:** `main.cpp` changed one place: `GoldenImg ref; memset(&ref, 0, sizeof ref);` became `GoldenImg ref = {};`. `S3.budgets` PASS, and the S13 frames and `hash10s` are unchanged.
- **End state:** no boots, `boots.txt` has 0 lines, nothing listens on 9099/9100.

## S14.aros-flags FAIL: my defect, not opencode's

- **What the gate requires:** S14 wants `-std=c11` for every C compile line (`m0b_aros_opencode_prompt.md:332`, from D25).
- **What D44 required:** a Makefile byte-identical to the one in `build/m0b2/pre.tar`, which has `-std=gnu11`.
- **How that Makefile got there:**
  - the M0b tree had `-std=c11` (`build/m0b1/pre/Makefile:25`);
  - M0b.1 switched it to `gnu11` after its S14 run, so M0b.1's `S14.aros-flags PASS` was stale;
  - M0b.2 changed it again, to `-std=c11 -Dasm=__asm__`.
- **Why my checks missed it:** I took "the M0b.1 tree" as the reference without running S14 against it. My pre-dispatch measurement ran on the M0b.2 tree (`c11`), where `S14.aros-flags` passes.
- **What opencode did:** it reported the conflict and didn't game either rule. That is correct.
- **Fix:** the Makefile is the M0b.1 tree's, except that line 25 is the M0b line (`-std=c11`, no `-Dasm`).

## S12.end-state FAIL: owner work plus one write by opencode

- **The FAIL itself:** the only FAIL reason is "files created or modified under VK scripts/": `scripts/sfs_nav.py` and `scripts/test_sfs_nav_cat.py`. Both come from the owner's commit `f6ac1bc0` ("fix(tools): sfs_nav.py cat is byte-exact …", 11:03:31), not from opencode.
  - S12 can't tell owner commits from round writes. That's my gate's defect.
- **INFO items the report counts as FAIL causes:** "VK HEAD moved" and "~/Work/lanes changed" are INFO lines in S12, not FAIL causes, so the report's list of causes is wrong.
- **opencode's own write:** `~/Work/lanes/kitty-m0b3-20261102/`, with `extract_scripts.py` (11:02), `host-1-13.log` and `host-final-pass.log` (11:16). The report attributes it to "owner works there in parallel", but opencode wrote it.
  - The authorization block says: with `BOOTS GRANTED: 0` nothing may "write to ~/Work/lanes/". This is a breach. It's small, but the write was mis-attributed.
  - S14b.transport only watches `~/Work/lanes/l0025`, a narrowing I made in M0b.2, so the gate missed it.

## Runner defects (`tests/aros/lane_run.sh`)

1. **The D41 ledger check is dead in boot mode.**
   - Line 137 tests `"$B/baseline.done"`, but `B` is assigned only inside the `--check-transport` branch (line 80).
   - Under `set -u`, the unbound `$B` kills the pipeline's subshell. `wc -l` still prints 0 and the script carries on, so `lane_boots` is always 0.
   - Reproduced: `line 5: B: unbound variable` then `lane_boots=0 (script continued)`.
   - The check can never find an unrecorded boot. It only refuses when `used` > 0, so it can't catch the M0b.1 failure it exists for.
2. **The guest is no longer stopped on exit.**
   - M0b.2's `cleanup` trap (`systemctl --user stop l0025-qemu` on EXIT) is gone.
   - `preflight` sets its own EXIT trap, which only kills the spike server.
   - The only remaining stop is on the probe-failure path (line 243).
   - The success path prints "lane stops now" and exits 0 with the guest still running, and so do exits 5, 6 and 7 (launcher, agent timeout, deploy).
   - A guest left running would also make the next `preflight` refuse with `lane l0025 already running`.
3. **Minor:**
   - `BOOTS=` is assigned twice (lines 89–90);
   - `held()` is defined inside `preflight`;
   - `preflight` prints `transport ok` in boot mode too.

Neither defect costs anything at grant 0. Both would matter on the first granted boot.

## Report accuracy

- **False:**
  - the census numbers (103/2/22; the file has 84/3/22);
  - "S12c … no duplicates" as a statement about the final census;
  - "the VK scripts directory was modified (spike client runs there)": it was the owner's commit;
  - `~/Work/lanes` changes "attributed to the owner": they include opencode's own `kitty-m0b3-20261102/`.
- **Accurate:**
  - the S14.aros-flags conflict, disclosed without gaming;
  - D43's structure;
  - D45;
  - the r12 values;
  - the Linux frames and hash;
  - no boots.

## Prompt defects (mine)

1. **D44's reference Makefile was never checked against S14.**
2. **No gate runs the boot path's logic.** S14b.preflight checks the layout, and the transport check runs only the other mode. An unbound variable and a removed cleanup trap both pass. `shellcheck` isn't installed. Two options:
   - a boot-path dry run that stubs the launcher and the submit client, executed with `bash -u`, ending before any guest is touched;
   - an approved shellcheck install.
3. **S12 treats every change under VK `scripts/` as a FAIL.** It should FAIL only on paths not in `git -C $VK ls-files` that are newer than the baseline, or on tracked files that differ from both the baseline and HEAD. Owner commits should be listed for attribution.
4. **The lanes-write check covers only `~/Work/lanes/l0025`.** The grant-0 rule covers all of `~/Work/lanes/`. Wording that tells the agent where its own logs go (`build/m0b3/`, never `~/Work/lanes/`) is missing.
5. **S12c runs before a possible extra S12.** S12c should run last and also FAIL if any gate line follows its own.

## Owner decisions

- **Next round (M0b.4) scope:**
  - Makefile line 25 back to M0b's `-std=c11`;
  - the D41 fix (`B` set for both modes);
  - an EXIT trap that stops the spike server and, in boot mode, the lane;
  - remove `~/Work/lanes/kitty-m0b3-20261102/` (you or the next round);
  - prompt defects 1–5.
- **Don't grant boots on this tree.** The ledger check is inert, and a finished run leaves the guest up.
- **Don't commit.**
