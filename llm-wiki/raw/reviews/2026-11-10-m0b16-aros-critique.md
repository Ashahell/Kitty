# M0b.16 AROS critique

> Source: Docs/Development/m0b16_aros_critique.md
> Collected: 2026-11-12
> Published: 2026-11-10

# M0b.16 review — 2026-11-10

Census: 116 PASS / 3 FAIL / 2 BLOCKED (121 gates).

Checked against the evidence, all correct:
- the seal `6fdacab5…`
- the runner `194a8a45…`
- `plat.cpp` `c47d9928…`
- HEAD `b5f929d`
- both ledger lines
- boot 2 graded (a 14/14 tie)

## Verdict

The harness round is clean, and audio still doesn't work. D80 did its job. D81 fixed something that wasn't the cause. The cause I gave in the M0b.15 review was wrong, and that was my mistake: I read an older driver source.

## What the round proved

- **D80 answered every question it asked.** Every audio run logged:
  - `envarc_config=1 envarc_err=0`: the config file is visible.
  - `hdaudio_open=0`: the driver still won't open, with the config in place.
  - `db_before=0` on run `a` and `db_before=1` on `soak`. The game's first open builds the mode database. Nothing earlier in the boot builds it, so the "frozen database" idea is ruled out.
- **D81 worked as built.** The copy is logged on both boots, the sha matches, and the census shows `S15b.envarc` PASS. It had no effect.
- **The three FAILs** (`S15.ahi`, `S15b.ahi-mode`, `S15b.sound`) are the same VOID-driver behaviour as M0b.14/15, byte for byte. The gates now name the cause precisely, as M0b.15 asked.

## Correction to the M0b.15 review

The deployed `hdaudio.audio` (`b3aa4d3c…`) has no built-in PCI ID list.
- `driver-init.c` (current tree) ignores the config's vendor/device lines: "Vendor/device ID lines are obsolete: controllers are discovered by PCI class through hdaudio.hidd."
- The config file now only carries `QUERY` and `speaker=` overrides. Steps 2–4 of the M0b.15 cause chain were wrong.

## Cause (from the source plus five rounds of evidence; a direct probe is still needed)

1. **The driver needs `DRIVERS:`.** `DriverInit` calls `ahi_hda_init`, which runs `OpenLibrary("DRIVERS:hdaudio.hidd", 0)` (`hda_hidd.c:405`). If that open fails, `DriverInit` returns FALSE, the open of `hdaudio.audio` fails, and the mode is dropped. The string `DRIVERS:hdaudio.hidd` is in the deployed binary.
2. **AROS boot puts one lock in two assigns.** `rom/dos/cliinit.c:644-647` locks `DEVS:Drivers` once, then passes that same lock to both:
   - `AssignLock("DRIVERS", lock)`
   - `AssignAdd("LIBS", lock)`

   Each call takes ownership of the lock (`assignadd.c:28`: "Do not use or free the lock after calling this function").
3. **D32 frees that lock.** The prelude runs `Assign LIBS: DH1:Libs`, which replaces `LIBS:` and unlocks every old element, the shared lock included. `DRIVERS:` is left holding a freed lock.
4. **The evidence fits.** The `assigns.txt` captured after the prelude shows `DRIVERS:` pointing somewhere different every round, wherever the freed lock was reused:

   | Round | `DRIVERS` |
   |---|---|
   | m0b7 | `AROS:` |
   | m0b8 | `AROS:System` |
   | m0b9 | `AROS:` |
   | m0b10 | `AROS:C` |
   | m0b11 | `AROS:S` |
   | m0b15 | `AROS:Demos` |
   | m0b16 | `AROS:Tools/Commodities` |

   The real file is at `DH0:Devs/Drivers/hdaudio.hidd` (24760 B, `tree-dh0.txt`). `LIBS:` no longer contains `AROS:Devs/Drivers` either, so neither path finds it.
5. **Why riaudio worked.** That lane never replaced `LIBS:`, so `DRIVERS:` stayed valid and `best_mode 0x003E0001` came up.

What this does not settle:
- **No direct proof.** No probe has yet locked `DRIVERS:hdaudio.hidd`. All five rounds match the chain, but nothing has tested it directly.
- **Is the AROS bug in the guest's build?** The double ownership is in the `aros-0038-pr` tree. Whether the guest image was built from code with the same `cliinit.c` is likely, given the evidence, but not checked.
- **Neither sister wiki records the bug.**

## Findings against the round itself

- **Departures:** none.
- **The report gives no causes for the FAILs,** as the prompt required. This is correct, not a gap.
- **D81 is now a permanent write to the DH1 image** for a file the driver only reads for overrides. It does no harm, but it is one exception too many to the IMAGES rule.

## What M0b.17 needs

1. **Fix in the prelude.** ~~Re-point `DRIVERS:` with `Assign DRIVERS: AROS:Devs/Drivers`.~~ This was replaced while writing M0b.17: re-pointing `DRIVERS:` would `UnLock` the already-freed lock a second time, which is a double free. M0b.17's D84 does this instead:
   - `prelude.aros` replaces its two LIBS lines with `Assign LIBS: DH1:Libs PREPEND`.
   - `PREPEND` calls `AssignAddToList(…, 0)`, which frees nothing. `DH1:Libs` still comes first.
   - The cost: `LIBS:` also keeps the boot-time `Devs/Drivers` and `SYS:Classes`.
2. **Probe to confirm, at the same spot as D80:**
   - `Lock("DRIVERS:hdaudio.hidd")` (result and IoErr);
   - `NameFromLock` on `DRIVERS:`;
   - `FindName(&SysBase->LibList, "hdaudio.hidd")` before and after the driver open.
   - A gate (`S15b.drivers`) that requires the lock to succeed.
3. **D81: drop it** and restore the IMAGES rule. Removing `DH1:W/hdaudio.config` then needs a single, logged cleanup step. **Or keep it.** Your call.
4. **Fault risk is unchanged from the M0b.15 review.** Once the HIDD opens, `card_init` runs on this lane for the first time. Treat the guest gates as unknown.
5. **Upstream:** the `cliinit.c` double ownership is a real AROS bug: any `Assign LIBS:` replace leaves `DRIVERS:` dangling. It belongs in the Vulkan4Aros / AROS PR queue, not in Kitty.

## Still open

- `returned=2` (VOID driver).
- Which `ahi.device` loads: the log shows `PROGDIR:Devs/ahi.device` 6.13, but the stock and rebuilt builds share the same `$VER`.
- Nothing is committed: 20 files are modified, including D80 in `plat.cpp`.
