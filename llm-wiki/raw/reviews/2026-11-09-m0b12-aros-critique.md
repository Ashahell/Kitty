# M0b.12 AROS critique

> Source: Docs/Development/m0b12_aros_critique.md
> Collected: 2026-11-09
> Published: 2026-11-09

# M0b.12 critique — zero-boot round (2026-11-09)

Report: `~/Work/Kitty/repo/build/m0b12/report.md`.

**Verdict:** M0b.12 moved nothing forward. With BOOTS GRANTED set to 0, all 26 guest gates are BLOCKED, so M0b is still open. The only real work is two small edits that can't be tested without a boot: the runner install (X2) and the D71 AHI hunk (X3). Everything else re-ran gates that already passed.

**Errors in the report**
- **The PASS count in §2 is wrong.** It says 89 PASS. `census.txt` has 119 lines: 93 PASS and 26 BLOCKED. The report left out the four binary-path lines as if they weren't PASS, but they are. Script 12's own 91, plus S12b and S12c, also gives 93.
- **Some evidence is empty.** S1 and S2 print nothing in the pass. Their PASS shows up only as a census line, and the report doesn't mention it.

**Risks it didn't flag**
- **`src/app` is at exactly 600 / 600.** Any line added there fails S3.
- **S7.golden is blocked by the host, not by the boot grant.** The pinned host lavapipe isn't built. It's the only blocked gate that doesn't need a boot, and the report treats it as if it were ambient.
- **D71: `audio_source=` doesn't prove anything.** It logs the path that was *requested*, not the bytes that loaded. The ReIncarnation 2026-09-21 ZZTEST note says later plain opens get whatever `ahi.device` is already resident. To make S15b.ahi-source conclusive, log `io_Device->dd_Library.lib_IdString` and its version as well.
- **D71: the fallback hides why it fell back.** If the `PROGDIR:Devs/ahi.device` open fails, its `io_Error` is overwritten by the plain `ahi.device` open. The boot would then show only `audio_source=ahi.device`, with no reason.
- **The rest of the AHI setup looks sound** (`src/platform/aros/plat.cpp:480-498`, `tests/aros/prelude.aros`). The prelude assigns `DEVS: RAM:kitty/Devs` first and then adds `AROS:Devs`. So `DEVS:AHI/sb128.audio` resolves to the rebuilt copy, and the stock `AudioModes` still come from AROS. That matches the Vulkan4Aros 2026-09-28 recipe.

**Minor**
- The first wrapper run happened before X0 had extracted the scripts. It did no harm, but it was a sequencing mistake.
- S5 shows binary hashes cut to 16 hex characters, which is weak as evidence.
- DH1 `.recycled/` holds three old `vulkan_loader` copies. They aren't on the LIBS path, so they're just clutter.

**What's needed next:** the next round needs at least one boot granted. Without one, M0b.13 would only re-run gates that already pass. Fix the two D71 logging gaps first, so that one boot shows which `ahi.device` actually loaded.

## D71 logging fix (applied after the review, 2026-11-09)

I fixed both D71 logging gaps in `src/platform/aros/plat.cpp`. The AROS build compiles clean, but it hasn't run on a guest yet.

**What changed:**
- **The PROGDIR failure is now logged.** When `PROGDIR:Devs/ahi.device` fails to open, the code logs `audio_progdir_error=<io_Error>` before trying plain `ahi.device`. Before, that second open overwrote the first error.
- **The log now shows which device actually loaded.** After `audio_source=`, it logs `audio_device_id="<lib_IdString>" version=<ver>.<rev>`, read from the opened device. The id string is cut at the first control character or `"` (the id string normally ends in CR/LF) and is capped at 79 characters. If there is no id string, it logs `none`. This is the line that makes S15b.ahi-source conclusive.

**Checks:**
- `make aros` passed with `-Werror`, using the v1 toolchain the scripts use. Both builds still show `aros-gates: type=REL osabi=AROS abi=1 und=0 r12=0`.
- `kitty-lint src` passed.
- `src/platform/aros` is at about 911 of its 1200-line budget.

**One gate will now fail:** the new sha256 of `plat.cpp` is `9aede77e…5995decfa5`. S14b.pinned and script 12's end state are pinned to the M0b.12 sha (`e71b5850…`), so they will fail until the next round's prompt pins the new one.

Running this still takes one boot. On that boot, look for `audio_device_id=` with the rebuilt `ahi.device`'s version, and no `audio_progdir_error=` line.
