# Open owner questions

> Sources: Kitty GDD v3, 2026-10-31; Kitty art bible v2, 2026-10-31; Kitty art bible v1, 2026-10-31; Engine design rev 5, 2026-10-30; Kitty project AGENTS.md, 2026-10-30; GDD v2, 2026-10-28
> Raw: [GDD v3](../../raw/game-design/2026-10-31-gdd-v3.md); [Art bible v2](../../raw/game-design/2026-10-31-art-bible-v2.md); [Art bible v1](../../raw/game-design/2026-10-31-art-bible-v1.md); [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [AGENTS.md](../../raw/project/2026-10-30-agents-md.md); [GDD v2](../../raw/game-design/2026-10-28-gdd-v2.md)
> Updated: 2026-10-31

## Overview

Engine design §15 lists Q01–Q14, each with a default that applies if the owner doesn't decide. Q11, the engine cap, was decided on 2026-10-29. This page also tracks project-level questions found while compiling this wiki.

## Engine design §15

| # | Question | Default / state |
|---|---|---|
| Q01 | `LP_NUM_THREADS` policy | 0, subject to an M0 probe of 0 vs 1 |
| Q02 | Joystick source on AROS | M0 probes all ports; a failing probe blocks M3, not M0 |
| Q03 | NVK goldens without a present path | baseline goldens only; H8 BLOCKED-with-reason |
| Q04 | State hash primitive | FNV-1a 64 |
| Q05 | MOD replayer provenance | own replayer; don't import micromod-class code before its licence is verified |
| Q06 | Catalogs | engine `.ct` reader on both platforms; `locale.library` as an offer |
| Q07 | Snapshot strategy | full keyframes per the §5.3 BUDGET line until M6 |
| Q08 | apiVersion | 1.3 floor although the manifest says 1.4.0 |
| Q09 | Boot-grant ownership for H7/H8 | spike agent per milestone |
| Q10 | Art/music ownership | v2 D5: named before M3 |
| Q11 | Engine cap | **Decided 2026-10-29: 23k** |
| Q12 | How `LP_NUM_THREADS` reaches lavapipe | process-local `SetVar`; the M0 probe reports whether it's read |
| Q13 | NVK self-test tolerance | per-channel ±4/255 + coverage-mask equality |
| Q14 | Is two-slot save enough? | two slots regardless; the owner decides at M6 on a third |

## Project-level (not yet in any doc)

- **Engine licence:** AGENTS.md says MIT, but v2 D3 says GPL-3.0-or-later. See [overview](../project/kitty-project-overview.md).
- **AGENTS.md is stale against v2:** its deps list and line budget predate v2 and the 23k decision.
- **Art bible E1/E2/E3 (2026-10-31):** a 32-entry palette uniform (L1–L3), cutscene/movie-clip playback, and per-chapter look levels L1–L4 are not in engine design rev 5. See [art bible](../game-design/art-bible.md).
- **GDD v3 D8/D9 (art bible Q4/Q5):** L4 aspect ratio (default: 5:4 puzzle area, 16:9 decorative margins) and the codec for L4 movie clips. See [Game design v3](../game-design/gdd-v3.md).
- **AGENTS.md** now points at GDD v3 and the art bible (2026-10-31).

## See Also

- [Engine design revision history](engine-design-revision-history.md)
