# M0b (AROS half of M0) critique

> Source: Work/Kitty/Docs/Development/m0b_aros_critique.md
> Collected: 2026-11-02
> Published: 2026-11-02

# M0b (AROS half of M0) — critique of the opencode report (2026-11-02)

Verdict: **M0b is not achieved, and the report says so honestly.** The host half is real: the AROS binary builds, links, passes every H6 gate and packages, and Linux is byte-unchanged. The guest half produced one fact: every run traps opening the Vulkan loader. Both boots are spent.

How the two boots were lost:
- **Boot 1 was wasted on opencode's own runner defect.** It deployed the package with `--put-tree`, which needs `tar` on the guest; the image has none, and the Shell reports the miss with rc=0.
- **Boot 2 hit a loader-name mismatch that my prompt set up.** D24's three-name list doesn't match where the lane stages the loader, and W4's pre-boot check asked whether the guest "carries" the loader, not whether its name resolves.

## Verified on disk

- **Git:** HEAD `38fc422`, 13 modified files, 4 untracked paths (`src/platform/aros/`, `pace.h`, `tests/aros/`, `t-trap.cpp`). Nothing committed.
- **Boots:** `build/m0b/boots.txt` has exactly 2 lines (07:37:26, 07:42:23), both l0025. No third boot.
- **Guest results:**
  - 11 `rc_*.txt` files, all `rc=4`;
  - every `run_*.log` is 0 bytes;
  - `host_wall.txt` shows 0–1 s per run.
- **The trap, in `exec_w.log`:** `Software Failure. src/platform/aros/plat.cpp:702: vulkan loader open failed: …`. It is a real location: `plat.cpp:702` is the `alog_trap` call after the three `OpenLibrary` attempts.
- **Facts the guest did prove** (from `exec_*.log`, not from the empty run logs):
  - `stack_bytes=1048576`;
  - `uptime_probe delta_us=100059`;
  - `lp_num_threads set=0 getvar="0"`;
  - `ReadJoyPort` 0–3 all `0x00000000`.
- **Linux:** `make -j8 all linux-debug` rc=0 and `make test` rc=0 (re-run by me).
- **Vulkan4Aros:** `scripts/build` no longer exists. The remaining working-tree changes (`SERIES`, `vulkan_lavapipe_init.c`, `0080-…diff`, `errcap-b*.log`, `docs/evidence/…`) are the owner's.
- **Still running:** the spike server on `0.0.0.0:9099` (python3, pid 3248605). The report says so.

## Root cause of the guest failure

- D24 opens `RAM:vulkan_loader.library`, then `vulkan_loader.library` (found through `LIBS:`), then `RAM:vulkan.library`.
- On this lane the loader lives on a staging volume. The 2026-10-27 boot opened it as `DH1:Libs/vulkan_loader.library` by full path (`serial-20261027-2225.log:177`), which is neither in `RAM:` nor in `LIBS:`.
- The M0b boots never mention `DH1:`, so whether that volume was even attached is not shown.
- **The lavapipe ICD on the guest is unverified.** No log in either round shows one.

## Code defects

1. **The trap path leaks a signal.** Every run ends with `'RAM:kitty/kitty-debug' returned with unfreed signal 0x10000`. `pl_init` allocates a signal (the timer or AHI port) and the trap exits without freeing it.
   - Exit 4 must still release what `pl_init` opened so far: either `alog_trap` runs the platform teardown, or the loader is opened before anything else is allocated.

The rest of the AROS source passed S14. I have not reviewed it line by line, because nothing past `pl_init` ever executed.

## Report accuracy

- **Accurate:**
  - the census;
  - the trap text;
  - the boot count;
  - the 0-byte logs read as "no data";
  - the S12 attribution: the protected-file failures are my prompt and wiki edits, and the Vulkan4Aros changes are the owner's;
  - the honesty section, which lists the `sed -i` on `main.cpp`, the python call without `-I`, and the stray write into Vulkan4Aros.
- **Overstated:**
  - "Proven on real AROS hardware": it is a QEMU guest.
  - "debug and release, headless and windowed": every run trapped before any window opened, so the windowed path is unproven.
- **Misattributed against itself:** "W4 requires … a name-resolvable `vulkan_loader.library`". W4 said "carries". opencode's boot complied with the prompt's literal text; the precondition was mine and too weak. Its call to boot was still careless, because it rested on a 6-day-old serial log.

## Authorization

- **A write into Vulkan4Aros:** `--get` resolved results in the client's cwd, which put 23 files in `Vulkan4Aros/scripts/build/`. They were moved out and the directory removed, and the tree now shows nothing there. It is a breach of the read-only rule, though cleaned and disclosed.
- **No other write outside the block** was found.

## Prompt defects (mine)

1. **W4's pre-boot check tested presence, not resolution.** It has to state the exact string `pl_init` will pass to `OpenLibrary`, and show that string resolving on that lane.
2. **D24's name list was copied from the loader's own lookup order, not from the lane's staging.** The lane stages the loader at `DH1:Libs`.
3. **Two specs for the guest run disagree.** "`m0b-run.aros` runs everything" conflicts with "one `--exec` per run". opencode split the runs and put `>log` on `Execute`, which doesn't redirect the child's output, so all output landed in `exec_*.log`.
4. **The 1800 s soak exceeds the guest agent's 900 s exec budget.** The late rc is discarded.
5. **The runner was never dry-run.** A no-boot dry-run of `lane_run.sh` couldn't have reached the guest anyway. A boot spent purely on deployment should have been planned as a deployment boot.

## Owner decisions

- **Loader staging:** have `m0b-run.aros` run `Assign LIBS: DH1:Libs ADD`, which needs no change to D24. The alternative is to authorize copying the loader to `RAM:`. Either way, first confirm `DH1:` is attached on l0025 and that a lavapipe ICD is on the guest.
- **Kill the spike server on :9099** or keep it.
- **Next round:** a fresh grant, but only after prompt defects 1–4 are fixed and code defect 1 is folded in. Recommended shape: 1 boot for a deployment and resolution probe, plus 2 for the ladder.
- **Don't commit the M0b tree yet.** It is green on the host but unproven on the guest. Committing it as "M0b" would be false.
