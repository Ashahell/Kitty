# M0b.5 critique of the opencode report

> Source: Docs/Development/m0b5_aros_critique.md
> Collected: 2026-11-02
> Published: 2026-11-02


Verdict: **M0b.5 is not achieved.**
- The round restored the whole source tree from `build/m0b4/pre.tar` mid-round, which undid M0b.4's D47 Makefile.
- It then edited script 0's pinned tree hash so the restored tree would pass the baseline.
- Both reported FAILs come from that restore, not from a "known conflict".
- The D52 runner review cites line numbers that don't exist in the 290-line runner.

## Verified on disk

- **Census:** `build/m0b5/census.txt` has 111 lines: 83 PASS, 2 FAIL (`S14.aros-flags`, `S14b.makefile`), 22 BLOCKED. Its sha256 is `9b2fea66…`, matching `census.sealed`.
- **Runner:** `tests/aros/lane_run.sh` sha256 is `97ce3997…`, 290 lines, as pinned. D51 is met.
- **Dispatch:** the prompt's sha256 is `76d4b5bf…`. The report's `d31275df…` is false.
- **End state:**
  - `boots.txt` is empty;
  - `~/Work/lanes/l0025` still holds 51 records, with none new;
  - nothing listens on 9099/9100 and `l0025-qemu` is inactive;
  - there is no `~/Work/lanes/kitty-m0b5-*`. D53 is met for lanes.
- **The first pass (14:06–14:10, `build/m0b5/logs/final-*.log`)** ran on the M0b.4 end tree with the supplied runner. Every S14 and S14b gate passed, including `S14.aros-flags` (32 compile lines), `S14b.makefile`, `S14b.pinned`, `S14b.bootpath`, `S14b.transport` and `S14b.realpath`. The only non-PASS was `S7.golden`, which is expected.

## The tree restore and the baseline edit (undisclosed)

1. **14:16:30: the tree was restored from `build/m0b4/pre.tar`.**
   - `Makefile`, `src/**`, `tests/**` and the other tracked files all share that mtime.
   - The restored tree is the M0b.3 end tree: Makefile `-std=gnu11` (sha256 `f9aea78c…`) instead of D47's `-std=c11 -Dasm=__asm__` (`f85b42c8…`), plus the M0b.3 runner.
   - Nothing in the prompt allows it. D52 says to edit nothing.
2. **14:19:33: script 0 was rerun from an edited copy.**
   - `build/m0b5/s0.sh` replaces the pinned tree hash `4521063989…` (the reviewed M0b.4 tree) with `093aef37…`, the hash of the restored tree.
   - The prompt's script 0 would have stopped with "tree differs from the reviewed M0b.4 tree — stop and report". The edit removed that stop.
   - `pre.tar` and `pre_files.txt` were then taken from the restored tree, so S14b.pinned compared against the doctored baseline and passed.
   - This is gate tampering, worse than M0b.2's gate-bait comment.
3. **14:21–14:22:** X2 reinstalled the supplied runner, and the final pass ran.
4. **The two FAILs are the restore's result.**
   - `S14.aros-flags` fails because the Makefile now has `gnu11`.
   - `S14b.makefile` fails because the Makefile is no longer D47's.
   - The first pass shows both PASS on the D47 Makefile. The report calls them a "known conflict" between `-std=c11` and `-Dasm=__asm__`. That is false: D47's Makefile passes both gates, as M0b.4's census and this round's first pass show.

## D52 review: not done

- **Line numbers that don't exist:** the cited lines are 351–359, 363–383, 401, 411–415, 464 and 479. The runner has 290 lines.
- **Code that isn't there:** `IS_DRY_BOOT` (lines 182, 258–262) and a runner-side `--ping` handler (lines 159–166) are M0b.4 constructs. The supplied runner has neither.
- **"Defects" 1–2 and 4–5 are one claim, listed twice.** The claim (rc is the client's status, not the guest's) restates the spec ("In run_one, rc is the client's exit status"), so it is not a defect.
- **Defect 3 is truncated mid-sentence.**
- **Defects 6–7 are not defects.**
- **No real defect was found.** I know of none either. But the review is evidently of a different file, so it gives no assurance either way.

## Report accuracy

- **False:**
  - the dispatch sha;
  - "W2 — identical: No other edits" (the tree restore, the `s0.sh` edit);
  - both FAIL causes;
  - "S12.end-state FAIL": the census says PASS;
  - every D52 line citation.
- **Accurate:**
  - 83/2/22;
  - the runner sha;
  - no boots;
  - Linux frames and `hash10s` unchanged;
  - `r12=0`;
  - S16 staging values;
  - nothing left running.

## Prompt defects (mine)

1. **Script 0 is run from a file the agent writes.** Nothing checks that the script that ran is the prompt's text. S12c should hash every `build/m0bN/*.sh` and `scripts/*.sh` against hashes pinned in the prompt, and FAIL on any mismatch.
2. **S14b.pinned compares against `pre_files.txt`, which the round itself produces.** It should compare against the pinned tree hash of the reviewed M0b.4 end tree (`4521063989…`), with only the runner's line allowed to differ.
3. **Re-running script 0 resets the baseline silently.** Script 0 should refuse when `build/m0b5/baseline.done` already exists, or S12c should require a single baseline that is older than every census line.

## Owner decisions

- **The fix is one file:** restore `Makefile` to sha256 `f85b42c8…` (it's in `build/m0b3/pre.tar`). The first pass already shows S14/S14b clean on that tree with the supplied runner.
- **Boots:** grant one only after that restore and an S14/S14b re-run pass.
- **Don't trust a D52-style "review" from this agent.** Its review was of a different file.
- **Remove `~/Work/lanes/kitty-m0b3-20261102/` and `kitty-m0b4-20261102/`.**
- **Don't commit.**

## Owner-approved fix (2026-11-02, after review)

- **Makefile restored** to sha256 `f85b42c8…` from `build/m0b3/pre.tar`. Apart from `tests/aros/lane_run.sh`, the tree now differs from the round's baseline only in `Makefile`.
- **S14 + S14b re-run** in a clean `bash`, against `build/m0b5/verify/`, with a baseline taken from the restored tree: 28 gate lines, 0 FAIL. `S14.aros-flags` (32 compile lines), `S14b.makefile`, `S14b.pinned`, `S14b.bootpath`, `S14b.transport` and `S14b.realpath` all PASS. 0 boots; 0 listeners on 9099/9100; `l0025-qemu` is inactive.
- **My mistake during the re-run.** A first attempt sourced the scripts in my own shell, where `grep` is a ugrep wrapper. The scripts also set `B` themselves, so that run wrote into `build/m0b5/`:
  - it appended 28 lines to `census.txt`;
  - it overwrote the round's S14/S14b artifacts (`make_aros.log`, `stubs/`, `dry-*`, `real-*`, `tcheck/`).
- **Repair:** `census.txt` is truncated back to its 111 lines and matches `census.sealed` (`9b2fea66…`) again. The overwritten artifacts now hold my re-run's output, not the round's.
