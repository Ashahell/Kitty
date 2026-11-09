# M0b.8 critique of the opencode report

> Source: Docs/Development/m0b8_aros_critique.md
> Collected: 2026-11-04
> Published: 2026-11-04

# M0b.8 critique — third guest round (2026-11-04)

**Verdict: M0b.8 not achieved. The round was run correctly, and it produced the first correct guest frames. No window has opened a swapchain, and audio, soak and clock are unmeasured. The report quotes the crash but does not explain it, and several of its statements are wrong. The source explains the `w` and `fifo` crash, and most of the boot's lost time is my runner's fault.**

Dispatch: `Docs/Development/dispatch/m0b8_aros_guest_opencode_prompt.md`, sha256 `bc3f429f260a9f4141ee05c5c18217e287744e5c4b7ad6f46120aba56112ea7e`. Grant: 1 boot on l0025, used. No boot is left.

## Verified

- **Scripts:** all 27 in `build/m0b8/scripts/` are byte-identical to my own extraction of the same prompt.
- **Census:** `build/m0b8/census.txt` has 113 lines: 97 PASS, 14 FAIL, 2 BLOCKED; seal `86197bb8…` matches.
- **Tree:** `S14b.pinned` PASS; the runner is the supplied `e2cf0786…`.
- **Ledger:** one `boots.txt` line; lane records went from 53 to 54.
- **Lane state:** nothing listens on 9099/9100/4479.

## What the boot proves

- **`dos_read` works:** the shaders load; `shaders=5 pipelines=5` on every run that got that far.
- **`pl_write_file` works, and the engine renders correctly on the guest:** `h/frame_150` and `h/frame_299` are byte-identical to host `r1`, on LLVM 20.1.0 against the host's 23.1.3. This is the first real pixel result of M0b.
- **`r_shutdown` works headless:** `p`, `h`, `lp0` and `lp1` exit with rc=0. No window has been torn down yet.
- **Window setup partly works:** the border probe, surface creation, and the surface capabilities, formats (2) and present-mode queries all returned.

Of the nine audit fixes, 1, 2 and 4 (headless only) are proven. 3, 5, 6, 7 and 9 did not run to a result, and 8 is not on the path. Not proven: swapchain, present, input poll, AHI, soak, clock, overdraw 2 and 4.

## Cause 1: the `w` and `fifo` crash is a NULL `CreateSwapchainKHR`

The report gives `IP=0` and stops. The fault block is enough:

- The return address at `SP+0` ends in `e` (`0x4cc617de`, `0x4d6bfbde`). `create_swapchain` has only two call sites with a return offset ending in `e`: `0x6e8e` (`DestroySemaphore`, in a loop over `sc_n`, which is 0 here) and `0x72ae`, which is `call *0xc0(%rbx)`. In `VkApi`, `0xc0` is `CreateSwapchainKHR`.
- `SP+8` and `SP+10` are `0x2c` and `0x25`: `fmt[0]` and `fmt[1]` of that function's `VkSurfaceFormatKHR fmt[256]`, formats 44 and 37. So the format query had already returned.

Why it is NULL, from the sources, **unverified on a guest**:

1. `vk_aros_loader.c`'s CreateDevice wrapper removes the app's `VK_KHR_swapchain`, then adds it back only if `want_swapchain` is set.
2. It sets `want_swapchain` by fetching `vkEnumerateDeviceExtensionProperties` with `vkGetInstanceProcAddrStock(VK_NULL_HANDLE, …)`. Its comment calls that a global entry point. It is not: `globalGetProcAddr` (`stock/gpa_helper.c:270`) serves four names and not this one, and `stock/trampoline.c:96` returns NULL for anything else with a NULL instance. The same wrapper's comment a few lines down records this exact trap for `vkCreateDevice`.
3. So the device is created without the extension, and `vk_device_get_proc_addr` (`vk_device.c:452`) returns NULL for its entries.
4. Vulkan4Aros's own apps never see this: they fetch `vkCreateSwapchainKHR` through instance GIPA. Kitty uses `vkGetDeviceProcAddr`, which the spec allows.

This is a **Vulkan4Aros defect**, and that tree is read-only here. Kitty can also fall back to instance GIPA for these entries.

## Cause 2: Kitty's fence wait never retries

`render_frame.cpp:221` and `:242` wait 1 s and pass the result to `rv_chk`. `VK_TIMEOUT` is classed "retry", but nothing retries: the frame carries on.

- **Frame 0 is blank on the guest.** `h/frame_000.bgra` is all `00000000`, and the host's is the real picture. `ms_max` is 1047–1060 ms, so the first render outlasts the wait, and the memcpy at `:244` copies an unfinished image. Then `:222` resets a fence still in use.
- **No gate saw it.** `S15.headless-vs-host` compares frame 150 only.
- `h`, `lp0` and `lp1` each log one timeout. The Linux host is fast enough never to hit it.

## `lp4` did not finish

- A worker task faulted (`CR2=0x12`, `IP=0x4dd7867e`), about 5 s in. Cause not established.
- 137 `VK_TIMEOUT` lines follow, on 300 frames. `ms_p95=2004.519` is two 1 s waits, not a frame time. **That BENCH line is not a measurement.**
- It printed `validation_messages=0` and never printed `vk_loader_open_count`, so it stopped between the two, which is `r_shutdown`. A device wait on a dead worker fits; that is an inference.

M0 runs `lp_threads=0`, so this blocks `S15.lp-probe` only.

## `a`: cause not established

GP fault, no registers, and no retrieved log. `a` runs the release binary, which does not echo to serial, so the serial silence says nothing about how far it got. TSC puts the fault about 2 s after launch: past Vulkan init, well short of the 10 s run. Lead only: this QEMU has no sound device (`lane_l0025_boot.sh:139` has only the rtl8139), and the skill notes record a GP fault in an AHI driver's init on codec-less hardware. The first run of `--music`, `run_seconds` and AHI all sit in those 2 s.

## Report errors

- **"a GP-faulted at launch."** Unsupported, for the reasons above.
- **lp4 "finished its 300-frame bench at serial line 4420, ~5 min after the runner's last fetch."** False. Line 4420 comes before the first `rc_lp4` poll (4423) and before the fetch (about 4889).
- **lp4 "kept going and finished."** Misleading: no rc, 137 timeouts, and `ms_p95=2004.519` is quoted as a measurement in item 6.
- **"run_w.log: absent. The guest faulted before writing it."** Unsupported. The evidence is only `get failed`. `run_lp4.log` also got `get failed`, and that run had plainly logged. A live task holding its redirect open fits both; not established.
- **`avail_after.txt` "never written by the runner's fetch" / "never fetched".** It never ran: serial 4921, `cap full`.
- **"the 14 S15 FAILs are the run results above, not gate defects."** Wrong for three, below.
- **No diagnosis.** Second round running where the report quotes the symptom and stops. D63 forbids edits, not reading. The same goes for the blank frame 0, which is in its own output directory, and the `VK_TIMEOUT` lines it quotes.

Accurate: hashes, census, seal, ledger, X3 shas, the departures in item 8, and the BUSY explanation.

## Owner defects (mine)

- **The audit missed the swapchain half of the wrapper.** I read `vk_aros_loader.c:414–440` and cleared the surface half. The swapchain half is about 100 lines later, and the audit's stated scope stopped at `src/platform/aros`. Kitty's fence handling and unchecked `vk_api_load_device` were out of scope too, though both sit on the guest path.
- **The runner lost 2 h 53 min of a 2 h 54 min ladder.** Four wedges at about 2600 s each (10,417 s of 10,467 s). It waits for an rc from a task that has already faulted and can never exit.
- **The runner did not know the agent's 4-exec cap.** The wedged `w`, `fifo`, `a`, `lp4` filled it, so `od2`, `od4`, `b1`, `soak` and `Avail` were refused. The order made it worse: the two runs most likely to crash went first, and the soak last.
- **`S15.stack`, `S15.timer-probe` and `S15.joystick` are false FAILs.** They read only `w`'s log. `stack_bytes=1048576`, the four joy lines and `delta_us` are in five other logs.
- **`S15b.output-captured` is too weak.** PASS with rc from 6 of 14 runs; it says nothing about the 8 lost.
- **`S15.headless-vs-host` is too narrow.** Frame 150 only let a blank frame 0 through.
- **`vk_api_load_device` has no NULL check.** A trap naming the entry would have cost seconds, not two wedges at 43 minutes each.
- **`a` used the release binary** on a first run, so it left no trace.
- **The dry pass was never confirmed.** I told the owner not to dispatch until it finished. It sat in `sha256sum -c` in s12 for 12 hours; I have now killed it. No harm came of it: every script matches and `S12c.complete` PASS on the real round. But I did not report it.

## Next

1. **Kitty:** fall back to instance GIPA for the swapchain entries, and trap on any NULL entry, by name.
2. **Kitty:** make the fence wait really wait, with a bound, then trap. Do not copy pixels from an unfinished frame.
3. **Runner:** treat a `PF-DBG` or `core_IRQHandle` on serial during a run as dead, and move on at once; stop launching at a budget below the cap; run soak and the headless runs before windowed and audio; use the debug binary on `a`.
4. **Gates:** fix the three false FAILs; compare all three frames; make `S15b.output-captured` count lost runs.
5. **Owner, Vulkan4Aros:** the CreateDevice wrapper's `want_swapchain` lookup, the two items in `m0b8_aros_audit.md`, and the `lp4` worker fault.
6. **Owner:** decide on a sound device for this lane; without one `S15.ahi` may not be able to pass.
7. **A new boot grant is needed.**
