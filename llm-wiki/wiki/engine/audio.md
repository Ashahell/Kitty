# Audio: AHI device path and modplay

> Sources: Engine design rev 5, 2026-10-30; Engine design sources rev 5, 2026-10-30; Engine design rev 4 critique, 2026-10-29; Vulkan4Aros source checks, 2026-10-30; M0b.13 critique, 2026-11-09; M0b.14 dispatch prompt, 2026-11-09
> Raw: [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [Sources rev 5](../../raw/engine-design/2026-10-30-engine-design-sources-rev5.md); [Rev 4 critique](../../raw/reviews/2026-10-29-engine-design-rev4-critique.md); [VK source checks](../../raw/platform/2026-10-30-vulkan4aros-source-checks.md); [Rev 5 hand patches](../../raw/reviews/2026-10-30-rev5-hand-patches.md); [M0b.13 critique](../../raw/reviews/2026-11-09-m0b13-aros-critique.md); [M0b.14 prompt](../../raw/dispatch/2026-11-09-m0b14-aros-guest-prompt.md)
> Updated: 2026-11-09

## Overview

On AROS the engine streams 48 kHz S16 stereo to `ahi.device` through the device API. On Linux it uses `SDL_QueueAudio` from the main thread, with no callback. Music is played by the engine's own deterministic ProTracker replayer, `modplay`, with per-channel mute. Audio never feeds `sim/` and is never hashed.

## AHI device contract

- "`ahi.device` is opened on `AHI_DEFAULT_UNIT` (0) with a full `struct AHIRequest`."
- "`AHI_NO_UNIT` is never used: it leaves `io_Unit` NULL, and `CMD_WRITE` dereferences `io_Unit`."
- The evidence is in the AHI source. `device.c:332` reads `if(iounit)    // Is NULL for AHI_NO_UNIT`, and `devcommands.c:109` does `PutMsg(&ioreq->ahir_Std.io_Unit->unit_MsgPort,&ioreq->ahir_Std.io_Message);` for `CMD_WRITE`. The autodoc at `device.c:204` says "AHI_NO_UNIT should be used when you're using the low-level" API.
- "Four chained 960-frame `CMD_WRITE` requests form an 80 ms queue." Each buffer is 3,840 B. They are serviced from the tick loop, every 20 ms tick, catch-up included. "The queue covers any gap between services shorter than 80 ms; a longer frame underruns, and the underrun is logged with its tick."
- **UNVERIFIED:** device behaviour with a chain deeper than the documented pair, and xrun behaviour on the device API (the earlier xrun lessons came from the library API). An M0 xrun probe settles both.
- An AHI open failure is non-fatal: the game logs it and continues silent.

> **Status: Outdated** (2026-10-29)
> Revision 4 opened the device on `AHI_NO_UNIT` and then sent `CMD_WRITE`. The rev-4 critique found that this NULL dereference was blocking. Revisions 2–3 also carried leftovers of a two-request, 40 ms queue.

## modplay facts (§7, rev 5 as patched)

- "`Fxx` with xx = 00–1F sets the speed (ticks per row), and xx = 00 is ignored; xx = 20–FF sets the tempo (32–255 BPM)." [S93] gives "speed (00-1F) / tempo (20-FF)"; [S68] gives "if xx is 0, then ignore this effect".
- The CIA timer itself accepts 28–255, but `modplay` follows the PT2.3A player, which raises a tempo below 32 to 32 (`MOVEQ #32,D0`).
- "A note's playback rate is the Paula clock divided by its period (PAL: 3546895 Hz); 50 Hz is the tick rate, not the pitch clock."
- "Set volume is effect `Cxx`, with volume 0–64."
- "MOD samples are signed 8-bit, so they have no byte order; `modplay` widens them to `i16` at load."

> **Status: Outdated** (2026-10-29)
> Revision 4 said tempo `Fxx` covered "17..64 BPM", that period→frequency ran on "the 50 Hz clock", and that 8-bit samples were "byte-swizzled". All three were wrong. Rev 5's [S91] then misquoted `PT-AHIPlay.s` line 415 as `divu.w #3546895,d2,d1`; the file has `divu.w d2,d1`. S91 was cut down to verbatim quotes on 2026-10-30.

## Measured on the AROS guest (2026-11-09)

- The game opens its own `PROGDIR:Devs/ahi.device`, version 6.13 (23.07.2026), on lane l0025 (M0b.13 boot 1).
- On that boot 5760 frames were queued: 6 sends of 960, so AHI returned 2 of the 4 requests and then none. `lane_audio.wav` was 44 B. The cause is open; M0b.14 logs `ahi_io_error=` and an `ahi_summary` line to find it.
- No `ahi.prefs` exists on this lane's images. `DH0:Devs/AudioModes/` holds `HDAUDIO` among six mode files. What unit 0 resolves to with no prefs file is not established.

## See Also

- [AROS platform facts](../platform/aros-platform-facts.md)
- [Engine architecture](engine-architecture.md)
- [M0 implementation status](../process/m0-implementation.md)
