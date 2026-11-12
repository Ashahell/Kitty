# M0b.15 AROS critique

> Source: Docs/Development/m0b15_aros_critique.md
> Collected: 2026-11-12
> Published: 2026-11-10

# M0b.15 review — 2026-11-10

Census 115 PASS / 2 FAIL / 2 BLOCKED (119 gates). The seal, the runner (`tests/aros/lane_run.sh` `42b38371…`), `plat.cpp` (`a5a84cdc…`), HEAD `b5f929d`, both ledger lines, and grading on boot 2 all check out against the evidence.

## Verdict

The harness round is clean, but audio is still not working. D78 did its job and still changed nothing: the hypothesis behind it was wrong.

## What the round proved

- **D79 works.** The only census change from M0b.14 is `S12.end-state` going from FAIL to PASS. Nothing else moved.
- **D78 works mechanically.** `lane_run: copied AROS:Devs/AudioModes/HDAUDIO …` appears on both boots, and the end-of-boot `List DEVS:AudioModes` shows `HDAUDIO 126`. The mode file is now visible through the first element of the multi-assign.
- **D77 answered its question.** Both boots, in every audio run, log `ahi_modes=5`. All five are `driver="void"`, and `ahi_best` is `0x1f0002 "VOID:HiFi 32 bit stereo++"`. HDAUDIO (`0x3e0001`) is not in the database even though its file is visible. So the M0b.14 explanation, that the mode file was invisible, was not the cause, or not the only one.
- **Nothing else changed.** `submitted=6 returned=2 inflight=4`, `queued=5760`, and the 44-byte WAV are identical to M0b.14. That is the VOID driver's behaviour, unchanged.

## Most likely cause (from source and the lane's disk trees; not seen in a log)

The chain, step by step:

1. **The scan tests the driver.** `AddModeFile` (`database.c`) only keeps a mode if `OpenLibrary("DEVS:AHI/<driver>.audio", 2)` succeeds. If it fails, the mode is dropped silently: no requester and no log line. When the database comes out empty, `device.c` falls back to `SYS:Storage/AudioModes/VOID`.
2. **The driver's init needs a card.** `hdaudio.audio`'s `DriverInit` (`Drivers/HDAudio/driver-init.c`) returns FALSE if it finds no card. Its built-in PCI list has one entry, `8086:27D8`. Every other ID comes from **`ENVARC:hdaudio.config`**, which it reads with a plain `Lock`/`Open`.
3. **QEMU's card isn't on the built-in list.** QEMU's `intel-hda` is `8086:2668` (Vulkan4Aros wiki, 2026-09-28 HDA lane note). `hdaudio.config` lists `0x8086, 0x2668`.
4. **D32 hides the config file.** The prelude runs `Assign ENVARC: DH1:W`, which replaces `ENVARC:` rather than adding to it. The file lives at `DH0:Prefs/Env-Archive/hdaudio.config` (2182 B, `build/m0b13/staging/tree-dh0.txt:25151`). `DH1:W` has no such file (`tree-dh1.txt`).
5. **Result.** The driver sees no card, `OpenLibrary` fails, HDAUDIO is dropped, and AHI falls back to VOID. This matches every observation in M0b.14 and M0b.15. It also matches the riaudio lane, where HDA worked with `best_mode 0x003E0001` under the stock `ENVARC:`.

What this does not settle:

- **The serial log has neither of the driver's config-file lines** (`No handle found. IoErr()=…` or `Opened config file`). Those lines go through `DebugPrintF`, and whether that reaches serial on this build is unproven: no lane anywhere has ever logged either line. So the log neither confirms nor refutes the chain.
- **The database is a global public semaphore** (`"Audio Mode Database"`), and the scan runs only when it is empty. An earlier AHI open in the same boot would freeze it. Nothing in the evidence shows such an open before the game's first run, but nothing rules it out either.

## Findings against the round itself

- **Departures:** none that matter. The `pass.sh` wrapper is declared and no gate depends on it.
- **The report gives no causes for the FAILs,** as the prompt required. This is correct, not a gap.
- **M0b.15 did not add a gate that tells "the mode file is visible" apart from "the mode is in the database".** D77's log line was the only signal. The next round should gate it: `S15.ahi` should fail with `best=VOID`, not just on `queued`.

## What M0b.16 needs (owner choice on the fix)

1. **Probe first, and keep it cheap.** Before `OpenDevice`, the game logs whether `FindSemaphore("Audio Mode Database")` exists and how many modes it holds. It then logs the result of `OpenLibrary("DEVS:AHI/hdaudio.audio", 2)` and `Lock("ENVARC:hdaudio.config")`. One boot settles both open questions.
2. **Fix options for the ENVARC conflict.** All of them touch D32 or the lane, so the call is yours:
   - **(a)** The runner copies `AROS:Prefs/Env-Archive/hdaudio.config` to `DH1:W/hdaudio.config`. This is a lane-disk write, so the protected-state gates need an exception.
   - **(b)** The prelude opens AHI once before `Assign ENVARC: DH1:W`, after `DEVS:` is reassigned to RAM. The global database then holds HDAUDIO for the rest of the boot. This depends on the semaphore persisting, which the source shows it does.
   - **(c)** The D32 prelude stops replacing `ENVARC:`, and the Vulkan loader's JSON lookup moves elsewhere. This is the largest change.
   - Making `ENVARC:` a multi-assign (`Assign ENVARC: ADD …`) does not work: the driver's `Lock` only checks the first element, the same trap as `DEVS:AudioModes`.
3. **Fault risk.** Once HDAUDIO is actually admitted, `hdaudio.audio` `card_init` runs for the first time on this lane. The riaudio fault ladder (`card_init+0xF6A`) was fixed in the rebuilt binary (`b3aa4d3c…`), but this lane has never exercised that path. Predict the guest gates as unknown again.

## Still open

- **`returned=2`** is still unexplained. It now looks like VOID-driver behaviour and may stop mattering once a real driver loads.
- **Which `ahi.device` loads** (stock or rebuilt, same `$VER`) is still unproven. A distinct `$VER` is the owner's call.
- **Nothing is committed.** Your working tree still has the M0b changes, including D77 in `plat.cpp`.
