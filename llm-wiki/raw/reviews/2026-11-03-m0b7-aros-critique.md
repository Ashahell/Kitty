# M0b.7 critique of the opencode report

> Source: Docs/Development/m0b7_aros_critique.md
> Collected: 2026-11-03
> Published: 2026-11-03

# M0b.7 critique — second guest round (2026-11-03)

**Verdict: M0b.7 not achieved; the D33 probe failed and the ladder did not run. The round was run correctly and is the first real guest progress. The trap has a one-line cause in Kitty code, found by reading the source; the report did not find it.**

Dispatch: `Docs/Development/dispatch/m0b7_aros_guest_opencode_prompt.md`, sha256 `35ad34e0393a7816a3dd18dd2ab7180eb3e3ddf5caa33d24cc9ab225561a899d`. Grant: 1 boot on l0025, used. The owner's grant of 2 is exhausted.

## Verified

- **Scripts:** the 9 scripts X0 writes are byte-identical to a fresh extraction, and all 25 pins in the published prompt match the round's files.
- **Census:** `build/m0b7/census.txt` has 112 lines: 93 PASS, 17 FAIL, 2 BLOCKED; seal `b2c0ff53…` matches. `S12c.complete` PASS, on the pins M0b.6 got wrong.
- **Tree:** the only file changed since script 0 is `tests/aros/lane_run.sh`, the D58 runner (`79feeb3e…`).
- **Ledger:** one `boots.txt` line, one new record `boot-20261103-2122.txt`.
- **Lane state:** inactive; nothing listens on 9099/9100/4479.

## What the boot proves

From `build/m0b7/guest/` and `~/Work/lanes/l0025/serial-20261103-2122.log`:

- **The crash did not recur.** No `PF-DBG` line; all four prelude lines answered `rc=0`. This does not isolate the cause: D58 changed two things at once (the prelude's form and the `Execute` redirect).
- **D32 wiring works.** `assigns.txt` shows `LIBS L25DATA:Libs + AROS:Libs` and `ENVARC L25DATA:W`.
- **D24 works.** `vk_loader="vulkan_loader.library"`, through `LIBS:`.
- **Kitty reached Vulkan on the guest:** `api=1.3`, `tier=lavapipe`, `device="llvmpipe (LLVM 20.1.0, 256 bits)"`, `stack_bytes=1048576`, `uptime_probe delta_us=100044`. These are the first guest lines past `pl_init`.
- **Trap teardown works (D37).** `S15b.trap-clean` PASS, `rc=4`, no `unfreed`; `S15b.plant-release` PASS, `rc=2`.

Not proven: any shader, pipeline, frame, window, present, audio, clock accuracy, or soak. `kitty` drew nothing.

## The trap: `src/platform/aros/plat.cpp:127` (not found by the report)

`p` trapped with `Software Failure. src/render/render_init.cpp:40: shader module missing or truncated`. The shaders were on the guest: serial lines 160–172 show all five `put`s, with correct sizes. Line 40 only reports the failure. The defect is in `dos_read`:

```c
LONG sz = Seek(fh, 0x7fffffffL, 0L);
```

AROS `rom/dos/seek.c` documents the result as "Absolute position in bytes before the Seek(), -1 if an error happened". So on a freshly opened file this returns 0 or -1, never the size. `-1` makes `dos_read` fail; `0` gives a zero-length buffer, which line 39 rejects (`buf.n < 4`). Either way **every AROS file read fails**, whatever the file. That includes `--music` (`main.cpp:318`).

The fix is the standard pair: seek to `OFFSET_END` with offset 0, then seek to `OFFSET_BEGINNING` with offset 0, whose return value is the size. This has not been run on a guest.

The defect has been there since M0b. Nothing reached it before: M0b's boots trapped at the loader, M0b.1 ran nothing, M0b.6 faulted in the prelude. No host gate can reach it, because the Linux build uses a different `pl_read_file`.

**Second defect, same file, line 161:** `pl_write_file` tests `Write(...) != FALSE`. `Write` returns the byte count, or -1 on error, so an error and a short write both count as success. Frame dumps could fail silently, and S15's frame gates would then report missing frames with no cause. Not yet exercised.

## Report errors

- **Wrong location.** "a Kitty-code defect in `src/render/render_init.cpp`" names the line that reports it, not the defect, and "the AROS package's `spv/` shaders appear not to load" stops at the symptom. D60 forbade edits, not reading. The serial log's `put`s and the 20-line `dos_read` would have settled it.
- **Misstatement.** "no `[PF-DBG]`/`Software Failure` in the guest's own kernel log": the serial log has `Software Failure.` at lines 276 and 294 (the output of `p` and `pt`). No `PF-DBG` is correct.
- **Miscounts.** "the 13 modified/untracked source paths": its own `git status` lists 17 (13 modified, 4 untracked). "all 26 match S12c's pins": 25 are pinned; `s12c.sh` is not, as the same sentence says.
- **Boilerplate left in.** Item 6's "`kitty` trapped before audio init" is an inference; no log line states it.

None of these changes a result. Everything else checked is accurate: hashes, census, seal, `exec_prelude.log`, `run_*.log`, `assigns.txt`, D60.

## Owner defects

- **`S15b.wiring` is a false FAIL (mine).** Its pattern wants `DH1:Libs` and `DH1:W`; the listing prints the volume name `L25DATA`. The prompt flagged the layout as UNVERIFIED, and opencode correctly reported rather than changed it. The wiring is correct. The pattern must accept the volume name.
- **No review ever read `dos_read`.** Eight M0b critiques audited runners and gates; none checked Kitty's AROS code against the DOS docs. `Seek` is a well-known AmigaDOS semantic, and D26 named it without its contract. Round after round went on process while a certain failure sat in the source.
- **Every gate on this path is host-only.** `S14.aros-source` checks the banned-API list, not API use. The next trap of this kind will also cost a boot unless the AROS layer is audited first.
- **The `sh_` logs are empty** (0 B each), as the prompt's fact predicted. They were a precaution for the crash, and may be needed; they carry no data.

## Next

1. Fix `dos_read` and `pl_write_file` under `src/platform/aros/`.
2. Before any boot, audit all of `src/platform/aros/` against the AROS API docs, since one boot per defect is the most expensive way to find them.
3. Correct `S15b.wiring`.
4. **The owner must grant a new boot**: the grant of 2 is used.
