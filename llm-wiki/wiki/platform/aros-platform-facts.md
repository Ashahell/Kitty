# AROS platform facts the engine depends on

> Sources: M0b dispatch prompt, 2026-11-01; Vulkan4Aros source checks, 2026-10-30; Engine design rev 5, 2026-10-30; Engine design sources rev 5, 2026-10-30; Engine design rev 2 critique, 2026-10-29; M0b.13 critique, 2026-11-09; M0b.14 dispatch prompt, 2026-11-09; M0b.16–M0b.18 critiques, 2026-11-10; M0b.19 critique, 2026-11-11; M0b.20 and M0b.21 critiques, 2026-11-12
> Raw: [M0b prompt](../../raw/dispatch/2026-11-01-m0b-aros-prompt.md); [VK source checks](../../raw/platform/2026-10-30-vulkan4aros-source-checks.md); [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [Sources rev 5](../../raw/engine-design/2026-10-30-engine-design-sources-rev5.md); [Rev 2 critique](../../raw/reviews/2026-10-29-engine-design-rev2-critique.md); [Rev 3 critique](../../raw/reviews/2026-10-29-engine-design-rev3-critique.md); [M0b.13 critique](../../raw/reviews/2026-11-09-m0b13-aros-critique.md); [M0b.14 prompt](../../raw/dispatch/2026-11-09-m0b14-aros-guest-prompt.md); [M0b.16 critique](../../raw/reviews/2026-11-10-m0b16-aros-critique.md); [M0b.17 critique](../../raw/reviews/2026-11-10-m0b17-aros-critique.md); [M0b.18 critique](../../raw/reviews/2026-11-10-m0b18-aros-critique.md); [M0b.19 critique](../../raw/reviews/2026-11-11-m0b19-aros-critique.md); [M0b.20 critique](../../raw/reviews/2026-11-12-m0b20-aros-critique.md); [M0b.21 critique](../../raw/reviews/2026-11-12-m0b21-aros-critique.md)
> Updated: 2026-11-12

## Overview

These are the platform facts checked against the Vulkan4Aros tree (read-only) during the engine-design reviews. Facts derived from code are marked "inferred". Runtime behaviour stays **UNVERIFIED** until an M0 boot probe, because no boots were run during design.

## Facts checked in source

| Fact | Evidence |
|---|---|
| The loader adds `VK_KHR_surface`/`VK_KHR_xcb_surface` only when the ICD advertises them. Appending them unconditionally made NVK fail with `rc=-7 (VK_ERROR_EXTENSION_NOT_PRESENT)` | `vk_aros_loader.c:415`–`:424` (`boundary_r7_create_instance`, `:406`) |
| `AHI_DEFAULT_UNIT` is `(0U)` and `AHI_NO_UNIT` is `(255U)`; `io_Unit` is NULL for `AHI_NO_UNIT`; `CMD_WRITE` dereferences `io_Unit->unit_MsgPort` | `ahi.h:354-355`, `device.c:331-332`, `devcommands.c:109` |
| `Rename` onto an existing name: `ERROR_OBJECT_EXISTS` is raised in the FAT and AFS handlers, not in `rom/dos/rename.c` | `fat/ops.c:574`, `afs/filehandles2.c:503` |
| `PT-AHIPlay.s` (Protracker V2.3A playroutine adapted for `ahi.device`) splits speed and tempo at 32 and computes pitch from `#3546895` | `PT-AHIPlay.s:1202-1203`, `:354-356`, `:414-415` |
| `vkpal_setenv` calls `SetVar(..., GVF_LOCAL_ONLY)`; posixc builds `environ` from `GVF_LOCAL_ONLY` variables | [S84]: `vkpal_aros.c:720-726`, `__env.c:208-222` |

## v1 lane facts measured for M0b (2026-11-01)

| Fact | Value |
|---|---|
| Toolchain | The v1 lane is clang 20.1.0 (`AROS_TOOLCHAIN_FAMILY=llvm`), target flags `--target=x86_64-unknown-aros -mcmodel=large -mno-red-zone`. A v11-gcc binary dies on the v1 guest. clang rejects `-ffixed-r12`. |
| Link | The plain clang driver (no `-nostartfiles`, no manual stamp) produces `Type: REL`, `OS/ABI: AROS`, `ABI Version: 1`, 0 undefined GLOBAL, no `.got` section. Every shared Kitty TU cross-compiles with 0 warnings. |
| GIPA | The engine's static `vkGetInstanceProcAddr` (`src/render/vk_ctx.cpp:28`) is the one non-platform undefined symbol on AROS; GIPA must come from the loader base at `-40`. |
| SDK gaps | The v1 SDK has no `devices/ahi.h` and no `liblowlevel.a`; `ReadJoyPort` still links through the proto inline. |
| WSI | `arosCreateWindowSurfaceVia` (LVO 64), `arosPollWindowEventsVia` (LVO 72, 0 = close), `arosDestroyWindowSurfaceVia` (LVO 80). The library opens the window and calls `vkCreateXcbSurfaceKHR` itself. The header is C-only. |
| Clock | `timer.device` `GetUpTime` (timer LVO 12) is built for pc and used by the shipping PAL (`vkpal_aros.c:1020`); the `TR_GETSYSTIME` command is not implemented on pc. |
| Stack | AROS startup has no `__stack` cookie; the Shell's `Stack` command sets it. |
| r12 pattern | `llvm-objdump` prints the move as `movq\t%rax, %r12`. The pattern `mov[[:space:]]*%rax,%r12` (`build_deqp_vk.sh:74`) never matches it: a planted move counts 0 with it and 1 with `movq?[[:space:]]+%rax,[[:space:]]*%r12`. |

## Rules carried from the Vulkan4Aros records

- **Pacing:** never call `WaitTOF()`, which hangs forever on headless QEMU; pace with `timer.device` `UNIT_MICROHZ` (C13).

> **Status: Disputed** (2026-11-01)
> Design C14 says pc-x86-64 AROS has no clock API, so the only time units are `Delay()` ticks and `timer.device` wakeups. The M0b check found `GetUpTime` built for pc and used by the shipping PAL; only the `TR_GETSYSTIME` command is missing. Whether `GetUpTime` advances correctly on the guest is UNVERIFIED until the M0b timer probe runs.

- **Library lifetime:** open the Vulkan loader once and keep it resident. Never `Avail FLUSH`; deploy a fix by rebooting (C12/C24).
- **Toolchain:** cross-compile against the ABI-patched v1 SDK and gate on an r12 count of 0 (C05), using a pattern that matches `llvm-objdump` output (table above). Never strip beyond `--strip-debug`, and use `-fno-pic -fno-pie` on app binaries (C25).
- **Windowing:** the loader library owns the one AROS window; SDL2 must not open a second one (C08).

> **Status: Outdated** (2026-11-01)
> The design names `vkCreateArosSurfaceKHR` / `vkPollArosEventsKHR` as the window entry points. They are retired; the current API is the LVO 64/72/80 set in the table above.

- **NVK:** lane-only today. Its present path has never been exercised on the lane, so H8 stays BLOCKED until it is.

## Lane l0025 guest flake: the `Assign` listing (2026-11-09)

- A bare `Assign` redirected to a file faulted the guest on two boots: M0b.6 (`serial-20261102-2244.log`) and M0b.13 boot 2 (`serial-20261109-1828.log:360`). Both: `core_IRQHandle(14)`, `ERR=00000005`, `CR2=0000000000000088`, `IP=0000000001bf8d25`.
- The other six prelude boots on l0025 ran it clean. The IP is not symbolized; neither the Vulkan4Aros nor the ReIncarnation wiki names it.
- Kitty's runner sends it last from M0b.14 on, so a fault there costs only S15b.wiring.
- **The held listing later hung, rather than faulted,** on M0b.17 boot 1, M0b.18 boot 2 and M0b.21 boot 2: the runner exits 11 (`guest agent wedged at the Assign listing`). The M0b.18 review names `NameFromLock` walking `DRIVERS:`'s freed lock as a credible suspect; this is unproven.

## Lane l0025: `DRIVERS:` left on a freed lock (M0b.16–M0b.18)

- **The AROS bug.** `rom/dos/cliinit.c:644-647` locks `DEVS:Drivers` once and passes that one lock to both `AssignLock("DRIVERS", lock)` and `AssignAdd("LIBS", lock)`. The prelude's `Assign LIBS: DH1:Libs` replaces `LIBS:` and unlocks every old element, so `DRIVERS:` points at a reused, freed lock.
- **Confirmed directly in M0b.18:** `ahi_drivers hidd_lock=0 hidd_err=205`, with `dir=` different every time. 205 is `ERROR_OBJECT_NOT_FOUND`.
- **Consequence:** `hdaudio.audio`'s `OpenLibrary("DRIVERS:hdaudio.hidd", 0)` fails, the HDA mode is dropped, and AHI falls back to VOID.
- **Workaround (D86):** LDDemon finds a resident library by its bare name before it loads anything (`lddemon.c:360, 434`). The game opens `SYS:Devs/Drivers/hdaudio.hidd` first and holds it, which gives `ahi_best id=0x3e0001`.
- **Not possible:** any prelude line that replaces or removes the shared lock (re-pointing `DRIVERS:`, `REMOVE`, `PATH`), since those free it a second time.

## `PREPEND` inserts in second place (M0b.17)

- `AssignAddToList(…, 0)` in `rom/dos/assignaddtolist.c` advances once on a non-empty list, so `Assign LIBS: DH1:Libs PREPEND` gave `LIBS AROS:Libs + AROS:Devs/Drivers + L25DATA:Libs + AROS:Classes`.
- `DH1:Libs` came out third, and the Vulkan loader didn't open (both boots exited 8). D84 was withdrawn.

## Guest clock against the host

- The guest timer runs about 3 % slow against the host (1800 s against 1857 s, M0b.20 soak). Measured in the device's clock, a 20 ms tick lasts about 20.6 ms.

## UNVERIFIED (each with its M0 probe)

- Whether lavapipe in the same process reads a process-local `LP_NUM_THREADS`. The probe logs the `GetVar` result and lavapipe's worker-thread count.
- AHI device-API xrun behaviour, and chains deeper than the documented pair. Settled by an xrun probe at 960-frame passes.

> **Status: Outdated** (2026-11-12)
> Settled on the lane: chains deeper than two never start (M0b.18), and two 40 ms requests give no xruns (M0b.21). See [Audio](../engine/audio.md).
- `lowlevel.library ReadJoyPort` availability and port numbers on pc-x86-64. Settled by probing all 4 ports.
- `locale.library` runtime catalogs on x86-64. Settled by a boot probe at M3.

> **Status: Outdated** (2026-10-29)
> Revisions 2–3 called the process-local `SetVar` "NOT visible" to Mesa and prescribed a launcher `ENV:` file. Rev 2 had also marked it "Verified 2026-10-29" without a boot. The current text is the UNVERIFIED probe above.

## See Also

- [Renderer](../engine/renderer.md)
- [Audio](../engine/audio.md)
- [Saves and settings](../engine/saves-and-settings.md)
