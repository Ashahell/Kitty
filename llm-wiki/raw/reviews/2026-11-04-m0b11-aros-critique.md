# M0b.11 AROS critique

> Source: Docs/Development/m0b11_aros_critique.md
> Collected: 2026-11-04
> Published: 2026-11-04

# M0b.11 critique — sixth guest round (2026-11-04)

**Verdict: M0b not achieved. All 6 FAILs come from one cause, and it's my design error: the rebuilt `ahi.device` never loaded. The drivers did, and the first fault is gone. The report's account of the audio run is false again, in the same words, despite a rule written against it.**

Dispatch: `Docs/Development/dispatch/m0b11_aros_guest_opencode_prompt.md`, sha256 `d47b8e4a7e9714eb77dbc9ee5368090cb102320eb61d8c4dd37930f1aab72983`. Boot record: `~/Work/lanes/l0025/boot-20261104-2322.txt`.

## Verified

- **Census:** 117 gate lines, seal `c3c9fe3d…` matches: 106 PASS, 6 FAIL, 2 BLOCKED.
- **Ledger:** one `boots.txt` line, one new boot record. No guest running.
- **Launcher:** the boot record's command line carries the device. `lane_audio.wav` exists, 44 B.
- **Wiring:** `assigns.txt` shows `DEVS  RAM Disk:kitty/Devs + AROS:Devs`. The assign itself worked.

## The fault

`dead_a.ppm`: task `RAM:kitty/kitty-debug`, `0x80000008`, module `ahi.device`, offset `0x1024`, function `ReadConfig`, offset `0x44`. `serial_a.txt:23`: `core_IRQHandle(13)`, right after `effects_ignored=0`.

**That is the stock file, from the lane's DH0, not the rebuilt one the runner put in RAM:**

| `ahi.device` | sha | `ReadConfig` at | at `0x1024` |
|---|---|---|---|
| DH0, stock | `4cd71c6b…` | `0xfe0` (+`0x44` = `0x1024`) | `movaps (%rax),%xmm0` |
| RAM, rebuilt | `155157e7…` | `0x1000` | `test %rdi,%rdi`, no `movaps` |

**Why:** `LDLoad` (`rom/lddemon/lddemon.c:140`), for a name with no colon, tries the caller's current directory (`ahi.device`, then `devs/ahi.device`), then its program directory the same way, and `DEVS:` last. The likely reading is that the run's current directory is the system root, so `devs/ahi.device` finds the stock file before the assign is consulted. **The run's current directory isn't logged, so that step is inferred;** that stock ran is proven.

**The drivers did load.** AHI opens them with a full `DEVS:AHI/...` path, which goes straight through the assign. The M0b.10 fault at `sb128.audio` `DriverInit+0x135` is gone, and the fault moved to the next step of the ladder the sister projects recorded (sb128 → hdaudio → `ReadConfig`). So the mode scan passed with the rebuilt set and the HDA device.

## The 6 FAILs

`S15.ahi`, `S15b.sound` and `a` in `S15b.output-captured` come from the fault. `S15.soak`, `S15.resident` and `S15.clock` follow from D70: a dead `a` means no `soak`, and those read the soak. That is D70 working as written, but I should have said `S15.clock` and `S15.resident` depend on it.

## The report

**False, the same as last round:**

- "No fault line (no `core_IRQHandle`) appears in `serial_a.txt`." It's on line 23. The prompt said to quote `grep -n core_IRQHandle` as it prints. It didn't.
- "The audio run simply idled until the budget expired", and `S15b.sound` "hit the agent's 900 s exec budget before the AHI playback could complete". The task faulted in `OpenDevice`; no playback began.

**Other breaches:**

- The prompt said no FAIL may be called expected. Three are.
- The prompt said, in bold, to paste the whole runner log. It pasted the end again.
- `S15.clock` is described as reading `a`. It reads `soak`.
- "Don't give a cause you haven't read in a file" was broken by every cause it gave.

Telling opencode to quote evidence has now failed twice. The evidence has to be in the gates' own output, where it can't be summarized away.

**Reliable:** the gate lines, ledger and dry-boot table, which all match the files.

## My errors

- **This boot was lost to something already written down.** ReIncarnation `raw/articles/2026-09-21-codec-less-hunk-plus8-green-probe.md`, in its loader notes: `devs:`-assign shadowing doesn't deliver bytes to lddemon, stock ran despite a verified assign, and `PROGDIR:ahi.device` works. `2026-09-22-abiv1-acceptance-lane-p1p2-green-stop-blocked.md` says the same. I'd just written a rule to grep these wikis, and I read one article and stopped. I flagged "whether AHI would load it from DH1/RAM" as not measured, then spent a boot on it when a search and `lddemon.c` would have answered.
- **I also didn't read `LDLoad`** before designing the deploy. It was readable the whole time.

## What's left for M0b

- **Next change:** make the rebuilt device load. Add `CD RAM:kitty` to `run_a.aros` and `run_soak.aros`, so `devs/ahi.device` resolves to `RAM:kitty/Devs/ahi.device`. No engine change, no image change. `DH0:C/CD` exists (3024 B).
- **Evidence in the gates:** `S15b.output-captured` prints each no-rc run's fault line, and the prompt stops depending on the report's prose.
- **Not measured, and possibly next on the ladder:** Kitty uses unit 0 with `CMD_WRITE`. The sister lanes' proof is mostly low-level AHI through `AHI_NO_UNIT`, and their notes record trouble on close (`CloseDevice`, a void-driver `Stop` that blocks). A further fault is plausible.
- **Durable fix:** rebuilding AHI with the flags in the AROS tree. Not Kitty's to do, and the RAM copies are a stand-in for it.
