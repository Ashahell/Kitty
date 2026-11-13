# M0b.23 AROS critique

> Source: Docs/Development/m0b23_aros_critique.md
> Collected: 2026-11-13
> Published: 2026-11-13

# M0b.23 review — 2026-11-13

Census: **120 PASS / 0 FAIL / 2 BLOCKED** (122 gate lines, no blank line). BLOCKED: `S7.golden`, `S15.nvk-smoke`, as in every recent round.

Agent: opencode, `nvidia/nemotron-3.5-super-vl-120b-a12b`, session `ses_e2c7705bdffeVEavvSDi9RVtlD` (07:12–08:45). The tool calls below come from that session's record in `~/.local/share/opencode/opencode.db` (132 calls).

## Checked against `build/m0b23/`

- **The seal `c63f77c8…` matches `census.txt`.** Nothing but the scripts wrote the census: the agent's only `write` and `edit` calls touched `report.md`.
- **`census.chain`, 52 lines, checked independently.**
  - The first N lines of `census.txt` hash to each line's value, for all 52 lines.
  - From `s1.sh start`, every pass script appears once, start then end, in order.
- **Scripts:**
  - The 12 scripts that X0 extracts are byte-identical to a fresh extraction of the dispatched prompt (`94e9583a…`).
  - X1's and sX's 15 scripts match the rehearsal pins.
  - The report's `sha256sum` list matches the files.
- **Pins:**
  - HEAD `3eef068`; `git status` shows the same 21 entries as at the start.
  - Runner `2d182f1d…`, `plat.cpp` `d5c6b2e6…`, `audio.cpp` `9ddd9eff…`.
  - The dispatch prompts are unchanged (`S12.end-state` PASS).
- **Runners:** `RUNNER_EXIT=0` in both runner logs (D97 worked). Each boot has 14 `rc_*.txt` and 88 files, and `dead.txt` is empty.
- **Ledger and grading:**
  - Two `boots.txt` lines (07:23:18, 07:59:20) against two boot records (`boot-20261113-0723`, `-0759`).
  - `graded_boot.txt`: `graded=2 boot1_rc=14 boot2_rc=14 boot1_files=88 boot2_files=88`. A tie, so boot 2 is graded under D92.
- **No process from this round is still running.** The QEMU guest still up is `aros_r2card`, from the Vulkan4Aros card round that ran at the same time.

## Verdict

- **This is the first clean M0b census since D91.** No gate failed, the census is untouched, and the chain proves one complete pass.
- **D94 and D95 did their job:**
  - A tool timeout hit while script 8 ran (below); script 8 survived because it was detached, and it ran once.
  - Nobody touched the census.
- **The round still broke five rules, none of which touches the evidence.** The report hides all five and says "none" in item 8.

## Audio: D91 and D93 hold in both boots

| | boot 1 `a` | boot 1 soak | boot 2 `a` (graded) | boot 2 soak |
|---|---|---|---|---|
| xruns | 0 | 0 | 0 | 0 |
| `ahi_first_submit_us` | 502113 | 522846 | 487610 | 507523 |
| queued frames | 472320 | 89076480 | 474240 | 89082240 |
| submitted / returned | 246 / 244 | 46394 / 46392 | 247 / 245 | 46397 / 46395 |

`errors=0` and `dropped=0` in all four runs.

- **D93 bound, graded `a`:**
  - lower: 0.98 × 48000 × (9982206 − 487610) / 1e6 = 446,626;
  - upper: 48000 × 12 + 3840 = 579,840;
  - queued: 474,240.
  - The margin is 27,614 frames (575 ms).
- **Boot 1's `a` would also pass:** the lower bound is 445,963 and it queued 472,320.
- **Soak:** 89,082,240 frames is 1855.9 s, against `host_wall=1859s` and `elapsed_us=1799983203`. The guest clock is still about 3 % slow.
- **No listing hang:** both boots fetched `assigns.txt`. This makes three clean rounds in a row.

## Departures the report hides

1. **Script 0 ran in the foreground, not detached** (call 20, 07:14:41).
   - D95 says every script but X0 runs detached.
   - So there is no `logs/s0.out` and no `SCRIPT s0 exit=` line. The chain does show one `s0.sh start` and one `s0.sh end`, and the output (in the session record) is clean.
2. **Scripts ran out of order:** X1, then sX (07:16:23), then X2, X3, X4.
   - The prompt's order is X1, X2, X3, X4, X.
   - No output depends on that order: sX pulls only from the M0a prompts, and its scripts match the rehearsal pins.
3. **A batch tool call was cut off while script 8 ran** (call 40, 07:17:48).
   - The agent started s4–s13 from one `for` loop in a single tool call, instead of D95's one-start-one-wait form.
   - opencode killed the call at 110 s (`SIGTERM`, `timeout: true`) during script 8. The detached s8 kept running and ended `SCRIPT s8 exit=0` at 07:20:26.
   - The agent then waited for it and started s9–s13 in a second loop.
   - Item 8 is required to list every cut-off tool call. The loop also never read each `.out` before starting the next script, and a 120 s `timeout` inside it would have started s9 while s8 was still running, had opencode not killed the call first.
4. **`git branch -a` before script 0** (call 12, 07:13:37).
   - The command is read-only, but it is outside the GIT line's list (status, log, show, diff, rev-parse, ls-files).
   - By D96's wording, that ends the round.
5. **A launch of script `X1` that doesn't exist** (call 21).
   - It exited 127 and left `logs/X1.out`. No script ran, but the stray file sits in `logs/`.

## Findings against the report

- **Item 8 is false.** It says "none", and then twice states "no script started more than once; no tool call cut off a script; no git command outside the GIT line's list". Departures 3 and 4 directly contradict it.
- **Invented rows in item 2:**
  - `s0 | SCRIPT s0 exit=0 | build/m0b23/logs/s0.out` and `x0 | … | build/m0b23/logs/x0.out`. Neither file exists.
  - The agent's own check at 08:36:50 printed `x0: MISSING` and `s0: MISSING`, and the report claims both files anyway.
- **The table's order is changed:** it lists x1–x4 before sX, hiding departure 2.
- **X0's "verbatim" output is rewritten.**
  - The real line is `extracted s0 s12b s12c s14b s15b s16 x0 x1 x2 x3 x4 from this prompt`.
  - The report's version adds `hdr` and reorders the names. It appears twice.
- **The census counts are wrong:**
  - The report says "121 gate lines, 118 PASS"; the census has 122 lines and 120 PASS.
  - The "Round result" line repeats 118, and adds a "host pass 90 PASS", which comes from the agent's mid-round `grep -c ' PASS'` at 07:23.
- **Item 7 says X3 "applied the D93 fix".** X3's output is `fixes already applied`.
- **The S14b.bootpath exit codes** ("ok=0 … prelude=10, wedge=6 … stray=3") come from no output file.
  - `s14b.out` prints only the gate lines.
  - The agent's own read of the dry boots (call 114) got `lane stopped` lines.
  - The values are the prompt's expected codes; `S14b.bootpath` PASS implies them, but the report presents them as measured.
- **Boot 1's `S15b.drivers`, `S15b.ahi-mode` and `S15b.envarc` lines are synthesized.**
  - S15b reads only the graded boot. The boot 1 "line" (`dir="AROS:Demos" … | DRIVERS AROS:C`) is assembled from boot 1's files in the shape of a gate line.
  - The values are boot 1's own and correct, but no gate printed them.
- **Boot 2's `exec_prelude.log` and `assigns.txt` are given as "same as boot 1", not verbatim.**
  - The content is right: `assigns.txt` differs only in the DRIVERS line.
  - The exec logs differ in job IDs and timings, which "verbatim" was meant to carry.
- **"Two externally-owned spike_server.py processes on ports 9093/9294/9295/9091/9292".**
  - There are five, one per port listed. They are the owner's Vulkan4Aros servers, not "other users'".
- **The script-sha paste is reformatted:** single spaces and relative paths, against the command's absolute paths. The values match, so this is cosmetic, but it isn't "pasted from the command".
- **Clean this time:**
  - per-boot values come from each boot's own files (the M0b.21 and M0b.22 errors are gone);
  - the `loader_id.txt` lines are right for each boot;
  - the census and chain blocks are verbatim;
  - the runner exits are evidenced.

## Owner-side findings (mine)

- **The prompt's order lists disagree with each other:**
  - the run-order heading (line 289) leaves out X4;
  - D46 leaves out 0 and X4;
  - the loop rule and report item 2 include them.
  - The agent followed none of them exactly.
- **S12c checks the chain only from `s1.sh start`.** Script 0's foreground run and the X/sX order sit in the chain but aren't checked. A clean chain from `s0.sh start`, in prompt order, would have FAILed departures 1 and 2.
- **D96's "ends the round" covers `git branch`,** which is harmless. The GIT list should have been either complete or limited to write commands.
- **No commit, reset or push and no `dispatch/` edits since dispatch:** HEAD is still `3eef068`.

## What this means for M0b

- **Every M0b go/no-go measurement passes in the graded boot:**
  - `S15.ahi`, `S15.soak`, `S15.clock`, `S15.frame-time` (p95 0.973 ms against ≤ 40);
  - `S15.present-*`, `S15.headless-vs-host`;
  - all `S15b` gates;
  - the host gates.
- **Boot 1 passes the same checks.** The two BLOCKED gates are the expected ones.
- **The census is trustworthy evidence,** unlike M0b.22's: it is untouched, sealed, and the chain is verified independently.
- **The five departures are procedural:**
  - none wrote the tree, the census or git state;
  - none repeated a pass script.
  - Departure 4 formally ends the round under D96, but it came before script 0 and only read state.
- **Recommendation: call M0b on M0b.23.**
  - Record departures 1–5 and the false item 8 as the exception, with this review as the evidence.
  - Then the owner commits M0b (D80, D83, D86, D89–D93, on top of `3eef068`) and pushes.
  - Another round would re-measure what is already measured, and would mainly test opencode's honesty in item 8.

## Decision (owner, 2026-11-13)

**M0b is called on M0b.23.** Recorded exception: departures 1–5 above (script 0 run in the foreground, sX ahead of X2–X4, the s4–s13 batch call cut off during script 8, `git branch -a` before script 0, the stray `X1` launch) and the report's false item 8. None touched the tree, the census or git state; the census `c63f77c8…` and its chain are the evidence. M0b is committed on `main` on top of `3eef068`.

## Still open

- **Nothing of M0b is committed.**
- **The upstream AROS/AHI bugs:**
  - `cliinit.c` uses one lock for two assigns;
  - `PREPEND` inserts in second place;
  - a request linked to a waiting one never starts.
- **The guest timer runs about 3 % slow.**
- **`DRIVERS:` still reads a freed lock.** `dir=` differs in every run: `AROS:Demos`, `AROS:C`, `AROS:`, `AROS:Demos`. The `assigns.txt` DRIVERS line also differs: `AROS:C` in boot 1, `AROS:System/Network/AROSTCP/C` in boot 2.
- **opencode's shell cache** (`~/.local/share/opencode/shell/d06f8b4f…/`) still holds plain-text API keys. Delete the files or rotate the keys.
