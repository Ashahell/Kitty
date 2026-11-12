# M0b.20 AROS critique

> Source: Docs/Development/m0b20_aros_critique.md
> Collected: 2026-11-12
> Published: 2026-11-12

# M0b.20 review — 2026-11-12

Census: 118 PASS / 2 FAIL / 2 BLOCKED (122 gates).

**There is no report.** DSH's last turn ended with `max-tokens` at 10:10:45, while it was about to write `build/m0b20/report.md`, so the file was never written. This review is made from the evidence alone.

Checked against `build/m0b20/`:
- the seal `1b37be08…` matches `census.txt`
- `audio.cpp` `9ddd9eff…` and `plat.cpp` `afd2b367…` (D90)
- two ledger lines (`2026-11-12T06:59:55`, `09:31:37`) against two boot records; `S12b.ledger` PASS
- both runners `RUNNER_EXIT=0`
- graded boot 2 (a 14/14 tie)
- no source edit after the host pass (the newest file is `tests/aros/lane_run.sh`, written by X2 at 15:31)

FAIL: `S15.ahi`, `S12.end-state`. BLOCKED: `S7.golden`, `S15.nvk-smoke`.

## Verdict

D90 changed nothing measurable. The dropouts are the same as in M0b.19:

| | M0b.19 boot 2 | M0b.20 boot 2 |
|---|---|---|
| xruns, run `a` / soak | 16 / 2784 | 17 / 2731 |
| silent 10 ms windows in the WAV | 2866 | 2799 |
| silence in total | 28.7 s | 28.0 s |
| longest silence | — | 30 ms (one); every other gap is a single 10 ms window |

The xruns are still periodic: one every 32 submissions (1279 cases; 31 and 33 make most of the rest). The median gap between them is 640 ms of guest time.

M0b.19's cause, a producer slower than the device, was right. Its fix was wrong, and so was its "correction" to M0b.18.

## Why D90 couldn't work

- **Production is still about one buffer per tick.** Boot 2's soak stepped 89981 ticks and submitted 90020 buffers, only 39 more than one per tick.
- **This is not a bug in D90; it follows from depth 2 with 20 ms buffers, refilled only at ticks.**
  - Measured in the device's clock, a tick lasts about 20.6 ms (guest `elapsed_us` 1800.0 s against 1857 s of host wall for the soak).
  - At each tick the loop tops up to two requests: the one playing, with remaining time *r*, and one full 20 ms buffer. By the next tick the device has consumed 20.6 ms, so *r* shrinks by 0.6 ms per tick, whatever the loop does.
  - After 20 / 0.6 ≈ 33 ticks the playing buffer ends before the tick. Both requests have then returned, and `pl_audio_queue` logs `ahi_xrun`. The loop refills both slots, *r* resets to 20, and the cycle repeats.
  - AHI restarts on a later mixing block, so each underrun is a fully silent 10 ms window in the WAV, not a sub-millisecond gap.
- **D90 can only refill what has been consumed; it cannot get ahead.** Getting ahead takes a buffer longer than one tick's consumption: with 40 ms buffers at depth 2, the queue holds at least about 19 ms after every tick.
- **Correction to M0b.19's correction:** "raise `kAhiFrames`" (M0b.18) was the right lever. M0b.19 dismissed it, arguing that bigger buffers drain at the same 3 %. That argument is wrong. The drift moves *r* within the playing buffer; it only causes an underrun when one buffer is shorter than one tick's consumption.

## `S15.ahi`

`queued=453120 elapsed_us=9982630 host_wall=11s summary=('472', '470', '0') xruns_a=17 xruns_soak=2731`

- **`returned ≥ submitted − 2` and `errors=0`:** pass.
- **Queued frames:** the lower bound is 0.98 × 48000 × 9.98 s = 469,563, and the run queued 453,120 (472 × 960). That's below the bound, and it's the same tick-count limit as before.
- **xruns:** 17 against ≤ 4, and 2731 against ≤ 30. Both fail.

The gate measured the problem correctly. Its definition stays as it is.

## `S12.end-state` — caused by the owner side, not the round

`m0b20_aros_guest_dsh_prompt.md: FAILED` and the new file `m0b20_dsh_resume_message.md` caused it. I hardened DSH3 and wrote the resume message on 2026-11-11 at about 18:55, after script 0 had snapshotted the protected docs. The lane_run.sh difference in the same output is the reviewed X2 install, as in M0b.19, which passed with it. DSH blamed the lane_run.sh install, without checking the first line of `s12.out`. Without the two doc edits this gate passes, so the real census is 119/1/2.

## Findings against the round

- **No report.** DSH ran out of output tokens writing one large file in a single call. Next time it has to write the report in sections: append item by item with the file tool.
- **The 2026-11-11 void attempt** (`build/m0b20/void-dsh-boot1-20261111-1800/`) belongs in item 8, with the premature s15. There's no report to carry it, so it's recorded here.
- **DSH looped:** at 10:07:00 the harness flagged "repeating the exact same tool call with identical arguments". There was no damage.
- **The startup stalls remain:** `catchup_drop` of 5+15 ticks in boot 2's `a` run (400 ms). Each stall longer than the queue gives one xrun no matter how big the buffers are, which is why the ≤ 4 allowance in `a` is right.

## What M0b.21 needs

1. **40 ms AHI requests, two in flight (D91).** In `src/platform/aros/plat.cpp`:
   - `kAhiFrames` = 1920, so `kAhiBytes` = 7680.
   - `pl_audio_queue` appends its 960 frames to the free slot's buffer and submits only when the slot holds 1920 frames.
   - `pl_audio_queued_frames` returns `inflight × 1920 + staged`, capped to 3840 at two in flight.
   - `audio.cpp` doesn't change: its loop renders 960-frame chunks while queued is below 3840, which now fills one 40 ms slot in two chunks.
   - Latency rises from 40 to 80 ms, which is acceptable for music.
   - Expected result: xruns only at the two catch-up stalls in `a` (≤ 4), and 0–2 in the soak; the WAV's silent windows drop from about 2800 to single digits.
2. **The `S15.ahi` gate stays as it is.** With D91, queued tracks the device: about 48000 × 10.3 ≈ 494k, inside 469,563…577,920.
3. **Report-writing rule for DSH:** write `report.md` in sections, one file-tool call per report item, never as one call.
4. **Owner rule (for me):** no edits under `Docs/Development/dispatch/` between a round's script 0 and its S12. A prompt fix mid-round goes into a new file outside the protected paths, or waits.

## Still open

- **The listing hang:** didn't recur (both boots exit 0, in this round and M0b.19).
- **Which `ahi.device` loads:** unresolved.
- **The upstream AROS/AHI bugs:** still to send (`cliinit.c` one lock for two assigns; `PREPEND` inserting second; a request linked to a waiting one never starts).
- **The guest's timer runs about 3 % slow against the host** (1800 s against 1857 s). That's the root of the drift. D91 makes audio tolerant of it rather than fixing it.
- **Nothing is committed.** The tree carries D80, D83, D86, D89 and D90.
