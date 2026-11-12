# M0b.17 AROS critique

> Source: Docs/Development/m0b17_aros_critique.md
> Collected: 2026-11-12
> Published: 2026-11-10

# M0b.17 review — 2026-11-10

Census: 100 PASS / 20 FAIL / 2 BLOCKED (122 gates).

Checked against the evidence, all correct:
- the seal `d3415f7b…`
- the runner `5420ec0e…`
- `plat.cpp` `69d7a7ae…`
- `prelude.aros` `fdd64026…`
- both ledger lines
- boot 2 graded (a 3/3 tie)

## Verdict

The round is a regression. My fix, D84, broke the Vulkan loader. Both boots exited 8 after `p` (rc 4, `vulkan loader open failed`), so the ladder and the audio runs never ran. The error is mine: I said `PREPEND` puts `DH1:Libs` first without reading the insert loop all the way through.

## Errors in the report

- **The census numbers are wrong.** The report says 121 gate lines and 98 PASS. `census.txt` has 122 lines and 100 PASS. FAIL (20) and BLOCKED (2) are right.
- **The FAIL list is wrong.** It includes `S15.joystick(no—PASS)`, which is a PASS, and it leaves out `S15b.probe` (`rc=4 no vk_loader line`), which did FAIL.
- **The "Root cause" paragraph breaks the prompt's no-explanations rule.** Its observation is correct (`LIBS` starts with `AROS:Libs`). "No longer resolves" is the report's own inference.

## What the round proved

- **D84 ran as written.** The guest answered `Assign LIBS: DH1:Libs PREPEND` with rc=0. Boot 2's `assigns.txt`:
  - `DRIVERS AROS:Devs/Drivers`: valid for the first time on this lane. Freeing nothing worked.
  - `LIBS AROS:Libs + AROS:Devs/Drivers + L25DATA:Libs + AROS:Classes`: `DH1:Libs` is **third**, not first.
- **`PREPEND` is broken on multi-element assigns** (`rom/dos/assignaddtolist.c`).
  - The insert loop is `for (al = &dol_List; *al && cnt < position + 1; …, cnt++)`. With `position` 0 and a non-empty list, it advances once, so `cnt` becomes 1. The new entry goes in after the first list element, behind `dol_Lock` and one more.
  - The top-replace branch (`cnt == 0`) runs only when the assign has a single directory.
  - That explains the observed order exactly. The Startup-Sequence's `IMAGES:` `PREPEND` works only because `IMAGES:` has a single element at that point.
- **The loader did not open,** even though `vulkan_loader.library` exists only in `DH1:Libs` and nothing on DH0 has the same name.
  - Read from the source, `Open`'s loop over a multi-assign (`open.c:210-219`, `getdeviceproc.c`) should reach the third element.
  - So there are two candidates: the walk does not get there, or the newly visible `AROS:Devs/Drivers` and `AROS:Classes` change what the loader's init opens.
  - The evidence doesn't separate them. The game logs no IoErr for the failed open.
- **`pt` rc=4 and `ptr` rc=2 are as designed.** Only the real loader open failed.
- **Boot 1's listing exec hung** (`TIMEOUT`, `guest agent wedged at the Assign listing`). That is new: M0b.14–16's listings answered. Boot 2's listing answered. The cause is unknown.
- **D83 never ran.** No audio run, so there is no `ahi_drivers` line. `S15b.drivers` FAILs only on that half; its `DRIVERS` half is satisfied.

## What M0b.18 needs (owner choice on 2)

1. **Revert D84.** Restore M0b.16's `prelude.aros` (`be92bf86…`: `Assign LIBS: DH1:Libs`, then `Assign LIBS: ADD AROS:Libs`). The loader worked under it in every round from M0b.7 to M0b.16. With it, `DRIVERS:` is stale again.
2. **Get `hdaudio.hidd` open without `DRIVERS:`.** LDDemon looks a library up in the resident list by `FilePart(name)` before it loads anything (`lddemon.c:360, 434`). So once `hdaudio.hidd` is resident, the driver's `OpenLibrary("DRIVERS:hdaudio.hidd")` succeeds whatever `DRIVERS:` points at.
   - **(a) Game side.** In `pl_audio_open`, before the D80/D83 probe, run `OpenLibrary("SYS:Devs/Drivers/hdaudio.hidd", 0)` and keep it open until the audio closes. Log `ahi_hidd_preload=<0|1>`. `SYS:` is a single-element assign, so that path is safe. It's small, but it puts a workaround for an OS bug, with an HDA-specific name, into the AROS platform layer. It could be made a run-script flag so only the lane uses it.
   - **(b) Lane side.** `Assign DRIVERS: AROS:Devs/Drivers ADD` after the LIBS replace. Nothing is freed, and lookups fall through the stale first element to the valid second one. But every lookup through `DRIVERS:` still reads freed memory first. Not recommended.
   - **Not possible:** any prelude line that replaces or removes the shared lock (re-pointing `DRIVERS:`, `REMOVE`, `PATH`). Those free it a second time.
3. **Keep D83.** The probe line will then show `hidd_before=1` (preloaded) and `hidd_lock`, which shows whether `DRIVERS:` is stale.
4. **The risk is unchanged.** If the HIDD loads, `card_init` runs on this lane for the first time.
5. **Upstream, two AROS bugs** for the Vulkan4Aros / AROS queue:
   - `cliinit.c` gives one lock to two assigns, so any `Assign LIBS:` replace leaves `DRIVERS:` dangling.
   - `AssignAddToList(…, 0)` inserts in second place on a multi-element assign, so `Assign … PREPEND` doesn't prepend.

## Still open

- **Why the loader failed** with `DH1:Libs` third. This doesn't matter once D84 is reverted, but a `Lock("LIBS:vulkan_loader.library")` plus IoErr in the game would settle it cheaply.
- **The boot-1 listing hang.**
- **`returned=2`,** and which `ahi.device` loads.
- **Nothing is committed.** The tree carries D83 and D84.
