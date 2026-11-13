# M0b.23 AROS guest prompt

> Source: Docs/Development/dispatch/m0b23_aros_guest_opencode_prompt.md
> Collected: 2026-11-13
> Published: 2026-11-12

# opencode prompt — Kitty: Core Awakening, **M0b.23** (eighteenth guest round: one clean pass, an append-only census, CopperForge engine) (2026-11-12)

## Owner authorization (read first — opencode never edits this block)

    BOOTS GRANTED: 2
    LANE: l0025
    WRITE: ~/Work/Kitty/repo/ — the tree as M0b.22 left it (HEAD 3eef068; it and b5f929d add only
           llm-wiki/ to 38fc422), plus the uncommitted M0b–M0b.22 changes, Makefile per D47, M0b.22's
           runner installed, and the owner's D93 edit to src/platform/aros/plat.cpp in place;
           tree hash pinned in script 0). The only change to the tree this round is script X2's
           install of the supplied tests/aros/lane_run.sh (D58): M0b.22's runner with its round name and paths changed.
           Script X3 checks plat.cpp and finds D93 applied.
           No other file is edited, before, between or after the boots (D73).
           README.md and LICENSE are never edited.
           build/m0a/, build/m0a1/, build/m0b/ and build/m0b1/ through build/m0b22/ (earlier evidence)
           are never edited or deleted; build/m0b23/, build/aros*/ belong to this round. Every file this
           round writes goes under build/m0b23/ (D50, D53), except what the runner itself writes during
           the granted boot. Never restore any file from a pre.tar or any other copy (D55).
    DO NOT EDIT: ~/Work/Kitty/Docs/ (all of it — reports go in build/m0b23/, never in Docs),
           ~/Work/Kitty/AGENTS.md, ~/Work/Kitty/repo/llm-wiki/ (D79), every ~/Work/host_lavapipe*/ directory.
    BUILDS: as in M0b (host g++ and the AROS cross-build through the repo Makefile; Vulkan4Aros v1
           toolchain and SDK used read-only). No Vulkan4Aros build script, no library rebuild.
    RUN: the repo's binaries and tests; the acceptance scripts below exactly as given; read-only shell
           commands. A host window may be opened only by scripts 8, 13 and 14b (each run ≤ 65 s).
    IMAGES: ~/Work/vms/aros_l0025_dh0.img and aros_l0025_dh1.img may be READ only by scripts 16 and
           15b, only through Vulkan4Aros scripts/sfs_nav.py, and only while no l0025 guest is running.
           ~/Work/vms/riaudio_dh0.img may be READ only by script X4, the same way, and only while
           no riaudio guest is running. No script writes an image. The one write to an image is
           the runner's D81 `Copy`, made by the guest itself: one file, DH1:W/hdaudio.config.
    LANES / BOOTS: a boot is any start of an AROS guest. Boots are allowed only while the number of
           lines in build/m0b23/boots.txt is below BOOTS GRANTED, only on the lane named in LANE, only
           through that lane's own launcher (LANE l0025 → ~/Work/projects/Vulkan4Aros/scripts/
           lane_l0025_boot.sh), and only after script 14 and script 14b are all PASS and S16 is PASS.
           Append one line `<ISO time> <lane> <reason>` to boots.txt BEFORE each launch attempt.
    LEDGER: build/m0b23/boots.txt is append-only. Never truncate, rewrite or delete a line, by any
           means; a launch that is refused or fails keeps its line and counts. Script 12b compares
           it with the lanes' own boot-*.txt records.
           With BOOTS GRANTED: 0 nothing may start a guest, write to ~/Work/lanes/, or submit a job to
           a spike server. The exceptions, at any grant: script 14b's transport check, which starts the
           runner's own spike server with its spool under build/m0b23/, submits nothing, starts no
           guest and stops the server; and S14b.realpath, which runs the runner under a stand-in
           HOME in build/m0b23/fakehome, where every call must refuse before starting anything. Never `Avail FLUSH`, never RemLibrary, never reload a library.
           Any spike server or other process this round starts is stopped before the final report.
           Never stop, kill or signal a process this round did not start.
    VULKAN4AROS REPO: READ-ONLY (cat/grep/ls/sed -n/git show/git log/git status, running its
           compilers, running scripts/sfs_nav.py and the spike client). Never write a file in it —
           that includes the spike client's output: every path you hand the client is absolute and
           under build/m0b23/. Never run a write or destructive git command in it; never touch
           `src/abi/v1/AROS*`.
    GIT: read-only git commands in ~/Work/Kitty/repo: status, log, show, diff, rev-parse, ls-files.
           No commit, reset, checkout, restore, switch, branch, stash, tag, merge, rebase, push, or
           any other command that moves HEAD, the index or a ref, in any repository, for any reason
           (D96). The owner commits M0b after review.
    PACKAGES: install nothing. WEB: read-only research allowed; post nothing, download no binaries.

Don't ask for, infer, widen or grant any authorization yourself. If the work seems to need
something outside this block, stop that item, mark it FAIL or BLOCKED, and list it in the final report.

## What this task is

M0b.23 is the eighteenth guest round on lane l0025. **`BOOTS GRANTED: 2`.** M0b.22 booted twice.
In both boots no audio run logged an `ahi_xrun` line, `S15.ahi` saw `queued=472320` against a lower
bound of 445,350 (D93), and both runners fetched `assigns.txt`.

M0b.22's census can't stand. Script 8 ran twice (its first call was cut off by a 120 s tool timeout),
and S12c FAILed on the repeat. The agent then deleted lines from `census.txt` by hand until S12c
passed. Before script 0, it also ran `git reset --soft`.

M0b.23 repeats the round with nothing in the tree changed, so that its census is one clean pass.
This round changes how the round is run, not the code:
- **D94:** the census is append-only. The header writes a chain of census hashes, and S12c checks
  the census against it.
- **D95:** every script runs detached, with its output in `logs/<name>.out`, so no tool timeout can
  cut one off. A script is started once per pass.
- **D96:** if script 0 fails, the round stops. Git is read-only, with no exception.
- **D97:** the runner's launch line records its exit code as `RUNNER_EXIT=`.

**How to report guest results: gate lines and files, no explanations.** For each FAIL, give the
gate line as printed and stop. Don't write why it failed, don't call it expected, don't connect it to
anything else, and **append nothing to a gate line**: not in the census, not in a FAIL list, not in
parentheses. The review reads the files. Don't paste the runner logs; give their paths. The census
counts and the FAIL list in the report are copied from `census.txt`, never retyped. No "key result"
paragraph, no summary of earlier rounds: the report's sections say what they ask for and nothing more.
**"Verbatim" means copied bytes.** A line shortened, joined or summarised is not verbatim; if you
shorten one, say "summary", never "verbatim". **Copy numbers, hashes and addresses from command
output; never type them.** Each boot's values come from that boot's own `guest-boot<n>/` files.

Everything else stands: D20–D31 (M0b prompt), D32–D38 (M0b.1) with D32 as amended by D70, D39–D42 (M0b.2), D43, D45, D46
(M0b.3) as amended by D94 and D95, D47–D50 (M0b.4), D53 (M0b.5), D55 and D57 (M0b.6) as amended by D76, D92 and D97, D58 and D59 as amended,
D64–D66 as amended by D69–D71, D75, D78 and D81 (D70 as amended by D78 and D81), D72, D73 as amended
by D76 and D92, D74–D83, D86–D93, the Linux gates, frames 0/150/299 byte-identical to `build/m0a/r1/`,
`hash10s=e663d2da535e69a5`. Wherever an earlier delta names `build/m0b1`–`build/m0b22`, read
`build/m0b23`, and read this file as the prompt. No new features, no design edits, no renames.

## Verified facts (measured 2026-11-02 and 2026-11-03 — don't re-derive, don't contradict)

| Fact | Measured |
|---|---|
| The M0b round | `build/m0b/report.md`; `build/m0b/boots.txt` has 2 lines; 11 `rc=4`; every `run_*.log` 0 B; the program output is in `build/m0b/guest/exec_*.log`, which also shows `unfreed signal 0x10000` on every run. |
| Lane l0025 disks | Both boots of 2026-11-02 attached `aros_l0025_dh0.img` (index 0) and `aros_l0025_dh1.img` (index 1) (`~/Work/lanes/l0025/boot-20261102-0742.txt`). |
| Where the loader is | Read with `sfs_nav.py` (read-only): `DH1:Libs/vulkan_loader.library` 850,632 B, `DH1:Libs/vulkan_lavapipe.library` 130,500,536 B, `DH1:W/vulkan_icd.json` and `DH1:Libs/vulkan_icd.json` (150 B each, `library_path` `DH1:Libs/vulkan_lavapipe.library`). DH0's only `vulkan_loader.library` and `vulkan_icd.json` are in `DH0:.recycled/` — that is why the bare name did not resolve. |
| Bare-name lookup order | lddemon tries the caller's current dir, `libs/` under it, PROGDIR, then the `LIBS:` assign chain (Vulkan4Aros `docs/evidence/2026-10-25-b6-rediag/step2-census-and-lookup-order.md` §2.4). |
| The lane's own wiring | The 2026-10-24 boot ran `Assign ENVARC: DH1:W`, `Assign LIBS: DH1:Libs`, `Assign LIBS: ADD AROS:Libs` (`~/Work/lanes/l0025/serial-20261024-1815.log`, frames 21570, 21726, 21727); the measured chain was `[DH1:Libs, AROS:Libs]` (Vulkan4Aros `docs/dev/dispatch/l0025_expunge_verify_2026-10-24_ledger.md:596`). |
| Guest Shell quirks | No `tar` on the image, and a missing command still returns rc=0 (M0b boot 1). `>file` on an `Execute` line does not capture the output of the programs the script runs (M0b boot 2). The agent's exec budget is 900 s; a command still running then is answered `RUNNING` and its late rc is discarded (M0b report). |
| Spike client | Resolves a relative `--get` target against its own cwd (`Vulkan4Aros/scripts/`); M0b wrote 23 files there and removed them. |
| M0b.1 boots | `~/Work/lanes/kitty-m0b1-20261102/runner-boot1.log`: QEMU launched at 09:20:50, 09:27:25 and 09:29:54 (boot records `~/Work/lanes/l0025/boot-20261102-0920.txt`, `-0927.txt`, `-0929.txt`); `build/m0b1/boots.txt` holds 1 line. |
| Why the M0b.1 guest ran nothing | `tests/aros/lane_run.sh:100` ran `python3 -I -u spike_server.py serve`; `-I` drops `scripts/` from `sys.path` and the server died with `ModuleNotFoundError: No module named 'ui_frames'` (`build/m0b1/guest/spike.log`). The runner never checked it. The agent was healthy: every serial log ends `hello sent; entering recv loop`. |
| Spike server imports | `spike_server.py` imports `ui_frames` and `spike_control` from its own directory, so it must run without `-I`; `Vulkan4Aros/scripts/__pycache__` exists, so it must run with `-B` (no bytecode written into Vulkan4Aros). Default `--port` is 9091; the l0025 guest agent connects to 9099. |
| r12 | `build/m0b1/make_aros.log`: `aros-gates: type=REL osabi=AROS abi=1 und=0 r12=2`, `GATE FAIL: 2 stale-r12 call sites`. M0b built the same Makefile flags (clang `-O2`, no `-ffixed-r12`) at r12=0 (S14 18/18); `src/platform/aros/plat.cpp` changed +81/−52 in M0b.1. M0b's workaround is in `aros_wsi.c:45`, `plat.cpp:292`, `plat.cpp:338`. |
| M0b.2 runner | `tests/aros/lane_run.sh` (M0b.2): the `--check-transport` branch (lines 40–83) checks ports, lane and liveness; the boot path (lines 150–167) starts a server only `if ! submit --ping`, then `sleep 3`. Line 162 is `( cd "$OUT" && exec python3 -B -u "$SPIKE" serve \  # spike_server.py`: `\ ` escapes a blank, the comment ends the line, and the server gets no `--port`/`--spool` (defaults: port 9091, held by the nvk lane's server). |
| M0b.2 census | `build/m0b2/census.txt`: 82 lines, no S14, S14b or S16 line. Script 1 truncates `census.txt`, so each pass's census starts at S1; the last pass ran S1–S11, S13, S15, S15b, S12, S3, S12b. |
| M0b.2 Makefile | `ARO_CC_FLAGS` went from `-std=gnu11` to `-std=c11 -Dasm=__asm__`; `aros_wsi.c` builds with 0 `mov %rax,%r12` under either. The M0b.1 tree's Makefile is in `build/m0b2/pre.tar`. |
| r12 now | A reviewer rebuild of this tree: `aros-gates: type=REL osabi=AROS abi=1 und=0 r12=0` for both AROS binaries. |
| S3 | `src/app 601 / 600` on this tree and on the M0b.1 tree alike. |
| M0b.3 runner | `tests/aros/lane_run.sh` (M0b.3): `B` is assigned only in the `--check-transport` branch (line 80); the D41 check at line 137 reads `"$B/baseline.done"` inside a pipeline, so under `set -u` the subshell dies with `B: unbound variable`, `wc -l` prints 0 and the script continues. The only EXIT trap (line 62, inside `preflight`) kills the spike server; the lane is stopped only on the probe-failure path (line 243). |
| Makefile line 25 | Plain `-std=c11` does not compile `src/platform/aros/aros_wsi.c`: the SDK's `inline/exec.h` uses `asm` (`error: use of undeclared identifier 'asm'`). The M0b.2 tree's Makefile (`tar -xOf build/m0b3/pre.tar Makefile`, sha256 `f85b42c8cd690d7dac4c96db4dea2621da6bbb644a932eb5a6a43dad05c2277b`, line 25 `-std=c11 -Dasm=__asm__`) passes every S14 gate on this tree's code, `S14.aros-flags` included, with `r12=0` for both binaries (measured on a copy, 2026-11-02). |
| M0b.3 lanes write | `~/Work/lanes/kitty-m0b3-20261102/` (`extract_scripts.py`, `host-1-13.log`, `host-final-pass.log`) was written by the M0b.3 round. It is the owner's to remove; don't touch it. |
| M0b.3 S12 | The only FAIL reason was the owner's commit `f6ac1bc0` under VK `scripts/`; S12's VK check now ignores committed files. |
| S2.interfaces | `platform.h` declares `PlatArgs` with `lp_threads` and `lp_src` (since M0b, mtime 2026-11-02 07:13). The owner amended the design doc's §3.1 to the same four fields on 2026-11-02 (doc sha256 `d05e5d672119a8fc…`); S2.interfaces PASSes on the M0b.1 tree. |
| M0b.4 runner | `tests/aros/lane_run.sh` (M0b.4): `cleanup_leftover_spike` (lines 40–54) kills every python process listening on 9099/9100 on every call, before any check; line 237 `lanes_boot_dir="${LANES:-~/Work/lanes/l0025}"` leaves the tilde unexpanded, so in real mode D41 counts 0 records; `stop_lane` (lines 96–105) runs on every exit, with no launch guard; lines 314–315 `out_rc=$?` then `rc=$?`; line 265 branches on `IS_DRY_BOOT` after the `ledger mismatch` line. |
| M0b.4 S14b.transport | The plant (`python3 -I -c …time.sleep(40)`) was killed by `cleanup_leftover_spike`; the runner then found 9099 free and returned 0. |
| M0b.4 S12 | The only FAIL reason was `scripts/__pycache__/sfs_nav.cpython-315.pyc`: git-ignored (`.gitignore:215 **/__pycache__/`), written 12:27:48 by the owner's b10-rebase tools. S12's VK check now passes `--exclude-standard`. |
| M0b.4 lanes write | `~/Work/lanes/kitty-m0b4-20261102/` (two extractors, five `host-*.log`) was written by the M0b.4 round, as `kitty-m0b3-20261102/` was by M0b.3. Both are the owner's to remove; don't touch them. |
| M0b.5 round | `build/m0b5/census.txt` 111 lines, 83 PASS / 2 FAIL / 22 BLOCKED, sealed `9b2fea66…`; 0 boots; nothing written under `~/Work/lanes/`. Its first pass (14:06–14:10, `build/m0b5/logs/final-*.log`) had every S14/S14b gate PASS. At 14:16:30 the tree was restored from `build/m0b4/pre.tar` (Makefile `-std=gnu11`, sha256 `f9aea78c…`), and at 14:19:33 an edited script 0 (`build/m0b5/s0.sh`, tree hash `093aef37…`) took a new baseline; the final pass's two FAILs (`S14.aros-flags`, `S14b.makefile`) come from that restore. |
| Guest commands | DH0:C (S16's `tree-dh0.txt`) has `Assign`, `Avail`, `Echo`, `Execute`, `List`, `MakeDir`, `Stack`, `Type`, `Wait`, `Search`, `If` and no `Exists`. A missing command still answers `-> rc=0` (M0b boot 1). |
| Agent output | A finished exec prints `[exec] '<cmd>' -> rc=<n> (<t> ms)`, then the command's output, each line indented by 7 blanks (`build/m0b/guest/exec_w.log`). A command still running at the agent's 900 s budget prints `[exec] '<cmd>' -> RUNNING (<t> ms): …` and has no rc yet; never re-submit it (Vulkan4Aros `scripts/spike_control.py`). `Execute` itself answers `rc=0` even when the program it runs exits 4; `rc_<name>.txt` is the program's rc. |
| `--put` into a subdirectory | `build/aros-pkg/` has 25 files, one subdirectory (`spv/`). M0b's runner made `RAM:kitty/spv` before the puts and deployed; M0b.5's runner made only `RAM:kitty`. |
| Spike server ports | A fresh `spike_server.py serve --port 9099 --bulk-port 9100` listens on 9099 only (measured 2026-11-02). The launcher refuses (exit 3) if 4479 or 9100 is held, allows 9099 (its own server), refuses (exit 5) if `l0025-qemu` is already active, and refuses (exit 2) without `/dev/kvm`; it never falls back to TCG. |
| Boot records | The launcher writes `~/Work/lanes/l0025/boot-<YYYYmmdd-HHMM>.txt` and `serial-<stamp>.log` per boot, runs QEMU in the transient unit `l0025-qemu`, and returns once QEMU is up; the guest agent answers `--ping` after the guest has booted. |
| This tree | Script 0's tree hash, taken in the C locale, `37779fc7dad798f69246290dd5344fe8fb93f1eca1f599225c9cd3bfe419420e` (88 files): M0b.22's tree (D93 in `plat.cpp`, `d5c6b2e6…`; its runner, sha256 `ad79ec5a…`, installed); X2 then installs this round's runner (M0b.22's, with its round name and paths changed). HEAD `3eef068` (the owner's M0b.22 wiki ingest, on `b5f929d`). |
| M0b.6 round | `build/m0b6/census.txt` 112 lines, 90 PASS / 20 FAIL / 2 BLOCKED, sealed `e92d88d9…`; one boot (`~/Work/lanes/l0025/boot-20261102-2244.txt`); every script byte-identical to the prompt's. `S12c.complete` FAILed on five owner-pinned hashes (`hdr.sh`, `sX.sh`, `s14.sh`, `s15.sh`, `s12.sh`), taken from a scratch run; the other FAILs are S15/S15b with no data. |
| M0b.6 boot 1 | `~/Work/lanes/l0025/serial-20261102-2244.log`: deploy, `List RAM:kitty`, the `out/` tree and `Avail` answered; at line 213 the agent ran `Execute RAM:kitty/prelude.aros`, then `[PF-DBG] CR2=0000000000000088 IP=0000000001bf8d25` (user-mode read, `present=protection`), and the log stops advancing. `p`, `pt` and `ptr` each got only the client's 600 s `TIMEOUT` (`build/m0b6/guest/exec_p.log`); no file was fetched. The cause of the fault is not established. |
| Forms this lane has run | The four prelude assigns as separate execs: `~/Work/lanes/l0025/serial-20261024-1815.log` lines 225–231 (`Assign LIBS: DH1:Libs`, `Assign LIBS: ADD AROS:Libs`, each `exec done`), then programs ran under that chain. `Execute RAM:kitty/run_<name>.aros >RAM:kitty/out/run_<name>.log`: both M0b boots (`serial-20261102-0737.log`, `serial-20261102-0742.log`), every one `exec done`; the program ran (line 201 of the second: the D24 `Software Failure` loader line). `Execute` of a script without a redirect has also run: `serial-20261021-1504.log` line 1943 (`Execute DH1:runDH1smoke.run`, `exec done`). The four assigns inside one `Execute`'d script have run once on this lane: M0b.6's fault. |
| Agent wedge | When the agent never answers, the client prints `[submit] TIMEOUT: no result after <n>s` and exits non-zero (`build/m0b6/guest/exec_p.log`); the job stays queued, and nothing later is answered. |
| M0b.7 round | `build/m0b7/census.txt` 112 lines, 93 PASS / 17 FAIL / 2 BLOCKED, sealed `b2c0ff53…`; one boot (`~/Work/lanes/l0025/boot-20261103-2122.txt`). `S12c.complete` PASS. |
| M0b.7 boot | `~/Work/lanes/l0025/serial-20261103-2122.log`: no `PF-DBG`, all four prelude lines `rc=0`; `vk_loader="vulkan_loader.library"`, `tier=lavapipe`, `device="llvmpipe (LLVM 20.1.0, 256 bits)"`, `stack_bytes=1048576`, `uptime_probe delta_us=100044`; `p` trapped at `src/render/render_init.cpp:40`, `rc=4`, no `unfreed`. Nothing has drawn on a guest yet. The `sh_` logs were 0 B. |
| M0b.8 round | `build/m0b8/census.txt` 113 lines, 97 PASS / 14 FAIL / 2 BLOCKED, sealed `86197bb8…`; one boot (`~/Work/lanes/l0025/boot-20261104-0748.txt`). |
| M0b.8 boot | `h`, `p`, `lp0` and `lp1` answered rc=0. `w` and `fifo` faulted with `IP=0` right after `shaders=5 pipelines=5`. `a` took a GP fault about 2 s in, cause not established. An `lp4` worker faulted (`CR2=0x12`), followed by 137 `VK_TIMEOUT` lines and no rc. The four filled the agent's exec cap, so `od2`, `od4`, `b1`, `soak` and the final `Avail` were refused. |
| Swapchain | M0b.8's `IP=0` is a NULL `CreateSwapchainKHR` in `create_swapchain`: return offset `0x72ae` in the debug binary, `fmt[]` on the stack. Source and the deployed loader (sha256 `19a3143e…`) agree on why: `boundary_r7_create_device` removes the app's `VK_KHR_swapchain` and re-adds it only if `vkGetInstanceProcAddrStock(NULL, "vkEnumerateDeviceExtensionProperties")` returns a function, and with a NULL instance that lookup returns NULL (`stock/gpa_helper.c:270`, `stock/trampoline.c:96`). **UNVERIFIED on a guest.** Instance GIPA does not get round it: the trampoline aborts on a NULL slot (`stock/wsi.c:452`). |
| Fence wait | M0b.8's `render_frame.cpp` waited 1 s and carried on after `VK_TIMEOUT`. `build/m0b8/guest/h/frame_000.bgra` is all zeros, and `ms_max` is 1047–1060 ms. |
| Agent exec | The guest answers RUNNING at its fixed 900 s budget (`ATCP_EXEC_BUDGET_SECS`, `src/vulkan/loader/arostcp.c`), and the host's job queue waits for that answer, so a dead run still costs 900 s. At four live execs the agent answers `cap full` (BUSY) and starts nothing. A task that faulted never exits. |
| Fixed loader | `DH1:Libs/vulkan_loader.library` is `90237ed2…`, 850,472 B, stamped `fixv3`, since M0b.9. `~/Work/vms/aros_l0025_dh1.pre-m0b9.img` holds the previous image. If the owner restores it for other work, S16 FAILs and there is no boot. |
| M0b.9 round | `build/m0b9/census.txt` 115 lines, 106 PASS / 7 FAIL / 2 BLOCKED, sealed `0a779b49…`; one boot (`~/Work/lanes/l0025/boot-20261104-1504.txt`). `w` and `fifo` rc=0, `present_mode=IMMEDIATE` in both. `serial_a.txt` has `core_IRQHandle(13)` right after `effects_ignored=0`, with no `audio_device=` line. `soak` no rc at 2609 s, no fault line. |
| Screendump | QEMU's monitor takes `screendump <file>` on 127.0.0.1:4479 (`lane_l0025_boot.sh:61`, `:141`). Measured 2026-11-04 on a separate test QEMU, not on this lane: it writes a PPM file. A dead AROS task's requester names the function that crashed (Vulkan4Aros `AGENTS.md`:61). |
| AHI on this lane | `DH0:Devs/ahi.device` and `DH0:Devs/AHI/*.audio` are the seven drivers and device with the shas S16 pins. `DEVS:AudioModes` is what AHI scans (`workbench/devs/AHI/Device/device.c:284`). It's the same set of mode files on `riaudio`. Without a device, this QEMU has only the rtl8139. |
| M0b.10 round | `build/m0b10/census.txt` 115 lines, 107 PASS / 3 FAIL / 2 BLOCKED, sealed `bbb4c189…`; one boot (`~/Work/lanes/l0025/boot-20261104-1938.txt`). `dead_a.ppm`: task `RAM:kitty/kitty-debug`, `0x80000008`, module `sb128.audio`, `DriverInit` offset `0x135`. `serial_a.txt:23`: `core_IRQHandle(13)`. `soak` without music: rc=0, `ticks=89968 catchup_dropped=32`, both drops in the first minute. |
| M0b.12 round | `build/m0b12/census.txt` 119 lines, 93 PASS / 0 FAIL / 26 BLOCKED, sealed `98984d2f…`; 0 boots. After it, the owner added D72's logging to `plat.cpp` (`e71b5850…` → `9aede77e…`); `make aros` passes on it. |
| M0b.13 round | `build/m0b13/census.txt` 119 lines, sealed `d08db401…`; two boots (`boot-20261109-1737.txt`, `boot-20261109-1828.txt`); host gates all PASS but `S7.golden BLOCKED`; the guest gates read boot 2 and FAIL with no data. Boot 1's gate lines, from the same expressions over `guest-boot1/`: `build/m0b13/boot1-census.txt`. |
| M0b.13 audio | Boot 1, `run_a.log` and `run_soak.log` alike: `audio_source=PROGDIR:Devs/ahi.device`, `audio_device_id="ahi.device 6.13 (23.07.2026) " version=6.13`, `audio_device=ahi.device unit=0 audio_frames_queued=5760`, no `audio_progdir_error=`, no `ahi_xrun`. `kAhiFrames` is 960 and there are four requests (`plat.cpp:435`), so six writes went out. `lane_audio.wav` 44 B, the header only. M0b.13's code never read `io_Error` of a returned request and never logged `g_ahi_dropped`. |
| AHI prefs on this lane | Measured 2026-11-09 with `sfs_nav.py tree` (`build/m0b13/staging/tree-dh0.txt`, `tree-dh1.txt`): no `ahi.prefs` under `DH0:Prefs/Env-Archive/SYS/` or `DH1:W/`. `DH0:Devs/AudioModes/` holds `CMI8738`, `HDAUDIO`, `NVHDMI`, `SB128`, `VIA-AC97`, `ac97`. What unit 0 resolves to with no prefs file is not established. |
| Listing fault | `Assign` with no arguments, redirected, is the only prelude line both faulting boots reached last: M0b.6 (`serial-20261102-2244.log`, the prelude in one `Execute`) and M0b.13 boot 2 (`serial-20261109-1828.log:360`, its own exec). Both: `core_IRQHandle(14)`, `ERR=00000005`, `CR2=0000000000000088`, `IP=0000000001bf8d25`. The other six prelude boots on l0025 ran it clean. The IP is not symbolized; neither the Vulkan4Aros nor the ReIncarnation wiki names it. |
| M0b.22 round | `build/m0b22/census.txt` sealed `73921fe2…` (121 PASS / 0 FAIL / 2 BLOCKED as sealed); two boots (ledger `2026-11-12T18:58:15`, `19:33:58`); `graded_boot.txt` `graded=2 boot1_rc=14 boot2_rc=14 boot1_files=88 boot2_files=88`. Before the seal: script 8 ran twice (the first call cut off by a 120 s tool timeout after 3 gate lines); S12c printed `gates repeated within the pass: ['S8.h5-headless', 'S8.h5-knownbad', 'S8.release-no-plant']` and `GATE S12c.complete FAIL`; the agent deleted the 3 repeated lines and, after a second FAIL, both `GATE S12c.complete FAIL` lines from `census.txt`, then ran S12c a third time. Before script 0 the agent ran `git reset --soft b5f929d637ae71920bd88822a5379d8a453206ee`. Neither runner's exit code was recorded. |
| M0b.22 audio | Boot 2: `run_a.log` `elapsed_us=9982295`, `audio_frames_queued=472320`, `ahi_summary submitted=246 returned=244 errors=0 first_error=0 dropped=0 inflight=2`, `ahi_first_submit_us=514830`; `run_soak.log` `ticks=89983 catchup_dropped=17`, `elapsed_us=1799982732`, `audio_frames_queued=89086080`, `ahi_summary submitted=46399 returned=46397 errors=0 first_error=0 dropped=0 inflight=2`, `ahi_first_submit_us=487577`. Boot 1: `run_a.log` `audio_frames_queued=470400`, `ahi_summary submitted=245 returned=243 …`, `ahi_first_submit_us=574631`; `run_soak.log` `ahi_summary submitted=46403 returned=46401 …`, `ahi_first_submit_us=536705`. No `ahi_xrun` line in any of the four. Both boots fetched `assigns.txt` (88 files each). |
| M0b.21 round | `build/m0b21/census.txt` 122 lines, 119 PASS / 1 FAIL / 2 BLOCKED, sealed `196ef4b1…`; two boots (ledger `2026-11-12T14:43:45`, `15:28:14`); boot 1 runner exit 0, boot 2 exit 11 (`guest agent wedged at the Assign listing`); `graded_boot.txt` `graded=2 boot1_rc=14 boot2_rc=14`; `guest-boot1/` holds 88 files, `guest-boot2/` 87 (no `assigns.txt`). FAIL: `S15b.wiring` (`assigns.txt: missing`). |
| M0b.21 audio | Boot 2: `run_a.log` `ticks=477 catchup_dropped=23` (catch-up drops of 5 and 18 ticks), `elapsed_us=9982398`, `audio_frames_queued=470400`, `ahi_summary submitted=245 returned=243 errors=0 first_error=0 dropped=0 inflight=1`, no `ahi_xrun`; `run_soak.log` `ticks=89981 catchup_dropped=19`, `elapsed_us=1799982794`, `audio_frames_queued=89078400`, `ahi_summary submitted=46395 returned=46393 errors=0 first_error=0 dropped=0 inflight=2`, no `ahi_xrun`; `host_wall.txt` `a` 11 s, `soak` 1858 s. Boot 1 alike (`a` 244 submitted, soak 46399, no `ahi_xrun`). Each boot's `lane_audio.wav` (44.1 kHz stereo 16-bit, 1865.5 s): three silent 10 ms windows, one gap of 30 ms. |
| M0b.20 round | `build/m0b20/census.txt` 122 lines, 118 PASS / 2 FAIL / 2 BLOCKED, sealed `1b37be08…`; no `report.md`; two boots (ledger `2026-11-12T06:59:55`, `09:31:37`), both runners exit 0; `graded_boot.txt` `graded=2 boot1_rc=14 boot2_rc=14`. FAIL: `S15.ahi` (`queued=453120 elapsed_us=9982630 host_wall=11s summary=('472', '470', '0') xruns_a=17 xruns_soak=2731`), `S12.end-state` (two protected dispatch files changed by the owner after script 0). |
| M0b.20 audio | Boot 2: `run_a.log` `ticks=480 catchup_dropped=20`, `audio_frames_queued=453120`, `ahi_summary submitted=472 returned=470 errors=0 first_error=0 dropped=0 inflight=2`, 17 `ahi_xrun`; `run_soak.log` `ticks=89981 catchup_dropped=19`, `elapsed_us=1799982572`, `audio_frames_queued=86419200`, `ahi_summary submitted=90020 returned=90018 errors=0 first_error=0 dropped=0 inflight=2`, 2731 `ahi_xrun`, one every 30–34 passes (median 640 ms apart); `host_wall.txt` `a` 11 s, `soak` 1857 s. Boot 1: `a` 474 submitted, 455040 queued, 16 `ahi_xrun`; soak 89991 submitted, 2758 `ahi_xrun`. Boot 2 `lane_audio.wav` 329114692 B, 44.1 kHz stereo 16-bit, sound from 0.04 s to 1865.71 s, 2799 silent 10 ms windows (2798 single, one of 30 ms). |
| M0b.19 round | `build/m0b19/census.txt` 122 lines, 119 PASS / 1 FAIL / 2 BLOCKED, sealed `6dbb3a2a…`; two boots (ledger `2026-11-11T00:35:09`, `01:11:17`), both runners exit 0; `graded_boot.txt` `graded=2 boot1_rc=14 boot2_rc=14`. FAIL: `S15.ahi` (`queued=444480 xruns=16`). |
| M0b.19 audio | Boot 2: `run_a.log` `ticks=470 catchup_dropped=30`, `elapsed_us=9982548`, `audio_frames_queued=444480`, `ahi_summary submitted=463 returned=461 errors=0 first_error=0 dropped=0 inflight=1`, 16 `ahi_xrun`; `run_soak.log` `ticks=89973 catchup_dropped=27`, `elapsed_us=1799982448`, `audio_frames_queued=86348160`, `ahi_summary submitted=89946 returned=89944 errors=0 first_error=0 dropped=0 inflight=1`, 2784 `ahi_xrun`, one every 31–32 passes; `host_wall.txt` `a` 11 s, `soak` 1857 s. Boot 1: `a` 470 submitted, 451200 queued, 16 `ahi_xrun`; soak 89952 submitted, 2804 `ahi_xrun`. Boot 2 `lane_audio.wav` 329054380 B, 44.1 kHz stereo 16-bit, sound from 0.04 s to 1865.38 s, 2866 silent 10 ms windows. Every `ahi_xrun` line ends `tick=0`. |
| M0b.18 round | `build/m0b18/census.txt` 122 lines, 117 PASS / 3 FAIL / 2 BLOCKED, sealed `0b99b188…`; two boots (ledger `2026-11-10T20:18:38`, `20:54:38`); boot 1 runner exit 0, boot 2 exit 11 (`guest agent wedged at the Assign listing`); `graded_boot.txt` `graded=2 boot1_rc=14 boot2_rc=14`. FAIL: `S15.ahi` (`queued=5760 xruns=0`), `S15b.wiring` (`assigns.txt: missing`), `S15b.sound` (`lane_audio.wav 329098824 B, non-zero bytes=3530`). |
| M0b.18 audio | Both boots, `run_a.log` and `run_soak.log`: `ahi_hidd_preload=1`; `ahi_probe … hdaudio_open=1`; `ahi_drivers hidd_lock=0 hidd_err=205 … hidd_before=1 hidd_after=1`, `dir=` a different directory each run; one mode, `ahi_mode id=0x3e0001 name="hdaudio:HiFi 16 bit stereo++" driver="hdaudio"`; `ahi_best id=0x3e0001`; `ahi_summary submitted=6 returned=2 errors=0 first_error=0 dropped=0 inflight=4`. Boot 2 `lane_audio.wav`: 44.1 kHz stereo 16-bit, 329098780 data bytes, non-zero bytes only at 0.040–0.080 s and 9.642–9.682 s. |
| Mixer pacing | Corrected 2026-11-12. `src/audio/audio.cpp`'s `aud_tick` (D90) renders 960 frames and calls `pl_audio_queue` in a loop, up to four times, while `pl_audio_queued_frames()` is below 3840; `main.cpp:231` calls it once per stepped tick, and catch-up-dropped ticks don't call it. On AROS before D91 each AHI request held 960 frames (20 ms) and `pl_audio_queued_frames` returned 3840 at two in flight, else 960 per request in flight. With D91 each request holds 1920 frames; the function returns 3840 at two in flight, else 1920 per request in flight plus the frames gathered for the next one. Linux (`SDL_AUDIODRIVER=dummy`, 10 s, measured 2026-11-11): `ticks=500 catchup_dropped=0`, `audio_frames_queued=479040`. |
| M0b.17 round | `build/m0b17/census.txt` 122 lines, 100 PASS / 20 FAIL / 2 BLOCKED, sealed `d3415f7b…`; two boots (ledger `2026-11-10T18:31:58`, `18:37:10`), both runners exit 8; `graded_boot.txt` `graded=2 boot1_rc=3 boot2_rc=3`. `rc_p.txt` rc=4, `rc_pt.txt` rc=4, `rc_ptr.txt` rc=2. Boot 1 `exec_listing.log`: `TIMEOUT: no result after 120s`. Its report gave 121 lines and 98 PASS, listed `S15.joystick` (a PASS) and left out `S15b.probe` (a FAIL). |
| M0b.16 round | `build/m0b16/census.txt` 121 lines, 116 PASS / 3 FAIL / 2 BLOCKED, sealed `6fdacab5…`; two boots (`boot-20261110-1621.txt`, `boot-20261110-1658.txt`), both runners exit 0; `graded_boot.txt` `graded=2 boot1_rc=14 boot2_rc=14`. FAIL: `S15.ahi` (`queued=5760 xruns=0`), `S15b.ahi-mode` (`ahi_probe db_before=0 envarc_config=1 envarc_err=0 hdaudio_open=0 \| ahi_best id=0x1f0002 name="VOID:HiFi 32 bit stereo++"`), `S15b.sound` (`lane_audio.wav 44 B, non-zero bytes=0`). `S15b.envarc` PASS. |
| M0b.16 audio | Both boots: `run_a.log` `ahi_probe db_before=0 envarc_config=1 envarc_err=0 hdaudio_open=0`, `run_soak.log` the same with `db_before=1`; five `driver="void"` modes, `ahi_best id=0x1f0002`, `ahi_summary submitted=6 returned=2 errors=0 first_error=0 dropped=0 inflight=4`, as in M0b.15. Runner: `copied AROS:Devs/AudioModes/HDAUDIO …` and `copied AROS:Prefs/Env-Archive/hdaudio.config to DH1:W`. `assigns.txt`: `DRIVERS AROS:Tools/Commodities`. |
| M0b.15 round | `build/m0b15/census.txt` 119 lines, 115 PASS / 2 FAIL / 2 BLOCKED, sealed `9cfca381…`; two boots (`boot-20261110-0744.txt`, `boot-20261110-0821.txt`), both runners exit 0; `graded_boot.txt` `graded=2 boot1_rc=14 boot2_rc=14`. FAIL: `S15.ahi` (`queued=5760 xruns=0`), `S15b.sound` (`lane_audio.wav 44 B, non-zero bytes=0`). |
| M0b.15 audio | Both boots, `run_a.log` and `run_soak.log` alike: `ahi_mode id=0x1f0001`…`0x1f0005`, every one `driver="void"`, `ahi_modes=5`, `ahi_best id=0x1f0002 name="VOID:HiFi 32 bit stereo++"`; `ahi_summary submitted=6 returned=2 errors=0 first_error=0 dropped=0 inflight=4`. `audiomodes.txt`: `HDAUDIO 126`. The serial logs (`serial-20261110-0744.log`, `-0821.log`) hold no line from `hdaudio.audio`. |
| AHI keeps a mode | `workbench/devs/AHI/Device/database.c`, `AddModeFile`: a mode file's driver is opened as `OpenLibrary("DEVS:AHI/<driver>.audio", DriverVersion)` (`DriverVersion` 2, `header.c:183`); if that fails the file's modes are not added, with no requester. The database is a public semaphore named `Audio Mode Database` (`database.h:32`); `device.c` scans only while it holds no mode. |
| HDA driver init | Corrected 2026-11-10 (the M0b.16 row was read from an older source). `workbench/devs/AHI/Drivers/HDAudio/driver-init.c`, `DriverInit`: opens `dos.library` and `stdc.library`, then `ahi_hda_init` (`hda_hidd.c:399`) opens `oop.library` and `OpenLibrary("DRIVERS:hdaudio.hidd", 0)`, returning FALSE if either fails; then `parse_config_file` reads `ENVARC:hdaudio.config` for `QUERY` and `speaker=` overrides only ("Vendor/device ID lines are obsolete: controllers are discovered by PCI class through hdaudio.hidd"); then `ahi_hda_obtain_controllers`, FALSE with none. The deployed `hdaudio.audio` (`b3aa4d3c…`) holds the strings `DRIVERS:hdaudio.hidd`, `hidd.audio.hda`, `ENVARC:hdaudio.config`. Its messages go through `DebugPrintF`; no lane's serial log has ever shown one. |
| DRIVERS: at boot | `rom/dos/cliinit.c:644-647`: `lock = Lock("DEVS:Drivers")`, then `AssignLock("DRIVERS", lock)` and `AssignAdd("LIBS", lock)`, the same lock. Both calls take ownership (`assignadd.c:28`: "Do not use or free the lock after calling this function"). `C:Assign` without a switch replaces an assign and unlocks its old elements, so D32's `Assign LIBS: DH1:Libs` frees the lock `DRIVERS:` still holds. Any later replace or `REMOVE` of `DRIVERS:` would unlock it a second time. |
| PREPEND on this lane | `C:Assign … PREPEND` calls `AssignAddToList(name, lock, 0)` (`workbench/c/Assign.c:637-643`). Its loop, `for (al = &dol_List; *al && cnt < position + 1; …, cnt++)`, steps once past the first list element whenever the assign has more than one directory, so the new lock goes third; only a single-directory assign gets it at the top. M0b.17 boot 2 `assigns.txt`: `LIBS AROS:Libs + AROS:Devs/Drivers + L25DATA:Libs + AROS:Classes`. With that order `p` failed to open `vulkan_loader.library`, which exists only in `DH1:Libs`; why is not established. |
| LDDemon lookup | `rom/lddemon/lddemon.c`, `LDRequestObject`: `stripped_libname = FilePart(libname)` (line 360), then `FindName(list, stripped_libname)` on the exec library list (line 434) before any load (line 456). A library already resident is returned whatever path the caller named. `hda_hidd.c:405` opens `DRIVERS:hdaudio.hidd`; DH0 holds `/Devs/Drivers/hdaudio.hidd size=24760`, and `SYS:` is a single-directory assign on this lane. |
| DRIVERS: on this lane | `assigns.txt`, taken after the prelude, each round: m0b7 `AROS:`, m0b8 `AROS:System`, m0b9 `AROS:`, m0b10 `AROS:C`, m0b11 `AROS:S`, m0b15 `AROS:Demos`, m0b16 `AROS:Tools/Commodities`; `LIBS` is `L25DATA:Libs` + `AROS:Libs` only. `tree-dh0.txt`: `/Devs/Drivers/hdaudio.hidd size=24760`. The riaudio lane, which never replaced `LIBS:`, gave `best_mode 0x003E0001`. |
| QEMU HDA | The launcher's `-device intel-hda -device hda-duplex` presents controller `8086:2668` (Vulkan4Aros `llm-wiki/raw/articles/2026-09-28-ahi-drivers-vector-alignment-qemu-hda.md`); on the riaudio lane, with the stock `ENVARC:`, the same drivers gave `best_mode 0x003E0001`. |
| hdaudio.config | Read 2026-11-10 with `sfs_nav.py cat`: `DH0:Prefs/Env-Archive/hdaudio.config` 2182 B, sha256 `3645c2cbc4b21d0af81c7c0b6f3fa72c68b12d0e7ecc79745741da8de6e70ae7`, line 13 `0x8086, 0x2668`. `sfs_nav.py tree` of `aros_l0025_dh1.img`: no `hdaudio` file anywhere; `DH1:W/` holds 30 entries. |
| M0b.14 round | `build/m0b14/census.txt` 119 lines, 114 PASS / 3 FAIL / 2 BLOCKED (sha256 `84887dde…`); two boots (`boot-20261109-2306.txt`, `boot-20261109-2343.txt`), both runners exit 0; `graded_boot.txt` `graded=2 boot1_rc=14 boot2_rc=14`. FAIL: `S15.ahi`, `S15b.sound`, `S12.end-state` (HEAD, branches and the protected Kitty list changed by the owner's wiki commit; the tree diff shows `Only in .: llm-wiki`). |
| M0b.14 audio | Both boots, `run_a.log` and `run_soak.log` alike: `audio_source=PROGDIR:Devs/ahi.device`, `audio_device_id="ahi.device 6.13 (23.07.2026) " version=6.13`, `audio_frames_queued=5760`, `ahi_summary submitted=6 returned=2 errors=0 first_error=0 dropped=0 inflight=4`; no `ahi_io_error=`, no `ahi_xrun`. |
| M0b.14 late reads | Both boots: `Type ENV:Sys/ahi.prefs HEX` rc=10, `ahi_prefs.txt` `TYPE: can't open ENV:Sys/ahi.prefs` / `object not found`; `List DEVS:AudioModes` rc=20, `audiomodes.txt` `List: object not found`; the listing rc=0, `assigns.txt` `DEVS RAM Disk:kitty/Devs` `+ AROS:Devs`, `ENV RAM Disk:ENV`. |
| AHI mode scan | `workbench/devs/AHI/Device/device.c:284` `AHI_LoadModeFile("DEVS:AudioModes")` when the database is empty; `:300` `AHI_LoadModeFile("SYS:Storage/AudioModes/VOID")` if that found no mode; `database.c:637` locks the name with `Lock(name, ACCESS_READ)`. With no prefs, the unit's mode is `AHI_BestAudioIDA` over `AHIDB_Realtime, TRUE` (`device.c`, end of `ReadConfig`). |
| Mode files | Read 2026-11-10 with `sfs_nav.py cat`: `DH0:Devs/AudioModes/HDAUDIO` 126 B, sha256 `a2d14b80…`, the same bytes on `riaudio_dh0.img`; driver `hdaudio`, one mode, name `hdaudio:HiFi 16 bit stereo++`. `DH0:Storage/AudioModes/VOID` 496 B, driver `void`, names `VOID:…`. `DH0:C/Copy` and `DH0:C/MakeDir` exist (`build/m0b13/staging/tree-dh0.txt`). |
| Device identity | The stock `DH0:Devs/ahi.device` (sha256 `4cd71c6b…`, 179,088 B) carries `$VER: ahi.device 6.13 (23.07.2026)`, the same as the staged one (`155157e7…`). `audio_device_id=` does not tell them apart. |
| Rebuilt AHI | Vulkan4Aros `llm-wiki/raw/articles/2026-09-28-ahi-drivers-vector-alignment-qemu-hda.md`: the same fault at the same offset, from auto-vectorized `movaps`; drivers rebuilt with `-fno-vectorize -fno-slp-vectorize`, proven on the `riaudio` lane with `intel-hda`: 3/3 clean boots, 20174 buffers, 0 xruns. Measured 2026-11-04 with `sfs_nav.py`: `riaudio_dh0.img` holds each rebuilt file with a `.orig` beside it, and all eight `.orig` files are byte-identical to this lane's DH0 files. **Not measured:** these files on this lane; `riaudio`'s system is older than this lane's. |
| Launcher audio | `LANE_AUDIO=<wav>` adds `-audiodev wav,id=snd,path=<wav> -device intel-hda -device hda-duplex,audiodev=snd`; unset, the boot is as before. Added by the owner's round `vk4aros_lane_audio_opencode_prompt.md`. If it isn't there, `S16.audio` FAILs and there is no boot. |
| M0b.11 round | `build/m0b11/census.txt` 117 lines, 106 PASS / 6 FAIL / 2 BLOCKED, sealed `c3c9fe3d…`; one boot (`~/Work/lanes/l0025/boot-20261104-2322.txt`). `dead_a.ppm`: `ahi.device`, `ReadConfig` offset `0x44`, module offset `0x1024`: stock file `4cd71c6b…`, whose `movaps` is at `0x1024`. The `sb128.audio` fault didn't recur. `lane_audio.wav` 44 B. |
| Forcing the device | ReIncarnation `llm-wiki/raw/articles/2026-09-21-zztest-lddemon-verdict.md`: its standing rule is to force the first open through `PROGDIR:`, after which all later opens serve that resident. `rom/lddemon/lddemon.c:123` loads a `PROGDIR:` name directly; the failure path sets `IOERR_OPENFAIL` (`:555`). `PROGDIR:Devs/` itself hasn't run on a guest; their file sat at `PROGDIR:ahi.device`. |
| Upstream AROS | Checked 2026-11-04 on `aros-development-team/AROS`. No commit fixes the AHI `movaps`, and #1281 is closed as a fork issue. `a5e2ee7e53` (2026-11-04, LDDemon keeps a borrowed current-directory lock) isn't in this lane's tree. `0a7587f325` (#1882, Void pacing) and `c0d71be0d8` (#1724, capture offsets) are later than the AHI files on DH0. #1754 (unit opens before its mode is registered) was closed unmerged. None is applied here. |
| AROS DOS | `Seek` returns the position before the seek, -1 on error (AROS `rom/dos/seek.c`). `Write` returns the byte count, 0, or -1 on error (`rom/dos/write.c`). |
| Window helper | `arosPollWindowEvents` returns non-zero to close and 1 for an unknown surface (`src/vulkan/loader/vk_aros_loader.c:672`, `src/vulkan/pal/vkpal_aros.c:1305`; `vkcube_aros.c:640` loops on `!arosPollWindowEvents`). The comment in `aros_window_surface.h` says the opposite and is wrong. The window is `WFLG_GIMMEZEROZERO` with the size as its outer size (`vkpal_aros.c:1125`). |
| Mesa AROS WSI | `currentExtent` is the window's inner size (`src/abi-patches/v1/mesa-26.2.2/wsi_common_aros.c:61`). `wsi_aros_blit` writes at `(BorderLeft, BorderTop)` of `window->RPort` (`wsi_aros_blit.c:74`), which on a GimmeZeroZero window is already the inner layer. The picture should therefore land offset by the border inside the inner area. **UNVERIFIED on a guest**; `present_150.inner.bgra` will show it. |
| AHI links | Corrected 2026-11-10. `workbench/devs/AHI/Device/devcommands.c`, `CMD_WRITE`: a request whose `ahir_Link` is on the playing, silent or waiting list is delayed. If that link has a channel (it is playing), the request is attached to the link's voice (`:1199-1249`); if the link is itself waiting (`Channel = NOCHANNEL`), the request is only put on the waiting list (`:1186-1197`). A second request linked to one that already has a follower fails with `AHIE_UNKNOWN`. When a request finishes, `RemPlayers` (`:1458-1490`) moves its follower to the playing list with the parent's channel but never starts it on the voice, so a follower that was not attached at write time never sounds and is never returned. The documented use is double buffering (`:816-819`). The lane has a sound device: `-device intel-hda -device hda-duplex` with `LANE_AUDIO` (D70). |

## Inputs (read before writing code)

`build/m0b6/report.md`; `~/Work/Kitty/Docs/Development/m0b6r2_aros_critique.md` and `m0b6_aros_critique.md`; the M0b.6 prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b6_aros_guest_opencode_prompt.md` (D54–D57);
`build/m0b5/report.md`; `~/Work/Kitty/Docs/Development/m0b5_aros_critique.md` (its "Owner-approved fix" section included); the M0b.5 prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b5_aros_fix_opencode_prompt.md` (D51–D53);
`build/m0b4/report.md`; `~/Work/Kitty/Docs/Development/m0b4_aros_critique.md`; the M0b.4 prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b4_aros_fix_opencode_prompt.md` (D47–D50);
`build/m0b3/report.md`; `~/Work/Kitty/Docs/Development/m0b3_aros_critique.md`; the M0b.3 prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b3_aros_fix_opencode_prompt.md` (D43–D46);
`build/m0b2/report.md`; `~/Work/Kitty/Docs/Development/m0b2_aros_critique.md`; the M0b.2 prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b2_aros_fix_opencode_prompt.md` (D39–D42);
`build/m0b1/report.md`; `~/Work/Kitty/Docs/Development/m0b1_aros_critique.md`; the M0b.1 prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b1_aros_fix_opencode_prompt.md` (D32–D38 still apply);
`build/m0b/report.md`; `~/Work/Kitty/Docs/Development/m0b_aros_critique.md`; the M0b prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b_aros_opencode_prompt.md` (D20–D31 and W1–W5 still apply);
`tests/aros/*`; `src/platform/aros/*`. Skills: `aros-development` and `karpathy-guidelines` — load
both and say so in the report.

## Design deltas (D32–D50, D53, D55, D57 and D59 stand, except D44, replaced by D47; D54 is replaced by D58, D56, D60 and D63 by D67; D61 is in the tree; D62 is replaced by D66; D64–D66 are amended by D69 and D70, D68 is replaced by D69; D58 and D59 are amended by D65; D64–D67 are new; D67 is replaced by D73; D57 and D73 are amended by D76; D66 and D70 are amended by D78 and D81; D84 is replaced by D87 and D85 by D88; D57, D73 and D76 are amended by D92; S15.ahi is amended by D93; D46 is amended by D94 and D95, D57 by D97; D94–D97 are new — implement exactly; list them in the report)

| # | Decision |
|---|---|
| D32 | **Guest wiring** (owner decision 2026-11-02). New `tests/aros/prelude.aros`, packaged by `make aros-pkg`, whose only command lines are, in this order: `Assign ENVARC: DH1:W`, `Assign LIBS: DH1:Libs`, `Assign LIBS: ADD AROS:Libs`, `Assign >RAM:kitty/out/assigns.txt`. `m0b-run.aros` executes it first (after `Stack 1048576`). D24 is unchanged: the bare name `vulkan_loader.library` now resolves through `LIBS:` to `DH1:Libs`, and the run logs `vk_loader="vulkan_loader.library"`. Never copy or put a library to the guest. |
| D33 | **Probe first, ladder only on success.** Three runs precede the eleven M0b runs: `p` = `KD --headless --frames 1 --out RAM:kitty/out/p S`; `pt` = `KD --plant-loader-fail --headless --frames 1 S`; `ptr` = `K --plant-loader-fail --headless --frames 1 S`. If `rc_p.txt` is not `rc=0` or `run_p.log` has no `vk_loader=` line, the runner skips the eleven runs, fetches what exists, stops the lane, and exits 8 with `probe failed; ladder not run`. |
| D34 | **Output capture.** Each `tests/aros/run_<name>.aros` is exactly three command lines: `Stack 1048576`; the program line, written `RAM:kitty/<kitty\|kitty-debug> >RAM:kitty/out/run_<name>.log <arguments>` (the redirect directly after the command name); `Echo "rc=$RC" >RAM:kitty/out/rc_<name>.txt`. No `Execute` line, in any guest script or in the runner, carries a `>` redirect, except the runner's per-run `Execute … >RAM:kitty/out/sh_<name>.log` (D58, D59). |
| D35 | **Runner paths and ledger.** `tests/aros/lane_run.sh` reads the authorization from this prompt (`dispatch/m0b23_aros_guest_opencode_prompt.md`) and counts boots in `build/m0b23/boots.txt`. Its first check resolves `<outdir>` with `realpath -m` and refuses (exit 3, `outdir outside build/m0b23`) unless it lies under `$KR/build/m0b23/`; every `--get`/`--put` path it passes is absolute. If it starts a spike server it records the PID in `<outdir>/spike.pid` and stops it before exiting. |
| D36 | **The soak and the 900 s budget.** On `RUNNING`, poll `Type RAM:kitty/out/rc_soak.txt` until its output has an indented `rc=<n>` line (or 2,600 s pass; the image has no `Exists`, D58); `rc_soak.txt`, written by the run script, is the authority, and the soak's host wall ends when it appears. |
| D37 | **Trap teardown and the plant.** On AROS, every exit-4 path (the Guru line from `pl_trap`/`alog_trap`, D23's timer check, D24's loader check, D26's stack check) first releases, in reverse order, everything `pl_init` acquired up to that point (watchdog process, AHI requests and device, `lowlevel.library`, the timer request and its port), then exits 4. No guest run may print `unfreed`. Debug-only `--plant-loader-fail` (AROS only) makes D24 skip all three `OpenLibrary` calls and take the real failure path (same trap text); release AROS builds and both Linux builds exit 2 on the flag. |
| D39 | **r12.** Both AROS binaries pass the r12 gate (0 stale-r12 call sites) with the Makefile's AROS flags unchanged. Fix it in `src/platform/aros/` the way M0b did (keep a long-lived call result out of a C++ frame that clang allocates in `%r12`, e.g. a helper in `aros_wsi.c`). No `-ffixed-*` flag, no `-O` change, no optimize pragma or attribute, no inline asm, no change to the gate. |
| D40 | **Runner transport.** (a) The runner always starts its own spike server: `cd <outdir> && python3 -B -u "$VK/scripts/spike_server.py" serve --port 9099 --bulk-port 9100 --spool <spool>` — never `-I`, never `cd` into Vulkan4Aros. It never reuses a server it did not start. (b) Before starting it: if anything listens on 9099 or 9100, refuse (exit 9, `port <n> held by <ss line>`) — no ledger line, no boot, nothing killed. (c) After starting it: within 10 s its PID is alive and `ss -ltnpH` shows that PID on `:9099`; otherwise stop it and exit 9 with `spike server did not start` and the last 5 lines of its log — no ledger line, no boot. (d) Before the ledger line: if a `qemu-system` process for the lane is running, refuse (exit 9, `lane <lane> already running`). (e) `lane_run.sh --check-transport <outdir>`: D35's outdir check, then (b) and (c) with the spool `<outdir>/tspool`, then stop the server, wait until 9099 is free, print `transport ok pid=<pid>` and exit 0. It reads no grant, writes no ledger line, starts no guest, submits nothing, writes nothing under `~/Work/lanes/`. |
| D41 | **Ledger.** The runner appends its `boots.txt` line immediately before calling the launcher and never removes it, whatever the launcher does. Before appending, it counts the lane's `~/Work/lanes/<lane>/boot-*.txt` files newer than `build/m0b23/baseline.done`; if that count is not equal to the `boots.txt` line count, it refuses (exit 3, `ledger mismatch: <n> boot records, <m> ledger lines`). |
| D42 | **`PlatArgs` (owner decision 2026-11-02).** The design doc §3.1 now reads `struct PlatArgs { const char* renderer_override; const char* save_root; int lp_threads; const char* lp_src; };`. `platform.h` already matches; don't edit either, and don't move `lp_threads`/`lp_src`. S2.interfaces must PASS. |
| D43 | **One server start.** `lane_run.sh` defines `SPIKE="$VK/scripts/spike_server.py"` once, at the top, and exactly one function `preflight() { … }` taking `<outdir> <spool>`. `preflight` does D40 (b), (d), then starts the server, then (c), and is the only code that starts `spike_server.py serve`: one command, `( cd "$1" && exec python3 -B -u "$SPIKE" serve --port 9099 --bulk-port 9100 --spool "$2" ) >"$1/spike.log" 2>&1 &`. The `--check-transport` branch calls `preflight "$OUT" "$OUT/tspool"` (D40e's spool — not `build/m0b23/tspool`). The boot path calls `preflight "$OUT" "$SPOOL"` after D41's ledger check and before the `boots.txt` append, and calls neither `submit` nor `sleep` before that append. No other server start, no reuse of a server that answers. In `lane_run.sh`, no line carries both code and a comment, and no line ends in a backslash followed by blanks. |
| D44 | *Replaced by D47* (the M0b.1 Makefile fails `S14.aros-flags`). |
| D45 | **src/app ≤ 600 lines** by S3's count. Remove lines from `src/app/main.cpp` without changing behaviour: same flags, same help text, same exit codes, Linux frames and music hash unchanged. Don't join statements onto one line or delete comments to make the count. |
| D46 | **One complete pass.** Script 1 truncates `census.txt`, so the census is the last pass. The final pass runs X0, X1, X2, X3, X, 1–11, 13, 14, 14b, 16, [runner], 15, 15b, 12, 12b, 12c in that order with no script skipped or repeated; script 12c FAILs any other census. Fix, then re-run the whole pass. |
| D47 | **Makefile.** `Makefile` is byte-identical to the M0b.2 tree's: `tar -xOf build/m0b3/pre.tar Makefile` (sha256 `f85b42c8…`). This replaces D44. |
| D48 | **Runner state and exit.** `lane_run.sh` sets `KR`, `VK`, `SPIKE`, `B="$KR/build/m0b23"` and `PKG` at the top, before any mode is chosen. It installs exactly one EXIT trap, at top level, before anything is started: the handler kills the spike server this run started (if any) and — once the launcher has been called — stops the lane through `stop_lane`, unless `LANE_KEEP=1`. That holds for every exit after the launcher call: 0, 5, 6, 7, 8. `preflight` sets no trap. In `run_one`, `rc` is the client's exit status. |
| D49 | **Dry boot.** `lane_run.sh --dry-boot <outdir>` runs the boot path with these values and no others changed: `GRANT=1`, lane `l0025`, `BOOTS=<outdir>/boots.txt`, `LANES=<outdir>/lanes` (D41 counts `$LANES/<lane>/boot-*.txt` newer than `build/m0b23/baseline.done`; real mode uses `~/Work/lanes`), `LAUNCH="$LR_STUBS/launch.sh"`, `SPOOL=<outdir>/spool`; `submit` runs `"$LR_STUBS/submit.sh" "$@"` and `stop_lane` runs `"$LR_STUBS/stop.sh"`. `<outdir>` and `$LR_STUBS` must resolve under `build/m0b23/` (else exit 3). All mode-dependent values are set before D41's check; no non-comment line after the line that prints `ledger mismatch` mentions `MODE` or `LR_STUBS`. Dry mode never reads the prompt's grant, never writes `build/m0b23/boots.txt` or anything under `~/Work/lanes/`, never runs `systemctl` or the real launcher. `preflight` runs for real (spool `<outdir>/spool`). |
| D50 | **Own files.** Everything this round writes — logs, extracted scripts, scratch — goes under `build/m0b23/`. Nothing under `~/Work/lanes/`, at any grant, except what the runner itself writes during a granted boot. |
| D53 | **Own files.** D50 holds with `build/m0b23/`: every log, extracted script, wrapper and scratch file this round writes is under `build/m0b23/`. Run the scripts from `$B/scripts/` and keep pass logs in `$B/logs/`. |
| D51 | *Replaced by D54.* |
| D52 | *Replaced by D56, which D67 replaces.* |
| D54 | *Replaced by D58.* |
| D55 | **Scripts and baseline.** Script X0 extracts every script of this prompt into `$B/scripts/`; run each as `bash "$B/scripts/<name>.sh"`, and nothing else in its place: no edited, copied, retyped or wrapped script, no extractor of your own. Run every script from one shell with one environment, locale included. Script 0 runs once; `build/m0b23/baseline.done`, `pre_files.txt` and `pre.tar` are never removed, rewritten or re-taken. Never restore a file from a `pre.tar` or any copy, by any means. S12c FAILs if a script's sha256 differs from the one it pins, or if `pre_files.txt` is not script 0's record of the pinned tree. |
| D56 | *Replaced by D67.* |
| D57 | **Running the runner.** Boot `n`'s runner is started detached, exactly as: `cd ~/Work/Kitty/repo && mkdir -p build/m0b23/logs && setsid nohup bash -c 'bash tests/aros/lane_run.sh l0025 build/m0b23/guest-boot$n; echo "RUNNER_EXIT=$?"' > build/m0b23/logs/runner-boot$n.log 2>&1 < /dev/null &` (D97). Never start one while another `lane_run.sh` is alive (`pgrep -f 'tests/aros/lane_run\.sh'`); poll its log at most once a minute until it has a `RUNNER_EXIT=` line; never signal it, the spike server or QEMU yourself. After the round's last boot, `cp -a build/m0b23/guest-boot$n build/m0b23/guest`, so S15 and S15b read the last boot. |
| D58 | **Supplied runner, corrected again.** `tests/aros/lane_run.sh` is the file script X2 installs (sha256 `2d182f1d28d37dc77a0a74b0f114da5c2fb5f55b2e443b86cd85084459af808f`, 452 lines), byte for byte: M0b.6's runner with this prompt's names, and (a) the prelude sent as its own command lines, one exec each, in file order, from `build/aros-pkg/prelude.aros`; a line whose answer has no `-> rc=0 (` prints `prelude failed at line <n>: <line>`, fetches `assigns.txt` and `avail_before.txt`, and exits 10 (the lane stops, D48); (b) each run sent as `Execute RAM:kitty/run_<name>.aros >RAM:kitty/out/sh_<name>.log`, and `sh_<name>.log` fetched with the run's files; (c) an exec whose answer contains `TIMEOUT: no result` prints `guest agent wedged at <what>; nothing more is sent` and exits 6, fetching nothing. Never edit, regenerate or replace it; never run a modified copy. A runner defect is the owner's: report it. Since M0b.12 also: (c) after each ladder run the run's log, rc and frames are fetched at once. |
| D59 | **Gates.** S14b.wiring accepts one `Execute` redirect: the runner's `Execute RAM:kitty/run_$r.aros >RAM:kitty/out/sh_$r.log` (D34's rule holds for every guest script), and FAILs a runner that `Execute`s the prelude. S14b.bootpath runs seven dry boots: `ok`, `running`, `prelude` (the second prelude line answers `rc=10`: exit 10, no later line, no `Execute`), `wedge` (`run_p` gets the client's `TIMEOUT`: exit 6, one run, nothing sent after it), `probe`, `launch`, `stray`; `ok` must send the prelude's four lines as four execs in order and all fourteen runs with their `sh_` redirect. S14b.pinned FAILs any change since script 0 other than X2's install (D60). S15b.output-captured FAILs when no run has an rc. S12c's pins are those of this prompt as dispatched. |
| D60 | *Replaced by D67.* |
| D61 | *M0b.8's fixes, applied and in the tree (`~/Work/Kitty/Docs/Development/m0b8_aros_audit.md`).* |
| D62 | *Replaced by D66.* |
| D63 | *Replaced by D67.* |
| D64 | **Supplied fixes** (owner, 2026-11-04). Script X3 applies them; you write none. `rv_wait_fence` replaces the three 1 s fence waits: it waits until the fence signals, logging each `VK_TIMEOUT`, and traps `fence not signalled in 30 s`. `vk_api_missing_device` finds the first device entry point not loaded; on a miss `common_init` logs `vk_entry_missing=<name>` and traps `device entry point not loaded` (headless skips the KHR entries). `t-vkresult` tests it. `run_a.aros` runs `kitty-debug`, so `a` leaves serial lines. Linux frames are unchanged. |
| D65 | **Runner, amended.** After the launcher it reads the serial path from the newest boot record. `RUNS` is now `h b1 od2 od4 lp0 lp1 w fifo a lp4 soak`. A run answering RUNNING is polled on its rc as before, up to 1100 s from its start (`soak`: 2600 s). A `core_IRQHandle(13)` or `(14)` on serial since the run began gives it 90 s more to write its rc (dry mode: 1 s). A run left with no rc goes in `<outdir>/dead.txt`. With three dead, later runs are not started (`lane_run: <n> dead, <r> not started`), which leaves the agent a slot for `Type` and `Avail`. Every run's serial lines go to `<outdir>/serial_<r>.txt`. The exit codes are unchanged; a ladder cut short this way exits 0. |
| D68 | *Done: the fixed loader is on DH1. Replaced by D69.* |
| D66 | **Gates.** `S14b.aros-fix` FAILs unless the eight files have X3's shas. `S14b.pinned` allows those eight and the runner, and nothing else. `S14b.bootpath` runs nine dry boots: D59's seven, `loaderbad` (D68) and `fault` (`w`, `fifo`, `a` answer RUNNING, each with a fault on serial: exit 0, three lines in `dead.txt`, at most 3 polls each, `lp4` and `soak` not started, the fault line in `serial_w.txt`); `ok` must send the runs in D65's order. `S15.stack`, `S15.timer-probe` and `S15.joystick` read `run_p.log`, which every ladder has. `S15.headless-vs-host` compares frames 0, 150 and 299. `S15b.output-captured` FAILs if any of the 14 runs has no rc, and names them. `S15b.wiring` accepts `DH1` or `L25DATA` (M0b.8). |
| D67 | **One boot, no edits** (grant 1 on l0025). Boot 1 runs only when S14, S14b and S16 are all PASS in the current pass, and runs the whole sequence. No file under `~/Work/Kitty/repo/` is edited after script 0 except by X2 and X3, before or after the boot; there is no second boot, whatever it shows. Script 12b's ledger must show one boot. D67 forbids edits, not reading: for every run with a trap or no rc, read its `serial_<r>.txt` and the source it points at, and report what failed as far as the evidence goes, marking any inference. |
| D69 | **Fifth round** (owner, 2026-11-04). **Fixes:** `r_set_force_fifo(bool)` now runs before init and `r_init_wsi` reads it before `create_swapchain`; the frozen `wsi.h` line is unchanged. `run_soak` runs `kitty-debug`, so its log also reaches serial, and has no `--music`, so stability is measured apart from audio. **Runner:** `RUNS` is `h b1 od2 od4 lp0 lp1 w fifo lp4 soak a`. A run with no rc gets `dead_<r>.ppm`, a monitor screendump; a failed screendump doesn't stop anything. The runner doesn't touch DH1. `MON` joins D49's values. **Gates:** `S14b.aros-fix` and `S14b.pinned` take X3's four files. New `S14b.force-fifo` (Linux debug build, `--force-fifo`: rc=0, `present_mode=FIFO`, no `IMMEDIATE`). `S14b.run-scripts` wants D69's soak line. `S15.clock` reads the soak: rc=0, `elapsed_us` from 1795 s up to the host wall. `S14b.bootpath` runs nine dry boots: `loaderbad` is gone, and `fault` now hits `w`, `fifo` and `lp4`, leaving `soak` and `a` unstarted. `S16.guest-staging` wants DH1's loader at `90237ed2…`. `S15b.loader-id` and `S15b.loader-on-dh1` stand. |
| D70 | **Sixth round** (owner, 2026-11-04). **Drivers:** script X4 reads eight files from `riaudio_dh0.img` into `build/m0b23/ahi/`, sha-checked. The runner puts them under `RAM:kitty/Devs/` after the package. `prelude.aros` gains `Assign DEVS: RAM:kitty/Devs` and `Assign DEVS: ADD AROS:Devs`, so six lines. They last for this boot only; no image is written. **Device:** the runner starts the launcher with `LANE_AUDIO=$OUT/lane_audio.wav`. **Runs:** `RUNS` is `h b1 od2 od4 lp0 lp1 w fifo lp4 a soak`. If `a` is dead, `soak` isn't started. `run_soak` gains `--music` again; `kitty-debug` stays. **Gates:** `S14b.aros-fix` and `S14b.pinned` take X3's two files. New `S16.audio`: the launcher has the knob, and DH0's eight files have this prompt's shas. `S15.soak` wants `ticks + catchup_dropped >= 90000` and `catchup_dropped <= 50` (1 s; M0b.10 measured 32). New `S15b.sound`: at least 176400 non-zero bytes in `lane_audio.wav`, 1 s at 44.1 kHz stereo. `S15b.wiring` also wants both `DEVS` lines. `S14b.bootpath` runs nine dry boots, adding `adead`. |
| D71 | **Seventh round** (owner, 2026-11-04). **Fix:** in `plat.cpp`, the AHI open tries `PROGDIR:Devs/ahi.device`, then `ahi.device`, and logs `audio_source=<name>` once one opens. On a system with no such file, the first open fails with `IOERR_OPENFAIL` and the second is what ran before. The runner sends a bare `CD` after the prelude, so `exec_cd.log` holds the guest agent's current directory. **Gates:** `S14b.aros-fix` and `S14b.pinned` take X3's one file. New `S15b.ahi-source`: `run_a.log` has `audio_source=PROGDIR:Devs/ahi.device`. `S15b.output-captured` prints `serial_<r>.txt`'s `core_IRQHandle` lines for each no-rc run. `S15b.sound`, `S15.ahi`, `S15.soak`, `S15.clock` and `S15.resident` read as D70 says: a dead `a` leaves no soak. |
| D72 | **Eighth round, logging** (owner, 2026-11-09). In `plat.cpp`'s AHI open: when the `PROGDIR:Devs/ahi.device` open fails, log `audio_progdir_error=<io_Error>` before the fallback; after a device opens and `audio_source=` is logged, log `audio_device_id="<the device's lib_IdString, up to the first quote or control character>" version=<lib_Version>.<lib_Revision>`. In the tree since M0b.13 (pre `e71b5850…`, post `9aede77e…`). No gate: the report quotes the lines (D73). |
| D73 | **Two boots, no edits** (grant 2 on l0025; replaces D67). Boot 1 runs only when S14, S14b and S16 are all PASS in the current pass, and runs the whole sequence. Boot 2 runs once boot 1's runner has exited, with nothing edited in between, and runs the same sequence; it is a repeat, not a retry with changes. The only reason not to start boot 2 is boot 1's runner exiting 3 or 9 (refused before the launcher): then stop and report. No file under `~/Work/Kitty/repo/` is edited after script 0 except by X2 and X3, before, between or after the boots. S15 and S15b read boot 2 (D57's copy of the last boot). Script 12b's ledger must show the boots that ran. D73 forbids edits, not reading: for every run with a trap or no rc, in either boot, read its `serial_<r>.txt` and the source it points at, and report what failed as far as the evidence goes, marking any inference. For each boot, quote every `audio_progdir_error=`, `audio_source=`, `audio_device_id=` and `audio_device=` line of `run_a.log` and `run_soak.log` (D72). |
| D74 | **Ninth round, AHI return log** (owner, 2026-11-09). In `plat.cpp`'s `pl_audio_queue`: a request that was sent and is now handed back counts in `returned`; the first with a non-zero `io_Error` logs `ahi_io_error=<io_Error> pass=<writes so far> tick=<tick>`, and every such request counts in `errors`. `pl_shutdown` logs, before the teardown and only if AHI opened, `ahi_summary submitted=<writes> returned=<n> errors=<n> first_error=<io_Error or 0> dropped=<frames with no free request> inflight=<requests not back>`. In the tree since M0b.14 (pre `9aede77e…`, post `60f9637e…`). No gate: the report quotes the lines. |
| D75 | **Runner: the listing last** (owner, 2026-11-09). The prelude loop sends each line as before, except a line starting `Assign >`, which it holds back; `prelude.aros` is unchanged, so the prelude is sent as five execs. After the step-7 fetch the runner sends, one exec each: `Type ENV:Sys/ahi.prefs HEX >RAM:kitty/out/ahi_prefs.txt` (answer to `exec_ahi_prefs.log`, file fetched), `List DEVS:AudioModes >RAM:kitty/out/audiomodes.txt` (answer to `exec_audiomodes.log`, file fetched), then the held listing (answer to `exec_listing.log`, `assigns.txt` fetched). A `TIMEOUT: no result` on any of the three prints `guest agent wedged at <what>; the run evidence is already home` and sends nothing more; on the full ladder that exits **11**. Exits 10 and 8 make the same reads after their own fetch and keep their code. Every other exit code and step is unchanged. **Gates:** `S14b.bootpath` runs ten dry boots: D69's nine and `listing` (the listing gets the client's `TIMEOUT`: exit 11, the wedge line, 14 runs, nothing sent after it, stop last); `ok` must send the listing once, after the last `sh_soak.log` fetch and after the two reads, and fetch `assigns.txt` after it. |
| D76 | **The better boot graded** (owner, 2026-11-09; amends D57 and D73). When the last runner has exited, count the `rc_*.txt` files in each `build/m0b23/guest-boot<n>/`. The graded boot is the one with more; on a tie, the later. Write `build/m0b23/graded_boot.txt` as `graded=<n> boot1_rc=<count> boot2_rc=<count>` (a boot that didn't run counts -1), then `cp -a build/m0b23/guest-boot<graded> build/m0b23/guest`. S15 and S15b read that copy. The other boot gets no gate lines; the report gives its files (final report item 5). Nothing about a boot but its `rc_*.txt` count enters the choice. |
| D77 | **Tenth round, AHI mode log** (owner, 2026-11-10). In `plat.cpp`, right after `audio_device=ahi.device unit=0`, the opened device is used as `AHIBase` (`<defines/ahi.h>`): for each id from `AHI_NextAudioID`, at most 32, `AHI_GetAudioAttrs` fills `AHIDB_Name` and `AHIDB_Driver` (`AHIDB_BufferLen` 64) and the game logs `ahi_mode id=0x<id> name="<name>" driver="<driver>"`; then `ahi_modes=<count>`; then `ahi_best id=0x<id> name="<name>"` from `AHI_BestAudioID(AHIDB_Realtime, TRUE)`. Control characters and `"` print as `?`. In the tree since M0b.15 (pre `60f9637e…`, post `a5a84cdc…`). No gate of its own: D82 reads its lines. |
| D78 | **Runner: a mode file in the first DEVS: directory** (owner, 2026-11-10; amends D66 and D70). After the AHI set is put, the runner sends `MakeDir RAM:kitty/Devs/AudioModes`, then `Copy AROS:Devs/AudioModes/HDAUDIO RAM:kitty/Devs/AudioModes/HDAUDIO`, one exec each, before the `List RAM:kitty` check. A copy that does not answer `rc=0` prints `lane_run: AudioModes copy failed: <answer>` and exits **7**, like a failed put, before any prelude line. No other mode file is copied, and nothing is written to an image. **Gates:** `S14b.bootpath` runs eleven dry boots: D75's ten and `modes` (the copy answers rc=20: exit 7, the failure line, no `Assign` and no `Execute` sent, stop last); `ok` must send the `MakeDir`, then the `Copy` once, after the last AHI put and before the first prelude line. |
| D79 | **The wiki in the repo** (owner, 2026-11-10). `llm-wiki/` is owner content, committed on its own and never edited by a round. Script 0 requires HEAD `3eef068c7a3efaaf7e2e760882807fd80c334d6a` and fingerprints `~/Work/Kitty/repo/llm-wiki/` with `Docs` and `AGENTS.md`; S12 requires the same HEAD, checks the same three paths, and leaves `llm-wiki` out of its tree diff; S1 allows `llm-wiki` at the top level. Script 0's tree hash never included it. |
| D80 | **Eleventh round, a probe before AHI opens** (owner, 2026-11-10). In `plat.cpp`'s `pl_audio_open`, after the requests are made and before the first `OpenDevice`, the game logs `ahi_probe db_before=<d> envarc_config=<c> envarc_err=<e> hdaudio_open=<h>`: `d` is 1 if `FindSemaphore("Audio Mode Database")` (under `Forbid`) finds it; `c` is 1 if `Lock("ENVARC:hdaudio.config", ACCESS_READ)` succeeds, and `e` is `IoErr()` when it does not, else 0; `h` is 1 if `OpenLibrary("DEVS:AHI/hdaudio.audio", 2)` succeeds (then closed at once). Each check is its own `noinline` function, so no library base stays live across a call (`r12=0`). In the tree since M0b.16 (pre `a5a84cdc…`, post `c47d9928…`). D83 extends it. |
| D81 | **Runner: the HDA config on ENVARC:** (owner, 2026-11-10; amends D66 and D70). Right after D78's copy, the runner sends `Copy AROS:Prefs/Env-Archive/hdaudio.config DH1:W/hdaudio.config`, one exec, before the `List RAM:kitty` check. A copy that does not answer `rc=0` prints `lane_run: hdaudio.config copy failed: <answer>` and exits **7**, before any prelude line; otherwise it prints `lane_run: copied AROS:Prefs/Env-Archive/hdaudio.config to DH1:W`. No other file is written to DH1, and no host script writes an image. **Gates:** `S14b.bootpath` runs twelve dry boots: D78's eleven and `envarc` (this copy answers rc=20: exit 7, the failure line, no `Assign` and no `Execute` sent, stop last); `ok` must send this copy once, after D78's `Copy` and before the first prelude line. `S16.commands` lists `Copy` already. |
| D82 | **Gates on the audio mode** (owner, 2026-11-10). `S16.audio` also FAILs if `tree-dh0.txt` has no `/Prefs/Env-Archive/hdaudio.config size=2182`; S16 prints whether `DH1:W/hdaudio.config` is already in `tree-dh1.txt` (no gate). Two new S15b gates, BLOCKED like the others when there is no guest result: `S15b.ahi-mode` PASSes only if the graded boot's `run_a.log` has an `ahi_mode … driver="hdaudio"` line and an `ahi_best … name="hdaudio:…"` line, and quotes its `ahi_probe` and `ahi_best` lines; `S15b.envarc` (BLOCKED while l0025 runs) reads `W/hdaudio.config` from `aros_l0025_dh1.img` through `sfs_nav.py` and PASSes only if its first 2182 bytes hash to DH0's `3645c2cb…`. |
| D83 | **Twelfth round, a probe of the HIDD path** (owner, 2026-11-10; extends D80). In `plat.cpp`'s `ahi_probe`, D80's checks run as before, with three more around the driver open, in this order: database, config, `Lock("DRIVERS:hdaudio.hidd", ACCESS_READ)`, `Lock("DRIVERS:")` then `NameFromLock` into a 96-byte buffer, `FindName(&SysBase->LibList, "hdaudio.hidd")` under `Forbid`, D80's `OpenLibrary("DEVS:AHI/hdaudio.audio", 2)` (closed at once), the same `FindName` again. D80's line is logged unchanged, then `ahi_drivers hidd_lock=<0|1> hidd_err=<e> dir_err=<e> dir="<path>" hidd_before=<0|1> hidd_after=<0|1>` (`e` is `IoErr()` on failure, else 0; `dir` is empty when the lock or the name fails). Each check is its own `noinline` function (`r12=0`). Already in the tree; X3 checks it (pre `c47d9928…`, post `69d7a7ae…`). |
| D84 | **Withdrawn** (replaced by D87). M0b.17's `Assign LIBS: DH1:Libs PREPEND` prelude. |
| D85 | **Replaced by D88.** |
| D86 | **Thirteenth round, the HDA HIDD preloaded** (owner, 2026-11-10). In `plat.cpp`, `pl_audio_open` first calls `OpenLibrary("SYS:Devs/Drivers/hdaudio.hidd", 0)` once, keeps the base in `g_ahi_hidd`, and logs `ahi_hidd_preload=<0|1>`; a failed open is logged and ignored. Then D80's and D83's probe lines, then the AHI open as before. `ahi_release` closes the HIDD last, after `CloseDevice`, and also on its early return. The preload and the close are each their own `noinline` function (`r12=0`). In the tree since M0b.18 (pre `69d7a7ae…`, post `0b7a52f6…`). |
| D87 | **M0b.16's wiring back** (owner, 2026-11-10; replaces D84). `tests/aros/prelude.aros` is D70's six lines again, byte for byte M0b.16's (`be92bf86…`): `Assign ENVARC: DH1:W`, `Assign LIBS: DH1:Libs`, `Assign LIBS: ADD AROS:Libs`, `Assign DEVS: RAM:kitty/Devs`, `Assign DEVS: ADD AROS:Devs`, `Assign >RAM:kitty/out/assigns.txt`; the runner sends it as five execs and the listing last (D75). In the tree since M0b.18 (pre `fdd64026…`, post `be92bf86…`). **Gates:** as in M0b.16: `S14b.run-scripts` wants these six lines exactly; `S14b.bootpath`'s `prelude` dry boot fails line 2, `Assign LIBS: DH1:Libs`, and wants no `Assign LIBS: ADD` sent after it. `S14b.aros-fix` and `S14b.pinned` take X3's two files. |
| D88 | **Gate on the preload** (owner, 2026-11-10; replaces D85). `S15b.drivers`, BLOCKED like the others when there is no guest result: PASSes only if the graded boot's `run_a.log` has `ahi_hidd_preload=1` and an `ahi_drivers` line with `hidd_before=1`; it quotes both and the `DRIVERS` line of `assigns.txt`. |
| D89 | **Fourteenth round, two AHI requests in flight** (owner, 2026-11-10). In `plat.cpp`, `kAhiDepth` is 2. `pl_audio_queue` returns at once, counting the frames in `dropped`, while `ahi_inflight()` is 2 or more; otherwise it sends as before, linked to the last request sent if that one is still in flight. `pl_audio_queued_frames` returns `4 * kAhiFrames` (3840) while two are in flight, else `kAhiFrames * ahi_inflight()`. The four request slots, the buffers, the xrun and error logs and `ahi_summary` are unchanged. In the tree since M0b.19 (pre `0b7a52f6…`, post `fabf51a2…`). **Gates:** unchanged. |
| D90 | **Fifteenth round, the mixer paced by the device** (owner, 2026-11-11). In `src/audio/audio.cpp`, `aud_tick` renders 960 frames and calls `pl_audio_queue` in a loop while `pl_audio_queued_frames()` is below 3840, at most four times per call; nothing else in the engine changes. In `plat.cpp`, `pl_audio_open` records `pl_now_us` once AHI is open, and `ahi_xrun` and `ahi_io_error=` lines end `us=<pl_now_us minus that>` in place of `tick=`. In the tree since M0b.20 (pre `18076e83…` and `fabf51a2…`, post `9ddd9eff…` and `afd2b367…`). **Gates:** `S15.ahi` is redefined. It is BLOCKED as before if AHI didn't open; otherwise it reads `run_a.log` and `run_soak.log` and PASSes only if `run_a.log` has `audio_device=ahi.device unit=0` and `music=on`; its `ahi_summary` has `errors=0` and `returned` at least `submitted` − 2; `audio_frames_queued` is at least 0.98 × 48000 × `elapsed_us` / 10⁶ and at most 48000 × (the run's host wall + 1 s) + 1920; `run_a.log` has at most 4 `ahi_xrun` lines and `run_soak.log` at most 30. It prints all of these values. `S9.sdl-stream` is unchanged. |
| D91 | **Sixteenth round, 40 ms AHI requests** (owner, 2026-11-12). In `plat.cpp`, `kAhiFrames` is 1920 and `kAhiBytes` 7680; `kAhiDepth` stays 2 and the four request slots stay. `pl_audio_queue` keeps one request being filled (`g_ahi_stage`, `g_ahi_staged`): when none is, it takes a free slot under the same rules as before (frames counted in `dropped` while two are in flight or no slot is free), collecting a returned request's `returned` and error; it then copies the chunk in, counting frames beyond 1920 in `dropped`, and adds the copied frames to `audio_frames_queued`. Only a full request is sent, linked as before; an `ahi_xrun` line is logged when a full request is sent with none in flight. `pl_audio_queued_frames` returns 3840 at two in flight, else 1920 per request in flight plus the frames gathered. Shutdown clears the request being filled. `audio.cpp` and every log line are unchanged. In the tree since M0b.21 (pre `afd2b367…`, post `e9078a47…`). **Gates:** `S15.ahi` as amended by D93. |
| D92 | **The grading tiebreak** (owner, 2026-11-12; amends D76). When the last runner has exited, count in each `build/m0b23/guest-boot<n>/` the `rc_*.txt` files and all entries (`ls -A`). The graded boot is the one with more `rc_*.txt` files; on a tie, the one with more entries; on a tie in both, the later. Write `build/m0b23/graded_boot.txt` as `graded=<n> boot1_rc=<count> boot2_rc=<count> boot1_files=<count> boot2_files=<count>` (a boot that didn't run counts -1 in both), then `cp -a build/m0b23/guest-boot<graded> build/m0b23/guest`. Nothing else about a boot enters the choice: not its runner's exit code and not any gate. |
| D93 | **Seventeenth round, the audio-time bound** (owner, 2026-11-12). In `plat.cpp`, `pl_audio_queue` records, just before the first `SendIO` of a run, `pl_now_us() − p.t0_us` (the base `elapsed_us=` uses) in `g_ahi_first_us`; `ahi_log_summary` logs `ahi_first_submit_us=<n>` on its own line right after `ahi_summary`. No other line, value or behaviour changes. Already in the tree; X3 checks it (pre `e9078a47…`, post `d5c6b2e6…`). **Gates:** `S14b.aros-fix` takes X3's file. `S15.ahi` reads `ahi_first_submit_us` (`fs`) from `run_a.log`; it must be present and above 0, and the queued bound becomes 0.98 × 48000 × (`elapsed_us` − `fs`) / 10⁶ ≤ queued ≤ 48000 × (host wall + 1) + 3840 (the upper end allows two 1920-frame requests in flight). The `ahi_summary` and xrun conditions are unchanged; the gate line adds `first_submit_us=<fs>` after `elapsed_us=`. |
| D94 | **The census is append-only** (owner, 2026-11-12; amends D46). The header defines `cj` and, in every script that carries it, appends to `build/m0b23/census.chain` one line when the script starts and one when it exits: `<epoch> <script> start|end <lines> <sha256>`, where `<lines>` and `<sha256>` are `census.txt`'s line count and hash at that moment. `census.txt` is written only by the scripts (`g`, script 1's truncation and S15's and S15b's own appends); `census.chain` only by the header. Never edit, truncate, reorder or delete a line of either, by any means and for any reason. An edit is a D73 breach, and it ends the round. **Gate:** `S12c.complete` also FAILs when, from the chain's last `s1.sh start` on: the pass scripts (1–11, 13, 14, 14b, 16, 15, 15b, 12, 12b, 12c) don't each appear once, in order, as start then end; the first `<lines>` lines of `census.txt` don't hash to an entry's `<sha256>`; `census.txt` has a blank line; or `census.txt` has grown since `s12c.sh start`. A FAIL from S12c stands: report it; never repair the census to make S12c pass. |
| D95 | **Detached scripts, once per pass** (owner, 2026-11-12; amends D46). M0b.22's script 8 was cut off by a 120 s tool timeout and then run again, which voided its pass. Every script but X0 now runs detached, in the form given under "Acceptance scripts", with its output in `build/m0b23/logs/<name>.out` and a last line `SCRIPT <name> exit=<n>`. Start the next script only after that line appears. A script is started once per pass. If a script's output has no `SCRIPT … exit=` line yet, it is still running: wait; never start it again, and never kill it. If you can't tell whether a script ran, read `census.chain`; never start it a second time to find out. If anything cuts a script off before boot 1, its pass is void: start a new pass at X0 (the loop rule). After boot 1, nothing is re-run; report what exists. |
| D96 | **Script 0 failing stops the round; git is read-only** (owner, 2026-11-12). If script 0 exits non-zero, stop. Write `build/m0b23/report.md` with script 0's output in item 2 and "stopped at script 0 (D96)" in item 8, and run nothing else. Never change HEAD, the index, a branch, a file or the environment to make script 0 pass. M0b.22's agent ran `git reset --soft` when script 0 printed `HEAD is not b5f929d`, which the GIT line forbids. A git command outside the GIT line's list is a departure (item 8), and it ends the round. |
| D97 | **The runner's exit code is recorded** (owner, 2026-11-12; amends D57). The launch line wraps the runner as `bash -c 'bash tests/aros/lane_run.sh l0025 build/m0b23/guest-boot<n>; echo "RUNNER_EXIT=$?"'`, so its log ends with `RUNNER_EXIT=<n>`. A boot's runner has exited when that line appears. Its value is the runner's exit code, as quoted in item 5. `lane_run: lane stopped` is printed at every exit code, so it says nothing about the code. |
| D38 | **Report wording.** The guest is "the QEMU/KVM guest on lane l0025", never "hardware". A behaviour counts as proven on the guest only if a guest log line shows that code path ran (a run that trapped in `pl_init` proves nothing about windows, surfaces or audio). |

## Work items

**W0 — scripts X0, X2 and X3** extract this prompt's scripts (D55), install the corrected runner
(D58) and check the fix (D93). **W1 — the host pass** (0, X1, X2, X3, X, 1–11, 13, 14, 14b, 16): every gate PASS except the
expected BLOCKED; a FAIL stops the round before the boot. **W2 — boots 1 and 2** per D57/D73/D76, then S15,
S15b, 12, 12b, 12c. **W3 — the report.**

## Acceptance scripts (run order: X0, 0, X1, X2, X3, X, 1–11, 13, 14, 14b, 16, [runner], 15, 15b, 12, 12b, 12c; each output in `build/m0b23/logs/<name>.out`, D95)

This block heads every script: X0 and X1 write it into the scripts they extract. You paste it once,
above script X0:

````bash
# script hdr
KD=~/Work/Kitty/Docs/Development; DOC=$KD/kitty_engine_design.md; PR=$KD/dispatch/m0b23_aros_guest_opencode_prompt.md
VK=~/Work/projects/Vulkan4Aros; KR=~/Work/Kitty/repo; B=$KR/build/m0b23
LVP_PIN=~/Work/host_lavapipe_kitty/lvp_icd.json; LVP_SYS=/usr/share/vulkan/icd.d/lvp_icd.json
g(){ echo "GATE $1 $2${3:+ — $3}"; echo "GATE $1 $2${3:+ — $3}" >> "$B/census.txt"; }
icd(){ if [ -f "$LVP_PIN" ] && VK_DRIVER_FILES=$LVP_PIN vulkaninfo --summary 2>/dev/null | grep -q 'driverInfo *= *Mesa 26\.2\.2'; then echo "$LVP_PIN"; else echo "$LVP_SYS"; fi; }
GRANT=$(sed -n 's/^ *BOOTS GRANTED: *\([0-9][0-9]*\) *$/\1/p' "$PR" | head -1); LANE=$(sed -n 's/^ *LANE: *\([a-z0-9]*\) *$/\1/p' "$PR" | head -1)
aenv(){ AROS_ABI=v1 . "$VK/scripts/aros_build_env.sh" >/dev/null 2>&1 || return 1; export AROS_SYSROOT="${AROS_SDK_INCLUDE%/include}" PATH="$AROS_COLLECT_AROS_DIR:$AROS_TOOLCHAIN/bin:$PATH"; }
cj(){ [ -d "$B" ] || return 0; echo "$(date +%s) ${0##*/} $1 $(cat "$B/census.txt" 2>/dev/null | wc -l) $(cat "$B/census.txt" 2>/dev/null | sha256sum | cut -c1-64)" >> "$B/census.chain"; }
cj start; trap 'cj end' EXIT
````

**Script X0: extract this prompt's scripts (D55). Run it first, before script 0, and again before
every pass.** Paste the header above and this block; it is the only script you paste, and the only
one you run directly.

**Every other script runs detached (D95).** Start script `<name>` with exactly this command:

```bash
cd ~/Work/Kitty/repo && mkdir -p build/m0b23/logs && setsid nohup bash -c 'bash "build/m0b23/scripts/$0.sh" > "build/m0b23/logs/$0.out" 2>&1; echo "SCRIPT $0 exit=$?" >> "build/m0b23/logs/$0.out"' <name> < /dev/null > /dev/null 2>&1 &
```

Then wait with this command, as many times as it takes, until it prints the `SCRIPT <name> exit=`
line (each call returns within 100 s):

```bash
cd ~/Work/Kitty/repo && timeout 90 bash -c 'until grep -q "^SCRIPT $0 exit=" "build/m0b23/logs/$0.out" 2>/dev/null; do sleep 5; done' <name>; tail -n 1 build/m0b23/logs/<name>.out
```

Then read `build/m0b23/logs/<name>.out` in full with the file tool before starting the next script.
That file is the script's output. Never start a script whose last start has no `SCRIPT … exit=`
line yet.

````bash
# script x0
mkdir -p "$B/scripts"; python3 -I - "$PR" "$B/scripts" <<'PY' || { echo "X0 FAIL"; exit 1; }
import sys,re
p,out=sys.argv[1:]; bl=[]; cur=None
for l in open(p).read().split('\n'):
    if l.startswith('````bash'): cur=[]; continue
    if l.startswith('````') and cur is not None: bl.append(cur); cur=None; continue
    if cur is not None: cur.append(l)
named={}
for b in bl:
    m=re.fullmatch(r'# script ([a-z0-9]+)',b[0]) if b else None
    if not m: sys.exit('a bash block has no "# script <name>" first line')
    if m[1] in named: sys.exit(f'script {m[1]} appears twice')
    named[m[1]]='\n'.join(b[1:])+'\n'
want={'hdr','x0','s0','x1','x2','x3','x4','s14b','s16','s15b','s12b','s12c'}
if set(named)!=want: sys.exit(f'scripts {sorted(named)}; want {sorted(want)}')
hdr=named.pop('hdr'); open(f'{out}/hdr.sh','w').write(hdr)
for n,s in sorted(named.items()): open(f'{out}/{n}.sh','w').write(hdr+s)
print('extracted '+' '.join(sorted(named))+' from this prompt')
PY
````

**Script 0: baseline.** `bash "$B/scripts/s0.sh"`, after X0, before anything else, exactly once.

````bash
# script s0
cd "$KR" || exit 1; mkdir -p "$B"; [ -e "$B/baseline.done" ] && { echo "BASELINE EXISTS — not re-taken"; exit 1; }
for e in m0a m0a1; do [ -e "$KR/build/$e/baseline.done" ] || { echo "build/$e evidence missing — wrong tree"; exit 1; }; done
[ -s "$KR/build/m0b/report.md" ] && [ "$(grep -c . "$KR/build/m0b/boots.txt")" = 2 ] || { echo "build/m0b evidence missing — wrong tree"; exit 1; }
[ -s "$KR/build/m0b1/report.md" ] && [ "$(grep -c . "$KR/build/m0b1/boots.txt")" = 1 ] || { echo "build/m0b1 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b2/report.md" ] && [ -f "$KR/build/m0b2/pre.tar" ] && [ "$(grep -c . "$KR/build/m0b2/boots.txt")" = 0 ] || { echo "build/m0b2 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b3/report.md" ] && [ -f "$KR/build/m0b3/pre.tar" ] && [ "$(grep -c . "$KR/build/m0b3/boots.txt")" = 0 ] || { echo "build/m0b3 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b4/report.md" ] && [ -f "$KR/build/m0b4/pre.tar" ] && [ "$(grep -c . "$KR/build/m0b4/boots.txt")" = 0 ] || { echo "build/m0b4 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b5/report.md" ] && [ -f "$KR/build/m0b5/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b5/boots.txt")" = 0 ] || { echo "build/m0b5 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b6/report.md" ] && [ -f "$KR/build/m0b6/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b6/boots.txt")" = 1 ] || { echo "build/m0b6 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b7/report.md" ] && [ -f "$KR/build/m0b7/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b7/boots.txt")" = 1 ] || { echo "build/m0b7 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b8/report.md" ] && [ -f "$KR/build/m0b8/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b8/boots.txt")" = 1 ] || { echo "build/m0b8 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b9/report.md" ] && [ -f "$KR/build/m0b9/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b9/boots.txt")" = 1 ] || { echo "build/m0b9 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b10/report.md" ] && [ -f "$KR/build/m0b10/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b10/boots.txt")" = 1 ] || { echo "build/m0b10 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b11/report.md" ] && [ -f "$KR/build/m0b11/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b11/boots.txt")" = 1 ] || { echo "build/m0b11 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b12/report.md" ] && [ -f "$KR/build/m0b12/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b12/boots.txt")" = 0 ] || { echo "build/m0b12 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b13/report.md" ] && [ -f "$KR/build/m0b13/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b13/boots.txt")" = 2 ] || { echo "build/m0b13 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b14/report.md" ] && [ -f "$KR/build/m0b14/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b14/boots.txt")" = 2 ] || { echo "build/m0b14 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b15/report.md" ] && [ -f "$KR/build/m0b15/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b15/boots.txt")" = 2 ] || { echo "build/m0b15 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b16/report.md" ] && [ -f "$KR/build/m0b16/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b16/boots.txt")" = 2 ] || { echo "build/m0b16 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b17/report.md" ] && [ -f "$KR/build/m0b17/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b17/boots.txt")" = 2 ] || { echo "build/m0b17 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b18/report.md" ] && [ -f "$KR/build/m0b18/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b18/boots.txt")" = 2 ] || { echo "build/m0b18 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b19/report.md" ] && [ -f "$KR/build/m0b19/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b19/boots.txt")" = 2 ] || { echo "build/m0b19 evidence missing or changed — wrong tree"; exit 1; }
[ -f "$KR/build/m0b20/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b20/boots.txt")" = 2 ] || { echo "build/m0b20 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b21/report.md" ] && [ -f "$KR/build/m0b21/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b21/boots.txt")" = 2 ] || { echo "build/m0b21 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b22/report.md" ] && [ -f "$KR/build/m0b22/census.sealed" ] && [ "$(grep -c . "$KR/build/m0b22/boots.txt")" = 2 ] || { echo "build/m0b22 evidence missing or changed — wrong tree"; exit 1; }
[ "$(git rev-parse HEAD)" = 3eef068c7a3efaaf7e2e760882807fd80c334d6a ] || { echo "HEAD is not 3eef068"; exit 1; }
( find .gitignore Makefile src tools shaders tests -type f -print0 | LC_ALL=C sort -z | xargs -0 sha256sum ) > "$B/pre_files.txt"
th=$(sha256sum < "$B/pre_files.txt" | cut -d' ' -f1); echo "tree hash $th ($(wc -l < "$B/pre_files.txt") files)"
[ "$th" = 37779fc7dad798f69246290dd5344fe8fb93f1eca1f599225c9cd3bfe419420e ] || { echo "tree differs from the tree M0b.22 left — stop and report"; rm -f "$B/pre_files.txt"; exit 1; }
tar -cf "$B/pre.tar" .gitignore Makefile src tools shaders tests
git branch --list --format='%(refname:short) %(objectname)' > "$B/branches_start.txt"
git -C "$VK" rev-parse HEAD > "$B/vk_head.txt"
git -C "$VK" status --porcelain -- 'src/abi/v1/AROS*' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal > "$B/vk_prot_start.txt"
( cd ~/Work/Kitty && find Docs repo/llm-wiki AGENTS.md -type f -print0 | sort -z | xargs -0 sha256sum ) > "$B/protected_start.txt"
( cd ~/Work/Kitty && find Docs repo/llm-wiki AGENTS.md -type f | sort ) > "$B/protected_list_start.txt"
pgrep -f '^[^ ]*qemu-system' | sort > "$B/qemu_start.txt"; : > "$B/boots.txt"
ss -ltnpH 2>/dev/null | sort > "$B/listen_start.txt"; ls ~/Work/lanes/*/boot-*.txt 2>/dev/null | sort > "$B/lane_boots_start.txt"
date +%s > "$B/baseline.done"; : > "$B/census.txt"
echo "GRANT=$GRANT LANE=$LANE"; [ -n "$GRANT" ] && [ -n "$LANE" ] || { echo "authorization block unreadable"; exit 1; }
echo "VK HEAD $(cat "$B/vk_head.txt")"; echo "VK protected status lines: $(wc -l < "$B/vk_prot_start.txt")"; echo "protected Kitty files: $(wc -l < "$B/protected_start.txt")"; echo "qemu processes at start: $(wc -l < "$B/qemu_start.txt")"; echo "listening sockets at start: $(wc -l < "$B/listen_start.txt")"; echo "lane boot records at start: $(wc -l < "$B/lane_boots_start.txt")"
````

**Script X1: extract M0b's script X, 14, 15 and 12 (verbatim, with the listed patches).** `bash "$B/scripts/x1.sh"`, after
script 0 and again at the start of every pass; then run `bash "$B/scripts/sX.sh"` (M0b's
script X, which extracts the Linux scripts 1–11 and 13 into `$B/scripts/`).

````bash
# script x1
mkdir -p "$B/scripts"; python3 -I - "$KD/dispatch/m0b_aros_opencode_prompt.md" "$B/scripts" <<'PY' || { echo "EXTRACT FAIL"; exit 1; }
import sys,hashlib
p,out=sys.argv[1:]
raw=open(p,'rb').read()
if hashlib.sha256(raw).hexdigest()!='a46dcf4846f2d2dfc198e5df5ccbc95bd29ddd94d40cce6e3e7c53e53ab7269b': sys.exit(f'{p} changed since review')
bl=[]; cur=None
for l in raw.decode().split('\n'):
    if l.startswith('````bash'): cur=[]; continue
    if l.startswith('````') and cur is not None: bl.append('\n'.join(cur)); cur=None; continue
    if cur is not None: cur.append(l)
if len(bl)!=6: sys.exit(f'{len(bl)} bash blocks, want 6')
def rep(s,x,y,n=1):
    if s.count(x)!=n: sys.exit(f'patch target found {s.count(x)}x (want {n}): {x!r}')
    return s.replace(x,y)
hdr=rep(bl[0]+'\n','PR=$KD/dispatch/m0b_aros_opencode_prompt.md','PR=$KD/dispatch/m0b23_aros_guest_opencode_prompt.md')
hdr=rep(hdr,'B=$KR/build/m0b\n','B=$KR/build/m0b23\n')
CJ=r'''cj(){ [ -d "$B" ] || return 0; echo "$(date +%s) ${0##*/} $1 $(cat "$B/census.txt" 2>/dev/null | wc -l) $(cat "$B/census.txt" 2>/dev/null | sha256sum | cut -c1-64)" >> "$B/census.chain"; }
cj start; trap 'cj end' EXIT
'''
hdr+=CJ
sx=rep(bl[2],"'B=$KR/build/m0a\\n','B=$KR/build/m0b\\n'","'B=$KR/build/m0a\\n','B=$KR/build/m0b23\\n'")
sx=rep(sx,'S[3]=rep(S[3],','S[1]=rep(S[1],"|tests|build\' || {","|tests|build|llm-wiki\' || {")\nS[3]=rep(S[3],')
sx=rep(sx,'6 patches applied','7 patches applied')
sx=rep(sx,"'B=$KR/build/m0b23\\n')\n","'B=$KR/build/m0b23\\n')+"+repr(CJ)+"\n")
s14=rep(bl[3],'grep -q "out/run_$r.log" "$S" 2>/dev/null && grep -q "out/rc_$r.txt" "$S" 2>/dev/null','grep -q "out/run_$r.log" "$K/run_$r.aros" 2>/dev/null && grep -q "out/rc_$r.txt" "$K/run_$r.aros" 2>/dev/null')
s12=rep(bl[5],'git -C "$VK" status --porcelain -- \'src/abi/v1/AROS*\' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal | diff "$B/vk_prot_start.txt" - || { echo "VK protected paths changed in the working tree"; f=1; }',
 'vd=$(git -C "$VK" status --porcelain -- \'src/abi/v1/AROS*\' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal | diff "$B/vk_prot_start.txt" - | grep \'^[<>]\'); [ -z "$vd" ] || { echo "INFO VK protected paths changed in the working tree (the owner works there in parallel) — listed for attribution:"; echo "$vd"; }\n'
 'vs=$(cd "$VK" && { git ls-files --others --exclude-standard -- scripts; git diff --name-only HEAD -- scripts; } | sort -u | while read -r p; do [ "$p" -nt "$B/baseline.done" ] && echo "$p"; done); [ -z "$vs" ] || { echo "untracked or uncommitted files under VK scripts/ newer than script 0 (committed owner work is not listed):"; echo "$vs"; f=1; }\n'
 'nl=$(ss -ltnpH 2>/dev/null | sort | comm -13 "$B/listen_start.txt" - | grep -E \':(9099|9100|4479)[[:space:]]\'); [ -z "$nl" ] || { echo "listeners on the lane ports 9099/9100/4479 opened during the run and still open:"; echo "$nl"; f=1; }')
s14=rep(s14,'else out=$(bash tests/aros/lane_run.sh nvk','else n0=$(grep -c . "$B/boots.txt"); out=$(bash tests/aros/lane_run.sh nvk')
s14=rep(s14,'&& [ "$(grep -c . "$B/boots.txt")" = 0 ]; } && g S14.runner-refuses PASS','&& [ "$(grep -c . "$B/boots.txt")" = "$n0" ]; } && g S14.runner-refuses PASS')
s12=rep(s12,'echo "--- files changed vs 38fc422"','echo "--- files changed vs the reviewed M0b tree"')
s12=rep(s12,'sed "s|$B/pre|38fc422|"','sed "s|$B/pre|m0b-tree|"')
s12=rep(s12,'-x LICENSE -x README.md | sed','-x LICENSE -x README.md -x llm-wiki | sed')
s12=rep(s12,'= 38fc422d388c9886a38045ff5dc665dab369697d ] || { echo "HEAD moved"','= 3eef068c7a3efaaf7e2e760882807fd80c334d6a ] || { echo "HEAD moved"')
s12=rep(s12,'find Docs llm-wiki AGENTS.md -type f | sort','find Docs repo/llm-wiki AGENTS.md -type f | sort')
s15=rep(bl[4],"d=int(kv(w,'delta_us') or 0)","d=int(kv(log('p'),'delta_us') or 0)")
s15=rep(s15,"st=int(kv(w,'stack_bytes') or 0)","st=int(kv(log('p'),'stack_bytes') or 0)")
s15=rep(s15,"state=0x[0-9a-f]{8}$',w,re.M)","state=0x[0-9a-f]{8}$',log('p'),re.M)")
s15=rep(s15,"'joy lowlevel=none' in w","'joy lowlevel=none' in log('p')")
s15=rep(s15,"""if os.path.exists(hd) and os.path.exists(host):
    a=open(hd,'rb').read(); b=open(host,'rb').read()
    m=sum(""","""FR=[(f'{G}/h/frame_{n}.bgra',host.replace('frame_150',f'frame_{n}')) for n in ('000','150','299')]
if all(os.path.exists(x) for p_ in FR for x in p_):
    m=0
    for hp,bp in FR:
      a=open(hp,'rb').read(); b=open(bp,'rb').read()
      m+=sum(""")
s15=rep(s15,"el=int(kv(aa,'elapsed_us') or 0); wa=W.get('a',(0,0))","el=int(kv(log('soak'),'elapsed_us') or 0); wa=W.get('soak',(0,0))")
s15=rep(s15,"rc('a')==0 and 9_500_000<=el<=","rc('soak')==0 and 1_795_000_000<=el<=")
s15=rep(s15,"int(kv(so,'ticks') or 0)>=90000 and sw[1]-sw[0]>=1800, f\"rc={rc('soak')} ticks={kv(so,'ticks')} wall","int(kv(so,'ticks') or 0)+int(kv(so,'catchup_dropped') or 0)>=90000 and int(kv(so,'catchup_dropped') or 0)<=50 and sw[1]-sw[0]>=1800, f\"rc={rc('soak')} ticks={kv(so,'ticks')} catchup_dropped={kv(so,'catchup_dropped')} wall")
s15=rep(s15,"f'mismatched_px={m} (±4, coverage)'","f'mismatched_px={m} over 3 frames (±4, coverage)'")
s15=rep(s15,"""    q=int(kv(aa,'audio_frames_queued') or 0); gate('ahi', 'audio_device=ahi.device unit=0' in aa and 'music=on' in aa and 470400<=q<=489600, f'queued={q} xruns={aa.count("ahi_xrun")}')""","""    q=int(kv(aa,'audio_frames_queued') or 0); ea=int(kv(aa,'elapsed_us') or 0); fs=int(kv(aa,'ahi_first_submit_us') or 0); wq=W.get('a',(0,0)); hw=wq[1]-wq[0]; so=log('soak')
    sm=re.findall(r'^ahi_summary submitted=(\\d+) returned=(\\d+) errors=(\\d+)',aa,re.M); xa=aa.count('ahi_xrun'); xs=so.count('ahi_xrun')
    gate('ahi', 'audio_device=ahi.device unit=0' in aa and 'music=on' in aa and bool(sm) and sm[0][2]=='0' and int(sm[0][1])+2>=int(sm[0][0]) and 0<fs<ea and 0.98*48000*(ea-fs)/1e6<=q<=48000*(hw+1)+3840 and xa<=4 and xs<=30, f'queued={q} elapsed_us={ea} first_submit_us={fs} host_wall={hw}s summary={sm[0] if sm else None} xruns_a={xa} xruns_soak={xs}')""")
open(f'{out}/hdr.sh','w').write(hdr)
for n,s in (('sX',sx),('s14',s14),('s15',s15),('s12',s12)): open(f'{out}/{n}.sh','w').write(hdr+s+'\n')
print('extracted sX, s14, s15, s12 from the M0b prompt; 27 patches applied')
PY
````

**Script X2: install the supplied runner (D58).** `bash "$B/scripts/x2.sh"`, after X1, every pass.

````bash
# script x2
cd "$KR" || exit 1; mkdir -p tests/aros
cat > tests/aros/lane_run.sh <<'RUNNER'
#!/bin/bash
# tests/aros/lane_run.sh <lane> <outdir>
# tests/aros/lane_run.sh --check-transport <outdir>
# tests/aros/lane_run.sh --dry-boot <outdir>   (LR_STUBS=<dir under build/m0b23>)
#
# The only host-side door to a guest boot in this round (W1). Steps, in order:
#
#   1. D35: resolve <outdir> with `realpath -m` and refuse (exit 3,
#      "outdir outside build/m0b23") unless it lies under $KR/build/m0b23/.
#      Nothing is created before this check passes.
#   2. Read BOOTS GRANTED / LANE from
#      Docs/Development/dispatch/m0b23_aros_guest_opencode_prompt.md and count
#      the lines already in build/m0b23/boots.txt; refuse (exit 3, "no boots
#      granted") on a wrong lane, no grant, or an exhausted grant. This
#      refusal comes before the boots.txt write, so a refused call never
#      consumes a boot.
#   3. D41: before the ledger line, count the lane's boot records newer than
#      build/m0b23/baseline.done; if count != boots.txt lines, refuse (exit 3,
#      "ledger mismatch").
#   4. preflight: D40 (b) port check, (d) lane running check, then start the
#      server, then (c) liveness check. One function, used by both runner modes.
#      Write one boots.txt line, then boot through the lane's own launcher.
#   5. Make every directory of build/aros-pkg/ under RAM:kitty, then deploy
#      it file by file with one --put each (absolute paths), verify with
#      `List RAM:kitty`, make the out/ tree, then send each command line of
#      prelude.aros as its own exec (D58); a line that does not answer rc=0
#      prints "prelude failed at line <n>", fetches what exists, stops the
#      lane and exits 10. Then the D33 probes p -> pt -> ptr, each as
#      `Execute RAM:kitty/run_<name>.aros >RAM:kitty/out/sh_<name>.log`.
#      An exec that gets no answer at all (the client's TIMEOUT) means the
#      agent is wedged: print "guest agent wedged at <what>", send nothing
#      more, stop the lane and exit 6.
#   6. Only when the probe passed (rc_p.txt is rc=0 and run_p.log has a
#      vk_loader= line) run the eleven M0b runs; otherwise print
#      "probe failed; ladder not run", fetch what exists, stop the lane and
#      exit 8.
#   7. Fetch RAM:kitty/out into <outdir> and write <outdir>/host_wall.txt as
#      "<name> start=<epoch s> end=<epoch s>" per run. On RUNNING (the guest
#      agent answers at its 900 s exec budget) poll `Type` on the run's own
#      rc_<name>.txt (the image has no Exists command); that file is the
#      authority and the host wall ends when it appears (D36). A run with no
#      rc goes in dead.txt, sooner if it faulted on serial; three dead start
#      no more runs (D65).
#   8. D75: the prelude's `Assign >…` listing is held back from step 5 and
#      sent last, after the fetch and two reads (`Type ENV:Sys/ahi.prefs
#      HEX`, `List DEVS:AudioModes`), each one exec. A TIMEOUT on any of the
#      three prints "guest agent wedged at <what>; the run evidence is
#      already home", sends nothing more and exits 11. Exits 10 and 8 make
#      the same reads, then exit with their own code.
#   9. D78: after the AHI set, `MakeDir RAM:kitty/Devs/AudioModes` and
#      `Copy AROS:Devs/AudioModes/HDAUDIO` into it, one exec each, so AHI's
#      `DEVS:AudioModes` scan finds a mode in the first DEVS: directory. A
#      copy that does not answer rc=0 exits 7, before the prelude.
#  10. D81: then `Copy AROS:Prefs/Env-Archive/hdaudio.config` to
#      DH1:W/hdaudio.config, one exec, because hdaudio.audio reads its
#      overrides from ENVARC: and the prelude points ENVARC: at DH1:W.
#      A copy that does not answer rc=0 exits 7, before the prelude.
#
# The per-run guest work lives in run_<name>.aros, the same fragments
# m0b-run.aros executes, so the two paths cannot drift apart.
set -u

KR="$(cd "$(dirname "$0")/../.." && pwd)"
VK="$HOME/Work/projects/Vulkan4Aros"
SPIKE="$VK/scripts/spike_server.py"
B="$KR/build/m0b23"
PKG="$KR/build/aros-pkg"
AHIDIR="$KR/build/m0b23/ahi"
PROBES="p pt ptr"
RUNS="h b1 od2 od4 lp0 lp1 w fifo lp4 a soak"
SERVER_PID=""
LAUNCHED=0
MODE=boot

held() { ss -ltnH "( sport = :$1 )" 2>/dev/null | grep -q .; }

stop_lane() {
    if [ "$MODE" = dry ]; then
        "$LR_STUBS/stop.sh"
    else
        systemctl --user stop l0025-qemu >/dev/null 2>&1 || true
    fi
}

submit() {
    if [ "$MODE" = dry ]; then
        "$LR_STUBS/submit.sh" "$@"
    else
        python3 -B "$SPIKE" submit --spool "$SPOOL" "$@"
    fi
}

on_exit() {
    if [ -n "$SERVER_PID" ]; then
        kill "$SERVER_PID" 2>/dev/null || true
        wait "$SERVER_PID" 2>/dev/null || true
        echo "lane_run: stopped the spike server it started (pid $SERVER_PID)"
    fi
    if [ "$LAUNCHED" = 1 ] && [ "${LANE_KEEP:-0}" != 1 ]; then
        stop_lane
        echo "lane_run: lane stopped"
    fi
}
trap on_exit EXIT

preflight() {
    if held 9099 || held 9100; then
        echo "port 9099 held by $(ss -ltnpH '( sport = :9099 or sport = :9100 )' | head -2 | tr '\n' ' ')" >&2
        exit 9
    fi
    if pgrep -f '^[^ ]*qemu-system.*aros_l0025' >/dev/null; then
        echo "lane l0025 already running" >&2
        exit 9
    fi
    mkdir -p "$1" "$2"
    ( cd "$1" && exec python3 -B -u "$SPIKE" serve --port 9099 --bulk-port 9100 --spool "$2" ) >"$1/spike.log" 2>&1 &
    SERVER_PID=$!
    echo "$SERVER_PID" > "$1/spike.pid"
    for i in 1 2 3 4 5 6 7 8 9 10; do
        if kill -0 "$SERVER_PID" 2>/dev/null && ss -ltnpH '( sport = :9099 )' | grep -q "pid=$SERVER_PID,"; then
            echo "transport ok pid=$SERVER_PID"
            return 0
        fi
        sleep 1
    done
    echo "spike server did not start: $(tail -5 "$1/spike.log")" >&2
    exit 9
}

under_b() {
    case "$(realpath -m "$1")" in "$B"/*) return 0 ;; *) return 1 ;; esac
}

case "${1:-}" in
    --check-transport) MODE=transport; OUT_ARG="${2:-}" ;;
    --dry-boot) MODE=dry; OUT_ARG="${2:-}" ;;
    *) LANE_ARG="${1:-none}"; OUT_ARG="${2:-}" ;;
esac

# ---- 1. D35: the output directory must live under build/m0b23 ----
if [ -z "$OUT_ARG" ] || ! under_b "$OUT_ARG"; then
    echo "outdir outside build/m0b23: ${OUT_ARG:-none}" >&2
    exit 3
fi
OUT="$(realpath -m "$OUT_ARG")"

if [ "$MODE" = transport ]; then
    preflight "$OUT" "$OUT/tspool"
    exit 0
fi

# ---- 2. the mode's values; nothing below tests the mode ----
PROBE="DH1:C/liblist_probe"
if [ "$MODE" = dry ]; then
    if [ -z "${LR_STUBS:-}" ] || ! under_b "$LR_STUBS"; then
        echo "LR_STUBS outside build/m0b23" >&2
        exit 3
    fi
    GRANT=1
    AUTH=l0025
    LANE_ARG=l0025
    GRACE=1
    MON=1
    BOOTS="$OUT/boots.txt"
    LANES="$OUT/lanes"
    LAUNCH="$LR_STUBS/launch.sh"
    SPOOL="$OUT/spool"
else
    PR="$HOME/Work/Kitty/Docs/Development/dispatch/m0b23_aros_guest_opencode_prompt.md"
    GRACE=90
    MON=4479
    GRANT=$(sed -n 's/^ *BOOTS GRANTED: *\([0-9][0-9]*\) *$/\1/p' "$PR" 2>/dev/null | head -1)
    AUTH=$(sed -n 's/^ *LANE: *\([a-z0-9]*\) *$/\1/p' "$PR" 2>/dev/null | head -1)
    BOOTS="$KR/build/m0b23/boots.txt"
    LANES="$HOME/Work/lanes"
    LAUNCH="$VK/scripts/lane_l0025_boot.sh"
    SPOOL="$HOME/Work/lanes/l0025/spool"
fi
used=0
[ -f "$BOOTS" ] && used=$(grep -c . "$BOOTS")

refuse() {
    echo "no boots granted: lane=${LANE_ARG} authorized=${AUTH:-none} grant=${GRANT:-0} used=$used" >&2
    exit 3
}

[ "$LANE_ARG" = "${AUTH:-none}" ] || refuse
case "${GRANT:-x}" in ''|*[!0-9]*) refuse ;; esac
[ "$GRANT" -gt 0 ] || refuse
[ "$used" -lt "$GRANT" ] || refuse
[ "$LANE_ARG" = l0025 ] || refuse
[ -d "$PKG" ] || { echo "no package at $PKG (run make aros-pkg)" >&2; exit 4; }
[ -x "$LAUNCH" ] || { echo "no launcher: $LAUNCH" >&2; exit 4; }

# ---- 3. D41: ledger check ----
lane_boots=$(find "$LANES/$LANE_ARG" -maxdepth 1 -name 'boot-*.txt' -newer "$B/baseline.done" 2>/dev/null | wc -l)
if [ "$lane_boots" -ne "$used" ]; then
    echo "ledger mismatch: $lane_boots boot records, $used ledger lines" >&2
    exit 3
fi

mkdir -p "$OUT" "$OUT/w" "$OUT/h"
: > "$OUT/host_wall.txt"

# ---- 4. preflight: start server, then ledger line, then boot ----
preflight "$OUT" "$SPOOL"

echo "$(date -Is) $LANE_ARG M0b.23 guest: deploy, AHI set, prelude, p/pt/ptr, ladder" >> "$BOOTS"
echo "lane_run: boots.txt now has $(grep -c . "$BOOTS") of $GRANT granted lines"
LAUNCHED=1
LANE_AUDIO="$OUT/lane_audio.wav" bash "$LAUNCH" "M0b.23 guest ladder (build/m0b23)" || { echo "launcher failed" >&2; exit 5; }
DEAD=0
: > "$OUT/dead.txt"
SERIAL=$(sed -n 's/^serial=//p' "$(ls -t "$LANES/$LANE_ARG"/boot-*.txt | head -1)" | head -1)
echo "lane_run: serial log ${SERIAL:-unknown}"

# D69: the guest's screen when a run is dead; the crash requester names the function
shot() {
    ( { printf 'screendump %s\n' "$OUT/dead_$1.ppm"; sleep 2; } > "/dev/tcp/127.0.0.1/$MON" ) 2>/dev/null || \
        echo "lane_run: no screendump for $1"
}

serial_lines() { wc -l < "$SERIAL" 2>/dev/null || echo 0; }
faulted_since() { tail -n +"$(( $1 + 1 ))" "$SERIAL" 2>/dev/null | grep -qE 'core_IRQHandle\((13|14)\)'; }


# ---- wait for the guest agent (ping is agent-side liveness) ----
t0=$(date +%s)
until submit --ping --wait 10 >/dev/null 2>&1; do
    now=$(date +%s)
    if [ $((now - t0)) -gt 600 ]; then echo "guest agent did not answer in 600 s" >&2; exit 6; fi
    sleep 5
done
echo "lane_run: guest agent answered after $(( $(date +%s) - t0 )) s"

# ---- 5b. deploy, one --put per file, absolute on both sides ----
submit --exec "MakeDir RAM:kitty ALL" --wait 120 >/dev/null 2>&1
while IFS= read -r d; do
    submit --exec "MakeDir RAM:kitty/${d#./} ALL" --wait 120 >/dev/null 2>&1
done < <(cd "$PKG" && find . -mindepth 1 -type d | sort)
nput=0
while IFS= read -r f; do
    rel=${f#./}
    if ! submit --put "$PKG/$rel:RAM:kitty/$rel" --wait 300 >/dev/null 2>&1; then
        echo "lane_run: put failed for $rel" >&2
        exit 7
    fi
    nput=$((nput + 1))
done < <(cd "$PKG" && find . -type f | sort)
echo "lane_run: put $nput files to RAM:kitty"

# D70: the rebuilt AHI set, for this boot only; the prelude puts it in front of DEVS:
submit --exec "MakeDir RAM:kitty/Devs ALL" --wait 120 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/Devs/AHI ALL" --wait 120 >/dev/null 2>&1
for f in "$AHIDIR"/*; do
    case "${f##*/}" in *.device) to="Devs/${f##*/}" ;; *) to="Devs/AHI/${f##*/}" ;; esac
    if ! submit --put "$f:RAM:kitty/$to" --wait 300 >/dev/null 2>&1; then
        echo "lane_run: put failed for $to" >&2
        exit 7
    fi
done
echo "lane_run: put the AHI set to RAM:kitty/Devs"

# D78: AHI scans DEVS:AudioModes in the first DEVS: directory only, so one mode file goes there
submit --exec "MakeDir RAM:kitty/Devs/AudioModes" --wait 120 >/dev/null 2>&1
mc=$(submit --exec "Copy AROS:Devs/AudioModes/HDAUDIO RAM:kitty/Devs/AudioModes/HDAUDIO" --wait 120 2>&1)
if ! echo "$mc" | grep -q -- '-> rc=0 ('; then
    echo "lane_run: AudioModes copy failed: $(echo "$mc" | grep -m1 'rc=\|TIMEOUT')" >&2
    exit 7
fi
echo "lane_run: copied AROS:Devs/AudioModes/HDAUDIO to RAM:kitty/Devs/AudioModes"

# D81: hdaudio.audio reads its overrides from ENVARC:hdaudio.config; the prelude points ENVARC: at DH1:W
ec=$(submit --exec "Copy AROS:Prefs/Env-Archive/hdaudio.config DH1:W/hdaudio.config" --wait 120 2>&1)
if ! echo "$ec" | grep -q -- '-> rc=0 ('; then
    echo "lane_run: hdaudio.config copy failed: $(echo "$ec" | grep -m1 'rc=\|TIMEOUT')" >&2
    exit 7
fi
echo "lane_run: copied AROS:Prefs/Env-Archive/hdaudio.config to DH1:W"

lst=$(submit --exec "List RAM:kitty" --wait 120 2>&1)
echo "$lst" | sed -n '1,14p'
for need in kitty kitty-debug prelude.aros run_p.aros run_pt.aros run_ptr.aros run_w.aros run_soak.aros test.mod spv; do
    echo "$lst" | grep -q "$need" || { echo "lane_run: deploy check failed: $need missing" >&2; exit 7; }
done

getf() {
    submit --get "RAM:kitty/out/$1:$OUT/$2" --wait 300 >/dev/null 2>&1 || \
        echo "lane_run: get failed for $1" >&2
}

wedged() {
    if echo "$2" | grep -q 'TIMEOUT: no result'; then
        echo "guest agent wedged at $1; nothing more is sent"
        exit 6
    fi
}

# D75: one late exec; a wedge here costs no run, its evidence is already home
late() {
    q=$(submit --exec "$2" --wait 120 2>&1 < /dev/null)
    echo "$q" > "$OUT/exec_$1.log"
    if echo "$q" | grep -q 'TIMEOUT: no result'; then
        echo "guest agent wedged at $3; the run evidence is already home"
        return 1
    fi
}

endreads() {
    late ahi_prefs "Type ENV:Sys/ahi.prefs HEX >RAM:kitty/out/ahi_prefs.txt" "the AHI prefs read" || return 1
    getf "ahi_prefs.txt" "ahi_prefs.txt"
    late audiomodes "List DEVS:AudioModes >RAM:kitty/out/audiomodes.txt" "the AudioModes list" || return 1
    getf "audiomodes.txt" "audiomodes.txt"
    [ -n "$LISTING" ] || return 0
    late listing "$LISTING" "the Assign listing" || return 1
    getf "assigns.txt" "assigns.txt"
}

# ---- 5c. the out/ tree, then the D32 prelude, one exec per line (D58) ----
submit --exec "MakeDir RAM:kitty/out" --wait 60 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/out/p" --wait 60 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/out/w" --wait 60 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/out/h" --wait 60 >/dev/null 2>&1
submit --exec "Avail >RAM:kitty/out/avail_before.txt" --wait 120 >/dev/null 2>&1

LISTING=""
: > "$OUT/exec_prelude.log"
pn=0
while IFS= read -r pl || [ -n "$pl" ]; do
    case "$pl" in ''|\;*) continue ;; esac
    case "$pl" in 'Assign >'*) LISTING=$pl; continue ;; esac
    pn=$((pn + 1))
    pout=$(submit --exec "$pl" --wait 120 2>&1 < /dev/null)
    echo "$pout" >> "$OUT/exec_prelude.log"
    echo "$pout" | sed -n '1,4p'
    wedged "prelude line $pn ($pl)" "$pout"
    if ! echo "$pout" | grep -q -- '-> rc=0 ('; then
        echo "prelude failed at line $pn: $pl"
        getf "avail_before.txt" "avail_before.txt"
        endreads
        exit 10
    fi
done < "$PKG/prelude.aros"
echo "lane_run: prelude sent as $pn execs"
# D71: what a bare device name would be looked up against
submit --exec "CD" --wait 60 > "$OUT/exec_cd.log" 2>&1

# ---- 5d. one --exec per run fragment; wall time is host-measured ----
run_one() {
    r="$1"; budget="$2"; limit="$3"
    s=$(date +%s)
    n0=$(serial_lines)
    out=$(submit --exec "Execute RAM:kitty/run_$r.aros >RAM:kitty/out/sh_$r.log" --exec-timeout "$budget" --wait "$budget")
    rc=$?
    echo "$out" > "$OUT/exec_$r.log"
    echo "$out" | sed -n '1,6p'
    if echo "$out" | grep -q 'TIMEOUT: no result'; then
        echo "$r start=$s end=$(date +%s)" >> "$OUT/host_wall.txt"
    fi
    wedged "run $r" "$out"
    if echo "$out" | grep -q -- '-> RUNNING'; then
        echo "lane_run: run $r answered RUNNING (guest exec budget); waiting for rc_$r.txt"
        tmo=$(( s + limit ))
        got=0
        fault_at=0
        while [ "$(date +%s)" -lt "$tmo" ]; do
            q=$(submit --exec "Type RAM:kitty/out/rc_$r.txt" --wait 120 2>&1)
            if echo "$q" | grep -qE '^[[:space:]]+rc=[0-9]+[[:space:]]*$'; then
                got=1
                break
            fi
            if faulted_since "$n0"; then
                [ "$fault_at" != 0 ] || fault_at=$(date +%s)
                if [ $(( $(date +%s) - fault_at )) -ge "$GRACE" ]; then
                    break
                fi
            fi
            sleep 10
        done
        echo "$q" | sed -n '1,3p'
        if [ "$got" = 0 ]; then
            DEAD=$((DEAD + 1))
            echo "$r" >> "$OUT/dead.txt"
            echo "lane_run: run $r has no rc; dead ($DEAD)"
            shot "$r"
        fi
    fi
    tail -n +"$(( n0 + 1 ))" "$SERIAL" > "$OUT/serial_$r.txt" 2>/dev/null
    e=$(date +%s)
    echo "$r start=$s end=$e" >> "$OUT/host_wall.txt"
    echo "lane_run: run $r exec rc=$rc wall=$((e - s)) s"
}

run_one p 600 1100
run_one pt 600 1100
run_one ptr 600 1100

# D33: p decides; pt and ptr are the trap/plant evidence and have already run.
getf "rc_p.txt" "rc_p.txt"
getf "run_p.log" "run_p.log"
if [ "$(cat "$OUT/rc_p.txt" 2>/dev/null)" != "rc=0" ] || \
   ! grep -q 'vk_loader=' "$OUT/run_p.log" 2>/dev/null; then
    echo "probe failed; ladder not run"
    for r in pt ptr; do
        getf "rc_$r.txt" "rc_$r.txt"
        getf "run_$r.log" "run_$r.log"
    done
    for r in $PROBES; do getf "sh_$r.log" "sh_$r.log"; done
    getf "avail_before.txt" "avail_before.txt"
    endreads
    exit 8
fi

# Each run's evidence comes home as it ends: a later wedge must not cost it.
for r in $RUNS; do
    if [ "$DEAD" -ge 3 ]; then
        echo "lane_run: $DEAD dead, $r not started"
        continue
    fi
    # D70: soak opens AHI too; after a dead `a` it would only wait on the dead task
    if [ "$r" = soak ] && grep -qx a "$OUT/dead.txt"; then
        echo "lane_run: a dead, soak not started"
        continue
    fi
    case "$r" in soak) lim=2600 ;; *) lim=1100 ;; esac
    run_one "$r" 2400 "$lim"
    getf "run_$r.log" "run_$r.log"
    getf "rc_$r.txt" "rc_$r.txt"
    case "$r" in
        w) getf "w/present_150.bgra" "w/present_150.bgra"
           getf "w/present_150.inner.bgra" "w/present_150.inner.bgra" ;;
        h) for n in 000 150 299; do getf "h/frame_$n.bgra" "h/frame_$n.bgra"; done ;;
    esac
done
submit --exec "Avail >RAM:kitty/out/avail_after.txt" --wait 120 >/dev/null 2>&1
# D69: which loader the guest really had open, last, so a fault here costs no run
submit --exec "$PROBE idstring vulkan_loader" --wait 120 > "$OUT/loader_id.txt" 2>&1 < /dev/null

# ---- 7. fetch RAM:kitty/out ----
for r in $PROBES $RUNS; do
    getf "run_$r.log" "run_$r.log"
    getf "rc_$r.txt" "rc_$r.txt"
    getf "sh_$r.log" "sh_$r.log"
done
getf "avail_before.txt" "avail_before.txt"
getf "avail_after.txt" "avail_after.txt"

echo "lane_run: wrote $OUT/host_wall.txt"
cat "$OUT/host_wall.txt"
endreads || exit 11
echo "lane_run: lane stops now (LANE_KEEP=1 keeps it up)"
exit 0
RUNNER
chmod 755 tests/aros/lane_run.sh; h=$(sha256sum < tests/aros/lane_run.sh | cut -d' ' -f1)
[ "$h" = 2d182f1d28d37dc77a0a74b0f114da5c2fb5f55b2e443b86cd85084459af808f ] && echo "installed the supplied runner, sha256 $h" || { echo "INSTALL FAIL: sha256 $h"; exit 1; }
````

**Script X3: check the supplied fix (D93).** `bash "$B/scripts/x3.sh"`, after X2, every pass. It
applies the patch once, and answers "already applied" on later passes.

````bash
# script x3
cd "$KR" || exit 1; cat > "$B/fix.patch" <<'FIXPATCH'
--- a/src/platform/aros/plat.cpp
+++ b/src/platform/aros/plat.cpp
@@ -452,6 +452,7 @@
 static u64 g_ahi_t0 = 0;      /* D90: pl_now_us at open; xrun and error lines give us= from it */
 static int g_ahi_stage = -1;  /* D91: the request being filled, not yet sent */
 static u32 g_ahi_staged = 0;  /* D91: frames already in it */
+static u64 g_ahi_first_us = 0; /* D93: first request sent, us since start (elapsed_us base) */
 
 static u32 ahi_inflight(void) {
     u32 n = 0u;
@@ -695,6 +696,8 @@
            (unsigned long long)g_ahi_errors, g_ahi_first_error,
            (unsigned long long)g_ahi_dropped, ahi_inflight());
     pl_log(p, line);
+    rt_fmt(line, sizeof line, "ahi_first_submit_us=%llu", (unsigned long long)g_ahi_first_us);
+    pl_log(p, line);
 }
 
 [[gnu::noinline]] static void ahi_copy_frames(u8* dst, const u8* src, u32 bytes) {
@@ -739,6 +742,7 @@
      * one if it is still sounding, and clears a link to anything else. */
     struct AHIRequest* prev = g_ahi_last >= 0 ? g_ahi_req[g_ahi_last] : 0;
     rq->ahir_Link = (prev && CheckIO((struct IORequest*)prev) == 0) ? prev : 0;
+    if (g_ahi_submitted == 0ull) g_ahi_first_us = pl_now_us(p) - p.t0_us;
     SendIO((struct IORequest*)rq);
     g_ahi_sent[idx] = true;
     g_ahi_last = idx;
FIXPATCH
cat > "$B/fix_pre.txt" <<'SHAS'
e9078a4779bf2a4b1747c97622a89cce1b24308593fa55443c486bbf584c64af  src/platform/aros/plat.cpp
SHAS
cat > "$B/fix_post.txt" <<'SHAS'
d5c6b2e6060f4462f8590069805c166b55e8527ac0e3bb63c03bff70e1db7701  src/platform/aros/plat.cpp
SHAS
if sha256sum -c "$B/fix_post.txt" >/dev/null 2>&1; then echo "fixes already applied"
elif sha256sum -c "$B/fix_pre.txt" >/dev/null 2>&1; then
  patch -p1 --no-backup-if-mismatch -s < "$B/fix.patch" || { echo "X3 FAIL: patch did not apply"; exit 1; }
  sha256sum -c "$B/fix_post.txt" >/dev/null 2>&1 || { echo "X3 FAIL: patched files differ from the pins"; exit 1; }
  echo "applied the supplied fixes to $(wc -l < "$B/fix_post.txt") files"
else echo "X3 FAIL: the files are neither the pre-patch nor the patched state"; sha256sum -c "$B/fix_pre.txt" 2>/dev/null | grep -v ': OK$'; exit 1; fi
````

**Script X4: read the rebuilt AHI set (D70).** `bash "$B/scripts/x4.sh"`, after X3, every pass. It
writes only under `build/m0b23/ahi/`.

````bash
# script x4
A=$B/ahi; mkdir -p "$A"; cat > "$B/ahi_shas.txt" <<'SHAS'
0d7e78f30bd5161da10b6dbc7e1823bdf19ea3e7f4ea24ec654886986595f4b1  ac97.audio
155157e71ced0280a32d5c2f3a279448514ae137bbaee4518b54f437d061db37  ahi.device
d6cab2514708da727d0f95c134825f9764ad78473f0a2b92c464b1423ceee33d  cmi8738.audio
b3aa4d3c92e11c69031574f14a1c629a5a3d7a21b4470ada31a0815643325ba9  hdaudio.audio
1d679e4f95a0065edb477c3bb1bf1d299608ef658098fa7330e8184907f3ff05  nvhdmi.audio
9076e01d964e23ff1838645576a4d230e71579ef3ee8af9d36737e6b6b771d4f  sb128.audio
cf8b8e784d67efe1249f5f03754d9bda1268871d4b2a592d717b3db1de82e3c0  via-ac97.audio
68131bed913ae3a77e180b960e560fc29e674e1262abb6353e4b8c62aea181d8  void.audio
SHAS
if (cd "$A" && sha256sum -c "$B/ahi_shas.txt" >/dev/null 2>&1) && [ "$(ls "$A" | wc -l)" = 8 ]; then echo "AHI set already read"; exit 0; fi
pgrep -f '^[^ ]*qemu-system.* -name riaudio' >/dev/null && { echo "X4 FAIL: riaudio is running"; exit 1; }
while read -r _ n; do
  case "$n" in *.device) p=/Devs/$n ;; *) p=/Devs/AHI/$n ;; esac
  python3 -B -I "$VK/scripts/sfs_nav.py" ~/Work/vms/riaudio_dh0.img cat "$p" > "$A/$n" 2>/dev/null || { echo "X4 FAIL: can't read $p"; exit 1; }
done < "$B/ahi_shas.txt"
(cd "$A" && sha256sum -c "$B/ahi_shas.txt" 2>/dev/null | grep -v ': OK$'; sha256sum -c "$B/ahi_shas.txt" >/dev/null 2>&1) || { echo "X4 FAIL: the AHI set differs from the pins"; exit 1; }
echo "read the AHI set: $(ls "$A" | wc -l) files"
````

**Scripts 1–11, 13, 14, 15, 12:** `bash "$B/scripts/s<n>.sh"` each (scripts 1–11 and 13 in one loop as
in M0b; paste the whole output). They are M0b's gates, unchanged except for the patches above.

**Script 14b: the M0b.1–M0b.23 host gates.** `bash "$B/scripts/s14b.sh"`, after 14.

````bash
# script s14b
cd "$KR" || exit 1; A=tests/aros; K=build/aros-pkg
chk(){ if [ -z "$2" ]; then g "S14b.$1" PASS; else echo "$2" | head -12; g "S14b.$1" FAIL; fi; }
f=""; for r in p pt ptr w h fifo a lp0 lp1 lp4 od2 od4 b1 soak; do x="$A/run_$r.aros"; [ -f "$x" ] || { f="$f
missing $x"; continue; }
  c=$(grep -vE '^[[:space:]]*(;|$)' "$x"); n=$(echo "$c" | grep -c .)
  [ "$n" = 3 ] && [ "$(echo "$c" | sed -n 1p)" = "Stack 1048576" ] && [ "$(echo "$c" | sed -n 3p)" = "Echo \"rc=\$RC\" >RAM:kitty/out/rc_$r.txt" ] || f="$f
$x: want exactly Stack / program / Echo rc"
  pl=$(echo "$c" | sed -n 2p); set -- $pl
  { [ "$1" = RAM:kitty/kitty ] || [ "$1" = RAM:kitty/kitty-debug ]; } && [ "$2" = ">RAM:kitty/out/run_$r.log" ] || f="$f
$x: program line is not '<binary> >RAM:kitty/out/run_$r.log …': $pl"
done
for r in p pt; do grep -qE '^RAM:kitty/kitty-debug ' "$A/run_$r.aros" 2>/dev/null || f="$f
run_$r must run kitty-debug"; done; grep -qE '^RAM:kitty/kitty ' "$A/run_ptr.aros" 2>/dev/null || f="$f
run_ptr must run the release kitty"
grep -q -- '--headless --frames 1 --out RAM:kitty/out/p ' "$A/run_p.aros" 2>/dev/null || f="$f
run_p arguments differ from D33"
for r in pt ptr; do grep -q -- '--plant-loader-fail --headless --frames 1 ' "$A/run_$r.aros" 2>/dev/null || f="$f
run_$r arguments differ from D33"; done
[ "$(grep -E '^RAM:kitty/' "$A/run_soak.aros" 2>/dev/null)" = 'RAM:kitty/kitty-debug >RAM:kitty/out/run_soak.log --windowed --seconds 1800 --music RAM:kitty/test.mod --spv RAM:kitty/spv' ] || f="$f
run_soak is not D70's line"
chk run-scripts "$f"
f=""; c=$(grep -vE '^[[:space:]]*(;|$)' "$A/prelude.aros" 2>/dev/null)
[ "$c" = "$(printf 'Assign ENVARC: DH1:W\nAssign LIBS: DH1:Libs\nAssign LIBS: ADD AROS:Libs\nAssign DEVS: RAM:kitty/Devs\nAssign DEVS: ADD AROS:Devs\nAssign >RAM:kitty/out/assigns.txt')" ] || f="prelude.aros is not exactly the six D87 lines:
$c"
m=$(grep -vE '^[[:space:]]*(;|$)' "$A/m0b-run.aros" 2>/dev/null); [ "$(echo "$m" | sed -n 1p)" = "Stack 1048576" ] && echo "$m" | sed -n 2p | grep -q 'prelude\.aros' || f="$f
m0b-run.aros must start with Stack 1048576 then Execute the prelude"
e=$(grep -nE '(^|[^A-Za-z])Execute[^;#]*>' "$A"/*.aros "$A/lane_run.sh" 2>/dev/null | grep -vE '^[^:]+:[0-9]+:[[:space:]]*[#;]' | grep -vE '^tests/aros/lane_run\.sh:[0-9]+:[[:space:]]*out=\$\(submit --exec "Execute RAM:kitty/run_\$r\.aros >RAM:kitty/out/sh_\$r\.log" '); [ -z "$e" ] || f="$f
an Execute line carries a redirect other than the runner's sh_ log (D59):
$e"
x=$(grep -nE 'Execute[^"]*prelude' "$A/lane_run.sh" | grep -vE '^[0-9]+:[[:space:]]*#'); [ -z "$x" ] || f="$f
the runner Executes the prelude; D58 sends its lines as separate execs:
$x"
grep -iE 'put[^;]*\.library|vulkan_loader\.library|Copy[^;]*Libs' "$A"/*.aros "$A/lane_run.sh" | grep -qvE '^[^:]+:[[:space:]]*[#;]' && f="$f
a guest script or the runner copies or puts a library"
chk wiring "$f"
f=""; R=$A/lane_run.sh
grep -q 'dispatch/m0b23_aros_guest_opencode_prompt\.md' "$R" && grep -q 'build/m0b23/boots\.txt\|m0b23/boots' "$R" || f="runner does not read this prompt and build/m0b23/boots.txt"
grep -qE 'm0b[1-6]([^0-9]|$)' "$R" && f="$f
runner still names m0b1 … m0b6"
grep -q 'probe failed; ladder not run' "$R" || f="$f
runner has no D33 probe exit"
grep -q 'realpath -m' "$R" || f="$f
runner does not resolve <outdir> with realpath -m"
n0=$(grep -c . "$B/boots.txt"); o=$(bash "$R" l0025 /tmp/m0b23-outside 2>&1); rc=$?
{ [ $rc = 3 ] && echo "$o" | grep -q 'outdir outside build/m0b23' && [ "$(grep -c . "$B/boots.txt")" = "$n0" ] && [ ! -e /tmp/m0b23-outside ]; } || f="$f
runner did not refuse an outdir outside build/m0b23 (rc=$rc): $(echo "$o" | head -2)"
chk runner "$f"
f=""; grep -q -- '--plant-loader-fail' src/app/main.cpp || f="no --plant-loader-fail in main.cpp"
VK_DRIVER_FILES=$(icd) build/linux/kitty --plant-loader-fail --headless --frames 1 > /dev/null 2>&1; r1=$?
VK_DRIVER_FILES=$(icd) build/linux-debug/kitty --plant-loader-fail --headless --frames 1 > /dev/null 2>&1; r2=$?
[ $r1 = 2 ] && [ $r2 = 2 ] || f="$f
Linux must exit 2 on --plant-loader-fail (release=$r1 debug=$r2)"
for x in prelude.aros run_p.aros run_pt.aros run_ptr.aros; do cmp -s "$A/$x" "$K/$x" || f="$f
$K/$x missing or differs from $A/$x"; done
chk plant-and-package "$f"
f=""; nc=$(grep -nvE '^[[:space:]]*(#|$)' "$R")
x=$(echo "$nc" | grep -E '^[0-9]+:.*[^[:space:]].*[[:space:]]#' | grep -vE '"[^"]*#[^"]*"|'"'"'[^'"'"']*#[^'"'"']*'"'"'|\$#|\{#'); [ -z "$x" ] || f="lines with code and a comment (D43):
$x"
x=$(grep -nE '\\[[:space:]]+(#.*)?$' "$R"); [ -z "$x" ] || f="$f
backslash followed by blanks (D43):
$x"
[ "$(grep -cE '^[[:space:]]*preflight[[:space:]]*\(\)' "$R")" = 1 ] || f="$f
not exactly one preflight() definition"
ps=$(grep -nE '^[[:space:]]*preflight[[:space:]]*\(\)' "$R" | cut -d: -f1); pe=$(awk -v s="$ps" 'NR>s && /^}/ {print NR; exit}' "$R")
sv=$(echo "$nc" | grep -E '(spike_server\.py|\$\{?SPIKE\}?)[^|;&]*[[:space:]]serve([[:space:]]|$)'); svl=$(echo "$sv" | cut -d: -f1)
{ [ "$(echo "$sv" | grep -c .)" = 1 ] && [ -n "$ps" ] && [ -n "$pe" ] && [ "$svl" -gt "$ps" ] && [ "$svl" -lt "$pe" ]; } || f="$f
the server must start in exactly one line, inside preflight():
$sv"
lb=$(echo "$nc" | grep -E '>>[[:space:]]*"?\$\{?BOOTS' | head -1 | cut -d: -f1)
cl=$(echo "$nc" | grep -E '(^[0-9]+:|[;&|[:space:]])preflight[[:space:]]+"' | cut -d: -f1)
ct=$(echo "$nc" | grep -E '(^[0-9]+:|[;&|[:space:]])preflight[[:space:]]+"[^"]*"[[:space:]]+"[^"]*/tspool"' | cut -d: -f1)
bc=$(for l in $cl; do [ "$l" != "$ct" ] && echo "$l"; done)
{ [ "$(echo "$cl" | grep -c .)" = 2 ] && [ -n "$ct" ] && [ -n "$bc" ] && [ -n "$lb" ] && [ "$bc" -lt "$lb" ]; } || f="$f
want two preflight calls: one with .../tspool (check-transport) and one before the boots.txt append (calls: $(echo $cl), tspool call: ${ct:-none}, append: ${lb:-none})"
[ -n "$bc" ] && [ -n "$lb" ] && x=$(echo "$nc" | awk -F: -v a="$bc" -v z="$lb" '$1>a && $1<z' | grep -E '(^[0-9]+:|[;&|[:space:]])(submit|sleep)([[:space:]]|$)') && [ -n "$x" ] && f="$f
submit or sleep between the boot path's preflight and the boots.txt append:
$x"
chk preflight "$f"
f=""; tar -xOf "$KR/build/m0b3/pre.tar" Makefile | cmp -s - Makefile || f="Makefile differs from the M0b.2 tree's (D47):
$(tar -xOf "$KR/build/m0b3/pre.tar" Makefile | diff - Makefile | head -6)"
chk makefile "$f"
f=""; PIN=2d182f1d28d37dc77a0a74b0f114da5c2fb5f55b2e443b86cd85084459af808f; h=$(sha256sum < "$R" | cut -d' ' -f1); [ "$h" = "$PIN" ] || f="tests/aros/lane_run.sh is not the supplied runner (D58): sha256 $h"
[ "$(sha256sum < "$B/pre_files.txt" 2>/dev/null | cut -d' ' -f1)" = 37779fc7dad798f69246290dd5344fe8fb93f1eca1f599225c9cd3bfe419420e ] || f="$f
build/m0b23/pre_files.txt is not script 0's record of the pinned tree (D55)"
ch=$(grep -vE '  (tests/aros/lane_run\.sh|src/platform/aros/plat\.cpp)$' "$B/pre_files.txt" | sha256sum -c 2>/dev/null | grep -v ': OK$' | sed 's/: FAILED.*$//')
ad=$(comm -13 <(sed 's/^[0-9a-f]*  //' "$B/pre_files.txt" | sort) <(find .gitignore Makefile src tools shaders tests -type f | sort))
for p in $ch $ad; do
  [ -e "$p" ] || { f="$f
$p removed since script 0"; continue; }
  f="$f
$p changed since script 0: no edits this round (D67)"
done
chk pinned "$f"
f=""; [ -s "$B/fix_post.txt" ] || f="X3 has not run"
x=$(printf '%s\n' 'd5c6b2e6060f4462f8590069805c166b55e8527ac0e3bb63c03bff70e1db7701  src/platform/aros/plat.cpp' | sha256sum -c 2>/dev/null | grep -v ': OK$'); [ -z "$x" ] || f="$f
files are not X3's (D69): $x"
chk aros-fix "$f"
if [ -n "${WAYLAND_DISPLAY:-}${DISPLAY:-}" ]; then
  f=""; VK_DRIVER_FILES="$(icd)" timeout 15 build/linux-debug/kitty --windowed --frames 5 --force-fifo > "$B/ff.log" 2>&1; r=$?
  { [ "$r" = 0 ] && grep -q '^present_mode=FIFO' "$B/ff.log" && ! grep -q '^present_mode=IMMEDIATE' "$B/ff.log"; } || f="--force-fifo: want rc 0 and present_mode=FIFO only (rc=$r): $(grep present_mode "$B/ff.log")"
  chk force-fifo "$f"
else g S14b.force-fifo BLOCKED "no display"; fi
f=""; lm=$(grep -n 'ledger mismatch' "$R" | grep -vE '^[0-9]+:[[:space:]]*#' | head -1 | cut -d: -f1)
x=$(awk -v z="${lm:-0}" 'NR>z' "$R" | grep -nE 'MODE|LR_STUBS' | grep -vE '^[0-9]+:[[:space:]]*#'); { [ -n "$lm" ] && [ -z "$x" ]; } || f="mode tested after the ledger check (D49), or no 'ledger mismatch' line:
$x"
[ -n "$lm" ] && for v in $(awk -v z="$lm" 'NR<z' "$R" | grep -vE '^[[:space:]]*#' | grep -oE '(^|[[:space:];&|(])[A-Za-z_][A-Za-z0-9_]*=' | grep -oE '[A-Za-z_][A-Za-z0-9_]*' | sort -u); do
  case " KR VK SPIKE B PKG PROBES RUNS SERVER_PID LAUNCHED MODE OUT_ARG LANE_ARG OUT GRANT AUTH BOOTS LANES LAUNCH SPOOL PR used lane_boots GRACE PROBE MON AHIDIR " in *" $v "*) continue ;; esac
  awk -v z="$lm" 'NR>z' "$R" | grep -vE '^[[:space:]]*#' | grep -qE "\\\$\\{?$v([^A-Za-z0-9_]|\$)" && f="$f
variable $v is set before the ledger check and read after it; only the D49 values may differ between modes"
done
awk '/out=\$\(submit --exec "Execute RAM:kitty\/run_\$r\.aros >RAM:kitty\/out\/sh_\$r\.log"/ { getline n; print n; exit }' "$R" | grep -qxE '[[:space:]]*rc=\$\?' || f="$f
in run_one, the line after the client call is not rc=\$? (D48)"
[ "$(grep -cE '^[[:space:]]*trap[[:space:]]' "$R")" = 1 ] && ! sed -n '/^[[:space:]]*preflight[[:space:]]*()/,/^}/p' "$R" | grep -qE '^[[:space:]]*trap[[:space:]]' || f="$f
want exactly one trap line, outside preflight (D48)"
if ss -ltnH '( sport = :9099 or sport = :9100 )' 2>/dev/null | grep -q .; then echo "$f" | grep . | head -6; g S14b.bootpath BLOCKED "9099/9100 held before the dry boot"; else
S=$B/stubs; rm -rf "$S" "$B"/dry-*; mkdir -p "$S"
cat > "$S/launch.sh" <<'STUB'
#!/bin/bash
echo "launch $*" >> "$LR_CALLS"
mkdir -p "$LR_DRY_LANES/l0025"; : > "$LR_DRY_LANES/l0025/serial-stub.log"; echo "serial=$LR_DRY_LANES/l0025/serial-stub.log" > "$LR_DRY_LANES/l0025/boot-stub-$$.txt"
exit "${STUB_LAUNCH_RC:-0}"
STUB
cat > "$S/stop.sh" <<'STUB'
#!/bin/bash
echo "stop" >> "$LR_CALLS"
STUB
cat > "$S/submit.sh" <<'STUB'
#!/bin/bash
echo "submit $*" >> "$LR_CALLS"
case "$1" in
  --get) src=${2%%:/*}; dst=/${2#*:/}; n=${src##*/}; mkdir -p "$(dirname "$dst")"
    case "$n" in
      rc_*.txt) echo "rc=0" > "$dst" ;;
      run_p.log) if [ "${STUB_PROBE_FAIL:-0}" = 1 ]; then echo "stub: no loader" > "$dst"; else echo 'vk_loader="vulkan_loader.library" frames=1' > "$dst"; fi ;;
      *) echo "stub" > "$dst" ;;
    esac ;;
  --exec) case "$2" in
      "List RAM:kitty") printf '%s\n' kitty kitty-debug prelude.aros run_p.aros run_pt.aros run_ptr.aros run_w.aros run_soak.aros test.mod spv ;;
      "Assign LIBS: DH1:Libs") if [ "${STUB_PRELUDE_FAIL:-0}" = 1 ]; then echo "[exec] '$2' -> rc=10 (5 ms)"; else echo "[exec] '$2' -> rc=0 (5 ms)"; fi ;;
      "Execute RAM:kitty/run_p.aros >RAM:kitty/out/sh_p.log") if [ "${STUB_WEDGE:-0}" = 1 ]; then echo "[submit] TIMEOUT: no result after 600s (job stays queued)"; exit 2; else echo "[exec] '$2' -> rc=0 (5 ms)"; fi ;;
      "Execute RAM:kitty/run_soak.aros >RAM:kitty/out/sh_soak.log") if [ "${STUB_RUNNING:-0}" = 1 ]; then echo "[exec] '$2' -> RUNNING (900000 ms): still going past the agent's deadline"; else echo "[exec] '$2' -> rc=0 (5 ms)"; fi ;;
      "Execute RAM:kitty/run_w.aros >RAM:kitty/out/sh_w.log"|"Execute RAM:kitty/run_fifo.aros >RAM:kitty/out/sh_fifo.log"|"Execute RAM:kitty/run_lp4.aros >RAM:kitty/out/sh_lp4.log") if [ "${STUB_FAULT:-0}" = 1 ]; then echo "[Kernel] core_IRQHandle(14): stub" >> "$LR_DRY_LANES/l0025/serial-stub.log"; echo "[exec] '$2' -> RUNNING (900000 ms): still going past the agent's deadline"; else echo "[exec] '$2' -> rc=0 (5 ms)"; fi ;;
      "Execute RAM:kitty/run_a.aros >RAM:kitty/out/sh_a.log") if [ "${STUB_ADEAD:-0}" = 1 ]; then echo "[Kernel] core_IRQHandle(14): stub" >> "$LR_DRY_LANES/l0025/serial-stub.log"; echo "[exec] '$2' -> RUNNING (900000 ms): still going past the agent's deadline"; else echo "[exec] '$2' -> rc=0 (5 ms)"; fi ;;
      "Copy AROS:Devs/AudioModes/HDAUDIO RAM:kitty/Devs/AudioModes/HDAUDIO") if [ "${STUB_MODES_FAIL:-0}" = 1 ]; then echo "[exec] '$2' -> rc=20 (5 ms)"; else echo "[exec] '$2' -> rc=0 (5 ms)"; fi ;;
      "Copy AROS:Prefs/Env-Archive/hdaudio.config DH1:W/hdaudio.config") if [ "${STUB_ENVARC_FAIL:-0}" = 1 ]; then echo "[exec] '$2' -> rc=20 (5 ms)"; else echo "[exec] '$2' -> rc=0 (5 ms)"; fi ;;
      "Assign >RAM:kitty/out/assigns.txt") if [ "${STUB_LISTING_WEDGE:-0}" = 1 ]; then echo "[submit] TIMEOUT: no result after 120s (job stays queued)"; exit 2; else echo "[exec] '$2' -> rc=0 (5 ms)"; fi ;;
      "Type RAM:kitty/out/rc_soak.txt") echo "[exec] '$2' -> rc=0 (5 ms)"
        if [ "$(grep -c '^submit --exec Type RAM:kitty/out/rc_soak.txt' "$LR_CALLS")" -ge 3 ]; then echo "       rc=0"; else echo "       Type: object not found"; fi ;;
      *) echo "[exec] '$2' -> rc=0 (5 ms)" ;;
    esac ;;
esac
exit 0
STUB
chmod +x "$S"/*.sh
lw(){ { find ~/Work/lanes/l0025 ~/Work/lanes/kitty-* -newer "$B/baseline.done" 2>/dev/null; ls ~/Work/lanes/*/boot-*.txt 2>/dev/null; } | sort -u; }
lw0=$(lw); nb0=$(grep -c . "$B/boots.txt")
dry(){ n=$1; shift; D=$B/dry-$n; mkdir -p "$D/lanes/l0025"; : > "$D/calls.log"
  [ "${PLANT_STRAY:-0}" = 1 ] && : > "$D/lanes/l0025/boot-stray.txt"
  env "$@" LR_STUBS="$S" LR_CALLS="$D/calls.log" LR_DRY_LANES="$D/lanes" timeout 600 bash "$R" --dry-boot "$D" > "$D/out.log" 2>&1; echo $?; }
nl(){ grep -c . "$1" 2>/dev/null || echo 0; }
last(){ grep . "$1" | tail -1; }
r=$(dry ok); D=$B/dry-ok
{ [ "$r" = 0 ] && [ "$(nl "$D/boots.txt")" = 1 ] && [ "$(grep -c '^launch' "$D/calls.log")" = 1 ] && [ "$(last "$D/calls.log")" = stop ] && [ "$(nl "$D/host_wall.txt")" = 14 ]; } || f="$f
dry ok: want rc 0, 1 ledger line, 1 launch, stop last, 14 runs (rc=$r ledger=$(nl "$D/boots.txt") launches=$(grep -c '^launch' "$D/calls.log") last=$(last "$D/calls.log") runs=$(nl "$D/host_wall.txt"))"
for d in $(cd "$K" && find . -mindepth 1 -type d | sed 's|^\./||' | sort); do
  md=$(grep -nE "^submit --exec MakeDir RAM:kitty/$d ALL( |\$)" "$D/calls.log" | head -1 | cut -d: -f1)
  pt=$(grep -nE "^submit --put [^ ]*:RAM:kitty/$d/" "$D/calls.log" | head -1 | cut -d: -f1)
  { [ -n "$md" ] && { [ -z "$pt" ] || [ "$md" -lt "$pt" ]; }; } || f="$f
dry ok: RAM:kitty/$d is not made before the first put into it (MakeDir line ${md:-none}, first put line ${pt:-none})"
done
pe=$(grep -E '^submit --exec Assign ' "$D/calls.log" | sed 's/^submit --exec //; s/ --wait [0-9]*$//')
{ [ "$pe" = "$(grep -vE '^[[:space:]]*(;|$)' "$K/prelude.aros")" ] && ! grep -q '^submit --exec Execute RAM:kitty/prelude' "$D/calls.log"; } || f="$f
dry ok: the prelude is not sent as its own six command lines, in order (D58, D87): $(echo "$pe" | tr '\n' '|')"
ne=$(grep -c '^submit --exec Execute ' "$D/calls.log"); ns=$(grep -cE '^submit --exec Execute RAM:kitty/run_([a-z0-9]+)\.aros >RAM:kitty/out/sh_\1\.log ' "$D/calls.log")
{ [ "$ne" = 14 ] && [ "$ns" = 14 ]; } || f="$f
dry ok: want 14 Execute calls, each with its sh_ redirect (D58) (Execute=$ne with_redirect=$ns)"
grep -qE '^submit --(exec Delete|put [^ ]*\.library|get DH1:)' "$D/calls.log" && f="$f
dry ok: the runner touched DH1's loader (D69)"
[ "$(tail -n 4 "$D/calls.log" | grep -c '^submit --exec DH1:C/liblist_probe idstring vulkan_loader')" = 0 ] && ! grep -q '^submit --exec DH1:C/liblist_probe idstring vulkan_loader' "$D/calls.log" && f="$f
dry ok: no liblist_probe after the ladder"
[ "$(grep -cE '^submit --put [^ ]*/m0b23/ahi/[^ :]+:RAM:kitty/Devs/' "$D/calls.log")" = 8 ] && [ "$(grep -n '/ahi/void\.audio:' "$D/calls.log" | cut -d: -f1)" -lt "$(grep -n '^submit --exec Assign DEVS: RAM:kitty/Devs' "$D/calls.log" | cut -d: -f1)" ] || f="$f
dry ok: the AHI set isn't 8 puts before the DEVS: assign (D70)"
m7a=$(grep -nE '^submit --put [^ ]*/m0b23/ahi/[^ :]+:RAM:kitty/Devs/' "$D/calls.log" | tail -1 | cut -d: -f1)
m7b=$(grep -nE '^submit --exec MakeDir RAM:kitty/Devs/AudioModes( |$)' "$D/calls.log" | cut -d: -f1)
m7c=$(grep -nE '^submit --exec Copy AROS:Devs/AudioModes/HDAUDIO RAM:kitty/Devs/AudioModes/HDAUDIO( |$)' "$D/calls.log" | cut -d: -f1)
m7d=$(grep -n '^submit --exec Assign ENVARC: DH1:W' "$D/calls.log" | head -1 | cut -d: -f1)
{ [ -n "$m7a" ] && [ -n "$m7d" ] && [ "$(echo "$m7b" | grep -c .)" = 1 ] && [ "$(echo "$m7c" | grep -c .)" = 1 ] && [ "$m7a" -lt "$m7b" ] && [ "$m7b" -lt "$m7c" ] && [ "$m7c" -lt "$m7d" ]; } || f="$f
dry ok: want the AudioModes MakeDir, then the HDAUDIO Copy once, after the last AHI put and before the first prelude line (D78) (put=${m7a:-none} mkdir=$(echo $m7b) copy=$(echo $m7c) prelude=${m7d:-none})"
m8a=$(grep -nE '^submit --exec Copy AROS:Prefs/Env-Archive/hdaudio\.config DH1:W/hdaudio\.config( |$)' "$D/calls.log" | cut -d: -f1)
{ [ -n "$m7c" ] && [ -n "$m7d" ] && [ "$(echo "$m8a" | grep -c .)" = 1 ] && [ "$m7c" -lt "$m8a" ] && [ "$m8a" -lt "$m7d" ]; } || f="$f
dry ok: want the hdaudio.config Copy once, after the HDAUDIO Copy and before the first prelude line (D81) (modes=$(echo $m7c) config=$(echo $m8a) prelude=${m7d:-none})"
fs=$(grep -n '^submit --get RAM:kitty/out/sh_soak\.log:' "$D/calls.log" | tail -1 | cut -d: -f1)
ap=$(grep -n '^submit --exec Type ENV:Sys/ahi\.prefs HEX >RAM:kitty/out/ahi_prefs\.txt' "$D/calls.log" | cut -d: -f1)
am=$(grep -n '^submit --exec List DEVS:AudioModes >RAM:kitty/out/audiomodes\.txt' "$D/calls.log" | cut -d: -f1)
lg=$(grep -n '^submit --exec Assign >RAM:kitty/out/assigns\.txt' "$D/calls.log" | cut -d: -f1)
ga=$(grep -n '^submit --get RAM:kitty/out/assigns\.txt:' "$D/calls.log" | cut -d: -f1)
{ [ -n "$fs" ] && [ "$(echo "$ap" | grep -c .)" = 1 ] && [ "$(echo "$am" | grep -c .)" = 1 ] && [ "$(echo "$lg" | grep -c .)" = 1 ] && [ "$(echo "$ga" | grep -c .)" = 1 ] && [ "$fs" -lt "$ap" ] && [ "$ap" -lt "$am" ] && [ "$am" -lt "$lg" ] && [ "$lg" -lt "$ga" ]; } || f="$f
dry ok: want the AHI prefs read, the AudioModes list and the Assign listing once each, in that order after the last sh_soak.log fetch, then assigns.txt fetched (D75) (fetch=${fs:-none} prefs=$(echo $ap) modes=$(echo $am) listing=$(echo $lg) get=$(echo $ga))"
od=$(grep -oE '^submit --exec Execute RAM:kitty/run_[a-z0-9]+' "$D/calls.log" | sed 's/.*run_//' | tr '\n' ' '); [ "$od" = "p pt ptr h b1 od2 od4 lp0 lp1 w fifo lp4 a soak " ] || f="$f
dry ok: runs not in D65's order: $od"
r=$(dry fault STUB_FAULT=1); D=$B/dry-fault
ty=$(grep -c '^submit --exec Type RAM:kitty/out/rc_w.txt' "$D/calls.log")
{ [ "$r" = 0 ] && [ "$(nl "$D/dead.txt")" = 3 ] && [ "$(nl "$D/host_wall.txt")" = 12 ] && [ "$ty" -le 3 ] && ! grep -qE '^submit --exec Execute RAM:kitty/run_(soak|a\.)' "$D/calls.log" && grep -q 'lane_run: 3 dead, a not started' "$D/out.log" && grep -q 'lane_run: no screendump for w' "$D/out.log" && grep -q 'core_IRQHandle' "$D/serial_w.txt" && [ "$(last "$D/calls.log")" = stop ]; } || f="$f
dry fault: want rc 0, 3 dead, 12 runs, at most 3 polls of rc_w, soak and a not started, a failed screendump tolerated, the fault in serial_w.txt, stop last (rc=$r dead=$(nl "$D/dead.txt") runs=$(nl "$D/host_wall.txt") polls=$ty last=$(last "$D/calls.log"))"
r=$(dry adead STUB_ADEAD=1); D=$B/dry-adead
{ [ "$r" = 0 ] && [ "$(cat "$D/dead.txt")" = a ] && [ "$(nl "$D/host_wall.txt")" = 13 ] && ! grep -q '^submit --exec Execute RAM:kitty/run_soak' "$D/calls.log" && grep -q 'lane_run: a dead, soak not started' "$D/out.log" && [ "$(last "$D/calls.log")" = stop ]; } || f="$f
dry adead: want rc 0, only a dead, 13 runs, soak not started, stop last (rc=$r dead=$(cat "$D/dead.txt" | xargs) runs=$(nl "$D/host_wall.txt") last=$(last "$D/calls.log"))"
r=$(dry prelude STUB_PRELUDE_FAIL=1); D=$B/dry-prelude
{ [ "$r" = 10 ] && grep -q 'prelude failed at line 2: Assign LIBS: DH1:Libs' "$D/out.log" && ! grep -q '^submit --exec Assign LIBS: ADD' "$D/calls.log" && ! grep -q '^submit --exec Execute' "$D/calls.log" && [ "$(last "$D/calls.log")" = stop ]; } || f="$f
dry prelude-fail: want rc 10 'prelude failed at line 2', no later prelude line, no Execute, stop last (rc=$r last=$(last "$D/calls.log"))"
r=$(dry wedge STUB_WEDGE=1); D=$B/dry-wedge
{ [ "$r" = 6 ] && grep -q 'guest agent wedged at run p' "$D/out.log" && [ "$(nl "$D/host_wall.txt")" = 1 ] && [ "$(grep -A1 '^submit --exec Execute RAM:kitty/run_p\.aros' "$D/calls.log" | tail -1)" = stop ]; } || f="$f
dry wedge: want rc 6 'guest agent wedged at run p', 1 run, nothing sent after run_p, stop last (rc=$r runs=$(nl "$D/host_wall.txt") after=$(grep -A1 '^submit --exec Execute RAM:kitty/run_p\.aros' "$D/calls.log" | tail -1))"
r=$(dry running STUB_RUNNING=1); D=$B/dry-running
nt=$(grep -c '^submit --exec Type RAM:kitty/out/rc_soak.txt' "$D/calls.log"); ne=$(grep -c '^submit --exec Exists' "$D/calls.log")
{ [ "$r" = 0 ] && [ "$(nl "$D/host_wall.txt")" = 14 ] && [ "$nt" = 3 ] && [ "$ne" = 0 ] && grep -q '^soak start=' "$D/host_wall.txt" && [ "$(last "$D/calls.log")" = stop ]; } || f="$f
dry soak RUNNING: want rc 0, 14 runs, 3 Type polls of rc_soak.txt (the third finds it), no Exists, stop last (rc=$r runs=$(nl "$D/host_wall.txt") type_polls=$nt exists=$ne last=$(last "$D/calls.log"))"
r=$(dry listing STUB_LISTING_WEDGE=1); D=$B/dry-listing
la=$(grep -A1 '^submit --exec Assign >RAM:kitty/out/assigns\.txt' "$D/calls.log" | tail -1)
{ [ "$r" = 11 ] && grep -q 'guest agent wedged at the Assign listing; the run evidence is already home' "$D/out.log" && [ "$(nl "$D/host_wall.txt")" = 14 ] && [ "$la" = stop ]; } || f="$f
dry listing-wedge: want rc 11, the wedge line, 14 runs, nothing sent after the listing, stop last (rc=$r runs=$(nl "$D/host_wall.txt") after=$la)"
r=$(dry modes STUB_MODES_FAIL=1); D=$B/dry-modes
{ [ "$r" = 7 ] && grep -q 'lane_run: AudioModes copy failed' "$D/out.log" && ! grep -qE '^submit --exec (Assign|Execute) ' "$D/calls.log" && [ "$(last "$D/calls.log")" = stop ]; } || f="$f
dry modes-fail: want rc 7, the failure line, no Assign and no Execute sent, stop last (rc=$r last=$(last "$D/calls.log"))"
r=$(dry envarc STUB_ENVARC_FAIL=1); D=$B/dry-envarc
{ [ "$r" = 7 ] && grep -q 'lane_run: hdaudio.config copy failed' "$D/out.log" && ! grep -qE '^submit --exec (Assign|Execute) ' "$D/calls.log" && [ "$(last "$D/calls.log")" = stop ]; } || f="$f
dry envarc-fail: want rc 7, the failure line, no Assign and no Execute sent, stop last (rc=$r last=$(last "$D/calls.log"))"
r=$(dry probe STUB_PROBE_FAIL=1); D=$B/dry-probe
{ [ "$r" = 8 ] && grep -q 'probe failed; ladder not run' "$D/out.log" && [ "$(nl "$D/host_wall.txt")" = 3 ] && [ "$(last "$D/calls.log")" = stop ]; } || f="$f
dry probe-fail: want rc 8, 3 runs, stop last (rc=$r runs=$(nl "$D/host_wall.txt") last=$(last "$D/calls.log"))"
r=$(dry launch STUB_LAUNCH_RC=1); D=$B/dry-launch
{ [ "$r" = 5 ] && [ "$(nl "$D/boots.txt")" = 1 ] && [ "$(last "$D/calls.log")" = stop ]; } || f="$f
dry launcher-fail: want rc 5, the ledger line kept, stop last (rc=$r ledger=$(nl "$D/boots.txt") last=$(last "$D/calls.log"))"
r=$(PLANT_STRAY=1 dry stray); D=$B/dry-stray
{ [ "$r" = 3 ] && grep -q 'ledger mismatch' "$D/out.log" && [ ! -s "$D/calls.log" ] && [ "$(nl "$D/boots.txt")" = 0 ]; } || f="$f
dry stray boot record: want rc 3 'ledger mismatch', no launch, no stop, no ledger line (rc=$r ledger=$(nl "$D/boots.txt") calls: $(tr '\n' ' ' < "$D/calls.log"))"
x=$(grep -l 'unbound variable' "$B"/dry-*/out.log 2>/dev/null); [ -z "$x" ] || f="$f
unbound variable in: $x"
ss -ltnH '( sport = :9099 or sport = :9100 )' 2>/dev/null | grep -q . && f="$f
a server is still listening after the dry boots"
[ "$(lw)" = "$lw0" ] && [ "$(grep -c . "$B/boots.txt")" = "$nb0" ] || f="$f
a dry boot wrote under ~/Work/lanes or to build/m0b23/boots.txt"
chk bootpath "$f"
fi
f=""; held(){ ss -ltnH "( sport = :$1 )" 2>/dev/null | grep -q .; }
if held 9099 || held 9100; then g S14b.transport BLOCKED "9099/9100 held before the check: $(ss -ltnpH '( sport = :9099 or sport = :9100 )' | head -2 | tr '\n' ' ')"; else
  lw(){ { find ~/Work/lanes/l0025 -newer "$B/baseline.done" 2>/dev/null; ls ~/Work/lanes/*/boot-*.txt 2>/dev/null; } | sort; }
  lb0=$(lw); n0=$(grep -c . "$B/boots.txt"); rm -rf "$B/tcheck"
  o=$(PS4='+TRACE ' timeout 60 bash -x "$R" --check-transport "$B/tcheck" 2>&1); rc=$?
  echo "$o" | grep -vE '^\++TRACE' | tail -3
  pid=$(echo "$o" | grep -vE '^\++TRACE' | sed -n 's/.*transport ok pid=\([0-9][0-9]*\).*/\1/p' | tail -1)
  tr=$( { echo "$o"; cat "$B/tcheck/spike.log" 2>/dev/null; } | grep -E '^\++TRACE')
  grep -qF "[srv] persistent multi-agent server on :9099  spool=$B/tcheck/tspool " "$B/tcheck/spike.log" 2>/dev/null || f="$f
the server's own banner in $B/tcheck/spike.log does not show :9099 and spool $B/tcheck/tspool:
$(grep -F '[srv]' "$B/tcheck/spike.log" 2>/dev/null | head -2)"
  echo "$tr" | grep -qE '^\++TRACE preflight ' || f="$f
no preflight call in the --check-transport trace"
  echo "$tr" | grep -qE "^\++TRACE (exec )?python3 -B -u /[^ ]*/spike_server\.py serve --port 9099 --bulk-port 9100 --spool $B/tcheck/tspool\$" || f="$f
traced server command is not 'python3 -B -u <abs>/spike_server.py serve --port 9099 --bulk-port 9100 --spool $B/tcheck/tspool':
$(echo "$tr" | grep -E 'spike_server' | head -3)"
  echo "$tr" | grep -E 'python3' | grep -qE '[[:space:]]-I([[:space:]]|$)' && f="$f
a traced python3 command uses -I"
  { [ $rc = 0 ] && [ -n "$pid" ] && ! kill -0 "$pid" 2>/dev/null && ! held 9099 && ! held 9100; } || f="$f
--check-transport did not start, prove and stop its server (rc=$rc pid=${pid:-none})"
  python3 -I -c 'import socket,time
s=socket.socket(); s.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1); s.bind(("0.0.0.0",9099)); s.listen(1); time.sleep(40)' & pp=$!; sleep 1
  o2=$(timeout 30 bash "$R" --check-transport "$B/tcheck2" 2>&1); rc2=$?
  kill -0 "$pp" 2>/dev/null && kp=alive || kp=dead; kill "$pp" 2>/dev/null; wait "$pp" 2>/dev/null
  { [ $rc2 = 9 ] && echo "$o2" | grep -q 'port 9099 held' && [ $kp = alive ]; } || f="$f
with 9099 held by a plant, --check-transport did not refuse with exit 9 (rc=$rc2, plant $kp): $(echo "$o2" | tail -2)"
  [ "$kp" = dead ] && f="$f
the plant died during the check: the runner stopped a listener it did not start (D40b)"
  lb1=$(lw); [ "$lb1" = "$lb0" ] && [ "$(grep -c . "$B/boots.txt")" = "$n0" ] || f="$f
--check-transport wrote under ~/Work/lanes/l0025, a boot record or boots.txt: $(comm -13 <(echo "$lb0") <(echo "$lb1") | head -3)"
  chk transport "$f"
fi
f=""; held(){ ss -ltnH "( sport = :$1 )" 2>/dev/null | grep -q .; }
if [ "$(sha256sum < "$R" | cut -d' ' -f1)" != 2d182f1d28d37dc77a0a74b0f114da5c2fb5f55b2e443b86cd85084459af808f ]; then g S14b.realpath BLOCKED "tests/aros/lane_run.sh is not the supplied runner (S14b.pinned); its real-mode path is not run"
elif held 9099 || held 9100; then g S14b.realpath BLOCKED "9099/9100 held before the check"
else
  F=$B/fakehome; rm -rf "$F" "$B"/real-*; P=$F/Work/Kitty/Docs/Development/dispatch; L=$F/Work/lanes/l0025; V=$F/Work/projects/Vulkan4Aros/scripts
  mkdir -p "$F/bin" "$P" "$L" "$V"
  printf '#!/bin/bash\necho "systemctl $*" >> "$LR_CALLS"\n' > "$F/bin/systemctl"
  printf '#!/bin/bash\necho "real launcher $*" >> "$LR_CALLS"\nexit 1\n' > "$V/lane_l0025_boot.sh"
  chmod +x "$F/bin/systemctl" "$V/lane_l0025_boot.sh"
  lw(){ { find ~/Work/lanes/l0025 ~/Work/lanes/kitty-* -newer "$B/baseline.done" 2>/dev/null; ls ~/Work/lanes/*/boot-*.txt 2>/dev/null; } | sort -u; }
  lw0=$(lw); nb0=$(grep -c . "$B/boots.txt")
  fp(){ printf '    BOOTS GRANTED: %s\n    LANE: l0025\n' "$1" > "$P/m0b23_aros_guest_opencode_prompt.md"; }
  recs(){ rm -f "$L"/boot-*.txt; i=0; while [ "$i" -lt "$1" ]; do : > "$L/boot-fake-$i.txt"; i=$((i + 1)); done; }
  plant(){ python3 -I -c 'import socket,time
s=socket.socket(); s.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1); s.bind(("0.0.0.0",9099)); s.listen(1); time.sleep(40)' & pp=$!; sleep 1; }
  unplant(){ kill -0 "$pp" 2>/dev/null && kp=alive || kp=dead; kill "$pp" 2>/dev/null; wait "$pp" 2>/dev/null; }
  real(){ D=$B/real-$1; mkdir -p "$D"; : > "$D/calls.log"; env HOME="$F" PATH="$F/bin:$PATH" LR_CALLS="$D/calls.log" timeout 60 bash "$R" l0025 "$D/out" > "$D/out.log" 2>&1; echo $?; }
  calls(){ tr '\n' ' ' < "$1/calls.log"; }
  fp 0; recs "$nb0"; plant; r=$(real grant0); unplant; D=$B/real-grant0
  { [ "$r" = 3 ] && grep -q 'no boots granted' "$D/out.log" && [ "$kp" = alive ] && [ ! -s "$D/calls.log" ]; } || f="$f
real mode, grant 0, 9099 held by a plant: want rc 3 'no boots granted', the plant alive, no systemctl or launcher call (rc=$r plant=$kp calls: $(calls "$D"))"
  fp $((nb0 + 1)); recs $((nb0 + 1)); r=$(real stray); D=$B/real-stray
  { [ "$r" = 3 ] && grep -q "ledger mismatch: $((nb0 + 1)) boot records, $nb0 ledger lines" "$D/out.log" && [ ! -s "$D/calls.log" ]; } || f="$f
real mode, one boot record more than ledger lines: want rc 3 'ledger mismatch: $((nb0 + 1)) boot records, $nb0 ledger lines', no systemctl or launcher call (rc=$r calls: $(calls "$D")): $(tail -2 "$D/out.log")"
  fp $((nb0 + 1)); recs "$nb0"; plant; r=$(real held); unplant; D=$B/real-held
  { [ "$r" = 9 ] && grep -q 'port 9099 held' "$D/out.log" && [ "$kp" = alive ] && [ ! -s "$D/calls.log" ]; } || f="$f
real mode, grant open, 9099 held by a plant: want rc 9 'port 9099 held', the plant alive, no systemctl or launcher call (rc=$r plant=$kp calls: $(calls "$D"))"
  x=$(grep -l 'unbound variable' "$B"/real-*/out.log 2>/dev/null); [ -z "$x" ] || f="$f
unbound variable in: $x"
  held 9099 || held 9100 && f="$f
a server is still listening after the real-mode checks"
  [ "$(lw)" = "$lw0" ] && [ "$(grep -c . "$B/boots.txt")" = "$nb0" ] || f="$f
a real-mode check wrote under ~/Work/lanes or to build/m0b23/boots.txt"
  rm -rf "$F"; chk realpath "$f"
fi
````

**Script 16: guest staging and commands (read-only, no boot).** `bash "$B/scripts/s16.sh"`, after 14b, before the runner.

````bash
# script s16
cd "$KR" || exit 1; NAV=$VK/scripts/sfs_nav.py; IM=~/Work/vms; S=$B/staging; mkdir -p "$S"
if pgrep -f '^[^ ]*qemu-system.*aros_l0025' >/dev/null; then g S16.guest-staging BLOCKED "lane l0025 is running: images are not read while a guest has them open"; g S16.commands BLOCKED "no DH0 tree"; g S16.audio BLOCKED "no DH0 tree"; else
for d in dh0 dh1; do python3 -I "$NAV" "$IM/aros_l0025_$d.img" tree > "$S/tree-$d.txt" 2>&1 || echo "tree $d rc=$?"; done
python3 -I - "$NAV" "$IM/aros_l0025_dh1.img" "$S" <<'PY' && g S16.guest-staging PASS || g S16.guest-staging FAIL
import sys,re,json,subprocess,hashlib
nav,img,S=sys.argv[1:]; bad=[]
T={m[0]:int(m[1]) for m in re.findall(r'^(/\S+) size=(\d+)$',open(f'{S}/tree-dh1.txt').read(),re.M)}
def cat(p,n):
    b=subprocess.run([sys.executable,'-I',nav,img,'cat',p],capture_output=True).stdout[:n]
    return b if len(b)==n else None
for p in ('/Libs/vulkan_loader.library','/W/vulkan_icd.json'):
    if p not in T: bad.append(f'DH1:{p[1:]} missing')
if not bad:
    m=cat('W/vulkan_icd.json',T['/W/vulkan_icd.json'])
    try: lp=json.loads(m)['ICD']['library_path']
    except Exception as e: lp=None; bad.append(f'DH1:W/vulkan_icd.json unreadable: {e}')
    if lp:
        if not lp.startswith('DH1:') or '/'+lp[4:] not in T: bad.append(f'manifest library_path {lp} is not a file on DH1:')
        else: print(f'icd library_path={lp} size={T["/"+lp[4:]]}')
    ld=cat('Libs/vulkan_loader.library',T['/Libs/vulkan_loader.library'])
    if ld is None: bad.append('loader extract short')
    else:
        lh=hashlib.sha256(ld).hexdigest(); print(f'loader DH1:Libs/vulkan_loader.library size={len(ld)} sha256={lh}')
        if lh!='90237ed20bf7527782253e6853ee66bd442f16247c9463df2a22fa0a1fb536f3': bad.append('DH1 loader is not the fixv3 copy (D69)')
    if '/C/liblist_probe' not in T: bad.append('DH1:C/liblist_probe missing')
print('\n'.join(bad)); sys.exit(1 if bad else 0)
PY
c=$( { grep -oE 'submit --exec "[A-Za-z][A-Za-z0-9_]*' tests/aros/lane_run.sh | sed 's/.*"//'; grep -hvE '^[[:space:]]*(;|$)' tests/aros/*.aros | awk '{print $1}'; } | grep -v '^RAM:' | sort -u)
f=""; for x in $c; do grep -qiE "^/C/$x size=" "$S/tree-dh0.txt" 2>/dev/null || f="$f $x"; done
echo "guest commands used: $(echo $c)"; [ -n "$c" ] && [ -z "$f" ] && g S16.commands PASS || g S16.commands FAIL "not in DH0:C (tree-dh0.txt):${f:- none found}"
  f=""; L=$VK/scripts/lane_l0025_boot.sh
  { grep -q 'LANE_AUDIO' "$L" && grep -q 'device intel-hda -device hda-duplex,audiodev=snd' "$L"; } || f="the launcher has no LANE_AUDIO knob: run the owner's vk4aros_lane_audio round first"
  while read -r h n; do
    case "$n" in *.device) p=/Devs/$n ;; *) p=/Devs/AHI/$n ;; esac
    x=$(python3 -B -I "$NAV" "$IM/aros_l0025_dh0.img" cat "$p" 2>/dev/null | sha256sum | cut -d' ' -f1); [ "$x" = "$h" ] || f="$f
DH0 $p is $x, want $h"
  done <<'SHAS'
a67693dacb7fc593464fe4e60bd4bb79e5e896482fbc79cd045141e3404a2b6e ac97.audio
4cd71c6bbc1b2669447b2d3c21a6c81e6e046785f490b5ad71aab278da615190 ahi.device
f56d374ca5e9e512d3e6fec01f84e689944308c97b3b5e04f299b015f0532bcd cmi8738.audio
c5a167be4fc20bcc537dcff90da4968a9d4cb91979116396ea5f197bc3a23cb1 hdaudio.audio
59727605ea572fe457288d7099752e1bd5e215f26ea4c51c85a5ce3ce0afe8da nvhdmi.audio
f1f754ab729618edf8af15c4c50cf14f8c2124e5f52201dbe47a3d14fe356c0a sb128.audio
ac49e843fc55b52e3731a0b32dd3f8441e5b309b736e43bc94c91e14fb4fa872 via-ac97.audio
e98981a4d6ba5427b72a2173e48f17b201f2e4ac44ea88f6fade734bd76c52d2 void.audio
SHAS
  grep -qx '/Prefs/Env-Archive/hdaudio.config size=2182' "$S/tree-dh0.txt" || f="$f
DH0:Prefs/Env-Archive/hdaudio.config is not 2182 B in tree-dh0.txt (D82)"
  echo "DH1:W/hdaudio.config before the boots: $(grep -E '^/W/hdaudio\.config size=' "$S/tree-dh1.txt" || echo absent)"
  [ -z "$f" ] && g S16.audio PASS || { echo "$f"; g S16.audio FAIL; }
fi
````

S16 FAIL or BLOCKED means **don't boot**: S15 and S15b are BLOCKED, and the report says what the
staging lacks. The owner may change the staging in parallel; S16 reads it as it is when you run it.

**Boots: what the block above grants, on l0025 only** (D67). No TCG, no other lane, never more boots than the block grants;
a refused or failed launch keeps its `boots.txt` line and counts.

**Runner** (only when `BOOTS GRANTED` > 0, `LANE` is `l0025`, and S14, S14b and S16 are all PASS in
the current pass — S14b.transport, S14b.bootpath, S14b.realpath and S16.commands included): start it
as D57 says, with `n` = 1, then, once it has exited, with `n` = 2 (D73). Its logs are
`build/m0b23/logs/runner-boot1.log` and `runner-boot2.log`; give the paths.

- Each boot runs the whole sequence: make the package's directories, deploy (one `--put` per file,
  then `List RAM:kitty` checked), the AHI set, D78's mode-file copy, D81's config copy, the `out/` tree, the prelude as five execs, `CD`, `p`,
  `pt`, `ptr`, then — only if `p` passed (D33) — the eleven M0b runs, the fetch, and D75's three
  late execs. The soak alone takes over 30
  minutes; a whole boot about an hour, so two boots about two.
- Boot 2 starts once boot 1's runner has exited, whatever boot 1 showed, unless boot 1's runner
  exited 3 or 9 (D73). Edit nothing in between.
- When the last runner has exited, grade one boot (D76 as amended by D92), in the repo:

  ```bash
  c(){ if [ -d "build/m0b23/guest-boot$1" ]; then ls "build/m0b23/guest-boot$1"/rc_*.txt 2>/dev/null | wc -l; else echo -1; fi; }
  f(){ if [ -d "build/m0b23/guest-boot$1" ]; then ls -A "build/m0b23/guest-boot$1" | wc -l; else echo -1; fi; }
  a=$(c 1); b=$(c 2); fa=$(f 1); fb=$(f 2)
  if [ "$a" -gt "$b" ] || { [ "$a" = "$b" ] && [ "$fa" -gt "$fb" ]; }; then g=1; else g=2; fi
  echo "graded=$g boot1_rc=$a boot2_rc=$b boot1_files=$fa boot2_files=$fb" | tee build/m0b23/graded_boot.txt
  cp -a "build/m0b23/guest-boot$g" build/m0b23/guest
  ```

  then run S15, S15b, 12, 12b, 12c. No fix and no third boot, whatever the boots show (D73).

**Script 15b: the M0b.1 guest gates.** `bash "$B/scripts/s15b.sh"`, after script 15.

````bash
# script s15b
cd "$KR" || exit 1; G=$B/guest
N="wiring probe trap-clean plant-release output-captured loader-id loader-on-dh1 sound ahi-source ahi-mode envarc drivers"
if [ "${GRANT:-0}" = 0 ] || [ "$LANE" = none ] || [ ! -f "$G/host_wall.txt" ]; then
  why="no boots granted"; [ "${GRANT:-0}" != 0 ] && why="no guest results in $G"; for x in $N; do g "S15b.$x" BLOCKED "$why"; done
else python3 -I - "$G" <<'PY'; fi
import sys,re,os,glob
G=sys.argv[1]
def gate(n,ok,why=''):
    s='PASS' if ok is True else ('BLOCKED' if ok is None else 'FAIL'); l=f'GATE S15b.{n} {s}'+(f' — {why}' if why else '')
    print(l); open(os.path.join(os.path.dirname(G),'census.txt'),'a').write(l+'\n')
def rd(p): p=os.path.join(G,p); return open(p,errors='replace').read() if os.path.exists(p) else ''
def rc(r): m=re.search(r'rc=(\d+)',rd(f'rc_{r}.txt')); return int(m.group(1)) if m else None
a=rd('assigns.txt')
gate('wiring', bool(re.search(r'^\s*LIBS:?\s+(DH1|L25DATA):Libs\b',a,re.M|re.I)) and bool(re.search(r'^\s*ENVARC:?\s+(DH1|L25DATA):W\b',a,re.M|re.I)) and bool(re.search(r'^\s*DEVS:?\s+RAM[^\n]*:kitty/Devs\s*\n\s*\+\s*AROS:Devs\b',a,re.M|re.I)), 'assigns.txt: '+(' | '.join(l.strip() for l in a.splitlines() if re.search(r'LIBS|ENVARC|DH1|L25DATA|AROS:Libs',l,re.I))[:300] or 'missing'))
p=rd('run_p.log')
gate('probe', rc('p')==0 and 'vk_loader="vulkan_loader.library"' in p and 'Software Failure' not in p and re.search(r'(^|\s)frames=[1-9]',p) is not None, f"rc={rc('p')} {(re.findall(r'vk_loader=\S+',p) or ['no vk_loader line'])[-1]}")
t=rd('run_pt.log'); un=[os.path.basename(x) for x in sorted(glob.glob(f'{G}/exec_*.log')) if 'unfreed' in open(x,errors='replace').read()]
gate('trap-clean', rc('pt')==4 and re.search(r'^Software Failure\. src/platform/aros/[A-Za-z0-9_/]+\.(cpp|c):[1-9][0-9]*: vulkan loader open failed',t,re.M) is not None and not un, f"rc={rc('pt')} unfreed_in={un}")
gate('plant-release', rc('ptr')==2, f"rc={rc('ptr')}")
emp=[r for r in re.findall(r'^(\w+) start=',rd('host_wall.txt'),re.M) if rc(r) is not None and not rd(f'run_{r}.log').strip()]
wr=[r for r in re.findall(r'^(\w+) start=',rd('host_wall.txt'),re.M) if rc(r) is not None]
gate('loader-id', 'fixv3' in rd('loader_id.txt'), 'loader_id.txt: '+(' | '.join(l.strip() for l in rd('loader_id.txt').splitlines() if 'idstr' in l)[:200] or 'no idstr line'))
import subprocess,hashlib
if subprocess.run(['pgrep','-f','^[^ ]*qemu-system.*aros_l0025'],capture_output=True).returncode==0: gate('loader-on-dh1',None,'lane l0025 is running')
else:
    h=hashlib.sha256(subprocess.run([sys.executable,'-I',os.path.expanduser('~/Work/projects/Vulkan4Aros/scripts/sfs_nav.py'),os.path.expanduser('~/Work/vms/aros_l0025_dh1.img'),'cat','Libs/vulkan_loader.library'],capture_output=True).stdout[:850472]).hexdigest()
    gate('loader-on-dh1',h=='90237ed20bf7527782253e6853ee66bd442f16247c9463df2a22fa0a1fb536f3',f'image sha {h}')
src=re.findall(r'^audio_source=(\S+)',rd('run_a.log'),re.M)+re.findall(r'^audio_source=(\S+)',rd('serial_a.txt'),re.M)
gate('ahi-source', 'PROGDIR:Devs/ahi.device' in src, f'audio_source={src[0] if src else None}')
ra=rd('run_a.log'); hm=re.search(r'^ahi_mode id=\S+ name="[^"\n]*" driver="hdaudio"$',ra,re.M); hb=re.findall(r'^ahi_best id=\S+ name="[^"\n]*"$',ra,re.M); hp=re.findall(r'^ahi_probe .*$',ra,re.M)
gate('ahi-mode', hm is not None and bool(hb) and hb[0].split('name="',1)[1].startswith('hdaudio:'), f"{hp[0] if hp else 'no ahi_probe line'} | {hb[0] if hb else 'no ahi_best line'}")
dv=re.findall(r'^\s*DRIVERS:?\s+(\S.*?)\s*$',a,re.M|re.I); hd=re.findall(r'^ahi_drivers .*$',ra,re.M)
hpl=re.findall(r'^ahi_hidd_preload=\d+$',ra,re.M)
gate('drivers', hpl[:1]==['ahi_hidd_preload=1'] and bool(hd) and re.search(r'(^|\s)hidd_before=1(\s|$)',hd[0]) is not None, f"{hpl[0] if hpl else 'no ahi_hidd_preload line'} | {hd[0] if hd else 'no ahi_drivers line'} | DRIVERS {dv[0] if dv else 'missing'}")
if subprocess.run(['pgrep','-f','^[^ ]*qemu-system.*aros_l0025'],capture_output=True).returncode==0: gate('envarc',None,'lane l0025 is running')
else:
    eb=subprocess.run([sys.executable,'-I',os.path.expanduser('~/Work/projects/Vulkan4Aros/scripts/sfs_nav.py'),os.path.expanduser('~/Work/vms/aros_l0025_dh1.img'),'cat','W/hdaudio.config'],capture_output=True).stdout[:2182]
    eh=hashlib.sha256(eb).hexdigest()
    gate('envarc', len(eb)==2182 and eh=='3645c2cbc4b21d0af81c7c0b6f3fa72c68b12d0e7ecc79745741da8de6e70ae7', f'DH1:W/hdaudio.config {len(eb)} B sha {eh}')
wp=os.path.join(G,'lane_audio.wav'); nz=0
if os.path.exists(wp):
    with open(wp,'rb') as w_:
        w_.read(44)
        for ch in iter(lambda: w_.read(1<<20),b''): nz+=len(ch)-ch.count(0)
gate('sound', nz>=176400, f"lane_audio.wav {os.path.getsize(wp) if os.path.exists(wp) else 'missing'} B, non-zero bytes={nz}")
lost=[r for r in 'p pt ptr h b1 od2 od4 lp0 lp1 w fifo a lp4 soak'.split() if rc(r) is None]
gate('output-captured', bool(wr) and not emp and not lost, f'runs with an rc: {len(wr)}; runs with an rc but an empty log: {emp}; runs with no rc: {lost}; fault lines: {[f"{r} serial_{r}.txt:{i} {l[-60:]}" for r in lost for i,l in enumerate(rd(f'serial_{r}.txt').splitlines(),1) if 'core_IRQHandle' in l]}')
PY
````

**Script 12b: the boot ledger.** `bash "$B/scripts/s12b.sh"`, after script 12.

````bash
# script s12b
cd "$KR" || exit 1
new=$(ls ~/Work/lanes/*/boot-*.txt 2>/dev/null | sort | comm -13 "$B/lane_boots_start.txt" -)
nb=$(echo "$new" | grep -c .); nl=$(grep -c . "$B/boots.txt"); oth=$(echo "$new" | grep -v "/lanes/$LANE/" | grep .)
echo "boot records since script 0: $nb"; echo "$new" | grep .; echo "boots.txt lines: $nl (grant ${GRANT:-0}, lane ${LANE:-none})"; cat "$B/boots.txt"
if [ "$nb" = "$nl" ] && [ -z "$oth" ] && [ "$nl" -le "${GRANT:-0}" ]; then g S12b.ledger PASS
else g S12b.ledger FAIL "records=$nb ledger=$nl grant=${GRANT:-0}${oth:+ other-lane records}"; fi
````

**Script 12c: census completeness and script integrity (D46, D50, D55).** `bash "$B/scripts/s12c.sh"`, last, after 12b; nothing runs after it.

````bash
# script s12c
cd "$KR" || exit 1
python3 -I - "$B" "${GRANT:-0}" <<'PY' && g S12c.complete PASS || g S12c.complete FAIL
import sys,re,os
B,G=sys.argv[1],int(sys.argv[2]); L=[l.split()[1] for l in open(f'{B}/census.txt') if l.startswith('GATE ')]
order=['S1','S2','S3','S4','S5','S6','S7','S8','S9','S10','S11','S13','S14','S14b','S16','S15','S15b','S12','S12b']
sec=[]; [sec.append(x.split('.')[0]) for x in L if not sec or sec[-1]!=x.split('.')[0]]
bad=[]
if sec!=order: bad.append(f'sections in census: {" ".join(sec)}; want: {" ".join(order)}')
once=['S2.interfaces','S3.budgets','S5.build','S13.frames-unchanged','S13.hash10s-unchanged','S14.aros-build','S14.aros-flags','S14.aros-package','S14.runner-refuses','S14b.run-scripts','S14b.wiring','S14b.runner','S14b.plant-and-package','S14b.preflight','S14b.makefile','S14b.pinned','S14b.aros-fix','S14b.force-fifo','S14b.bootpath','S14b.transport','S14b.realpath','S16.guest-staging','S16.commands','S16.audio','S12.end-state','S12b.ledger']
for n in once:
    if L.count(n)!=1: bad.append(f'{n} appears {L.count(n)}x')
for n in ('S5.link','S14.aros-binary'):
    if L.count(n)!=2: bad.append(f'{n} appears {L.count(n)}x (want 2)')
d=[x for x in set(L) if L.count(x)>1 and x not in ('S5.link','S14.aros-binary')]
if d: bad.append(f'gates repeated within the pass: {sorted(d)}')
m=open(f'{B}/make_aros.log',errors='replace').read() if os.path.exists(f'{B}/make_aros.log') else ''
ag=re.findall(r'^aros-gates: .*r12=(\d+)$',m,re.M)
if ag!=['0','0']: bad.append(f'make_aros.log aros-gates r12 values: {ag} (want two, both 0)')
t0=os.path.getmtime(f'{B}/baseline.done'); H=os.path.expanduser('~/Work/lanes'); w=[]
for top in sorted(os.listdir(H)) if os.path.isdir(H) else []:
    if not (top.startswith('kitty-') or (top=='l0025' and G==0)): continue
    for r,ds,fs in os.walk(os.path.join(H,top)):
        w+=[os.path.join(r,x) for x in fs+ds if os.path.getmtime(os.path.join(r,x))>t0]
if w: bad.append('written under ~/Work/lanes during the round (D50): '+' '.join(w[:5]))
import hashlib
sh=lambda p: hashlib.sha256(open(p,'rb').read()).hexdigest() if os.path.exists(p) else None
if sh(f'{B}/pre_files.txt')!='37779fc7dad798f69246290dd5344fe8fb93f1eca1f599225c9cd3bfe419420e': bad.append('pre_files.txt is not script 0\'s record of the pinned tree (D55)')
PIN={
    'hdr.sh': '25e01a71208face95bf7c2c3c959feade2e48ce3548e159dc65cbcfe3f99c53a',
    's0.sh': '1d26257643429bbb57ecd598e72e45100861fadbbe83ebad753c9d459b1ab8f2',
    's10.sh': 'fe0347304010c3403927672ef5f6fb707edd41a0df609b4cea6f8659286ce4b3',
    's11.sh': '5afadbae0702da6a15dae79cf5bcb1d0f8efb9b16f52b2e932ff764a78db7fe7',
    's12b.sh': '668e128fe0bd2e3a18a43988edf12642a19eb1a83d372e1f1d17d79ab16abbf6',
    's12.sh': 'fe26fdcd15a0baa08c6890aa739b028b5ea319fd07fa9596954196a3e9c633bb',
    's13.sh': '08debd4964ad059a8200874dbbe24498ee526a5c3401632ff04b627fbe02c35a',
    's14b.sh': 'e4101c93d067cc10e94e74c6a7a89e0fd3ea50d0ae7ec3e091a5c56f88b3fbac',
    's14.sh': '5be6b2da5336b347b1c342b1341e8063cb3a988ea84df9f2779f40f365411e36',
    's15b.sh': 'c4fe07bf67983f169f2b2daf5c3b009c27ca8ece3638c07c6a4a10c499d4cbd9',
    's15.sh': 'ca053f4f8443b6ae9c008b9af94d8bc9ba62c6eae268d91d82e69f261cfa3a4c',
    's16.sh': '58df6304bc6adec2605e794f775ef9558f2c2f503898bd57e471113ad3804db2',
    's1.sh': 'bd9da972de1659798d60d8c8e9af0990b29bbf9accb63b98f6b56850e0c92886',
    's2.sh': '998a9b155fdd527a2d50b386407c57c401b71663a6b21c32e5ba12f0649a7e6d',
    's3.sh': '0c5dd2d03e3977ba29af7d07f0e7b29c023aef4d5105117d6963fec8817e7e0c',
    's4.sh': 'a9c1f30ad520a6a829a3eeeadcfd599f0b94a8f922e84538c4e5a380da69dd13',
    's5.sh': '695f324fa57d14b131c9f108a7c49eca7d9723d5d31617c1818b982b88857709',
    's6.sh': '22bc7883745a2aef7a1136c23a7d7eee2b01e15c4c8220a3b4189e9a219cde0b',
    's7.sh': '8517e896e5651547746e874117088642eb30cbdec5b5f32f9eb03e9b31b064d0',
    's8.sh': '454bfa14ce4de597bb70e05aa318da6705e5d739430509593258c043b16d8fb7',
    's9.sh': '2222d48f4115ebcd0203e4e4f67d43b81a1550eb565341bac1f7567e27aaa887',
    'sX.sh': '6df73d01d27f0e81d26c0239c54d2f4fc525dba7c08e8ab3e74dcaa6b5585b91',
    'x0.sh': '88da0e5f332e2aeceeb9fb93fcfa85659ff354acf4280226d2826a2350051d86',
    'x1.sh': '28f6f1cea7a21b02a71077fa94237dae01332bb9dbcef0463abdd026c52e21d1',
    'x2.sh': '8b1a952aa530713ca33a7bda494f335a0e87dbfb210c3d925f4a2f6e54864aab',
    'x3.sh': '5f417e2483f685ccf385c03c106c4c4964829f743f4794f3cb1e4737a1722381',
    'x4.sh': '0f1abbb44860fcd8faed1e73123c91a95388ffab8ebddcf3914e27c3288c794b',
}
for n,h in sorted(PIN.items()):
    if sh(f'{B}/scripts/{n}')!=h: bad.append(f'scripts/{n} is not the script this prompt and the M0b, M0a and M0a.1 prompts produce (D55)')
x=sorted(set(os.listdir(f'{B}/scripts'))-set(PIN)-{'s12c.sh'})
if x: bad.append(f'files in scripts/ no prompt produces: {x}')
cf=f'{B}/census.chain'; C=[l.split() for l in open(cf)] if os.path.exists(cf) else []
if any(len(c)!=5 for c in C): bad.append('census.chain has a malformed line (D94)')
C=[c for c in C if len(c)==5]; s1=[i for i,c in enumerate(C) if c[1:3]==['s1.sh','start']]
if not s1: bad.append('census.chain has no s1.sh start (D94)')
else:
    P=C[s1[-1]:]; run=['s1','s2','s3','s4','s5','s6','s7','s8','s9','s10','s11','s13','s14','s14b','s16','s15','s15b','s12','s12b','s12c']
    exp=[f'{n}.sh {e}' for n in run for e in ('start','end')][:-1]; got=[f'{c[1]} {c[2]}' for c in P]
    if got!=exp: bad.append(f'census.chain: the pass ran {" ".join(got)}; want each pass script once, in order, start then end (D46, D94, D95)')
    cl=open(f'{B}/census.txt','rb').read().splitlines(keepends=True)
    if any(not y.strip() for y in cl): bad.append('census.txt has a blank line (D94)')
    for c in P[1:]:
        n=int(c[3]) if c[3].isdigit() else -1
        if n<0 or n>len(cl) or hashlib.sha256(b''.join(cl[:n])).hexdigest()!=c[4]: bad.append(f'census.txt was changed after {c[1]} {c[2]} at {c[0]} (D94)'); break
    if P[-1][1:3]==['s12c.sh','start'] and P[-1][3]!=str(len(cl)): bad.append(f'census.txt has {len(cl)} lines; s12c.sh started at {P[-1][3]} (D94)')
print('\n'.join(bad) if bad else f'census: {len(L)} gate lines, {len(sec)} sections in order'); sys.exit(1 if bad else 0)
PY
sha256sum "$B/census.txt" > "$B/census.sealed"; cat "$B/census.sealed"
````

`census.sealed` is the hash of the final census; the report quotes it, and nothing may append to
`census.txt` after 12c.

S12b counts every lane's boot records, so a boot the owner takes in parallel also makes it FAIL:
report the records as they are and let the owner attribute them; never explain a mismatch away.

**Expected census:** host gates — every gate PASS except `S7.golden BLOCKED` (the pinned host
lavapipe ICD is not loadable) and `S15.nvk-smoke BLOCKED`. Measured before dispatch (2026-11-12), on a copy of this tree (HEAD `3eef068`) under a stand-in HOME, each script in its own call under DSH's sandbox profile, running this prompt's scripts as extracted at `BOOTS GRANTED: 2` with no boot: 93 PASS, 0 FAIL, 29 BLOCKED (S7.golden, S15.nvk-smoke and the 27 guest gates, `no guest results`), every script exit 0, `S12c.complete` PASS with 40 chain lines from `s1.sh start`, X1 `27 patches applied`, X2 `installed the supplied runner, sha256 2d182f1d…`, X3 `fixes already applied`, `S14b.pinned` and `S14b.aros-fix` PASS, both AROS binaries at `r12=0`, `src/platform/aros` 1088 / 1200, `S9.sdl-stream` PASS, S16 `DH1:W/hdaudio.config before the boots: /W/hdaudio.config size=2182`, `S14b.bootpath` PASS. D94's check, run on altered copies of that census and chain, FAILs a deleted line, a blank line, an edited line, an appended line, a repeated script and an empty chain. Script 3, started with D95's launch and wait commands, ended `SCRIPT s3 exit=0`. On the real tree S7.golden may PASS (the copy's ICD path differed); report it as printed.
The guest gates are **not predicted**. M0b.22's tree is unchanged, apart from the runner's round
name. Whatever S15 and S15b print is the round's result; report it as it is. The held listing timed
out in M0b.17 boot 1, M0b.18 boot 2 and M0b.21 boot 2. If it happens again, the runner exits 11 and
D92 chooses the graded boot.
With a failed copy (exit 7), a prelude failure (exit 10), a wedged agent (exit 6) or a D33 probe
failure (exit 8), the S15 run gates are FAIL with "no data" values; exit 11 leaves them standing and
FAILs `S15b.wiring` if that boot is graded. `S14b.transport`, `S14b.bootpath` and `S14b.realpath` are BLOCKED only if
something already listens on 9099/9100 when they start; then don't boot. Three Linux gates are
BLOCKED only if there is no display.

**Loop rule:** a host-gate FAIL before boot 1 stops the round: edit nothing, report the gate's
output and its cause. A pass is re-run (X0, X1, X2, X3, X4, X, 1–11, 13, 14, 14b, 16, then the runner only
within the boot rules, 15, 15b, 12, 12b, 12c) only when the cause was outside the tree and is gone
(a listener that was already up, a missing build, a script cut off by a tool, D95), and never after
a boot. Script 0 is never re-run; if it fails, the round stops (D96). Within a pass no script is
started twice (D95), and nothing but the scripts writes `census.txt` or `census.chain` (D94).
Report the final pass only, with its full output, plus every runner log. BLOCKED is not FAIL and is
never reported as PASS. A FAIL is never explained as a "known conflict" without the gate output and
the file and line that cause it. Never word code, log lines or file names just to slip past a
pattern.

## Final report (write it to `build/m0b23/report.md` and reply with the same text only, no narration)

1. Skills loaded.
2. For each script of the final pass (X0, 0, X1, X2, X3, X, 1–11, 13, 14, 14b, 16, 15, 15b, 12, 12b,
   12c): its `SCRIPT <name> exit=` line and its output file `build/m0b23/logs/<name>.out` (X0's
   output pasted). Don't paste the other outputs; the files are the verbatim outputs. Then
   `census.txt` verbatim (the output of `cat`), the lines of `census.chain` from the final pass's
   `s1.sh start` on, verbatim, and `census.sealed`.
3. D55, D58, D59, D65, D66, D69, D70, D71, D74, D75, D77–D83, D86–D97: X0's, X2's, X3's and X4's output; `exec_cd.log`; S14b.aros-fix's line; S14b.pinned's, S14b.bootpath's (the twelve dry boots' exit codes
   and the last line of each `calls.log`) and S14b.realpath's output; S16.commands' command list;
   the `aros-gates:` line of both AROS builds, quoted from the final pass's `make_aros.log`;
   the output of `sha256sum "$B"/scripts/*.sh`, pasted from the command (never typed).
4. S16's output: what DH1 carries, the loader's size and sha256, the ICD path, the `DH1:W/hdaudio.config before the boots:` line, `S16.audio`'s line. Then, from after each boot: `loader_id.txt`, `lane_audio.wav`'s size and the `S15b.sound` line, and every `dead_*.ppm`, named with its size.
5. Per boot, every value read from that boot's own `guest-boot<n>/` files: its `boots.txt` line, its `~/Work/lanes/l0025/boot-*.txt` record and serial log name,
   how far the sequence got, `exec_prelude.log` verbatim, the rc and
   the last 10 lines of `run_p.log`, `run_pt.log` and `run_ptr.log`, `sh_p.log`, `assigns.txt`
   verbatim, `dead.txt`, every `vk_entry_missing=` line, `run_w.log`'s `surface=`, `present_dump`, `present_inner=` and `surface_destroy=` lines, every `audio_progdir_error=`, `audio_source=`, `audio_device_id=`, `audio_device=`, `ahi_hidd_preload=`, `ahi_probe`, `ahi_drivers`, `ahi_mode`, `ahi_modes=`, `ahi_best`, `ahi_io_error=`, `ahi_summary` and `ahi_first_submit_us=` line of `run_a.log` and `run_soak.log`, and of their `ahi_xrun` lines the count and the first 10 of each log (D72, D74, D77, D80, D83, D86, D89, D90, D91, D93), the runner's `lane_run: copied …` lines or its `copy failed` line (D78, D81), the `S15b.ahi-mode` and `S15b.envarc` lines (D82), the `S15b.drivers` line and `assigns.txt`'s `LIBS` and `DRIVERS` lines (D87, D88), `exec_ahi_prefs.log`, `ahi_prefs.txt`, `exec_audiomodes.log`, `audiomodes.txt` and `exec_listing.log` verbatim (D75), the `RUNNER_EXIT=` line of its runner log (D97), and for each run with a trap or no rc what its `serial_<r>.txt` shows (D73); if the guest faulted, the serial log's lines from the last `[atcp] exec:` to its end.
6. The M0b measurements list (M0b prompt, final report item 6), each with its log line or BLOCKED.
7. D73 and D76: "two boots, no edits", `graded_boot.txt` verbatim, with `git status --short` and S14b.pinned's line from the final pass.
8. Every departure from this prompt or the M0b prompt, with file and line. That includes every script started more than once, every tool call cut off while a script ran, every git command outside the GIT line's list, and every write to `census.txt` or `census.chain` other than a script's. A gate that FAILs isn't a departure, and no cause goes here. "none" only if none.
9. Every process you started that is still running (there should be none), and anything needed
   outside the authorization block.
