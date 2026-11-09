# opencode prompt: M0b (AROS half of M0), revision fixing the trap-location gate

> Source: Work/Kitty/Docs/Development/dispatch/m0b_aros_opencode_prompt.md
> Collected: 2026-11-02
> Published: 2026-11-02

# opencode prompt — Kitty: Core Awakening, **M0b** (AROS half of M0, CopperForge engine) (2026-11-01)

## Owner authorization (read first — opencode never edits this block)

    BOOTS GRANTED: 2
    LANE: l0025
    WRITE: ~/Work/Kitty/repo/ — the committed M0a + M0a.1 tree (main = 38fc422). Edit only src/ tools/
           shaders/ tests/, Makefile and .gitignore. README.md and LICENSE are never edited.
           build/m0a/ and build/m0a1/ (earlier evidence) are never edited or deleted; build/m0b/,
           build/aros*/ belong to this round.
    DO NOT EDIT: ~/Work/Kitty/Docs/ (all of it — reports go in build/m0b/, never in Docs),
           ~/Work/Kitty/AGENTS.md, ~/Work/Kitty/llm-wiki/, every ~/Work/host_lavapipe*/ directory.
    BUILDS: host g++ builds through the repo Makefile; AROS cross-builds through the repo Makefile with
           the Vulkan4Aros v1 toolchain and SDK (read-only use: run the compilers, read the headers).
           NO run of build_host_lavapipe.sh, no Vulkan4Aros build script, no library rebuild.
    RUN: the repo's binaries and tests; the acceptance scripts below exactly as given; read-only shell
           commands. A host window may be opened only by scripts 8 and 13 (each run ≤ 65 s).
    LANES / BOOTS: a boot is any start of an AROS guest. Boots are allowed only while the number of
           lines in build/m0b/boots.txt is below BOOTS GRANTED, only on the lane named in LANE, and
           only through that lane's own launcher (LANE l0025 → ~/Work/projects/Vulkan4Aros/scripts/
           lane_l0025_boot.sh). Append one line `<ISO time> <lane> <reason>` to boots.txt BEFORE
           each boot. With BOOTS GRANTED: 0 nothing may start a guest, write to ~/Work/lanes/, or
           submit a job to a spike server: every on-AROS gate is BLOCKED, and that is the expected
           result of this round. Never `Avail FLUSH`, never RemLibrary, never reload a library.
    VULKAN4AROS REPO: READ-ONLY (cat/grep/ls/sed -n/git show/git log/git status, and running its
           compilers). Never run a write or destructive git command anywhere in it, never write a file
           in it, and never touch `src/abi/v1/AROS*`.
    GIT: read-only git commands in ~/Work/Kitty/repo. No commit, no branch, no stash, no tag, no
           push, in any repository. The owner commits M0b after review.
    PACKAGES: install nothing. WEB: read-only research allowed; post nothing, download no binaries.

Don't ask for, infer, widen or grant any authorization yourself. If the work seems to need
something outside this block, stop that item, mark it FAIL or BLOCKED, and list it in the final report.

## What this task is

M0a + M0a.1 (the Linux half of M0) are committed on `main` (`38fc422`, pushed 2026-11-01). M0b is
the AROS half: `src/platform/aros` implementing the same `Plat` interface, an AROS cross-build with
the H6 gates, the AROS presented-pixel readback, and a guest package plus runner for the on-AROS
gates (H7-M0, `t-aros-boot`, the M0 measurements). It also clears the three code defects and two
notes the M0a.1 review carried forward (W0).

- **The Linux build must not change behaviour.** Headless frames 0/150/299 stay byte-identical to
  `build/m0a/r1/` and `hash10s` stays `e663d2da535e69a5`; scripts 1–11 and 13 re-run every Linux gate.
- **Every plant appends to `src/platform/aros/plat.cpp`**, so that file must exist and be part of `make aros`.
- **Boots: 2 on lane l0025 (owner grant 2026-11-01).** Script 14 must be all PASS before the first
  boot: no guest run on a binary the host gates reject. One boot is meant to carry the whole
  `m0b-run.aros` sequence; the second is for a rerun after a fix or a crash. A third boot is never
  taken: whatever is still missing is BLOCKED. No TCG, no other lane.
- The engine is named **CopperForge** (owner decision 2026-10-31): game-agnostic, namespace `cf` when
  code is renamed (not this round — no renames). New engine-side strings use "CopperForge", never
  "Kitty" (the window title, D27).
- No new features, no design edits, no golden frames (S7.golden stays BLOCKED: pin not built).

## Verified facts (measured 2026-11-01 — don't re-derive, don't contradict)

| Fact | Value |
|---|---|
| Tree under work | `main` = `38fc422d388c9886a38045ff5dc665dab369697d`; branches `m0a`, `main`; no tags, no stash. 66 files in `.gitignore Makefile src tools shaders tests`; the sorted `sha256sum` list hashes to `a6ca25673ae77a5b67c8b8dadc083ab4af6d1ad87a366629dffac7fbe0d39dfb`. Script 0 stops if the tree differs. |
| v1 toolchain | `AROS_ABI=v1 . ~/Work/projects/Vulkan4Aros/scripts/aros_build_env.sh` exports `AROS_TOOLCHAIN_FAMILY=llvm`, `AROS_CC`/`AROS_CXX` = `…/src/abi/v1/toolchain-core-x86_64/bin/clang[++]` (clang 20.1.0), `AROS_TARGET_CFLAGS="--target=x86_64-unknown-aros -mcmodel=large -mno-red-zone"`, `AROS_SDK_INCLUDE=…/src/abi/v1/sdk/Developer/include`, `AROS_READELF`, `AROS_OBJDUMP`, `AROS_COLLECT_AROS_DIR` (must be on `PATH` with `$AROS_TOOLCHAIN/bin`). Sysroot = `${AROS_SDK_INCLUDE%/include}`. A v11-gcc binary dies on the v1 guest (`build_juggler_rt_v1.sh:6–7`). **gcc's design flag set does not apply:** clang rejects `-ffixed-r12` ("unknown argument"). |
| Cross-compile of the current tree | Every shared TU (`src/{audio,benchmark,guru,render,platform/rt}/*.cpp`, `src/app/*.cpp`) compiles with `$AROS_CXX $AROS_TARGET_CFLAGS --sysroot=<sysroot> -std=c++20 -fno-exceptions -fno-rtti -ffp-contract=off -Wall -Wextra -Werror -fno-pic -fno-pie -fno-stack-protector -fno-strict-aliasing -mno-ms-bitfields -O2` with **0 warnings**. `-nostdinc++` breaks `<cstdarg>`; don't use it. `rt_fmt.o` built that way has **no** undefined symbols. Linking all of them with the plain driver (`$AROS_CXX $AROS_TARGET_CFLAGS --sysroot=…  objs -o kitty`) leaves exactly these undefined: the 23 `pl_*`/`plat_*` functions the Linux platform provides, and **`vkGetInstanceProcAddr`** (`src/render/vk_ctx.cpp:28`, declared `vk_ctx.h:4`). libc symbols (`memcpy memset strcmp atoi strtoul`) resolve through the driver's default libraries. |
| Driver link output | A C++ probe (exec, dos, lowlevel, intuition, cybergraphics, vulkan headers; `GetVar`, `SetVar`, `AllocVec`, `ReadJoyPort`) linked by the plain driver, **without** `-nostartfiles` and **without** a manual stamp, comes out `Type: REL`, `OS/ABI: AROS`, `ABI Version: 1`, 0 undefined GLOBAL, 0 r12 sites, no `.got` section. (The juggler's manual `printf '\017\001' \| dd` stamp is only for its direct `ld.lld` link.) |
| r12 gate pattern | `llvm-objdump` prints the stale-SDK move as `movq\t%rax, %r12` (suffix, space after the comma). The pattern `mov[[:space:]]*%rax,%r12` (Vulkan4Aros `build_deqp_vk.sh:74`, and the design's `grep -c 'mov %rax,%r12'`) **never matches it**: a planted `mov %rax,%r12` linked into a probe counts 0 with it and 1 with `movq?[[:space:]]+%rax,[[:space:]]*%r12`. Use only the second pattern. |
| SDK gaps | The v1 SDK (`sdk/Developer` → `toolchain-core-x86_64-build/bin/linux-x86_64/AROS/Developer`) has `proto/{exec,dos,timer,lowlevel,intuition,cybergraphics}.h`, `devices/timer.h`, `libraries/lowlevel.h`, `intuition/intuitionbase.h`, `vulkan/`, but **no `devices/ahi.h`** and no `liblowlevel.a`. `devices/ahi.h` exists at `~/Work/projects/Vulkan4Aros/src/abi/v1/core-pc-x86_64/bin/pc-x86_64/AROS/Developer/include/devices/ahi.h`. `ReadJoyPort` links with 0 UND through the proto inline and a `struct Library* LowLevelBase` global. |
| Clock | `timer.device` LVO `GetUpTime(struct timeval*)` (timer LVO 12, `rom/timer/getuptime.c`, built for pc by `rom/timer/mmakefile.src:23`, "can be considered to be a monotonic clock") is what the shipping PAL uses (`src/vulkan/pal/vkpal_aros.c:1020`). The **command** `TR_GETSYSTIME` is not implemented on pc (Vulkan4Aros wiki, probe-evidence rule 239). Design C14 ("no clock API") conflates the two — report it. GetUpTime advancing correctly on the guest is **UNVERIFIED** until a boot (D23 probe). |
| Loader | `OpenLibrary` order: `"RAM:vulkan_loader.library"`, then `"vulkan_loader.library"`, then `"RAM:vulkan.library"`. GIPA = `*(void**)((u8*)base - 40)`. Never `CloseLibrary` it. Source: `src/vulkan/loader/aros_cube/dlfcn.h`. |
| WSI | `vkCreateArosSurfaceKHR` / `vkPollArosEventsKHR` (design §4.1, §8 C08, §597) are **retired**. Current API, `src/vulkan/loader/aros_window_surface.h` (last change `78d44619`): `arosCreateWindowSurfaceVia(base, inst, w, h, title, alloc, &surf)` (LVO 64), `arosPollWindowEventsVia(base, surf)` (LVO 72; returns 0 when close was requested), `arosDestroyWindowSurfaceVia(base, inst, surf, alloc)` (LVO 80; older libraries lack the slot). The library creates the Intuition window and calls `vkCreateXcbSurfaceKHR` itself; the app enables only `VK_KHR_surface`. Present is a synchronous `WritePixelArray`; FIFO and IMMEDIATE behave the same. **The header is C-only:** included from C++ it fails (`aros_window_surface.h:144`, `return 0;` into `VkResult`); from C11 with `-Wall -Wextra -Werror` it compiles. |
| Window lookup | The window belongs to the library. To read it back: assign the **global** `IntuitionBase` (a local leaves the global NULL and faults in `LockIBase`), `LockIBase(0)`, walk `FirstScreen`→`FirstWindow` matching `Title`, `UnlockIBase`. Pattern: `src/vulkan/loader/surface_probe.c:355–385`. Readback: cybergraphics `ReadPixelArray` (LVO −160), `RECTFMT_BGRA32` = 111 (design §6.6). |
| Stack | AROS startup has no `__stack` cookie; the Shell's `Stack` command sets it. |
| Spike client | `python3 ~/Work/projects/Vulkan4Aros/scripts/spike_server.py submit --spool <lane spool> [--put-tree L:R] [--exec CMD] [--run-script PATH] [--get R:L] [--get-dir DIR] [--wait S]` (run from the scripts directory: it imports `ui_frames`). Lane l0025: launcher `lane_l0025_boot.sh`, servers 9099/9100, spool `~/Work/lanes/l0025/spool`, monitor 4479, KVM only. Transfer limits: `~/Work/projects/Vulkan4Aros/llm-wiki/atcp-transfer-paths.md`. |
| Pinned lavapipe | Patch 0025 fails 2 of 2 hunks; S7.golden stays BLOCKED (owner task). |
| Host | g++ 16.2.1, Vulkan 1.4.363 + `VK_LAYER_KHRONOS_validation`, sdl2-compat 2.32.74, system lavapipe Mesa 26.2.5. Guest lane Mesa is 26.2.2 + patches. |

## Inputs (read before writing code)

- `~/Work/Kitty/Docs/Development/m0a1_linux_critique.md` — the review that defines W0.
- `~/Work/Kitty/Docs/Development/dispatch/m0a1_linux_fix_opencode_prompt.md` and
  `…/m0a_linux_opencode_prompt.md` — their deltas D1–D19, CLI, bans and gates still hold, except
  where D20–D31 below replace a sentence.
- `~/Work/Kitty/Docs/Development/kitty_engine_design.md` — §4.1, §4.5 A, §4.6 D, §4.7 A, §6.6, §6.7,
  §7, §9 (AROS cross, CRT/allocator), §10 H6/H7/H7-M0, §11 M0, §12. Where the design names the
  retired WSI entry points, the gcc flag set, `-nostartfiles`/`__startup_entry`, or "no clock API",
  the facts table and D20–D31 win; report each as a design problem. Anywhere else the design wins.
- `~/Work/Kitty/Docs/Development/kitty_core_awakening_design_v3.md` §8 (CopperForge, platform) and
  §11.2 M0 (go/no-go numbers).
- Vulkan4Aros, read-only: `src/vulkan/loader/aros_window_surface.h`, `aros_cube/dlfcn.h`,
  `surface_probe.c`, `src/vulkan/pal/vkpal_aros.c` (timer use), `llm-wiki/aros-v1-lane-chain.md`,
  `llm-wiki/atcp-transfer-paths.md`.

## Skills (load each, and report which ones you loaded)

| Skill | Use it for |
|---|---|
| `karpathy-guidelines` | **Mandatory, first.** Surgical changes; W0 touches only what the three defects need |
| `aros-development` | **Mandatory.** Library bases, LVO conventions, AHI `struct AHIRequest`, the stale-SDK r12 trap, guest triage |

## Design deltas (owner decisions for M0b — implement exactly; list them in the report)

| # | Decision |
|---|---|
| D20 | **`pl_trap` names its caller.** `plat_svc.h` line 6 becomes `[[noreturn]] void pl_trap(const char* file, u32 line, const char* msg);` and the header defines `#define PL_TRAP(msg) pl_trap(__FILE__, __LINE__, (msg))`. Output: `Software Failure. <file>:<line>: <msg>`, exit 4. `S14.trap-locations-real` checks these source locations in every `build/m0b/*.log` except `t-guru.log`: that log holds `guru_report` lines whose location is a game-data file (`room.txt:12`, required by S10), not a source line. Every caller (`traps.cpp`, both `rv_die` fallbacks, anything else) passes a real location: `PL_TRAP`, or its own `file`/`line` arguments when it already has them. Make is run from the repo root, so `__FILE__` is repo-relative (`src/…`, `tests/…`). Add `tests/t-trap.cpp` (in `make tests`, not in `make test`): it calls `PL_TRAP("t-trap probe")` on exactly one line and nothing after it. |
| D21 | **Probe devices fall back, never exit** (corrects D16 for probes). When the candidate does not support `shaderDemoteToHelperInvocation`, `r_tier_probe` creates no device and sets `res->ok=false`, `reason="shaderDemoteToHelperInvocation unsupported"`; the probe line is logged and the run continues on lavapipe (GDD §8.3). The main device keeps D16 (Guru line, exit 4). Debug-only `--plant-no-demote` (with `--tier-probe`) makes the probe treat the candidate's feature as unsupported; release builds exit 2 on it. |
| D22 | **`rt_fmt` outside D14's range.** `%.Nf`: NaN prints `nan`, +∞ `inf`, −∞ `-inf`; a finite value with `|v| ≥ 1e12` copies the `%.Nf` sequence verbatim. A width above 32 copies the whole sequence verbatim. In both cases the argument **is consumed** (so the next conversion reads the next argument). No UB on any input: no float→integer cast unless `|v| < 1e12`. |
| D23 | **AROS clock and pacing.** `platform/aros` opens `timer.device` `UNIT_MICROHZ` once in `pl_init` (a full `struct timerequest` from `CreateIORequest` on a `CreateMsgPort` port) and sets the global `TimerBase` from `io_Device`. `pl_now_us` = `GetUpTime` in µs. Sleeping is a `TR_ADDREQUEST` + `DoIO` on that request. `pl_step` uses the same `pace_step` as Linux (moved to `src/platform/rt/pace.h`, included by both `plat_priv.h` files and by `t-pacing`). No `WaitTOF`, no `Delay()`, no busy-wait, anywhere in `src/`. Startup probe: `GetUpTime`, a 100,000 µs `TR_ADDREQUEST`, `GetUpTime`; log `uptime_probe delta_us=<n>`; if `n` is outside 80,000–250,000, log `Software Failure. <file>:<line>: timer.device uptime not advancing` and exit 4. |
| D24 | **AROS loader and GIPA.** `pl_init` opens the loader once, in the order of the facts table, into `Plat::vk_lib`, and logs `vk_loader="<name that opened>"`. It is never closed. Open failure: Guru line naming the three names, exit 4. New `plat_svc.h` line: `PFN_vkGetInstanceProcAddr pl_vk_gipa(Plat&);` (`plat_svc.h` may include `<vulkan/vulkan.h>`). AROS returns `*(void**)((u8*)p.vk_lib - 40)`; Linux returns `&vkGetInstanceProcAddr`. `vk_ctx.cpp` gets GIPA only through `pl_vk_gipa`: the identifier `vkGetInstanceProcAddr`, as a whole word, appears nowhere in `src/` outside `src/platform/linux/`. The type name `PFN_vkGetInstanceProcAddr` is a different identifier and is allowed anywhere (the `plat_svc.h` line above needs it). At shutdown AROS logs `vk_loader_open_count=<lib_OpenCnt>`. |
| D25 | **AROS WSI.** The loader header is included by exactly one C11 file, `src/platform/aros/aros_wsi.c`, which exports three `extern "C"` wrappers (`cf_aros_create_surface`, `cf_aros_poll`, `cf_aros_destroy_surface`) over the `…Via` inlines, using `Plat::vk_lib` as `base`. `pl_wsi_instance_extensions` returns exactly `VK_KHR_surface`. `pl_wsi_create_surface` creates a 320×256 window titled `CopperForge` and logs `surface=ok window_w=320 window_h=256` (or the `VkResult` and exit 4). `pl_poll_events` calls the poll wrapper once per call; a 0 return sets `quit` and queues one kind-1 event (as `SDL_QUIT` does on Linux). New `plat_svc.h` line: `void pl_wsi_destroy_surface(Plat&, VkInstance, VkSurfaceKHR);` — the renderer destroys its surface only through it. AROS: LVO 80; if that returns non-success, destroy through GIPA's `vkDestroySurfaceKHR` and close any window titled `CopperForge` by the window walk (the `surface_probe.c` fallback), logging `surface_destroy=<lvo80|fallback>`. Linux: `vkDestroySurfaceKHR` through GIPA. |
| D26 | **AROS files, env, memory, log.** `Alloc` = `AllocVec(n, MEMF_ANY \| MEMF_CLEAR)`; every free is `FreeVec`. Files: DOS `Open`/`Read`/`Write`/`Seek`/`Close` (`MODE_OLDFILE`/`MODE_NEWFILE`). `pl_env_get` = `GetVar(key, v, n, 0)` (empty string on failure). `pl_log` writes the line plus `\n` to `Output()` with `Write`, guarded by one `struct SignalSemaphore` (the debug messenger may run on another task); the debug build also mirrors each line to the serial log with `RawPutChar`. All text is formatted with `rt_fmt`: no `Printf`, `VPrintf`, `FPrintf`, `RawDoFmt`, `VNewRawDoFmt`, `printf`-family, `malloc`/`free`, `<cstdio>` or `<iostream>` in `src/platform/aros/`. `pl_init` also logs `stack_bytes=<tc_SPUpper − tc_SPLower of FindTask(0)>` and exits 4 through the Guru line if it is under 262,144. |
| D27 | **`LP_NUM_THREADS`.** In `pl_init`, before any Vulkan call: `SetVar("LP_NUM_THREADS", v, -1, GVF_LOCAL_ONLY)` with `v` = `"0"`, or the debug-only `--lp-threads N` value (`N` ∈ {0,1,4}; release exits 2 on the flag). Then log `lp_num_threads set=<v> getvar="<GetVar result>" getvar_rc=<rc>`. After `vkCreateDevice`, log `lp_worker_tasks=<n>`: under `Forbid()`, the number of tasks in `SysBase->TaskReady` and `TaskWait` whose `ln_Name` begins with `llvmpipe`. **UNVERIFIED:** that lavapipe names its workers that way on AROS; report the raw count. Linux keeps `LP_NUM_THREADS` untouched (no `--lp-threads` on Linux: exit 2). |
| D28 | **AHI** (design §4.6 D exactly). `pl_audio_open(p, 48000, 2)` opens `ahi.device` unit `AHI_DEFAULT_UNIT` (0) with a full `struct AHIRequest` (never `AHI_NO_UNIT`, never a bare `IORequest`); four `struct AHIRequest` buffers of 960 stereo S16 frames (3,840 B each, `AllocVec`), chained through `ahir_Link` in submission order, `CMD_WRITE`, `AHIST_S16S`, `ahir_Frequency=48000`, `ahir_Volume=0x10000`, `ahir_Position=0x8000`. `pl_audio_queue` fills and `SendIO`s the next free buffer; a call with no free buffer drops the frames and counts them. `pl_audio_queued_frames` = 960 × buffers in flight (`CheckIO`). When all four complete before a refill, log `ahi_xrun pass=<n> tick=<t>` (pass = buffers submitted so far). Open failure is non-fatal: `audio_device=none reason="<OpenDevice rc>"`, music off. Success logs `audio_device=ahi.device unit=0`. Only `src/platform/aros/` TUs that include `devices/ahi.h` get `-idirafter <core-pc Developer include>`; no other compile line carries it. At exit `AbortIO`+`WaitIO` every in-flight request, then `CloseDevice`. |
| D29 | **Joystick probe.** `pl_init` opens `lowlevel.library` (global `LowLevelBase`); if absent log `joy lowlevel=none`. Otherwise `ReadJoyPort(n)` for n = 0..3, log `joy port=<n> state=0x<8 hex>`. No other use in M0. |
| D30 | **Presented-pixel proof** (design §6.6). Flag `--present-dump F` (AROS only; Linux exits 2): after frame `F` has been presented, find the window titled `CopperForge` (D25 walk), check the client area is 320×256 and the screen depth (`GetCyberMapAttr(rp->BitMap, CYBRMATTR_DEPTH)`) is ≥ 24, then `ReadPixelArray(buf, 0, 0, 1280, w->RPort, w->BorderLeft, w->BorderTop, 320, 256, RECTFMT_BGRA32)` and write `<out>/present_<F>.bgra` (327,680 B). Log `present_dump frame=<F> client=<w>x<h> depth=<d> rpa=<return>`; when the area or depth is wrong log `present_dump blocked reason="…"` and write nothing (the gate is then BLOCKED, never compared). Debug-only `--force-fifo` forces the FIFO present mode (the `t-aros-boot` known-bad: must not hang). |
| D31 | **AROS build and package.** `make aros` → `build/aros/kitty`, `make aros-debug` → `build/aros-debug/kitty` (`-DKITTY_DEBUG`), `make aros-pkg` → `build/aros-pkg/` (both binaries, `spv/*.spv` from the host shader build, `tests/fixtures/test.mod`, the guest scripts, and `MANIFEST.sha256` over every other file in the package). None of them is part of `all`, `linux`, `tests` or `test`. The Makefile takes the toolchain only from the environment (`AROS_CC AROS_CXX AROS_TARGET_CFLAGS AROS_SDK_INCLUDE AROS_READELF AROS_OBJDUMP AROS_COLLECT_AROS_DIR`, plus `VK4A ?= $(HOME)/Work/projects/Vulkan4Aros`). Before compiling it fails with `stale SDK: <path>` unless `realpath(AROS_SDK_INCLUDE)` equals `realpath($(VK4A)/src/abi/v1/sdk/Developer/include)` and `AROS_ABI_EXPECT_MAJOR=1`. Before compiling it runs `kitty-lint src` and a banned-API scan that prints `banned API <name>` for `WaitTOF`, `Delay(`, `RemLibrary`, the word `Avail` (`AvailMem` is fine), and `CloseLibrary` on `vk_lib`. C++ compile lines: `$(AROS_CXX) $(AROS_TARGET_CFLAGS) --sysroot=<sysroot> -std=c++20 -fno-exceptions -fno-rtti -ffp-contract=off -Wall -Wextra -Werror -fno-pic -fno-pie -fno-stack-protector -fno-strict-aliasing -mno-ms-bitfields -O2` with D12's per-directory `-I` sets, `src/platform/aros` in place of `src/platform/linux`. The one C line (`aros_wsi.c`) uses `$(AROS_CC)` with the same target/sysroot/warning/codegen flags, `-std=c11`, and `-I$(VK4A)/src/vulkan/loader`. Link: the plain driver (`$(AROS_CXX) $(AROS_TARGET_CFLAGS) --sysroot=… objs -o …`), no `-nostartfiles`, no manual stamp. After linking, the Makefile gate prints `aros-gates: type=<T> osabi=<O> abi=<A> und=<n> r12=<n>` and fails unless `REL`/`AROS`/`1`/`0`/`0`; the r12 count is `$(AROS_OBJDUMP) -d` lines matching `movq?[[:space:]]+%rax,[[:space:]]*%r12` (facts table), and on a nonzero count it prints `GATE FAIL: <n> stale-r12 call sites`. Never `--strip-all`. |

## Work items

**W0 — carried defects.** D20, D21, D22 (extend `t-rtfmt` with the six rows below), plus: the
watchdog reads the validation counter with `__atomic_load_n(…, __ATOMIC_RELAXED)` on both platforms.
Frames, music, log keys and every M0a/M0a.1 gate stay as they are.

| case | call | out | ret |
|---|---|---|---|
| `nan` | `rt_fmt(b,64,"%.3f",__builtin_nan(""))` | `nan` | 3 |
| `inf` | `rt_fmt(b,64,"%.0f",__builtin_inf())` | `inf` | 3 |
| `ninf` | `rt_fmt(b,64,"%.2f",-__builtin_inf())` | `-inf` | 4 |
| `big` | `rt_fmt(b,64,"%.1f\|%u",1e12,7u)` | `%.1f\|7` | 6 |
| `w33` | `rt_fmt(b,64,"%33u\|%u",5u,6u)` | `%33u\|6` | 6 |
| `w32` | `rt_fmt(b,64,"%32u",5u)` | 31 spaces then `5` | 32 |

(`\|` above is a literal `|`.) Same `FMT case=<name> out="<b>" ret=<n>` line format as M0a.1 W2.

**W1 — `src/platform/aros`.** `plat.cpp`, `plat_priv.h` (same private helper names `src/app/main.cpp`
uses: `plat_alloc_fn plat_audio_driver plat_audio_total_frames plat_log_raw plat_read_path
plat_sleep_us plat_exit_watchdog_ms plat_exit_watchdog_cancel`; the AROS watchdog is a child process
from `CreateNewProcTags` waking on its own `TR_ADDREQUEST`, `_exit` replaced by setting a shared flag
plus `Signal` to the main task, which then exits 3/4 by D19's rule), `aros_wsi.c`, and as many more
files as you need inside the 1,200-line budget. Everything in D23–D30. `pl_shutdown` frees what
`pl_init` allocated except the loader, logs `vk_loader_open_count`, and exits with the code.

**W2 — AROS build.** D31. `src/app/main.cpp` builds unchanged for both platforms except for flags it
must add (`--present-dump`, `--force-fifo`, `--lp-threads`, `--plant-no-demote`), each rejected with
exit 2 where D21/D27/D30 say so.

**W3 — readback.** D30.

**W4 — guest package and runner.** Guest scripts in `tests/aros/` (AROS Shell, copied by
`make aros-pkg`): `m0b-run.aros` does, in this order, `Stack 1048576`, `Avail >out/avail_before.txt`
(never `FLUSH`), then the runs below, each `>out/run_<name>.log` followed by
`Echo "rc=$RC" >out/rc_<name>.txt`, then `Avail >out/avail_after.txt`. All paths under `RAM:kitty`
(no path ends in `:`). Runs (`K`=`kitty`, `KD`=`kitty-debug`, `S`=`--spv RAM:kitty/spv`):

| name | command |
|---|---|
| `w` | `KD --windowed --frames 300 --present-dump 150 --out RAM:kitty/out/w S` |
| `h` | `K --headless --frames 300 --dump 0,150,299 --out RAM:kitty/out/h S` |
| `fifo` | `KD --windowed --frames 60 --force-fifo S` |
| `a` | `K --headless --seconds 10 --music RAM:kitty/test.mod S` |
| `lp0` `lp1` `lp4` | `KD --headless --frames 300 --bench --lp-threads <0\|1\|4> S` |
| `od2` `od4` | `K --headless --frames 300 --bench --overdraw <2\|4> S` |
| `b1` | `K --headless --frames 300 --bench S` |
| `soak` | `K --windowed --seconds 1800 --music RAM:kitty/test.mod S` |

Log keys the guest gates read, all AROS-only (the Linux log does not change): at exit every run
logs the existing `ticks=… catchup_dropped=… frames=…` line plus `elapsed_us=<pl_now_us since pl_init>`
and `events_drained=<pl_poll_events calls>`; the soak logs `mem_free_bytes=<AvailMem(MEMF_ANY)>` once
per 3,000 ticks. Timing comes from the existing `BENCH` line (`init_ms=`, `ms_p95=`); its
`lp_threads=` reports the value D27 set.
Host side: `tests/aros/lane_run.sh <lane> <outdir>` — refuses to run (exit 3, message
`no boots granted`) unless the authorization block grants a boot on that lane and `boots.txt` has
room; otherwise it uses only the lane's launcher and the spike client to put `build/aros-pkg/` at
`RAM:kitty`, run `m0b-run.aros`, fetch `RAM:kitty/out` into `<outdir>`, and write
`<outdir>/host_wall.txt` with `start=<epoch s> end=<epoch s>` per run name (one `--exec` per run).
Before any boot, read the lane's newest boot record under `~/Work/lanes/l0025/` and
`aros-v1-lane-chain.md`, and state in the report what the guest carries (loader library, ICD,
Mesa version). If it carries no `vulkan_loader.library` or no lavapipe ICD, don't boot: S15 BLOCKED.

**W5 — keep everything else identical.** If a change would alter a Linux frame byte or the music
hash, stop and report it instead.

## Acceptance scripts (run order: 0, X, 1–11, 13, 14, [W4 runner], 15, 12; paste every output verbatim)

Paste this block at the top of **every** script run (scripts 1–11 and 13 carry it already):

````bash
KD=~/Work/Kitty/Docs/Development; DOC=$KD/kitty_engine_design.md; PR=$KD/dispatch/m0b_aros_opencode_prompt.md
VK=~/Work/projects/Vulkan4Aros; KR=~/Work/Kitty/repo; B=$KR/build/m0b
LVP_PIN=~/Work/host_lavapipe_kitty/lvp_icd.json; LVP_SYS=/usr/share/vulkan/icd.d/lvp_icd.json
g(){ echo "GATE $1 $2${3:+ — $3}"; echo "GATE $1 $2${3:+ — $3}" >> "$B/census.txt"; }
icd(){ if [ -f "$LVP_PIN" ] && VK_DRIVER_FILES=$LVP_PIN vulkaninfo --summary 2>/dev/null | grep -q 'driverInfo *= *Mesa 26\.2\.2'; then echo "$LVP_PIN"; else echo "$LVP_SYS"; fi; }
GRANT=$(sed -n 's/^ *BOOTS GRANTED: *\([0-9][0-9]*\) *$/\1/p' "$PR" | head -1); LANE=$(sed -n 's/^ *LANE: *\([a-z0-9]*\) *$/\1/p' "$PR" | head -1)
aenv(){ AROS_ABI=v1 . "$VK/scripts/aros_build_env.sh" >/dev/null 2>&1 || return 1; export AROS_SYSROOT="${AROS_SDK_INCLUDE%/include}" PATH="$AROS_COLLECT_AROS_DIR:$AROS_TOOLCHAIN/bin:$PATH"; }
````

**Script 0: baseline. Run it BEFORE editing any file, exactly once.**

````bash
cd "$KR" || exit 1; mkdir -p "$B"; [ -e "$B/baseline.done" ] && { echo "BASELINE EXISTS — not re-taken"; exit 1; }
[ -e "$KR/build/m0a/baseline.done" ] && [ -e "$KR/build/m0a1/baseline.done" ] || { echo "earlier evidence missing — wrong tree"; exit 1; }
[ "$(git rev-parse HEAD)" = 38fc422d388c9886a38045ff5dc665dab369697d ] || { echo "HEAD is not 38fc422"; exit 1; }
[ -z "$(git status --porcelain)" ] || { echo "working tree not clean"; git status --porcelain | head; exit 1; }
( find .gitignore Makefile src tools shaders tests -type f -print0 | sort -z | xargs -0 sha256sum ) > "$B/pre_files.txt"
th=$(sha256sum < "$B/pre_files.txt" | cut -d' ' -f1); echo "tree hash $th ($(wc -l < "$B/pre_files.txt") files)"
[ "$th" = a6ca25673ae77a5b67c8b8dadc083ab4af6d1ad87a366629dffac7fbe0d39dfb ] || { echo "tree differs from the committed tree — stop and report"; rm -f "$B/pre_files.txt"; exit 1; }
tar -cf "$B/pre.tar" .gitignore Makefile src tools shaders tests
git branch --list --format='%(refname:short) %(objectname)' > "$B/branches_start.txt"
git -C "$VK" rev-parse HEAD > "$B/vk_head.txt"
git -C "$VK" status --porcelain -- 'src/abi/v1/AROS*' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal > "$B/vk_prot_start.txt"
( cd ~/Work/Kitty && find Docs llm-wiki AGENTS.md -type f -print0 | sort -z | xargs -0 sha256sum ) > "$B/protected_start.txt"
( cd ~/Work/Kitty && find Docs llm-wiki AGENTS.md -type f | sort ) > "$B/protected_list_start.txt"
pgrep -f '^[^ ]*qemu-system' | sort > "$B/qemu_start.txt"; : > "$B/boots.txt"
date +%s > "$B/baseline.done"; : > "$B/census.txt"
echo "GRANT=$GRANT LANE=$LANE"; [ -n "$GRANT" ] && [ -n "$LANE" ] || { echo "authorization block unreadable"; exit 1; }
echo "VK HEAD $(cat "$B/vk_head.txt")"; echo "VK protected status lines: $(wc -l < "$B/vk_prot_start.txt")"; echo "protected Kitty files: $(wc -l < "$B/protected_start.txt")"; echo "qemu processes at start: $(wc -l < "$B/qemu_start.txt")"
````

**Script X: extract M0a scripts 1–11 and M0a.1 script 13 (verbatim, with the listed patches).**
Run it after script 0 and again whenever you re-run the scripts.

````bash
mkdir -p "$B/scripts"; python3 -I - "$KD/dispatch/m0a_linux_opencode_prompt.md" "$KD/dispatch/m0a1_linux_fix_opencode_prompt.md" "$B/scripts" <<'PY' || { echo "EXTRACT FAIL"; exit 1; }
import sys,hashlib
a,a1,out=sys.argv[1:]
def blocks(p,sha,n):
    raw=open(p,'rb').read()
    if hashlib.sha256(raw).hexdigest()!=sha: sys.exit(f'{p} changed since review')
    bl=[]; cur=None
    for l in raw.decode().split('\n'):
        if l.startswith('````bash'): cur=[]; continue
        if l.startswith('````') and cur is not None: bl.append('\n'.join(cur)); cur=None; continue
        if cur is not None: cur.append(l)
    if len(bl)!=n: sys.exit(f'{p}: {len(bl)} bash blocks, want {n}')
    return bl
def rep(s,x,y,n=1):
    if s.count(x)!=n: sys.exit(f'patch target found {s.count(x)}x (want {n}): {x!r}')
    return s.replace(x,y)
A=blocks(a,'55259d101f65476cfde49eb25f90965bb953644b1ef206ae0d13b8b040712c39',14)
A1=blocks(a1,'36aa92124cecb08424f405e9e8c5fd48e38ada12eecae23e3e5d32b912e5d4b7',5)
hdr=rep(A[0]+'\n','B=$KR/build/m0a\n','B=$KR/build/m0b\n')
S={i:A[i+1] for i in range(13)}
S[7]=rep(S[7],"for i in range(3,len(d),4)): bad.append('alpha","for i in range(0,len(d),4)): bad.append('alpha")
line=[l for l in S[7].split('\n') if 'alpha is not 255' in l][0].strip(); expr=line[3:line.index(': bad.append')]
ok=bytes([1,2,3,255])*81920; nok=ok[:-1]+bytes([254])
if eval(expr,{'d':ok}) or not eval(expr,{'d':nok}): sys.exit('alpha probe self-test FAILED')
S[1]=rep(S[1],"src/(include|platform|platform/linux|platform/rt|render","src/(include|platform|platform/aros|platform/linux|platform/rt|render")
S[3]=rep(S[3],"'src/platform/linux':1200,","'src/platform/linux':1200,'src/platform/aros':1200,")
s13=A1[3]
s13=rep(s13,"'[[noreturn]] void pl_trap(const char* msg);'","'[[noreturn]] void pl_trap(const char* file, u32 line, const char* msg);','PFN_vkGetInstanceProcAddr pl_vk_gipa(Plat&);','void pl_wsi_destroy_surface(Plat&, VkInstance, VkSurfaceKHR);'")
s13=rep(s13,"'^Software Failure\\. [^ :]+:[0-9]+: present did not return","'^Software Failure\\. src/[A-Za-z0-9_/]+\\.(cpp|h|c):[1-9][0-9]*: present did not return",2)
for i in range(1,12): open(f'{out}/s{i}.sh','w').write(hdr+S[i]+'\n')
open(f'{out}/s13.sh','w').write(hdr+s13+'\n')
print('extracted s1..s11 (M0a) and s13 (M0a.1); alpha probe self-test PASS; 6 patches applied')
PY
````

**Scripts 1–11 and 13:** `for n in 1 2 3 4 5 6 7 8 9 10 11 13; do echo "########## SCRIPT $n"; bash "$B/scripts/s$n.sh"; done`
(paste the whole output). Script 1 resets the census. These are the Linux gates, unchanged except
for the patches script X lists.

**Script 14: the M0b host gates (W0, H6, layering, package). Run after 13.**

````bash
cd "$KR" || exit 1; ICD=$(icd); VL=VK_LAYER_KHRONOS_validation
chk(){ if [ -z "$2" ]; then g "S14.$1" PASS; else echo "$2" | head -12; g "S14.$1" FAIL; fi; }
make tests > "$B/make_tests14.log" 2>&1 || { tail -5 "$B/make_tests14.log"; g S14.tests-build FAIL; }
# --- W0: D20 trap location
n=$(grep -c 'PL_TRAP("t-trap probe")' tests/t-trap.cpp 2>/dev/null); ln=$(grep -n 'PL_TRAP("t-trap probe")' tests/t-trap.cpp 2>/dev/null | cut -d: -f1)
build/tests/t-trap > "$B/t-trap.log" 2>&1; rc=$?; cat "$B/t-trap.log"
{ [ "$n" = 1 ] && [ $rc = 4 ] && [ "$(cat "$B/t-trap.log")" = "Software Failure. tests/t-trap.cpp:$ln: t-trap probe" ]; } && g S14.trap-location PASS || g S14.trap-location FAIL "rc=$rc PL_TRAP lines=$n line=$ln"
f=$(grep -rhoE 'Software Failure\. [^ ]+:[0-9]+:' "$B" --include='*.log' --exclude=t-guru.log 2>/dev/null | sort -u | while read -r _ _ loc; do loc=${loc%:}; p=${loc%:*}; l=${loc##*:}
  { [ -f "$p" ] && [ "$l" -ge 1 ] && [ "$l" -le "$(wc -l < "$p")" ] && [ -n "$(sed -n "${l}p" "$p" | tr -d '[:space:]')" ] && [ "$loc" != src/platform/rt/traps.cpp:1 ]; } || echo "false location $loc"; done)
chk trap-locations-real "$f"
# --- W0: D21 probe demote fallback
P=$B/probe14; rm -rf "$P"; mkdir -p "$P/n"
VK_INSTANCE_LAYERS=$VL VK_DRIVER_FILES="$ICD" build/linux-debug/kitty --tier-probe --probe-candidate lavapipe --plant-no-demote --out "$P/n" > "$P/n.log" 2>&1; rn=$?
VK_DRIVER_FILES="$ICD" build/linux/kitty --tier-probe --probe-candidate lavapipe --plant-no-demote --out "$P/n" > /dev/null 2>&1; rr=$?
grep '^probe ' "$P/n.log"
{ [ $rn = 0 ] && [ $rr = 2 ] && grep -qE '^probe .*candidate=lavapipe.* ok=0 reason="shaderDemoteToHelperInvocation unsupported"$' "$P/n.log" && [ ! -e "$P/n/probe_cand.bgra" ] && [ -s "$P/n/probe_ref.bgra" ] && ! grep -q 'Software Failure' "$P/n.log"; } && g S14.probe-demote-fallback PASS || g S14.probe-demote-fallback FAIL "rc=$rn release=$rr"
# --- W0: D22 rt_fmt edges
build/tests/t-rtfmt > "$B/t-rtfmt14.log" 2>&1; echo "t-rtfmt rc=$?" >> "$B/t-rtfmt14.log"
python3 -I - "$B/t-rtfmt14.log" <<'PY' && g S14.rtfmt-edges PASS || g S14.rtfmt-edges FAIL
import sys
s=open(sys.argv[1]).read().splitlines(); bad=[]
W=[('nan','nan',3),('inf','inf',3),('ninf','-inf',4),('big','%.1f|7',6),('w33','%33u|6',6),('w32',' '*31+'5',32)]
for c,o,r in W:
    l=f'FMT case={c} out="{o}" ret={r}'
    if l not in s: bad.append(f'missing: {l!r}')
for l in ('PASS t-rtfmt','t-rtfmt rc=0'):
    if l not in s: bad.append(f'missing: {l}')
print('\n'.join(bad) or f'all {len(W)} edge lines present'); sys.exit(1 if bad else 0)
PY
f=""; grep -q '__atomic_load_n' src/platform/linux/plat.cpp || f="$f linux watchdog has no __atomic_load_n;"
grep -rqs '__atomic_load_n' src/platform/aros || f="$f aros watchdog has no __atomic_load_n;"
chk watchdog-atomic "$f"
# --- D24 GIPA only through the platform
chk gipa-platform-only "$(grep -rnw 'vkGetInstanceProcAddr' src | grep -v '^src/platform/linux/')"
# --- D25/D26 AROS source rules
A=src/platform/aros
if [ ! -d "$A" ]; then g S14.aros-source FAIL "no $A"; else
f=$(grep -rnE '\b(WaitTOF|Delay|RemLibrary|Printf|VPrintf|FPrintf|RawDoFmt|VNewRawDoFmt|v?s?n?printf|v?fprintf|malloc|calloc|realloc|free|fopen|SDL_[A-Za-z]+)[[:space:]]*\(|<cstdio>|<stdio\.h>|<iostream>|\bAvail\b' "$A" | grep -vE '^\S+:[0-9]+:\s*//')
f="$f$(grep -rnE 'CloseLibrary[[:space:]]*\([^)]*vk_lib' "$A")"
w=$(grep -rlE '#\s*include\s*[<"]aros_window_surface\.h[>"]' src); [ "$w" = "$A/aros_wsi.c" ] || f="$f
aros_window_surface.h included by: ${w:-nothing} (want exactly $A/aros_wsi.c)"
grep -qE 'RAM:vulkan_loader\.library' "$A"/*.cpp && grep -qE '"vulkan_loader\.library"' "$A"/*.cpp && grep -qE 'RAM:vulkan\.library' "$A"/*.cpp || f="$f
loader names not all present"
grep -rqE 'UNIT_MICROHZ' "$A" && grep -rqE 'GetUpTime' "$A" && grep -rqE 'TR_ADDREQUEST' "$A" || f="$f
timer.device UNIT_MICROHZ / GetUpTime / TR_ADDREQUEST missing"
grep -rqE 'GVF_LOCAL_ONLY' "$A" && grep -rqE 'AHI_DEFAULT_UNIT' "$A" && ! grep -rqE 'AHI_NO_UNIT' "$A" && grep -rqE 'ahir_Link' "$A" && grep -rqE 'ReadJoyPort' "$A" && grep -rqE 'ReadPixelArray' "$A" && grep -rqE 'RECTFMT_BGRA32' "$A" && grep -rqE 'ObtainSemaphore' "$A" || f="$f
one of GVF_LOCAL_ONLY / AHI_DEFAULT_UNIT / ahir_Link / ReadJoyPort / ReadPixelArray / RECTFMT_BGRA32 / ObtainSemaphore missing, or AHI_NO_UNIT present"
grep -rqE '"CopperForge"' "$A" && ! grep -rqiE '"[^"]*kitty[^"]*"' "$A" || f="$f
window title is not \"CopperForge\" or a Kitty string is in platform/aros"
chk aros-source "$f"; fi
# --- D31 build + binary gates
if ! aenv; then g S14.aros-env FAIL "aros_build_env.sh failed"; else
make aros aros-debug aros-pkg > "$B/make_aros.log" 2>&1; mrc=$?; echo "make aros rc=$mrc"; grep -E '^aros-gates:|GATE FAIL|stale SDK|banned API' "$B/make_aros.log"
[ $mrc = 0 ] && g S14.aros-build PASS || { tail -15 "$B/make_aros.log"; g S14.aros-build FAIL; }
for k in build/aros/kitty build/aros-debug/kitty; do
  if [ ! -f "$k" ]; then g "S14.aros-binary $k" FAIL missing; continue; fi
  t=$("$AROS_READELF" -h "$k" | awk '/^  Type:/{print $2}'); o=$("$AROS_READELF" -h "$k" | awk -F: '/OS\/ABI/{gsub(/ /,"",$2);print $2}'); a=$("$AROS_READELF" -h "$k" | awk '/ABI Version/{print $NF}')
  u=$("$AROS_READELF" -s -W "$k" | awk '$5=="GLOBAL" && $7=="UND" && $8!=""' | wc -l); r=$("$AROS_OBJDUMP" -d "$k" | grep -cE 'movq?[[:space:]]+%rax,[[:space:]]*%r12')
  gt=$("$AROS_READELF" -S -W "$k" | grep -cE '\.got'); dbg=$("$AROS_READELF" -S -W "$k" | grep -c '\.symtab')
  sy=$("$AROS_READELF" -s -W "$k" | awk '{print $8}' | grep -xE 'vkGetInstanceProcAddr|SDL_.*|WaitTOF' | sort -u | tr '\n' ' ')
  echo "$k type=$t osabi=$o abi=$a und=$u r12=$r got=$gt symtab=$dbg size=$(stat -c%s "$k") syms=[$sy]"
  { [ "$t" = REL ] && [ "$o" = AROS ] && [ "$a" = 1 ] && [ "$u" = 0 ] && [ "$r" = 0 ] && [ "$gt" = 0 ] && [ "$dbg" -ge 1 ] && [ -z "$sy" ]; } && g "S14.aros-binary $k" PASS || g "S14.aros-binary $k" FAIL; done
make -n -B aros aros-debug 2>/dev/null | grep -E ' -c [^ ]*src/' > "$B/aros_cc_lines.txt"; n=$(grep -c . "$B/aros_cc_lines.txt")
python3 -I - "$AROS_CXX" "$AROS_CC" "$AROS_TARGET_CFLAGS" "$AROS_SYSROOT" "$VK" "$B/aros_cc_lines.txt" <<'PY' && g S14.aros-flags PASS "$n compile lines" || g S14.aros-flags FAIL
import sys,re,shlex
cxx,cc,tf,sr,vk,lf=sys.argv[1:]; L=[l for l in open(lf).read().splitlines() if l.strip()]; bad=[]
MODS={'render','audio','guru','benchmark'}
common=tf.split()+[f'--sysroot={sr}','-Wall','-Wextra','-Werror','-fno-pic','-fno-pie','-fno-stack-protector','-fno-strict-aliasing','-mno-ms-bitfields','-O2','-ffp-contract=off']
if not L: bad.append('no aros compile lines')
for l in L:
    t=shlex.split(l); src=t[t.index('-c')+1]; d=src.split('/')[1] if src.split('/')[1]!='platform' else '/'.join(src.split('/')[1:3])
    want=common+(['-std=c11'] if src.endswith('.c') else ['-std=c++20','-fno-exceptions','-fno-rtti'])
    if t[0]!=(cc if src.endswith('.c') else cxx): bad.append(f'{src}: compiler {t[0]}')
    miss=[w for w in want if w not in t]
    if miss: bad.append(f'{src}: missing {miss}')
    if '-ffast-math' in t or '-nostartfiles' in t: bad.append(f'{src}: forbidden flag')
    inc={x[2:] for x in t if x.startswith('-I')}; ida=[t[i+1] for i,x in enumerate(t) if x=='-idirafter']
    if any('platform/linux' in i for i in inc): bad.append(f'{src}: -Isrc/platform/linux on an AROS line')
    if d in MODS or d=='platform/rt':
        allow={'src/include','src/platform/rt'}|({f'src/{d}'} if d!='platform/rt' else set())|({'src/render'} if d=='benchmark' else set())
        if inc-allow: bad.append(f'{src}: -I not allowed {sorted(inc-allow)}')
    if (ida or any(i.startswith(vk) for i in inc)) and d!='platform/aros': bad.append(f'{src}: Vulkan4Aros include outside platform/aros')
    if any(i.startswith(vk) for i in inc) and src!='src/platform/aros/aros_wsi.c': bad.append(f'{src}: loader -I outside aros_wsi.c')
    if ida and not all(x.endswith('core-pc-x86_64/bin/pc-x86_64/AROS/Developer/include') for x in ida): bad.append(f'{src}: unexpected -idirafter {ida}')
print('\n'.join(bad)); sys.exit(1 if bad else 0)
PY
# --- H6 known-bad plants: each must make "make aros" fail for its own reason (scratch copies)
plant(){ d="$B/plant/$1"; rm -rf "$d"; mkdir -p "$d"; cp -a .gitignore Makefile src tools shaders tests "$d"/; mkdir -p "$d/build"; cp -a build/spv "$d/build/" 2>/dev/null; echo "$d"; }
d=$(plant stale); AROS_SDK_INCLUDE=$VK/src/abi/v11/sdk/Developer/include make -C "$d" aros > "$d.log" 2>&1; rc=$?; { [ $rc != 0 ] && grep -q 'stale SDK' "$d.log"; } && g S14.knownbad-stale-sdk PASS || g S14.knownbad-stale-sdk FAIL "rc=$rc"
d=$(plant iostream); printf '\n#include <iostream>\n' >> "$d/src/platform/aros/plat.cpp"; make -C "$d" aros > "$d.log" 2>&1; rc=$?; { [ $rc != 0 ] && grep -qE ': L1 ' "$d.log"; } && g S14.knownbad-iostream PASS || g S14.knownbad-iostream FAIL "rc=$rc"
d=$(plant waittof); printf '\nextern "C" void cf_plant_wait(void) { WaitTOF(); }\n' >> "$d/src/platform/aros/plat.cpp"; make -C "$d" aros > "$d.log" 2>&1; rc=$?; { [ $rc != 0 ] && grep -q 'banned API WaitTOF' "$d.log"; } && g S14.knownbad-waittof PASS || g S14.knownbad-waittof FAIL "rc=$rc"
d=$(plant r12); printf '\nextern "C" void cf_plant_r12(void) { __asm__ volatile("mov %%%%rax,%%%%r12" ::: "r12"); }\n' >> "$d/src/platform/aros/plat.cpp"; make -C "$d" aros > "$d.log" 2>&1; rc=$?; { [ $rc != 0 ] && grep -qE 'GATE FAIL: [1-9][0-9]* stale-r12 call sites' "$d.log"; } && g S14.knownbad-r12 PASS || g S14.knownbad-r12 FAIL "rc=$rc"
d=$(plant und); printf '\nextern "C" void cf_plant_und(void);\nextern "C" void cf_plant_call(void) { cf_plant_und(); }\n' >> "$d/src/platform/aros/plat.cpp"; make -C "$d" aros > "$d.log" 2>&1; rc=$?; { [ $rc != 0 ] && grep -q 'cf_plant_und' "$d.log"; } && g S14.knownbad-undefined PASS || g S14.knownbad-undefined FAIL "rc=$rc"
[ -f src/platform/aros/plat.cpp ] && ! grep -q 'mov %%rax,%%r12' "$B/plant/r12/src/platform/aros/plat.cpp" && echo "SCRIPT DEFECT: r12 plant text malformed — report it"
# --- D31 package
K=build/aros-pkg; f=""
for x in kitty kitty-debug test.mod m0b-run.aros MANIFEST.sha256; do [ -s "$K/$x" ] || f="$f missing $K/$x;"; done
[ "$(ls "$K/spv" 2>/dev/null | grep -c '\.spv$')" = "$(ls build/spv | grep -c '\.spv$')" ] || f="$f spv count differs from build/spv;"
( cd "$K" 2>/dev/null && sha256sum -c --quiet MANIFEST.sha256 ) || f="$f MANIFEST does not verify;"
[ "$(cd "$K" 2>/dev/null && find . -type f ! -name MANIFEST.sha256 | wc -l)" = "$(wc -l < "$K/MANIFEST.sha256" 2>/dev/null)" ] || f="$f MANIFEST does not list every file;"
cmp -s "$K/kitty" build/aros/kitty && cmp -s "$K/kitty-debug" build/aros-debug/kitty || f="$f packaged binaries differ from build/aros*;"
S=$K/m0b-run.aros; grep -qx 'Stack 1048576' "$S" 2>/dev/null || f="$f no 'Stack 1048576' line;"; grep -qiE 'flush|RemLibrary' "$S" 2>/dev/null && f="$f FLUSH/RemLibrary in guest script;"
for r in w h fifo a lp0 lp1 lp4 od2 od4 b1 soak; do grep -q "out/run_$r.log" "$S" 2>/dev/null && grep -q "out/rc_$r.txt" "$S" 2>/dev/null || f="$f run $r missing;"; done
grep -qE '(^|[^A-Za-z])RAM:[^ /]*:' "$S" 2>/dev/null && f="$f a path with two colons;"
chk aros-package "$f"
if [ "$GRANT" = 0 ]; then out=$(bash tests/aros/lane_run.sh l0025 "$B/guest-refusal" 2>&1); rc=$?; echo "lane_run with GRANT=0: rc=$rc $out" | head -3
  { [ $rc = 3 ] && echo "$out" | grep -q 'no boots granted' && [ ! -e "$B/guest-refusal/host_wall.txt" ]; } && g S14.runner-refuses PASS || g S14.runner-refuses FAIL "rc=$rc"
else out=$(bash tests/aros/lane_run.sh nvk "$B/guest-refusal" 2>&1); rc=$?; echo "lane_run on an ungranted lane: rc=$rc $out" | head -3
  { [ $rc = 3 ] && echo "$out" | grep -q 'no boots granted' && [ ! -e "$B/guest-refusal/host_wall.txt" ] && [ "$(grep -c . "$B/boots.txt")" = 0 ]; } && g S14.runner-refuses PASS || g S14.runner-refuses FAIL "rc=$rc"; fi
fi
````

**W4 runner** (only when `BOOTS GRANTED` > 0 and `LANE` is not `none`): `bash tests/aros/lane_run.sh "$LANE" "$B/guest"`.
Paste its whole output. Each boot it needs is one line in `boots.txt`, written before the boot.

**Script 15: on-AROS gates (H7-M0, `t-aros-boot`, measurements). Run after the runner, or straight
after 14 when no boots are granted.**

````bash
cd "$KR" || exit 1; G=$B/guest
N="boot fifo-no-hang present-readback present-vs-headless headless-vs-host clock timer-probe stack init-bounded frame-time ahi joystick lp-probe soak resident"
if [ "${GRANT:-0}" = 0 ] || [ "$LANE" = none ] || [ ! -f "$G/host_wall.txt" ]; then
  why="no boots granted"; [ "${GRANT:-0}" != 0 ] && why="no guest results in $G"; for x in $N; do g "S15.$x" BLOCKED "$why"; done
else python3 -I - "$G" "$KR/build/m0a/r1/frame_150.bgra" <<'PY'; fi
import sys,re,os
G,host=sys.argv[1:]; R={}
def gate(n,ok,why=''):
    s='PASS' if ok is True else ('BLOCKED' if ok is None else 'FAIL'); l=f'GATE S15.{n} {s}'+(f' — {why}' if why else '')
    print(l); open(os.path.join(os.path.dirname(G),'census.txt'),'a').write(l+'\n')
def log(r):
    p=f'{G}/run_{r}.log'; return open(p,errors='replace').read() if os.path.exists(p) else ''
def rc(r):
    p=f'{G}/rc_{r}.txt'; m=re.search(r'rc=(\d+)',open(p).read()) if os.path.exists(p) else None; return int(m.group(1)) if m else None
def kv(t,k):
    m=re.findall(r'(?:^|\s)'+k+r'=("[^"]*"|\S+)',t); return m[-1].strip('"') if m else None
W={m[0]:(int(m[1]),int(m[2])) for m in re.findall(r'^(\w+) start=(\d+) end=(\d+)$',open(f'{G}/host_wall.txt').read(),re.M)}
guru=lambda t:'Software Failure' in t
w=log('w')
gate('boot', rc('w')==0 and not guru(w) and 'surface=ok window_w=320 window_h=256' in w and int(kv(w,'ticks') or 0)>=60 and 'events_drained=' in w and int(kv(w,'vk_loader_open_count') or 0)>=1, f"rc={rc('w')} ticks={kv(w,'ticks')} open_count={kv(w,'vk_loader_open_count')}")
ff=log('fifo'); fw=W.get('fifo',(0,10**9))
gate('fifo-no-hang', rc('fifo')==0 and 'present_mode=FIFO' in ff and int(kv(ff,'ticks') or 0)>=60 and fw[1]-fw[0]<=120, f"rc={rc('fifo')} wall={fw[1]-fw[0]}s")
pd=f'{G}/w/present_150.bgra'; hd=f'{G}/h/frame_150.bgra'
if 'present_dump blocked' in w: gate('present-readback',None,kv(w,'reason') or 'blocked'); gate('present-vs-headless',None,'no readback')
else:
    okr=os.path.exists(pd) and os.path.getsize(pd)==327680 and re.search(r'present_dump frame=150 client=320x256 depth=(2[4-9]|3[0-2]) ',w)
    gate('present-readback', bool(okr), w[w.find('present_dump'):].splitlines()[0] if 'present_dump' in w else 'no present_dump line')
    if okr and os.path.exists(hd):
        a=open(pd,'rb').read(); b=open(hd,'rb').read(); m=sum(1 for i in range(0,327680,4) if a[i:i+3]!=b[i:i+3])
        gate('present-vs-headless', m==0, f'rgb_mismatched_px={m}')
    else: gate('present-vs-headless', False, 'missing present or headless frame')
if os.path.exists(hd) and os.path.exists(host):
    a=open(hd,'rb').read(); b=open(host,'rb').read()
    m=sum(1 for i in range(0,327680,4) if any(abs(a[i+c]-b[i+c])>4 for c in range(3)) or ((a[i+3]!=0)!=(b[i+3]!=0)))
    gate('headless-vs-host', m==0, f'mismatched_px={m} (±4, coverage)')
else: gate('headless-vs-host', False, 'missing frame')
aa=log('a'); el=int(kv(aa,'elapsed_us') or 0); wa=W.get('a',(0,0))
gate('clock', rc('a')==0 and 9_500_000<=el<=(wa[1]-wa[0]+1)*1_000_000, f'elapsed_us={el} host_wall={wa[1]-wa[0]}s')
d=int(kv(w,'delta_us') or 0); gate('timer-probe', 80_000<=d<=250_000, f'delta_us={d}')
st=int(kv(w,'stack_bytes') or 0); gate('stack', st>=262144, f'stack_bytes={st}')
im=[float(kv(log(r),'init_ms') or 1e9) for r in ('b1','od2','od4','lp0','lp1','lp4')]; gate('init-bounded', max(im)<=30000, f'init_ms={im}')
b1=log('b1'); p95=kv(b1,'ms_p95'); gate('frame-time', p95 is not None and float(p95)<=40.0, f'p95={p95} ms (M0 go/no-go ≤ 40)')
if 'audio_device=none' in aa: gate('ahi', None, f'ahi open failed: {kv(aa,"reason")}')
else:
    q=int(kv(aa,'audio_frames_queued') or 0); gate('ahi', 'audio_device=ahi.device unit=0' in aa and 'music=on' in aa and 470400<=q<=489600, f'queued={q} xruns={aa.count("ahi_xrun")}')
jl=re.findall(r'^joy port=([0-3]) state=0x[0-9a-f]{8}$',w,re.M); gate('joystick', sorted(jl)==['0','1','2','3'] or 'joy lowlevel=none' in w, f'ports={jl}')
lp=[(r,kv(log(r),'getvar'),kv(log(r),'lp_worker_tasks'),kv(log(r),'ms_p95')) for r in ('lp0','lp1','lp4')]
gate('lp-probe', all(rc(r)==0 and g_==r[2:] and t_ is not None and p_ is not None for r,g_,t_,p_ in lp), f'{lp}')
so=log('soak'); sw=W.get('soak',(0,0)); mem=[int(x) for x in re.findall(r'mem_free_bytes=(\d+)',so)]
gate('soak', rc('soak')==0 and not guru(so) and int(kv(so,'ticks') or 0)>=90000 and sw[1]-sw[0]>=1800, f"rc={rc('soak')} ticks={kv(so,'ticks')} wall={sw[1]-sw[0]}s")
gate('resident', len(mem)>=29 and abs(mem[1]-mem[-1])<=1<<20, f'mem samples={len(mem)} drift={mem[1]-mem[-1] if len(mem)>1 else None} B')
PY
g S15.nvk-smoke BLOCKED "card lane not granted this round (informational at M0)"
````

**Script 12: end state and census. Run it LAST.**

````bash
cd "$KR" || exit 1; f=0
[ "$(git rev-parse HEAD)" = 38fc422d388c9886a38045ff5dc665dab369697d ] || { echo "HEAD moved"; f=1; }
git branch --list --format='%(refname:short) %(objectname)' | diff "$B/branches_start.txt" - || { echo "branches changed"; f=1; }
{ [ -z "$(git stash list)" ] && [ -z "$(git tag)" ]; } || { echo "stash or tag"; f=1; }
for x in LICENSE README.md; do git diff --quiet HEAD -- "$x" || { echo "$x modified"; f=1; }; done
for e in m0a m0a1; do [ -e "$KR/build/$e/baseline.done" ] && [ -f "$KR/build/$e/r1/frame_150.bgra" ] || { echo "build/$e evidence deleted"; f=1; }; done
( cd ~/Work/Kitty && sha256sum -c --quiet "$B/protected_start.txt" ) || { echo "protected Kitty files changed"; f=1; }
( cd ~/Work/Kitty && find Docs llm-wiki AGENTS.md -type f | sort ) | diff "$B/protected_list_start.txt" - || { echo "files added to or removed from protected Kitty paths"; f=1; }
git -C "$VK" status --porcelain -- 'src/abi/v1/AROS*' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal | diff "$B/vk_prot_start.txt" - || { echo "VK protected paths changed in the working tree"; f=1; }
v0=$(cat "$B/vk_head.txt"); if [ "$(git -C "$VK" rev-parse HEAD)" != "$v0" ]; then echo "INFO VK HEAD moved — owner commits are expected; listed for attribution:"; git -C "$VK" log --format='%h %an %ad %s' --date=iso "$v0..HEAD" | head -40; fi
nb=$(grep -c . "$B/boots.txt"); echo "boots used: $nb of ${GRANT:-0} granted"; [ "$nb" -le "${GRANT:-0}" ] || { echo "boots over grant"; f=1; }
lw=$(find ~/Work/lanes -newer "$B/baseline.done" 2>/dev/null | head -20); [ -z "$lw" ] || { echo "INFO ~/Work/lanes changed during the run (the owner works there in parallel) — listed for attribution:"; echo "$lw"; }
new=$(pgrep -f '^[^ ]*qemu-system' | sort | comm -13 "$B/qemu_start.txt" -); [ -z "$new" ] || { echo "INFO qemu processes started during the run — listed for attribution:"; for q in $new; do tr '\0' ' ' < /proc/$q/cmdline 2>/dev/null | cut -c1-200; echo; done; }
[ $f = 0 ] && g S12.end-state PASS || g S12.end-state FAIL
rm -rf "$B/pre" && mkdir -p "$B/pre" && tar -xf "$B/pre.tar" -C "$B/pre"
echo "--- files changed vs 38fc422"; diff -rq "$B/pre" . -x .git -x build -x LICENSE -x README.md | sed "s|$B/pre|38fc422|" | sort
echo "--- line delta"; for d in src tools tests shaders Makefile; do git diff --no-index --shortstat "$B/pre/$d" "$d" | sed "s|^|$d: |"; done
echo "--- census"; sort "$B/census.txt" | uniq; for s in PASS FAIL BLOCKED; do echo "$s=$(grep -c " $s" "$B/census.txt")"; done
````

**Expected census:** every gate PASS except `S7.golden BLOCKED` (pin not built) and
`S15.nvk-smoke BLOCKED`. With `BOOTS GRANTED: 0` the other 15 `S15.*` gates are BLOCKED too; with
boots granted, an S15 gate is BLOCKED only for the reason its own line states (no guest results,
an AHI open failure, a readback area or depth outside D30), and each such reason goes in the report. Three Linux gates are also BLOCKED only if there is no
display. With boots granted, any S15 FAIL is reported with its measured value, never retried into
a PASS by changing a threshold.

**Loop rule:** if any gate prints FAIL, fix the code (never the scripts, never an earlier prompt)
and re-run X, 1–11, 13, 14, 15, 12 in one pass (the runner only if boots remain). Report the final
pass only, with its full output. BLOCKED is not FAIL and is never reported as PASS. Never word code,
log lines or file names just to slip past a pattern: the scripts are the floor, and review reads
the code.

## Final report (write it to `build/m0b/report.md` and reply with the same text only, no narration)

1. Skills loaded.
2. **The verbatim output of scripts 0, X, 1–11, 13, 14, (runner), 15 and 12 from the final pass**,
   census included. Capture each script's exit code numerically.
3. D20–D31 as implemented: for each, the file(s) with line numbers, and anything you had to decide.
4. The AROS binaries: size, `aros-gates` line, the full `make -n` compile line for one shared TU and
   for `aros_wsi.c`, and the link line.
5. What the lane's guest carries (W4) — or "not checked: no boots granted".
6. Measurements (each with its log line, or BLOCKED): `uptime_probe`, `stack_bytes`, `init_ms` per
   run, frame time p50/p95/max at overdraw 1/2/4, `LP_NUM_THREADS` 0/1/4 (`getvar`, `lp_worker_tasks`,
   p95), AHI queued frames and every `ahi_xrun` line, the four `joy` lines, `Avail` before/after, the
   soak's `mem_free_bytes` series, `headless-vs-host` mismatched pixels.
7. Every departure from this prompt, the earlier prompts or the design, with file and line. Write
   "none" only if there is none.
8. Design problems for revision 6 found this round (sentence in the design + why). At least: the
   retired `vkCreateArosSurfaceKHR`/`vkPollArosEventsKHR`; the gcc flag set and `-nostartfiles`/
   `__startup_entry` vs the v1 clang lane; C14 "no clock API" vs `GetUpTime`; the window title
   needing a `PlatArgs` field; the services `plat_svc.h` now carries (9 lines) that belong in
   `platform.h`/`wsi.h`; the opaque `Plat` in `main`; `devices/ahi.h` missing from the v1 SDK; the
   design's r12 grep pattern, which cannot match `llvm-objdump` output.
9. Anything needed outside the authorization block.
