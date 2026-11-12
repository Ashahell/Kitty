# M0b.19 AROS critique

> Source: Docs/Development/m0b19_aros_critique.md
> Collected: 2026-11-12
> Published: 2026-11-11

# M0b.19 review — 2026-11-11

Census: 119 PASS / 1 FAIL / 2 BLOCKED (122 gates).

Checked against the evidence, all correct:
- the seal `6dbb3a2a…`
- `plat.cpp` `fabf51a2…`
- `prelude.aros` `be92bf86…`
- both ledger lines
- boot 2 graded (a 14/14 tie)
- both boots exit 0, and both `assigns.txt` written

The FAIL list and BLOCKED list match `census.txt`.

## Verdict

D89 fixed the stall. Kitty's music now plays through the real HDA driver for the whole soak. What's left is a stutter: about 1.5 dropouts a second, each roughly one buffer (20 ms) long. The cause is the mixer: it produces one buffer per game tick, while the sound card consumes at its own clock, which runs about 3% faster than the guest's. `S15.ahi` fails for a separate reason: its window cannot be reached on this lane.

## What the round proved

- **The request chain no longer stalls.** Every run shows `inflight=1` at shutdown, `dropped=0` and `errors=0`:

  | Run | Boot | submitted | returned |
  |---|---|---|---|
  | `a` | 2 | 463 | 461 |
  | `soak` | 2 | 89946 | 89944 |
  | `a` | 1 | 470 | 468 |
  | `soak` | 1 | 89952 | 89950 |

  That is the "within two" the M0b.18 review predicted.
- **Sound for the whole run.** The boot-2 WAV (44.1 kHz stereo) has sound from 0.04 s to 1865.38 s with no silence longer than 200 ms. Measured in 10 ms windows, it has 2865 stretches of sound separated by 2866 fully silent windows (28.7 s in total).
- **The dropouts are the xruns.** There are 16 (`a`) + 2784 (`soak`) `ahi_xrun` lines, against about 2865 gaps. The xruns are periodic, one every 31–32 submissions in both runs.
- **The clocks disagree by the same ratio.**
  - The soak's guest `elapsed_us` is 1800.0 s; its host wall time is 1857 s, and the WAV shows about 1854 s of soak streaming. That is a ratio of 1.03, so the guest ticks are slower than the HDA consumes. One xrun per 1/0.031 ≈ 32 buffers follows exactly.
  - The game produces 960 frames per tick (one `aud_tick` per stepped tick, `main.cpp:231`) and refills at most one buffer per tick. A producer that is slower than the consumer drains a two-deep queue no matter how deep it is.
- **The main loop is not the cause.** The `b1` frame times are `ms_p95=0.990`, `ms_med=0.942`.

## Why `S15.ahi` fails

- **The gate's window is unreachable on this lane.**
  - The gate requires `audio_frames_queued` between 470400 and 489600 (98–102 % of 10 s at 48 kHz). That assumes one 960-frame buffer for each of 500 ticks.
  - Run `a` stepped 470 ticks and dropped 30 at startup through catch-up (`catchup_drop ticks=7`, `=23`). Dropped ticks run no `aud_tick`, so the most it could reach is 470 × 960 = 451200.
  - Boot 1 hit exactly that. Boot 2 got 444480: 7 ticks found the queue full.
- **The gate is a tick count, not an audio measure.** Once the mixer follows the device's demand (below), queued frames track the device's time, about 3% more than the guest's. On this lane that is about 494k frames, which overshoots the window's top.

## Findings against the round itself

- **The report's "Key result" paragraph is back,** against the prompt's explicit rule. It interprets as well: the FAIL line's "(D89 two-in-flight: mixer now keeps rendering continuously…)" is the report's own story, and wrong. The mixer refills one buffer per tick, not continuously.
- **The report repeats M0b.18's history** (D84 withdrawn, D87 restored). That is irrelevant to this round.
- **`ahi_xrun … tick=0` on every line.** `p.ticks` is set once (`plat.cpp:872`) and never updated on AROS, so the tick field in the xrun and error lines is dead. It's a game bug, not a report error.

## Correction to the M0b.18 review

Its fallback, "raise `kAhiFrames` if the xruns are too many", is wrong. The xruns come from the clock mismatch, not from buffers being too short. Bigger buffers with production still paced by ticks drain at the same 3 % rate.

## What M0b.20 needs

1. **Pace the mixer by the device, not by ticks.** In `audio.cpp` `aud_tick`, render and queue in a loop while `pl_audio_queued_frames() < 3840`, capped at 4 iterations.
   - On AROS, D89 already reports 3840 at two in flight, so each tick tops up both slots, and production then follows the HDA clock.
   - The change is engine-side and game-agnostic.
   - On Linux, SDL's queue gets the same top-up policy. Before it counts, re-run `S9.sdl-stream`, `S16.audio` and every gate that counts queued frames per tick.
   - Expected: the xruns fall from about 1.5/s to only startup or real stalls over about 20 ms.
2. **Optional: more margin.** With depth 2, a loop stall longer than about 20 ms still gives a dropout. Raising `kAhiFrames` to 1920 (two 40 ms buffers, 80 ms latency) covers that. This is a separate choice from 1, and only worthwhile if 1 leaves xruns.
3. **Redefine `S15.ahi`** against the device, not ticks. For example: `returned ≥ submitted − 2`, `errors=0`, and xruns ≤ 2 outside the first second. Then `queued ≥ 0.98 × 48000 × elapsed_us` and `≤ 48000 × host_wall + 2 × 960`. The fixed 470400–489600 window goes.
4. **Fix the tick field:** update `p.ticks` (or pass the tick in) so `ahi_xrun` lines show where in the run they fell.
5. **Report rule:** the reporter must not editorialise in FAIL lines either. Gate lines verbatim, nothing appended.

## Still open

- **The listing hang** didn't recur this round (both boots exit 0). It has hung in 2 of the last 3 rounds, so it isn't gone.
- **Which `ahi.device` loads:** the same `$VER` 6.13 (23.07.2026) as before, so it's still unresolved.
- **The upstream AROS/AHI bugs** to send:
  - `cliinit.c` gives one lock to two assigns;
  - `PREPEND` inserts in second place;
  - a request linked to a waiting one is never started.
- **Nothing is committed.** The tree carries D80, D83, D86 and D89 in `plat.cpp`. Audio now works end to end on the lane, which makes this a reasonable point to commit the M0b audio work once the owner has reviewed it.
