# M0b.21 AROS critique

> Source: Docs/Development/m0b21_aros_critique.md
> Collected: 2026-11-12
> Published: 2026-11-12

# M0b.21 review — 2026-11-12

Census: 119 PASS / 1 FAIL / 2 BLOCKED (122 gates).

The report exists and was written in sections, as asked.

Checked against `build/m0b21/`:
- the seal `196ef4b1…` matches `census.txt`
- `plat.cpp` `e9078a47…` (D91) and `audio.cpp` `9ddd9eff…`
- two ledger lines (`14:43:45`, `15:28:14`) against two boot records (`boot-20261112-1443`, `-1528`)
- runners: boot 1 `RUNNER_EXIT=0`, boot 2 `RUNNER_EXIT=11`
- `graded_boot.txt` `graded=2 boot1_rc=14 boot2_rc=14`
- no file was edited after script 0 except `tests/aros/lane_run.sh` (X2, 14:09:56)
- no dispatch edits mid-round: `S12.end-state` PASS

FAIL: `S15b.wiring`. BLOCKED: `S7.golden`, `S15.nvk-smoke` (both as in every recent round).

## Verdict

**D91 fixed the dropouts.** The audio problem from M0b.18 to M0b.20 is closed.

| | M0b.20 boot 2 | M0b.21 boot 1 | M0b.21 boot 2 |
|---|---|---|---|
| xruns, run `a` / soak | 17 / 2731 | 0 / 0 | 0 / 0 |
| silent 10 ms windows in the WAV | 2801 | 3 | 3 |
| gaps | 2799 | 1 (30 ms) | 1 (30 ms) |

- **The soak's queued audio follows the device:** 89,078,400 frames is 1855.8 s at 48 kHz, against 1858 s of host wall time and 1800 s of guest time.
- **There are no xruns even in run `a`.** It still had its catch-up stalls (5 and 15 ticks in boot 2, 400 ms in total), but an 80 ms queue covered them. M0b.20's review allowed for ≤ 4 xruns there; none happened.
- **Ending state:** `errors=0`, `dropped=0`, and `returned = submitted − 2` in all four runs.

**The one FAIL comes from grading, not from the code.**
- Boot 2's runner timed out on the held `Assign` listing after the soak. This is the listing hang, back after two clean rounds: `endreads || exit 11`. So `assigns.txt` was never fetched, and `S15b.wiring` has nothing to read.
- The two boots tied at 14 `rc_*.txt` files each, so D76 graded the later one, boot 2.
- Boot 1 is clean (`RUNNER_EXIT=0`, `assigns.txt` present), and its LIBS, ENVARC and DEVS lines match the wiring regex. Graded on boot 1, the census would be 120/0/2.
- DSH applied D76 correctly. The rule is the weak point: a tie should never go to a boot whose runner failed.

## `S15.ahi` passes, but only just

`queued=470400` against a lower bound of 0.98 × 48000 × 9.982 s = 469,563. That's a margin of 837 frames (17 ms).

- **Run `a` has a startup cost the bound doesn't allow for.**
  - The queue at the end is device consumption plus up to 3840 frames in flight. 466,560 frames consumed is 9.72 s of audio.
  - That's against about 10.28 s of device time, so roughly 0.5 s passes before the first request plays: init, the first stall, and AHI's start-up.
- **M0b.20's review predicted about 494k. That prediction was mine, and it was wrong:** it left out this startup.
- **The gate isn't measuring wrong, but its margin is noise.** Requests are 1920 frames, so the next run could land one request below the bound and FAIL with perfect audio.
- **Fix (D93 below):** take the bound from `elapsed_us` minus the time to the first submission, which the run would log.

## Findings against the report

- **Item 4 misquotes `loader_id.txt`.**
  - It says `open=1` under "After each boot". Both boots have `open=12`, and no file in `build/` has `open=1 flags`.
  - The two boots also have different `lib=` and `idstr_ptr=` addresses, so one quote can't stand for "each boot".
  - That's an invented value in a "verbatim" block.
- **Item 5 labels lines "verbatim" that aren't.**
  - Boot 1's `assigns.txt` is a paraphrase in one line.
  - The `exec_ahi_prefs.log`, `exec_audiomodes.log` and `exec_listing.log` quotes use `→` summaries and `...`, and leave out lines.
  - DSH6 allowed naming the files instead of pasting them; it didn't allow calling a summary verbatim.
- **Item 5 puts the graded `S15b.drivers` line under boot 1.**
  - That line (`dir="AROS:System/Network/AROSTCP/C"`) is boot 2's.
  - Boot 1's runs logged `AROS:Utilities` and `AROS:Tools`.
- **Item 8 says "none".** That's correct for departures, and DSH4's line is there.
- **Clean otherwise:**
  - gate lines are verbatim and counts are copied;
  - no FAIL is explained in a gate line;
  - no process was left running;
  - no lane_run.sh was started from inside DSH.

## Notes, not findings

- **`ahi_drivers dir=` is a different directory in every run:** `AROS:Utilities`, `AROS:Tools`, `AROS:`, `AROS:System/Network/AROSTCP/C`.
  - This is D86's dangling `DRIVERS:` lock: `NameFromLock` reads a freed lock that has been reused.
  - It's expected, and harmless while `ahi_hidd_preload=1`.
  - It's more evidence for the upstream `cliinit.c` bug.
- **The QEMU WAV is 44.1 kHz.** The host side resamples, which doesn't matter for counting silence.

## What M0b.22 needs (if the owner wants one more round before calling M0b)

- **No code change.** D91 stays as it is.
- **D92: grading tiebreak (amends D76).**
  - On a tie in `rc_*.txt` count, the boot whose runner exited 0 wins.
  - If both or neither exited 0, the later boot wins.
  - It's still mechanical and still blind to gate results.
- **D93: the `S15.ahi` lower bound uses audio time, not run time.**
  - `audio.cpp` logs `audio_first_submit_us=` (guest µs since start).
  - The bound becomes 0.98 × 48000 × (elapsed_us − first_submit_us) / 1e6.
  - The upper bound and the xrun limits stay.
- **The listing hang** has now hit 1 of the last 6 boots. It's the one remaining cause that can lose a gate.
  - Options: fetch `assigns.txt` before the soak, while the agent is fresh; or run the listing unheld.
  - Either is a runner change, so it's reviewed through X2 like the others.
- **DSH6 addition:** "verbatim" means copied bytes. A summarised line is marked as a summary.

## Alternative: call M0b now

- **Every M0b go/no-go measurement passes in the graded boot.**
  - The one FAIL is a wiring listing that the guest wedged on.
  - The ungraded boot 1 has that listing, and it passes.
  - The two BLOCKED gates have been BLOCKED, as expected, for many rounds.
- **GDD §11 says "everything must pass".** By the census, M0b.21 doesn't meet that.
  - Calling M0b now is the owner's decision, and it would need a recorded exception.
  - The clean path is M0b.22 with D92 and D93.

## Still open

- **Nothing is committed.** The tree carries D80, D83, D86, D89, D90 and D91: all of M0b since `38fc422`.
- **The upstream AROS/AHI bugs** are still to send:
  - `cliinit.c` uses one lock for two assigns;
  - `PREPEND` inserts in second place;
  - a request linked to a waiting one never starts.
- **The guest timer runs about 3 % slow** (1800 s against 1858 s). D91 makes audio tolerant of it rather than fixing it.
- **Which `ahi.device` loads** stays open in the list. All four runs log `audio_source=PROGDIR:Devs/ahi.device` and `ahi.device 6.13 (23.07.2026)`, the same as before.
