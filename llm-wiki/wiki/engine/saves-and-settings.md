# Saves and settings

> Sources: Engine design rev 5, 2026-10-30; Engine design sources rev 5, 2026-10-30; Engine design rev 3 critique, 2026-10-29; Vulkan4Aros source checks, 2026-10-30
> Raw: [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [Sources rev 5](../../raw/engine-design/2026-10-30-engine-design-sources-rev5.md); [Rev 3 critique](../../raw/reviews/2026-10-29-engine-design-rev3-critique.md); [VK source checks](../../raw/platform/2026-10-30-vulkan4aros-source-checks.md)
> Updated: 2026-10-30

## Overview

Saves are a visible directory tree, `PROGDIR:Saves/Kitty/v1/`, that players can copy and trade. The tree holds two progress slots, `current/`, `replays/` and `prefs.envarc`. Settings are literally `ENV:`/`ENVARC:` variables. The content is byte-identical between AROS and Linux.

## Two-slot save

- `progress.a`/`progress.b`, plus a generation counter and a checksum; the newest valid slot is loaded.
- "Inferred from the handler sources: `Rename` onto an existing destination fails with `ERROR_OBJECT_EXISTS`, raised by the filesystem handler, not by `rom/dos/rename.c`; saves therefore use two slots instead of replace-by-rename."
- The evidence: `fat/ops.c:574` and `afs/filehandles2.c:503` both return `ERROR_OBJECT_EXISTS`, while `rom/dos/rename.c` has 0 occurrences. The `dos.md` autodoc says only "DOSTRUE or DOSFALSE. IoErr() provides additional information on DOSFALSE." This is inferred, not runtime-verified.
- "A `current/` that fails to load is discarded with a log line; progress loads from the newest valid slot."
- A malformed file gets a Guru-style report and the load is refused; there is never a partial load.

> **Status: Outdated** (2026-10-29)
> Revision 3's [S88] put into the `dos.md` autodoc a sentence about `ERROR_OBJECT_EXISTS` that the autodoc doesn't contain, and called the claim "verified at M0". Revision 4 replaced it with the handler-source citation ([S90]).

## Settings

`settings/` reads and writes through the `platform/` ENV API. Setting `ENV:Kitty/Renderer` to `lavapipe` forces the renderer tier. On Linux the values live under `~/.config`.

## See Also

- [AROS platform facts](../platform/aros-platform-facts.md)
