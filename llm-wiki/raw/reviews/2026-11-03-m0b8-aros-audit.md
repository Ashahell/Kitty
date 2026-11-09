# M0b.8 AROS layer audit

> Source: Docs/Development/m0b8_aros_audit.md
> Collected: 2026-11-04
> Published: 2026-11-03

# M0b.8 audit of `src/platform/aros` (2026-11-03)

**Result: nine defects in Kitty's AROS layer, all fixed in the patch M0b.8's X3 applies. Two more findings are in Vulkan4Aros, which is read-only. None of the fixes has run on a guest.**

Method: every AROS call in `src/platform/aros` read against the AROS and Vulkan4Aros sources. No boot was used. Line numbers in `plat.cpp` are for the tree before X3. Vulkan4Aros paths are relative to `~/Work/projects/Vulkan4Aros`; AROS paths are relative to its `src/abi/v1/AROS`, which was read only.

## Defects

| # | Location | Defect | Effect | Fix in X3 |
|---|---|---|---|---|
| 1 | `plat.cpp:127` `dos_read` | `Seek` returns the position before the seek (`rom/dos/seek.c`), not the size | Every AROS file read fails. This is the M0b.7 trap | Seek to `OFFSET_END`, then take the size from the seek back to `OFFSET_BEGINNING` |
| 2 | `plat.cpp:161` `pl_write_file` | `Write != FALSE`; `Write` returns a count, or -1 (`rom/dos/write.c`) | Errors and short writes count as success | `Write == (LONG)n` |
| 3 | `pl_poll_events` | Inverted. `arosPollWindowEvents` returns non-zero to close and 1 for an unknown surface (`src/vulkan/loader/vk_aros_loader.c:672`, `src/vulkan/pal/vkpal_aros.c:1305`; `src/vulkan/loader/vkcube_aros.c:640` loops on `!arosPollWindowEvents`) | A windowed `--seconds` run, so the soak, quits on the first poll | Quit on non-zero; return 0 with no surface, so headless does not poll |
| 4 | `src/app/main.cpp` `run_engine` | `r_shutdown` is never called | Every AROS run leaks the window, device and instance; a window left at exit is unserviced | Call `r_shutdown` before both `pl_shutdown`s |
| 5 | `pl_wsi_create_surface` | The window is `WFLG_GIMMEZEROZERO`, sized by outer size (`vkpal_aros.c:1125`). Mesa's `currentExtent` is the inner size (`src/abi-patches/v1/mesa-26.2.2/wsi_common_aros.c:61`) | With outer 320×256 the extent is under 320×256, and Kitty's `CmdBlitImage` writes out of bounds | Probe the borders (`cf_aros_window_borders`), size the window so the picture fits, centre the picture in `pl_wsi_extent`'s area, clamped to the swapchain |
| 6 | `present_dump_impl` | Its client check uses outer width and height, on a window whose picture is offset by defect V1 | The dump's size check and region are wrong | Use inner size less the border; also write `present_<F>.inner.bgra` and log `present_inner` and `border`, so the guest shows where the picture lands |
| 7 | `plat.cpp:445` AHI | `ahir_Link` set as a ring at open. AHI clears a link to a request not on its list and plays at once (`workbench/devs/AHI/Device/devcommands.c:1141`, `:1230`); a second follower fails (`:1161`, `:1235`) | No chaining happened; buffers overlapped | Per submission: link to the previous request only if it is still in flight |
| 8 | `plat.cpp:256`, `:528` watchdog | `Signal` on a process that may have exited, which is sleeping in `DoIO` anyway | A dangling task pointer, and the signal does nothing. Not exercised on the ladder | Both `Signal`s removed |
| 9 | `pl_present_dump` | `CyberGfxBase` is not checked before `GetCyberMapAttr` and `ReadPixelArray` (it is opened at `plat.cpp:732`, and can be NULL) | NULL base call | Blocks when there is no `CyberGfxBase` |

Defect 1 was found in the M0b.7 critique, defect 2 with it; the rest are new.

## Vulkan4Aros items for the owner (read-only, not changed)

- **V1: `src/abi-patches/v1/mesa-26.2.2/wsi_aros_blit.c:74`.** `WritePixelArray` writes at `(BorderLeft, BorderTop)` of `window->RPort`. On a GIMMEZEROZERO window that RastPort is already the inner layer, so the picture lands offset by the border and clipped at the far edge. **UNVERIFIED on a guest.** Kitty works around it in defect 5; the inner dump from defect 6 should show it.
- **V2: `src/vulkan/loader/aros_window_surface.h`.** Its comment on `arosPollWindowEvents` says the opposite of what the function does, and defect 3 follows that comment. The implementations and `vkcube_aros.c` agree with each other.

## Checked and cleared

- `WaitIO` and `CheckIO` on never-sent requests: `ln_Type` is 0, which is harmless (`rom/exec/waitio.c`, `checkio.c`, `createiorequest.c`).
- `BeginIO` sets `NT_MESSAGE`.
- Copying only `io_Device` and `io_Unit` to requests 1–3: `Private[1]` only affects volume grouping.
- AHI extras are allocated per request.
- `VK_KHR_xcb_surface`: the loader adds it itself (`vk_aros_loader.c:414–440`). `juggler_rt` enables only surface and debug_utils.

## Not fixed

- **Engine `AllocVec` leaks per run.** AROS does not free them at exit. Repeated runs in one boot lose memory, including the ladder. Not addressed in M0b.8.
- V1 and V2 above.

## Patch

Four files: `src/platform/aros/plat.cpp`, `src/platform/aros/aros_wsi.c`, `src/render/render_frame.cpp`, `src/app/main.cpp`. The real repo is unedited until X3 runs.

Host checks on a copy: Linux `frame_150` byte-identical; windowed, headless and debug runs clean; both AROS binaries `r12=0`.

**Not verified:** anything on a guest. Windows, frames, presented pixels, AHI, `r_shutdown` and the soak are all first runs.
