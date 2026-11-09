# M0b.13 AROS critique

> Source: Docs/Development/m0b13_aros_critique.md
> Collected: 2026-11-09
> Published: 2026-11-09

# M0b.13 critique — eighth guest round (2026-11-09)

Report: `~/Work/Kitty/repo/build/m0b13/report.md`. Dispatch: `Docs/Development/dispatch/m0b13_aros_guest_opencode_prompt.md`, sha256 `1730a0dadc8436081515234d90c72fe8670741f95b16173f3c4a8cdc85c1a629`. Boot records: `~/Work/lanes/l0025/boot-20261109-1737.txt`, `~/Work/lanes/l0025/boot-20261109-1828.txt`.


**Verdict: M0b.13 fails by its own rule.** D73 grades the guest gates on boot 2, and boot 2 never ran Kitty, so all 24 guest gates in `census.txt` are FAIL (sealed `d08db401…`). Boot 1 is a near-pass. The report is accurate: I checked its claims against the files on disk and found nothing fabricated.

## Host pass
- Every host gate passes, except S7.golden, which is BLOCKED because the pinned host lavapipe isn't built.
- The tree hash is `f102fb81…`. X3 reported "already applied". The runner sha is `dbbe709d…`. S12c passes.
- The only change in the tree is the installed runner. No third boot was started, and the boot ledger matches (2/2).

## Boot 1 (l0025, 17:37): 22 of the 24 guest gates pass
- **Rendering and timing:**
  - Rendering matches: the window, headless and host frames all agree (0 mismatched pixels).
  - Frame time: p95 is 0.974 ms against the 40 ms limit.
  - Soak: ran 1861 s; memory drift 38 KB; 19 catch-up ticks dropped.
- **Loader:** the loader identity check and the release build pass, with no fault lines anywhere.
- **Both failures are audio:**
  - **S15.ahi:** 5760 frames queued; the gate needs about 480,000.
  - **S15b.sound:** `lane_audio.wav` is 44 B, a header and no samples.
- **What the D72 logging settled:**
  - The game opened its own copy of the driver, `PROGDIR:Devs/ahi.device` version 6.13 (23.07.2026), the file from the staged AHI set. No `audio_progdir_error` line appeared.
  - So the problem is not which AHI device gets loaded. That question is closed.
- **What the audio numbers mean:**
  - 5760 frames is 6 sends of 960 (`kAhiFrames`). With 4 requests, AHI returned exactly 2 of them and then stopped returning any for the rest of the 10 s run.
  - Nothing reached the HDA output.
  - There is no `ahi_xrun` line because the 4 requests are never all back at once.
  - The code never logs `g_ahi_dropped`, and never reads `io_Error` on the requests that did come back. That gap is why we still can't say why AHI stalls.

## Boot 2 (18:28): an AROS guest fault, not a Kitty failure
- **What failed:** prelude line 6, the bare `Assign >RAM:kitty/out/assigns.txt`.
  - The guest page-faulted (`core_IRQHandle(14)`, CR2=0x88, IP=0x1bf8d25, a user-mode read).
  - The agent lost its connection, and the runner exited 6.
  - No Kitty binary had run yet.
- **It has happened before:**
  - The same CR2 and IP appear in `serial-20261102-2244.log`, where the whole prelude ran as one `Execute`. The Vulkan4Aros review of 2026-11-03 (H1) recorded it.
  - Across the 8 l0025 boots that ran the prelude, it faulted 2 times. It's a recurring guest flake, not caused by this round.
- **Sister wikis:** nothing in Vulkan4Aros or ReIncarnation symbolizes or fixes this IP.

## My mistakes in the M0b.13 prompt
- **D73:** making boot 2 the graded boot meant a guest flake could throw away a good boot 1. I should have graded the better of the two boots, or required both to pass.
- **Prelude order:** the prelude still runs the `Assign` listing, which faulted once before (2026-11-02), and it runs it before anything the gates need. It belongs at the end of the run.

## Next round (M0b.14)
- **Move the `Assign` listing** to after the last run has written its rc. A fault there then costs only S15b.wiring.
- **Grade the best boot** under D73, and keep reporting both boots.
- **Two audio logging additions** to `plat.cpp`, in the same style as D72:
  - `io_Error` on each request that comes back;
  - `g_ahi_dropped` at shutdown.
- **Read the AHI preferences in `ENVARC:`** (which is `DH1:W` on this lane) to see which audio mode unit 0 is mapped to.

**M0b isn't ready to commit**, because S15.ahi and S15b.sound are still open even on the good boot.