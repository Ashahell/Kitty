# M0b.10 AROS critique

> Source: Docs/Development/m0b10_aros_critique.md
> Collected: 2026-11-04
> Published: 2026-11-04

# M0b.10 critique — fifth guest round (2026-11-04)

**Verdict: M0b not achieved, but no Kitty defect is left open on the guest. 3 FAIL: two are one AROS driver fault that an AROS fix already exists for, and one is a gate that can't pass. The report's explanation of the audio failure is false; the screendump and the serial log contradict it.**

Dispatch: `Docs/Development/dispatch/m0b10_aros_guest_opencode_prompt.md`, sha256 `23dfdd968b6c9f688603f15645347e94cae53f9de24d3785fc2c7e6ab26ea4c7`. Boot record: `~/Work/lanes/l0025/boot-20261104-1938.txt`.

## Verified

- **Census:** 115 gate lines: 107 PASS, 3 FAIL, 2 BLOCKED (with the three binary-detail lines). Seal `bbb4c189…` matches.
- **Ledger:** one `boots.txt` line, one new boot record. No guest running.
- **Commit:** loader fix `023aeeec` is in Vulkan4Aros; it was HEAD at script 0. Three owner commits followed during the round, as S12 says.
- **DH1:** loader `90237ed2…`, `fixv3` stamp in `loader_id.txt`.

## Proven on the guest

- **FIFO:** `fifo` logs `present_mode=FIFO`, 60 frames, rc=0, `validation_messages=0`. FIFO pacing isn't gated, and `fifo`'s picture isn't compared. `elapsed_us=945426` is only consistent with pacing.
- **30-minute windowed run:** `soak` rc=0, `elapsed_us=1799983030`, 89968 frames, `validation_messages=0`, no fault.
- **Memory:** Kitty's memory is flat. `mem_free_bytes` doesn't move for the first 14 samples, then falls about 2.5 KB a minute from the sample after the agent's first `Type` poll of `rc_soak.txt` (`serial_soak.txt:43`, `:51`). The drift, 38032 B, tracks the host's polling, not Kitty. That is a correlation, not a proof.
- **The rest:** `S15.boot`, present-readback, present-vs-headless, headless-vs-host and lp-probe pass again.

## The three FAILs

### `S15.ahi` and `a` in `S15b.output-captured`: AROS driver fault, already diagnosed

`dead_a.ppm` shows the requester: task `RAM:kitty/kitty-debug`, error `0x80000008` Privilege violation, module `sb128.audio`, offset `0x6C5`, function `DriverInit`, offset `0x135`. `serial_a.txt:23` has `core_IRQHandle(13)` right after `effects_ignored=0`.

`objdump -d` of `src/abi/v1/core-pc-x86_64/.../Devs/AHI/sb128.audio` has `movaps (%rdx),%xmm0` at `0x6c5`. That is the build-tree copy; DH0's own copy wasn't read.

The sister projects found this exact fault on 2026-09-28: `sb128.audio` `DriverInit+0x135`, during AHI's mode scan, on any `OpenDevice("ahi.device", AHI_NO_UNIT)` (Vulkan4Aros `llm-wiki/raw/articles/2026-09-28-ahi-drivers-vector-alignment-qemu-hda.md`, ReIncarnation `llm-wiki/raw/articles/2026-09-28-audio-in-qemu.md`):

- **Cause:** aligned SSE from clang's auto-vectorizer, on data the loaded image doesn't align.
- **Fix:** rebuild with `-fno-vectorize -fno-slp-vectorize`, sb128 plus 8 drivers and the device core. Each rebuild moved the fault one site: sb128 → hdaudio `card_init+0xF6A` → `ahi.device ReadConfig+0x6F`.
- **Result on their lane:** `riaudio`, with `-audiodev wav,... -device intel-hda -device hda-duplex`: 3/3 clean boots, 20174 buffers, 0 xruns. Before the fix, 3/3 faulted.

So:

- **It is not a Kitty bug,** and Kitty can't guard against it: the fault is in AHI's driver scan inside `OpenDevice`.
- **A sound device alone won't fix it.** The scan faults before any device matters. The open item I gave the owner ("a sound device for the lane") was framed wrongly.
- **Not yet checked:** whether the rebuilt set is still on `~/Work/vms/riaudio_dh0.img`, and whether AHI would load it from DH1 on this lane.

### `S15.soak`: the gate can't pass, and the fault is mine

`run_seconds` stops at `ticks + dropped == target` (`src/app/main.cpp:214`), and the gate wants `ticks >= 90000`. So any dropped tick fails it. Here: 89968 + 32 = 90000. The two drops, 5 and 27 ticks, are both logged before the first memory sample, so in the first minute, and none after. I carried that gate through ten rounds without reading the loop. It should read `ticks + dropped >= 90000` with a bound on `dropped`.

## The report

**False:**

- "No fault line (no `core_IRQHandle`) appears in `serial_a.txt`." It's on line 23. A one-word grep shows it, and the prompt required quoting that grep.
- "The audio run simply idled until the budget expired." The task faulted right after `effects_ignored=0`, and the requester sat there for the rest. The cause it gives, no sound device, is invented.
- `soak`'s rc "was eventually found after the runner had already marked it dead". `dead.txt` has only `a`.

**Weak:**

- §8 cites the prompt's "`a` is expected to fault again" while §5 says it didn't fault.
- `S15.soak` is called "a gate threshold issue" with no reading of the loop and no mention of the drops. Roughly right, by luck.
- "All FAIL/BLOCKED gates are as expected or explained" isn't true of the above.
- The prompt asked for the whole runner log; the report gives its ending. `serial_a.txt` is summarized rather than quoted, which hid the fault line.

It did not describe `dead_a.ppm`, which the prompt allowed. Nobody opened the one file made to answer this question until this review.

## Other notes

- `S15.frame-time`: p95 went from 4.370 ms to 1.009 ms and `ms_max` from 776 to 396, with no change on the `b1` path. Run-to-run noise on this host is that large. Only the ≤ 40 ms limit means anything.
- The M0b.9 soak hang isn't explained. Between the rounds, both `--music` and the preceding dead `a` went away, so this round can't separate them.

## My errors

- **I missed a known fault for two rounds.** My own notes say to check the Vulkan4Aros wiki first. At M0b.9 I wrote "likely `OpenDevice`; not established" and suggested a sound device, without searching for `DriverInit` or `core_IRQHandle` there.
- **`S15.soak`:** see above.
- **Prompt facts row:** it said 90 files; script 0 counts 88. The hash matched, so nothing broke.

## What's left for M0b

- **Audio:** the rebuilt AHI set on this lane, and a sound device for `S15.ahi` to measure anything. Launcher and DH0 aren't Kitty's to change: owner decision.
- **`S15.soak`:** fix the gate; a re-run needs no new evidence beyond this boot's log, but gates are only read in a round.
- **S7.golden:** still BLOCKED on the lavapipe pin.
