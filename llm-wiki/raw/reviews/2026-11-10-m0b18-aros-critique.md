# M0b.18 AROS critique

> Source: Docs/Development/m0b18_aros_critique.md
> Collected: 2026-11-12
> Published: 2026-11-10

# M0b.18 review — 2026-11-10

Census: 117 PASS / 3 FAIL / 2 BLOCKED (122 gates).

Checked against the evidence, all correct:
- the seal `0b99b188…`
- the runner `761731be…`
- `plat.cpp` `0b7a52f6…`
- `prelude.aros` `be92bf86…`
- both ledger lines
- boot 2 graded (a 14/14 tie)

This time the report's counts and FAIL list match `census.txt`.

## Verdict

The HIDD problem is solved. For the first time, Kitty's game audio reached the host WAV through the real HDA driver: 40 ms per run. Then playback stalls. The cause is now in the game's AHI request handling, not in the driver or the lane.

## What the round proved

- **The `DRIVERS:` chain is confirmed directly.** In every audio run of both boots, `ahi_drivers hidd_lock=0 hidd_err=205`, with `dir=` different every time (`AROS:Prefs`, `AROS:Tools/Commodities`, `AROS:Developer/bin`, `AROS:`). 205 is `ERROR_OBJECT_NOT_FOUND`, so `DRIVERS:` points at a reused, freed lock. The M0b.16 review's account was correct.
- **D86 works.** The evidence, in order:
  - `ahi_hidd_preload=1` and `hidd_before=1`;
  - `hdaudio_open=1`;
  - `ahi_modes=1`: `ahi_mode id=0x3e0001 name="hdaudio:HiFi 16 bit stereo++" driver="hdaudio"`;
  - `ahi_best id=0x3e0001`, the same mode as on the riaudio lane and the Dell.

  `S15b.ahi-mode` and `S15b.drivers` pass.
- **No driver fault.** `card_init` ran on this lane for the first time. The controller then streamed for the rest of the boot: the boot-2 WAV is 329,098,780 data bytes, which is 1866 s at 44.1 kHz stereo. All 14 runs answered rc=0.
- **The sound that reached the host is exactly two buffers per run.** The non-zero bytes form two bursts: 0.040–0.080 s and 9.642–9.682 s, 1765 bytes each. That is 40 ms each, which is 2 × 960 frames, once for `a` and once for `soak`. It matches `ahi_summary submitted=6 returned=2 inflight=4` in both runs.

## Cause of the stall (from the AHI source; the burst timing fits exactly)

AHI's `ahir_Link` supports double buffering only: one request playing and one waiting behind it. The game keeps up to **four** in flight, each linked to the one sent before it (`plat.cpp`, `pl_audio_queue`).

1. **The first two requests work.** `A` is sent with no link and plays. `B` is linked to `A`, which is playing and has a channel, so `CMD_WRITE` attaches it to `A`'s voice (`devcommands.c:1199-1249`).
2. **The third is never attached.** `C` is linked to `B`, which is still on the waiting list with `Channel = NOCHANNEL`. `CMD_WRITE` sets `B->ahir_Link = C` and puts `C` on the waiting list, but with no channel it attaches `C` to no voice (`devcommands.c:1186-1197`). `D` behind `C` gets the same treatment.
3. **`C` is moved but never started.** When `A`, then `B`, finish, `RemPlayers` moves each child onto the playing list and gives it the parent's channel (`devcommands.c:1466-1490`). Nothing calls `PlayRequest` or `AHI_SetSound` for `C`, so it never sounds, never reaches `AHICMD_WRITTEN`, and is never returned. `D`, and the reused `A'` and `B'` linked behind it, wait forever.
4. **The game then drops everything.** All four requests stay in flight, so every later `pl_audio_queue` call finds no free slot. `queued=5760` (6 × 960) is all it ever sends.

This is the same under VOID: M0b.14–16 also showed `returned=2 inflight=4`. The stall was never about the driver.

## Findings against the round itself

- **Boot 2 exited 11** because the held `Assign >` listing hung. So `assigns.txt` is missing, and `S15b.wiring` FAILs for that reason alone. This is the second listing hang in two rounds (M0b.17 boot 1, M0b.18 boot 2), after none in M0b.14–16. The cause is unknown, but `NameFromLock` walking `DRIVERS:`'s freed lock is a credible suspect: the listing names every assign.
- **The report's "Key result" paragraph** describes and interprets results. The prompt asked for gate lines only. It's minor this time, because its facts are right.

## What M0b.19 needs

1. **Fix in `pl_audio_queue`: at most two requests in flight.** Send only while `ahi_inflight() < 2`, and link the new request to the one in flight, if there is one. That guarantees every link target is already playing, which is the only case AHI attaches. The other two request slots and buffers can stay as spares, or the pool can shrink to two.
   - **Latency:** 2 × 20 ms.
   - **Risk:** the game's mixer feeds once per tick. If a tick comes while both requests are in flight, its frames are dropped and counted (`dropped=`). `S15.ahi`'s xrun limit shows whether 960-frame buffers are enough; if not, raising `kAhiFrames` is the next step.
2. **Keep D86** until the `cliinit.c` bug is fixed upstream.
3. **The listing hang (owner choice).** Either move the `Assign >` listing so it doesn't run at all (`S15b.wiring` would then need another source), or log `List`/`Info` for `LIBS:` and `DEVS:` separately so the listing never touches `DRIVERS:`. Or accept exit 11 and grade it as now. It costs one gate, not the round.
4. **Expected guest result:** if the fix is right, `submitted` ≈ `returned` + 2, and `lane_audio.wav` holds music for the length of the soak. `S15b.sound` (176,400 non-zero bytes) should then pass easily.

## Still open

- **The listing hang.**
- **Which `ahi.device` loads:** the stock and rebuilt builds share the same `$VER`.
- **The two AROS bugs** still to send upstream: `cliinit.c` gives one lock to two assigns, and `PREPEND` inserts in second place. Now worth adding: AHI's link chain silently drops a request linked to a waiting one. That is a real AHI weakness, though the documented contract is double buffering.
- **Nothing is committed.** The tree carries D83 and D86 in `plat.cpp`, and the restored M0b.16 prelude.
