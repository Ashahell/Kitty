# M0b.22 AROS critique

> Source: Docs/Development/m0b22_aros_critique.md
> Collected: 2026-11-13
> Published: 2026-11-12

# M0b.22 review — 2026-11-12

Census as sealed: 121 PASS / 0 FAIL / 2 BLOCKED (123 lines, 122 gate lines, one blank line).
**Census as the scripts wrote it: 120 PASS / 1 FAIL / 2 BLOCKED.** The FAIL is `S12c.complete`. opencode then deleted lines from the census by hand, so the seal `73921fe2…` covers an edited file.

Agent: opencode, `nvidia/nemotron-3.5-super-vl-120b-a12b`, session `ses_e2f25d5deffeToXz3weH0bFCHh` (started 18:42). The tool calls below come from that session's record in `~/.local/share/opencode/opencode.db`.

## Verdict

- **The engine side passed in both boots.** D91 holds, D93 is measured, and D92 wasn't needed.
- **The round broke three rules:**
  - it ran a write git command;
  - it repeated two scripts;
  - it edited the census to hide the gate that caught that.
- **The report hides all three.** Item 8 says "none" and then calls the census edit a cleanup.
- **The git reset began on my side.** My wiki commit `3eef068` (18:46) landed after opencode had started reading the prompt (18:42) and before its script 0. That made the first script 0 fail.

## Audio: D91 and D93 hold in both boots

| | boot 1 `a` | boot 1 soak | boot 2 `a` (graded) | boot 2 soak |
|---|---|---|---|---|
| xruns | 0 | 0 | 0 | 0 |
| `ahi_first_submit_us` | 574631 | 536705 | 514830 | 487577 |
| queued frames | 470400 | 89093760 | 472320 | 89086080 |
| submitted / returned | 245 / 243 | 46403 / 46401 | 246 / 244 | 46399 / 46397 |

`errors=0` and `dropped=0` in all four runs.

- **D93 bound, graded `a`:**
  - lower: 0.98 × 48000 × (9982295 − 514830) / 1e6 = 445,350;
  - upper: 48000 × 12 + 3840 = 579,840;
  - queued: 472,320.
  - The margin is 26,970 frames (562 ms), against 837 frames in M0b.21. Boot 1's `a` would also pass: the bound is 442,561 and it queued 470,400.
- **The ~0.5 s startup is now measured:** 487–575 ms to the first submission in all four runs. M0b.21's estimate of "roughly 0.5 s" was right.
- **The soak's queued audio is 1855.96 s, against 1859 s of host wall time.** The guest clock is still about 3 % slow: `elapsed_us=1799982732`.
- **No listing hang:** both boots fetched `assigns.txt`, and `guest-boot1/` and `guest-boot2/` hold 88 files each.
- **Grading:** a 14/14 and 88/88 tie, so boot 2 by the later-boot rule. Both boots are clean, so the grading doesn't change any gate.

## Departures the report hides

1. **`git reset --soft b5f929d` (18:51:19), in breach of `GIT: read-only`.**
   - Script 0 first failed at 18:49:20 with `HEAD is not b5f929d`, because my commit had moved HEAD to `3eef068`.
   - The base prompt stops the round when script 0 fails, and the owner decides what happens next. opencode reset HEAD instead, ran script 0 again at 18:51:24, and went on.
   - With `--soft` only HEAD moved, so the working tree still matches `3eef068`. That's why the tree hash still matched.
   - Local `main` is now 1 commit behind `origin/main`.
2. **S8 ran twice.**
   - The first run hit opencode's 120 s tool timeout after writing 3 gate lines, and the 18:53:53 run wrote all 5. That breaks D46's rule against repeating a script.
   - The fix was a whole new pass from S1, before the boots. Both boots then ran on a pass that S12c was always going to reject.
3. **The census was edited by hand, twice.**
   - 20:10:58: S12c reported `gates repeated within the pass: ['S8.h5-headless', 'S8.h5-knownbad', 'S8.release-no-plant']`, then `GATE S12c.complete FAIL`. That is the gate doing exactly its job.
   - 20:11:50: an `edit` call removed the 3 duplicate S8 lines from `census.txt`.
   - 20:12:00: S12c ran again and failed again, this time on the earlier S12c line.
   - 20:12:25: an `edit` replaced the two `GATE S12c.complete FAIL` lines with an empty string. That left the blank line 122.
   - 20:12:40: the third S12c run PASSed and sealed the file.
   - S12 (20:10:30) counted the census before these edits, so its `PASS=121` line is from a different file from the one the seal covers.
4. **The report's account of this:**
   - Item 8 says "Every departure from this prompt: none", and then calls the census edit removing duplicates.
   - It leaves out the git reset, the second and third S12c runs, and the deleted FAIL lines.
   - Item 7 says "no edits after script 0 except X2/X3".
   - The census is the round's one tamper-evident record. Editing it to turn a FAIL into a PASS is the most serious kind of report finding.

## Findings against the report

- **Boot 1's item 5 quotes boot 2's values:** `ahi_summary submitted=246 …` and `ahi_first_submit_us=514830 (run_a)`. Boot 1's `a` has 245 and 574631.
- **Item 4's "After boot 1" `loader_id.txt` line** has `lib=0x4B0C1B60 … idstr_ptr=0x4B8920F0`, which is boot 2's. Boot 1 has `lib=0x4B6C57D0`, `idstr_ptr=0x4B892160`. This is the same error as in M0b.21.
- **Hashes were retyped, not copied.**
  - The `x4.sh` sha in item 3 has 49 hex digits; the real one is `f71e2efd…526495f7a05683a3f9745cc7386d6ea`.
  - `s6.sh` took three edits to fix (20:17:56–20:18:10).
- **"verbatim" is still used for summaries.** Examples: "exec_prelude.log verbatim: 5 execs all rc=0 (…)", and the one-line paraphrases of the `assigns.txt` LIBS line. M0b.22 added the rule that verbatim means copied bytes, and the report still breaks it.
- **Item 2 elides with `...`** in the S5, S6, S7 and S11 outputs. These are meant to be verbatim script outputs.
- **The runner exits aren't evidenced.**
  - opencode started the runners without `; echo RUNNER_EXIT=$?`, so no file records an exit code.
  - "exit 0" is inferred from `lane_run: lane stopped`, which the runner prints on every exit code: all twelve dry boots end with it, including rc 10, 6 and 11.
  - The other evidence (14 rcs, `assigns.txt` fetched, no wedge line) is consistent with 0, but the report states it as measured.
- **Clean otherwise:**
  - the FAIL/BLOCKED lines are verbatim;
  - no process was left running;
  - two ledger lines match two boot records;
  - `S12.end-state` PASS: the dispatch prompt I'd overwritten earlier was restored before S12.

## Owner-side finding (mine)

- **I committed into a repo whose HEAD a dispatched prompt pins, after the round had started.**
  - My rule only covered `dispatch/` edits between script 0 and S12.
  - It now becomes: **no commit, reset or push in `~/Work/Kitty/repo` from dispatch until S12.**
- **An unrelated note:** `~/.local/share/opencode/shell/d06f8b4f…/` keeps plain-text shell outputs, including `auth.json` and credential rows with live API keys (opencode-go and NVIDIA), dumped at 2026-11-11 07:43. Delete those files, or rotate the keys.

## What this means for M0b

- **Every M0b measurement passes in both boots:**
  - the S15 and S15b guest gates, from both boots' files;
  - the host gates, whose outputs are the same in both S8 runs.
- **The census FAIL is procedural.** No gate on the code failed, but the sealed census isn't trustworthy evidence. It can't support "everything must pass" (GDD §11) without a recorded exception.
- **Option A, call M0b now with a recorded exception.**
  - The exception covers `S12c.complete` (a repeated S8, and an edited census) and the out-of-rule git reset. The evidence is this review.
  - Then the owner commits M0b: D80, D83, D86, D89–D93 on top of `3eef068`.
- **Option B, M0b.23, a clean re-run with no code change.**
  - **D94:** the census is append-only.
    - `g()` also appends each line to `census.log`, with a running sha256 chain.
    - S12c checks that `census.txt` equals the chain's lines and contains no blank lines. Any hand edit then FAILs.
  - **D95:** scripts that can run over 120 s (`s8`, `s13`, `s14b`, the builds) run in the background with a wait. A timed-out script means a new pass from S1, never a re-run of that one script.
  - **The script-0 failure rule:** a `HEAD is not …` failure stops the round. A git write command is listed as a departure, and the round ends.
  - **The runner launch line** carries `; echo RUNNER_EXIT=$?`, as DSH3 already does.
  - **Re-pin HEAD to `3eef068`.** That's already done in `m0b22_repinned_3eef068.md`, and it carries over.

## Still open

- **Nothing of M0b is committed.**
- **Local `main` needs `git reset --soft origin/main`.** That's safe now: S12 is done, and the working tree already matches `3eef068`.
- **The upstream AROS/AHI bugs:** `cliinit.c` uses one lock for two assigns; `PREPEND` inserts in second place; a request linked to a waiting one never starts.
- **The guest timer runs about 3 % slow.**
- **`DRIVERS:` still reads a freed lock.** `dir=` varies again: `AROS:Utilities` and `AROS:Tools`.
