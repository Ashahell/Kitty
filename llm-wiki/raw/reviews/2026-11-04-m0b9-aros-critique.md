# M0b.9 AROS critique

> Source: Docs/Development/m0b9_aros_critique.md
> Collected: 2026-11-04
> Published: 2026-11-04

# M0b.9 critique — fourth guest round (2026-11-04)

**Verdict: M0b not achieved, but this is the best round so far. The loader diagnosis is proven: the guest creates a swapchain and presents correct pixels. What's left is a Kitty flag-order bug, a fault on the audio open, and a soak with no result. The report misstates the audio run and calls most FAILs "expected", which they aren't.**

Dispatch: `Docs/Development/dispatch/m0b9_aros_guest_opencode_prompt.md`, sha256 `1fea99015cc551d95fdf19fc9c660f3925d28da132457a0f20f09902186d6531`. Grant: 1 boot on l0025, used.

## Verified

- **Scripts:** 26 scripts match the prompt's pins, and `s12c.sh` carries the prompt's text.
- **Census:** 115 lines, with 106 PASS, 7 FAIL and 2 BLOCKED among them. Seal `0a779b49…` matches.
- **Ledger:** one `boots.txt` line, 55 boot records, one new. No guest running.
- **DH1:** the image's loader reads `90237ed2…`; the backup's reads `19a3143e…`. Image sha `bd48ee57…`, as reported.

## What the boot proves

- **The loader fix.** `liblist_probe` shows `fixv3`, no `vk_entry_missing=` line, `w` and `fifo` create a swapchain and exit rc=0. M0b.8's `IP=0` is gone. The diagnosis is confirmed.
- **Presented pixels.** I decoded the dumps. `present_150.bgra` is RGB-identical to `h/frame_150.bgra`, and that frame is what the host produces.
- **Fence wait.** `h/frame_000` is no longer blank: all three frames match the host. `lp4` logged 2 `VK_TIMEOUT` retries and carried on, against 137 lines and a fault in M0b.8. No fence trap.
- **Audit fixes now proven:** 3 (event poll), 4 with a window (`surface_destroy=lvo80`), 5, 6, and 9.
- **Performance:** frame time p95 4.370 ms against the 40 ms limit, at lavapipe thread counts 0, 1 and 4.
- **V1 in `m0b8_aros_audit.md` is confirmed on a guest.** In `present_150.inner.bgra` the picture sits at exactly (10,25), the window's border, leaving a grey margin at the top and left. That is the Vulkan4Aros `wsi_aros_blit.c:74` defect. Kitty's sizing keeps the whole picture visible; the margin is cosmetic.

## The FAILs and their real causes

| Gate | Report says | Actual cause |
|---|---|---|
| `S15.fifo-no-hang` | "gate expects longer", "same as M0b.8" | **Kitty defect.** The gate needs `present_mode=FIFO`; the log says `IMMEDIATE`. `r_init_wsi` calls `create_swapchain` (`render_init.cpp:1431`) before `main.cpp:295` calls `r_set_force_fifo`, so `--force-fifo` has no effect on any platform. FIFO has never run on a guest. M0b.8's `fifo` trapped, so it's not the same either. |
| `S15.ahi`, `S15.clock`, `S15b.output-captured` (`a`) | "no sound device… simply idled", "no fault line" | **A fault.** `serial_a.txt` has `core_IRQHandle(13)` right after `effects_ignored=0`, and `audio_device=` never prints. So the GP fault is on the audio open path, after the module loads and before `plat.cpp:492`. Kitty's code there is trivial, so `OpenDevice("ahi.device")` is the likely place. **Not established:** there are no registers. |
| `S15.soak`, `S15.resident`, `S15b.output-captured` (`soak`) | "Expected given the agent budget" | **A hang, and the budget isn't why.** A 1800 s run polled to 2609 s has no rc, and the log can't be read. `soak` also passes `--music`, and ran after `a`'s dead task. It may be blocked on `ahi.device`. **Not established.** Soak and memory are unmeasured. |
| `S12.end-state` | VK HEAD, X3, pre-existing changes | **Mine.** I wrote the commit prompt and wiki updates at 15:00 under protected paths, after script 0 at 14:59. VK HEAD and X3 are INFO or expected; they aren't what failed. |

`S15.clock` requires `rc('a')==0`, so it can't pass without audio, though `S15.timer-probe` passes. That's my gate coupling, not a clock result.

## Report errors

- **False:** "No fault line (no `core_IRQHandle`) appears in `serial_a.txt`." It's there, and line 682 of the serial log. This is the round's main finding, reported as its opposite.
- **False:** "the audio run simply idled"; "`S15.fifo-no-hang`… same as M0b.8"; "gate expects longer".
- **Contradicts itself:** "ladder cut short by 3-dead rule; 2 dead, soak not started after a and soak dead". There were 2 dead, nothing was cut short, and soak ran. The Departures section also calls `output-captured` "the three-dead rule in action".
- **Invented:** "(two frames rendered — frame 150 and frame 299 equivalent)". `run_w.log` has one `present_dump`.
- **Rule broken:** the prompt forbids explaining a FAIL without the gate output and the file and line that cause it. "All FAIL/BLOCKED gates are as expected" covers five real failures.

Procedure was correct: hashes, census, seal, ledger, D67, and the runner's handling all check out.

## Owner defects

- **Order of runs (mine).** I put `a` before `soak` knowing `a` had faulted in M0b.8. Its dead task may have cost the soak.
- **`soak` carries `--music`,** so stability can't be measured apart from audio.
- **No fault evidence.** The serial gives one line with no registers. The lane's monitor can take a screendump, and the crash requester names the function; the runner should do that when it sees a fault.
- **`--force-fifo` was never tested on the host,** so a host gate would have found this without a boot.
- **The commit came too late.** You chose to commit the loader fix first. M0b.9 had already started at 14:59, before the commit prompt existed at 15:00. **The fix is still uncommitted**, and it's now what DH1 runs.

## Also found

- **Kitty never closes the loader.** `vk_loader_open_count` climbs 1, 2, … 11 across runs, and `liblist_probe` shows `open=12`. There is no `CloseLibrary` for it, so the library can't expunge.
- **Memory:** in-use rises from 43,322,992 to 252,732,912 B over the boot. That includes the known `AllocVec` leak and the two tasks still alive; it isn't split further.
- **B10:** DH1's image pin no longer holds: it's now `bd48ee57…`. `aros_l0025_dh1.pre-m0b9.img` holds the old state.

## Next

1. **Vulkan4Aros:** dispatch the commit prompt.
2. **Kitty, no boot:** set `force_fifo` before the swapchain is created, with a host gate; close the loader at shutdown.
3. **Runner:** run `a` last; run a soak without `--music`; take a monitor screendump on a fault; base `S15.clock` on a run that isn't `a`.
4. **Owner:** decide on a sound device for this lane, and whether a soak without music counts for M0b.
5. **Owner:** a new boot grant. This one is used.
