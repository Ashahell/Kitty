# M0b.8 AROS guest dispatch prompt

> Source: Docs/Development/dispatch/m0b8_aros_guest_opencode_prompt.md
> Collected: 2026-11-04
> Published: 2026-11-03

# opencode prompt — Kitty: Core Awakening, **M0b.8** (third guest round: the audited AROS fixes, 1 boot on l0025, CopperForge engine) (2026-11-03)

## Owner authorization (read first — opencode never edits this block)

    BOOTS GRANTED: 1
    LANE: l0025
    WRITE: ~/Work/Kitty/repo/ — the tree as M0b.7 left it (HEAD 38fc422 plus the uncommitted M0b–M0b.7
           changes, Makefile per D47, M0b.7's runner installed; tree hash pinned in script 0). The
           only changes to the tree this round are script X2's install of the supplied
           tests/aros/lane_run.sh (D58) and script X3's supplied patch to four source files (D61).
           No other file is edited, before or after the boot (D63).
           README.md and LICENSE are never edited.
           build/m0a/, build/m0a1/, build/m0b/ and build/m0b1/ through build/m0b7/ (earlier evidence)
           are never edited or deleted; build/m0b8/, build/aros*/ belong to this round. Every file this
           round writes goes under build/m0b8/ (D50, D53), except what the runner itself writes during
           the granted boot. Never restore any file from a pre.tar or any other copy (D55).
    DO NOT EDIT: ~/Work/Kitty/Docs/ (all of it — reports go in build/m0b8/, never in Docs),
           ~/Work/Kitty/AGENTS.md, ~/Work/Kitty/llm-wiki/, every ~/Work/host_lavapipe*/ directory.
    BUILDS: as in M0b (host g++ and the AROS cross-build through the repo Makefile; Vulkan4Aros v1
           toolchain and SDK used read-only). No Vulkan4Aros build script, no library rebuild.
    RUN: the repo's binaries and tests; the acceptance scripts below exactly as given; read-only shell
           commands. A host window may be opened only by scripts 8 and 13 (each run ≤ 65 s).
    IMAGES: ~/Work/vms/aros_l0025_dh0.img and aros_l0025_dh1.img may be READ only by script 16, only
           through Vulkan4Aros scripts/sfs_nav.py, and only while no l0025 guest is running.
    LANES / BOOTS: a boot is any start of an AROS guest. Boots are allowed only while the number of
           lines in build/m0b8/boots.txt is below BOOTS GRANTED, only on the lane named in LANE, only
           through that lane's own launcher (LANE l0025 → ~/Work/projects/Vulkan4Aros/scripts/
           lane_l0025_boot.sh), and only after script 14 and script 14b are all PASS and S16 is PASS.
           Append one line `<ISO time> <lane> <reason>` to boots.txt BEFORE each launch attempt.
    LEDGER: build/m0b8/boots.txt is append-only. Never truncate, rewrite or delete a line, by any
           means; a launch that is refused or fails keeps its line and counts. Script 12b compares
           it with the lanes' own boot-*.txt records.
           With BOOTS GRANTED: 0 nothing may start a guest, write to ~/Work/lanes/, or submit a job to
           a spike server. The exceptions, at any grant: script 14b's transport check, which starts the
           runner's own spike server with its spool under build/m0b8/, submits nothing, starts no
           guest and stops the server; and S14b.realpath, which runs the runner under a stand-in
           HOME in build/m0b8/fakehome, where every call must refuse before starting anything. Never `Avail FLUSH`, never RemLibrary, never reload a library.
           Any spike server or other process this round starts is stopped before the final report.
           Never stop, kill or signal a process this round did not start.
    VULKAN4AROS REPO: READ-ONLY (cat/grep/ls/sed -n/git show/git log/git status, running its
           compilers, running scripts/sfs_nav.py and the spike client). Never write a file in it —
           that includes the spike client's output: every path you hand the client is absolute and
           under build/m0b8/. Never run a write or destructive git command in it; never touch
           `src/abi/v1/AROS*`.
    GIT: read-only git commands in ~/Work/Kitty/repo. No commit, no branch, no stash, no tag, no
           push, in any repository. The owner commits M0b after review.
    PACKAGES: install nothing. WEB: read-only research allowed; post nothing, download no binaries.

Don't ask for, infer, widen or grant any authorization yourself. If the work seems to need
something outside this block, stop that item, mark it FAIL or BLOCKED, and list it in the final report.

## What this task is

M0b.8 is the third guest round on lane l0025, on a new owner grant of 1 boot. M0b.7 ran as written
(review: `~/Work/Kitty/Docs/Development/m0b7_aros_critique.md`): the prelude answered, the loader
opened, and Kitty reached Vulkan on the guest for the first time. Then `p` trapped: every AROS file
read failed, because `dos_read` took `Seek`'s result for the file size.

The owner then read all of `src/platform/aros/` against the AROS, AHI and Mesa sources, before any
boot, and found eight more defects. This prompt supplies the fixes:

- **The fixes** (D61). Script X3 applies a supplied patch to four files and checks every sha. You
  write no fix: you are the mechanic for this round.
- **The runner** (D58). Each ladder run's log, rc and frames are fetched as soon as it ends, so a
  later wedge can't cost them. The inner-area dump is fetched too.
- **The gates** (D62). S14b.aros-fix pins the patched files; S14b.pinned allows those four and no
  others; S15b.wiring accepts the volume name.
- **One boot, no edits** (D63). Boot 1 runs the whole sequence; there is no second boot and no fix,
  whatever it shows.

Everything else stands: D20–D31 (M0b prompt), D32–D38 (M0b.1), D39–D42 (M0b.2), D43, D45, D46
(M0b.3), D47–D50 (M0b.4), D53 (M0b.5), D55 and D57 (M0b.6), D59 (M0b.7), the Linux gates, frames
0/150/299 byte-identical to `build/m0a/r1/`, `hash10s=e663d2da535e69a5`. Wherever an earlier delta
names `build/m0b1`–`build/m0b7`, read `build/m0b8`, and read this file as the prompt. No new
features, no design edits, no renames.

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
| This tree | Script 0's tree hash, taken in the C locale, `34613b67b18a25d86046a699769f56f025eaf8967357ee8b8adbb3fe7450e38b` (88 files): M0b.7's tree with its runner (sha256 `79feeb3e…`) installed; X2 then installs D58's and X3 applies D61's patch. |
| M0b.6 round | `build/m0b6/census.txt` 112 lines, 90 PASS / 20 FAIL / 2 BLOCKED, sealed `e92d88d9…`; one boot (`~/Work/lanes/l0025/boot-20261102-2244.txt`); every script byte-identical to the prompt's. `S12c.complete` FAILed on five owner-pinned hashes (`hdr.sh`, `sX.sh`, `s14.sh`, `s15.sh`, `s12.sh`), taken from a scratch run; the other FAILs are S15/S15b with no data. |
| M0b.6 boot 1 | `~/Work/lanes/l0025/serial-20261102-2244.log`: deploy, `List RAM:kitty`, the `out/` tree and `Avail` answered; at line 213 the agent ran `Execute RAM:kitty/prelude.aros`, then `[PF-DBG] CR2=0000000000000088 IP=0000000001bf8d25` (user-mode read, `present=protection`), and the log stops advancing. `p`, `pt` and `ptr` each got only the client's 600 s `TIMEOUT` (`build/m0b6/guest/exec_p.log`); no file was fetched. The cause of the fault is not established. |
| Forms this lane has run | The four prelude assigns as separate execs: `~/Work/lanes/l0025/serial-20261024-1815.log` lines 225–231 (`Assign LIBS: DH1:Libs`, `Assign LIBS: ADD AROS:Libs`, each `exec done`), then programs ran under that chain. `Execute RAM:kitty/run_<name>.aros >RAM:kitty/out/run_<name>.log`: both M0b boots (`serial-20261102-0737.log`, `serial-20261102-0742.log`), every one `exec done`; the program ran (line 201 of the second: the D24 `Software Failure` loader line). `Execute` of a script without a redirect has also run: `serial-20261021-1504.log` line 1943 (`Execute DH1:runDH1smoke.run`, `exec done`). The four assigns inside one `Execute`'d script have run once on this lane: M0b.6's fault. |
| Agent wedge | When the agent never answers, the client prints `[submit] TIMEOUT: no result after <n>s` and exits non-zero (`build/m0b6/guest/exec_p.log`); the job stays queued, and nothing later is answered. |
| M0b.7 round | `build/m0b7/census.txt` 112 lines, 93 PASS / 17 FAIL / 2 BLOCKED, sealed `b2c0ff53…`; one boot (`~/Work/lanes/l0025/boot-20261103-2122.txt`). `S12c.complete` PASS. |
| M0b.7 boot | `~/Work/lanes/l0025/serial-20261103-2122.log`: no `PF-DBG`, all four prelude lines `rc=0`; `vk_loader="vulkan_loader.library"`, `tier=lavapipe`, `device="llvmpipe (LLVM 20.1.0, 256 bits)"`, `stack_bytes=1048576`, `uptime_probe delta_us=100044`; `p` trapped at `src/render/render_init.cpp:40`, `rc=4`, no `unfreed`. Nothing has drawn on a guest yet. The `sh_` logs were 0 B. |
| AROS DOS | `Seek` returns the position before the seek, -1 on error (AROS `rom/dos/seek.c`). `Write` returns the byte count, 0, or -1 on error (`rom/dos/write.c`). |
| Window helper | `arosPollWindowEvents` returns non-zero to close and 1 for an unknown surface (`src/vulkan/loader/vk_aros_loader.c:672`, `src/vulkan/pal/vkpal_aros.c:1305`; `vkcube_aros.c:640` loops on `!arosPollWindowEvents`). The comment in `aros_window_surface.h` says the opposite and is wrong. The window is `WFLG_GIMMEZEROZERO` with the size as its outer size (`vkpal_aros.c:1125`). |
| Mesa AROS WSI | `currentExtent` is the window's inner size (`src/abi-patches/v1/mesa-26.2.2/wsi_common_aros.c:61`). `wsi_aros_blit` writes at `(BorderLeft, BorderTop)` of `window->RPort` (`wsi_aros_blit.c:74`), which on a GimmeZeroZero window is already the inner layer. The picture should therefore land offset by the border inside the inner area. **UNVERIFIED on a guest**; `present_150.inner.bgra` will show it. |
| AHI links | A link to a request not in AHI's lists is cleared and the request plays at once (`workbench/devs/AHI/Device/devcommands.c:1141`, `:1230`); a second request linked to one that already has a follower fails with `AHIE_UNKNOWN` (`:1161`, `:1235`). Lane l0025's QEMU has no sound device (`lane_l0025_boot.sh:139`), so `S15.ahi` may be BLOCKED on an open failure. |

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

## Design deltas (D32–D50, D53, D55, D57 and D59 stand, except D44, replaced by D47; D54 is replaced by D58, D56 and D60 by D63; D58 is amended; D61–D63 are new — implement exactly; list them in the report)

| # | Decision |
|---|---|
| D32 | **Guest wiring** (owner decision 2026-11-02). New `tests/aros/prelude.aros`, packaged by `make aros-pkg`, whose only command lines are, in this order: `Assign ENVARC: DH1:W`, `Assign LIBS: DH1:Libs`, `Assign LIBS: ADD AROS:Libs`, `Assign >RAM:kitty/out/assigns.txt`. `m0b-run.aros` executes it first (after `Stack 1048576`). D24 is unchanged: the bare name `vulkan_loader.library` now resolves through `LIBS:` to `DH1:Libs`, and the run logs `vk_loader="vulkan_loader.library"`. Never copy or put a library to the guest. |
| D33 | **Probe first, ladder only on success.** Three runs precede the eleven M0b runs: `p` = `KD --headless --frames 1 --out RAM:kitty/out/p S`; `pt` = `KD --plant-loader-fail --headless --frames 1 S`; `ptr` = `K --plant-loader-fail --headless --frames 1 S`. If `rc_p.txt` is not `rc=0` or `run_p.log` has no `vk_loader=` line, the runner skips the eleven runs, fetches what exists, stops the lane, and exits 8 with `probe failed; ladder not run`. |
| D34 | **Output capture.** Each `tests/aros/run_<name>.aros` is exactly three command lines: `Stack 1048576`; the program line, written `RAM:kitty/<kitty\|kitty-debug> >RAM:kitty/out/run_<name>.log <arguments>` (the redirect directly after the command name); `Echo "rc=$RC" >RAM:kitty/out/rc_<name>.txt`. No `Execute` line, in any guest script or in the runner, carries a `>` redirect, except the runner's per-run `Execute … >RAM:kitty/out/sh_<name>.log` (D58, D59). |
| D35 | **Runner paths and ledger.** `tests/aros/lane_run.sh` reads the authorization from this prompt (`dispatch/m0b8_aros_guest_opencode_prompt.md`) and counts boots in `build/m0b8/boots.txt`. Its first check resolves `<outdir>` with `realpath -m` and refuses (exit 3, `outdir outside build/m0b8`) unless it lies under `$KR/build/m0b8/`; every `--get`/`--put` path it passes is absolute. If it starts a spike server it records the PID in `<outdir>/spike.pid` and stops it before exiting. |
| D36 | **The soak and the 900 s budget.** On `RUNNING`, poll `Type RAM:kitty/out/rc_soak.txt` until its output has an indented `rc=<n>` line (or 2,600 s pass; the image has no `Exists`, D58); `rc_soak.txt`, written by the run script, is the authority, and the soak's host wall ends when it appears. |
| D37 | **Trap teardown and the plant.** On AROS, every exit-4 path (the Guru line from `pl_trap`/`alog_trap`, D23's timer check, D24's loader check, D26's stack check) first releases, in reverse order, everything `pl_init` acquired up to that point (watchdog process, AHI requests and device, `lowlevel.library`, the timer request and its port), then exits 4. No guest run may print `unfreed`. Debug-only `--plant-loader-fail` (AROS only) makes D24 skip all three `OpenLibrary` calls and take the real failure path (same trap text); release AROS builds and both Linux builds exit 2 on the flag. |
| D39 | **r12.** Both AROS binaries pass the r12 gate (0 stale-r12 call sites) with the Makefile's AROS flags unchanged. Fix it in `src/platform/aros/` the way M0b did (keep a long-lived call result out of a C++ frame that clang allocates in `%r12`, e.g. a helper in `aros_wsi.c`). No `-ffixed-*` flag, no `-O` change, no optimize pragma or attribute, no inline asm, no change to the gate. |
| D40 | **Runner transport.** (a) The runner always starts its own spike server: `cd <outdir> && python3 -B -u "$VK/scripts/spike_server.py" serve --port 9099 --bulk-port 9100 --spool <spool>` — never `-I`, never `cd` into Vulkan4Aros. It never reuses a server it did not start. (b) Before starting it: if anything listens on 9099 or 9100, refuse (exit 9, `port <n> held by <ss line>`) — no ledger line, no boot, nothing killed. (c) After starting it: within 10 s its PID is alive and `ss -ltnpH` shows that PID on `:9099`; otherwise stop it and exit 9 with `spike server did not start` and the last 5 lines of its log — no ledger line, no boot. (d) Before the ledger line: if a `qemu-system` process for the lane is running, refuse (exit 9, `lane <lane> already running`). (e) `lane_run.sh --check-transport <outdir>`: D35's outdir check, then (b) and (c) with the spool `<outdir>/tspool`, then stop the server, wait until 9099 is free, print `transport ok pid=<pid>` and exit 0. It reads no grant, writes no ledger line, starts no guest, submits nothing, writes nothing under `~/Work/lanes/`. |
| D41 | **Ledger.** The runner appends its `boots.txt` line immediately before calling the launcher and never removes it, whatever the launcher does. Before appending, it counts the lane's `~/Work/lanes/<lane>/boot-*.txt` files newer than `build/m0b8/baseline.done`; if that count is not equal to the `boots.txt` line count, it refuses (exit 3, `ledger mismatch: <n> boot records, <m> ledger lines`). |
| D42 | **`PlatArgs` (owner decision 2026-11-02).** The design doc §3.1 now reads `struct PlatArgs { const char* renderer_override; const char* save_root; int lp_threads; const char* lp_src; };`. `platform.h` already matches; don't edit either, and don't move `lp_threads`/`lp_src`. S2.interfaces must PASS. |
| D43 | **One server start.** `lane_run.sh` defines `SPIKE="$VK/scripts/spike_server.py"` once, at the top, and exactly one function `preflight() { … }` taking `<outdir> <spool>`. `preflight` does D40 (b), (d), then starts the server, then (c), and is the only code that starts `spike_server.py serve`: one command, `( cd "$1" && exec python3 -B -u "$SPIKE" serve --port 9099 --bulk-port 9100 --spool "$2" ) >"$1/spike.log" 2>&1 &`. The `--check-transport` branch calls `preflight "$OUT" "$OUT/tspool"` (D40e's spool — not `build/m0b8/tspool`). The boot path calls `preflight "$OUT" "$SPOOL"` after D41's ledger check and before the `boots.txt` append, and calls neither `submit` nor `sleep` before that append. No other server start, no reuse of a server that answers. In `lane_run.sh`, no line carries both code and a comment, and no line ends in a backslash followed by blanks. |
| D44 | *Replaced by D47* (the M0b.1 Makefile fails `S14.aros-flags`). |
| D45 | **src/app ≤ 600 lines** by S3's count. Remove lines from `src/app/main.cpp` without changing behaviour: same flags, same help text, same exit codes, Linux frames and music hash unchanged. Don't join statements onto one line or delete comments to make the count. |
| D46 | **One complete pass.** Script 1 truncates `census.txt`, so the census is the last pass. The final pass runs X0, X1, X2, X3, X, 1–11, 13, 14, 14b, 16, [runner], 15, 15b, 12, 12b, 12c in that order with no script skipped or repeated; script 12c FAILs any other census. Fix, then re-run the whole pass. |
| D47 | **Makefile.** `Makefile` is byte-identical to the M0b.2 tree's: `tar -xOf build/m0b3/pre.tar Makefile` (sha256 `f85b42c8…`). This replaces D44. |
| D48 | **Runner state and exit.** `lane_run.sh` sets `KR`, `VK`, `SPIKE`, `B="$KR/build/m0b8"` and `PKG` at the top, before any mode is chosen. It installs exactly one EXIT trap, at top level, before anything is started: the handler kills the spike server this run started (if any) and — once the launcher has been called — stops the lane through `stop_lane`, unless `LANE_KEEP=1`. That holds for every exit after the launcher call: 0, 5, 6, 7, 8. `preflight` sets no trap. In `run_one`, `rc` is the client's exit status. |
| D49 | **Dry boot.** `lane_run.sh --dry-boot <outdir>` runs the boot path with these values and no others changed: `GRANT=1`, lane `l0025`, `BOOTS=<outdir>/boots.txt`, `LANES=<outdir>/lanes` (D41 counts `$LANES/<lane>/boot-*.txt` newer than `build/m0b8/baseline.done`; real mode uses `~/Work/lanes`), `LAUNCH="$LR_STUBS/launch.sh"`, `SPOOL=<outdir>/spool`; `submit` runs `"$LR_STUBS/submit.sh" "$@"` and `stop_lane` runs `"$LR_STUBS/stop.sh"`. `<outdir>` and `$LR_STUBS` must resolve under `build/m0b8/` (else exit 3). All mode-dependent values are set before D41's check; no non-comment line after the line that prints `ledger mismatch` mentions `MODE` or `LR_STUBS`. Dry mode never reads the prompt's grant, never writes `build/m0b8/boots.txt` or anything under `~/Work/lanes/`, never runs `systemctl` or the real launcher. `preflight` runs for real (spool `<outdir>/spool`). |
| D50 | **Own files.** Everything this round writes — logs, extracted scripts, scratch — goes under `build/m0b8/`. Nothing under `~/Work/lanes/`, at any grant, except what the runner itself writes during a granted boot. |
| D53 | **Own files.** D50 holds with `build/m0b8/`: every log, extracted script, wrapper and scratch file this round writes is under `build/m0b8/`. Run the scripts from `$B/scripts/` and keep pass logs in `$B/logs/`. |
| D51 | *Replaced by D54.* |
| D52 | *Replaced by D56, which D63 replaces.* |
| D54 | *Replaced by D58.* |
| D55 | **Scripts and baseline.** Script X0 extracts every script of this prompt into `$B/scripts/`; run each as `bash "$B/scripts/<name>.sh"`, and nothing else in its place: no edited, copied, retyped or wrapped script, no extractor of your own. Run every script from one shell with one environment, locale included. Script 0 runs once; `build/m0b8/baseline.done`, `pre_files.txt` and `pre.tar` are never removed, rewritten or re-taken. Never restore a file from a `pre.tar` or any copy, by any means. S12c FAILs if a script's sha256 differs from the one it pins, or if `pre_files.txt` is not script 0's record of the pinned tree. |
| D56 | *Replaced by D63.* |
| D57 | **Running the runner.** Boot `n`'s runner is started detached, exactly as: `cd ~/Work/Kitty/repo && mkdir -p build/m0b8/logs && setsid nohup bash tests/aros/lane_run.sh l0025 build/m0b8/guest-boot$n > build/m0b8/logs/runner-boot$n.log 2>&1 < /dev/null &`. Never start one while another `lane_run.sh` is alive (`pgrep -f 'tests/aros/lane_run\.sh'`); poll its log at most once a minute until it exits; never signal it, the spike server or QEMU yourself. After the round's last boot, `cp -a build/m0b8/guest-boot$n build/m0b8/guest`, so S15 and S15b read the last boot. |
| D58 | **Supplied runner, corrected again.** `tests/aros/lane_run.sh` is the file script X2 installs (sha256 `e2cf0786bdad92dc208282574960c232e0b04ad4351cc580f65ed129d2a2615a`, 335 lines), byte for byte: M0b.6's runner with this prompt's names, and (a) the prelude sent as its own command lines, one exec each, in file order, from `build/aros-pkg/prelude.aros`; a line whose answer has no `-> rc=0 (` prints `prelude failed at line <n>: <line>`, fetches `assigns.txt` and `avail_before.txt`, and exits 10 (the lane stops, D48); (b) each run sent as `Execute RAM:kitty/run_<name>.aros >RAM:kitty/out/sh_<name>.log`, and `sh_<name>.log` fetched with the run's files; (c) an exec whose answer contains `TIMEOUT: no result` prints `guest agent wedged at <what>; nothing more is sent` and exits 6, fetching nothing. Never edit, regenerate or replace it; never run a modified copy. A runner defect is the owner's: report it. Since M0b.8 also: (c) after each ladder run the run's log, rc and frames are fetched at once. |
| D59 | **Gates.** S14b.wiring accepts one `Execute` redirect: the runner's `Execute RAM:kitty/run_$r.aros >RAM:kitty/out/sh_$r.log` (D34's rule holds for every guest script), and FAILs a runner that `Execute`s the prelude. S14b.bootpath runs seven dry boots: `ok`, `running`, `prelude` (the second prelude line answers `rc=10`: exit 10, no later line, no `Execute`), `wedge` (`run_p` gets the client's `TIMEOUT`: exit 6, one run, nothing sent after it), `probe`, `launch`, `stray`; `ok` must send the prelude's four lines as four execs in order and all fourteen runs with their `sh_` redirect. S14b.pinned FAILs any change since script 0 other than X2's install (D60). S15b.output-captured FAILs when no run has an rc. S12c's pins are those of this prompt as dispatched. |
| D60 | *Replaced by D63.* |
| D61 | **Supplied fixes** (owner audit 2026-11-03). Script X3 applies them; you write none. `dos_read` seeks to `OFFSET_END`, then takes the size from the seek back to `OFFSET_BEGINNING`. `pl_write_file` wants `Write == n`. `pl_poll_events` quits on a non-zero poll and doesn't poll without a surface (this replaces D25's "a 0 return sets quit"). `pl_wsi_create_surface` reads the borders from a probe window flagged as the helper's, then asks for an outer size whose inner area is `320+BorderLeft` × `256+BorderTop`, so all 320×256 of the picture shows after Mesa's offset; the renderer centres the picture in `pl_wsi_extent`'s area rather than the swapchain, which puts it top-left on AROS and changes nothing on Linux. `src/app` stays at 600 lines (`= {}` replaces two `memset`s). `pl_present_dump` reads 320×256 at `(BorderLeft, BorderTop)` and also writes the whole inner area to `<out>/present_<F>.inner.bgra`, logging `present_inner=<w>x<h> border=<l>,<t>`. It reports `client` as the inner size less the border offset, and is blocked if cybergraphics is missing. `ahir_Link` is set per submission to the last request if still in flight, else NULL, not as a ring at open. The watchdog is no longer `Signal`led (it sleeps in `DoIO` and may be gone). `run_engine` calls `r_shutdown` before `pl_shutdown`: AROS frees nothing at exit, so the window and device leaked per run. **Not fixed:** AROS `AllocVec` memory the engine never frees (scene, shader buffers, music) also leaks per run; `wsi_aros_blit`'s offset belongs to Vulkan4Aros, which is read-only. |
| D62 | **Gates.** `S14b.aros-fix` FAILs unless the four files have X3's shas. `S14b.pinned` allows those four files and the runner, and nothing else. `S15b.wiring` accepts `DH1` or its volume name `L25DATA` (M0b.7's `assigns.txt` printed `LIBS L25DATA:Libs`). S12c requires X3 in the scripts and S14b.aros-fix once in the census. No gate tests AROS API use by behaviour; only the guest does. |
| D63 | **One boot, no edits** (grant 1 on l0025). Boot 1 runs only when S14, S14b and S16 are all PASS in the current pass, and runs the whole sequence. No file under `~/Work/Kitty/repo/` is edited after script 0 except by X2 and X3, before or after the boot; there is no second boot, whatever it shows. Script 12b's ledger must show one boot. Report results; don't diagnose by editing. |
| D38 | **Report wording.** The guest is "the QEMU/KVM guest on lane l0025", never "hardware". A behaviour counts as proven on the guest only if a guest log line shows that code path ran (a run that trapped in `pl_init` proves nothing about windows, surfaces or audio). |

## Work items

**W0 — scripts X0, X2 and X3** extract this prompt's scripts (D55), install the corrected runner
(D58) and apply the fixes (D61). **W1 — the host pass** (0, X1, X2, X3, X, 1–11, 13, 14, 14b, 16): every gate PASS except the
expected BLOCKED; a FAIL stops the round before the boot. **W2 — boot 1** per D57/D60, then S15,
S15b, 12, 12b, 12c. **W3 — the report.**

## Acceptance scripts (run order: X0, 0, X1, X2, X3, X, 1–11, 13, 14, 14b, 16, [runner], 15, 15b, 12, 12b, 12c; paste every output verbatim)

This block heads every script: X0 and X1 write it into the scripts they extract. You paste it once,
above script X0:

````bash
# script hdr
KD=~/Work/Kitty/Docs/Development; DOC=$KD/kitty_engine_design.md; PR=$KD/dispatch/m0b8_aros_guest_opencode_prompt.md
VK=~/Work/projects/Vulkan4Aros; KR=~/Work/Kitty/repo; B=$KR/build/m0b8
LVP_PIN=~/Work/host_lavapipe_kitty/lvp_icd.json; LVP_SYS=/usr/share/vulkan/icd.d/lvp_icd.json
g(){ echo "GATE $1 $2${3:+ — $3}"; echo "GATE $1 $2${3:+ — $3}" >> "$B/census.txt"; }
icd(){ if [ -f "$LVP_PIN" ] && VK_DRIVER_FILES=$LVP_PIN vulkaninfo --summary 2>/dev/null | grep -q 'driverInfo *= *Mesa 26\.2\.2'; then echo "$LVP_PIN"; else echo "$LVP_SYS"; fi; }
GRANT=$(sed -n 's/^ *BOOTS GRANTED: *\([0-9][0-9]*\) *$/\1/p' "$PR" | head -1); LANE=$(sed -n 's/^ *LANE: *\([a-z0-9]*\) *$/\1/p' "$PR" | head -1)
aenv(){ AROS_ABI=v1 . "$VK/scripts/aros_build_env.sh" >/dev/null 2>&1 || return 1; export AROS_SYSROOT="${AROS_SDK_INCLUDE%/include}" PATH="$AROS_COLLECT_AROS_DIR:$AROS_TOOLCHAIN/bin:$PATH"; }
````

**Script X0: extract this prompt's scripts (D55). Run it first, before script 0, and again before
every pass.** Paste the header above and this block; it is the only script you paste. Every other
script runs as `bash "$B/scripts/<name>.sh"`.

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
want={'hdr','x0','s0','x1','x2','x3','s14b','s16','s15b','s12b','s12c'}
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
[ "$(git rev-parse HEAD)" = 38fc422d388c9886a38045ff5dc665dab369697d ] || { echo "HEAD is not 38fc422"; exit 1; }
( find .gitignore Makefile src tools shaders tests -type f -print0 | LC_ALL=C sort -z | xargs -0 sha256sum ) > "$B/pre_files.txt"
th=$(sha256sum < "$B/pre_files.txt" | cut -d' ' -f1); echo "tree hash $th ($(wc -l < "$B/pre_files.txt") files)"
[ "$th" = 34613b67b18a25d86046a699769f56f025eaf8967357ee8b8adbb3fe7450e38b ] || { echo "tree differs from the tree M0b.7 left — stop and report"; rm -f "$B/pre_files.txt"; exit 1; }
tar -cf "$B/pre.tar" .gitignore Makefile src tools shaders tests
git branch --list --format='%(refname:short) %(objectname)' > "$B/branches_start.txt"
git -C "$VK" rev-parse HEAD > "$B/vk_head.txt"
git -C "$VK" status --porcelain -- 'src/abi/v1/AROS*' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal > "$B/vk_prot_start.txt"
( cd ~/Work/Kitty && find Docs llm-wiki AGENTS.md -type f -print0 | sort -z | xargs -0 sha256sum ) > "$B/protected_start.txt"
( cd ~/Work/Kitty && find Docs llm-wiki AGENTS.md -type f | sort ) > "$B/protected_list_start.txt"
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
hdr=rep(bl[0]+'\n','PR=$KD/dispatch/m0b_aros_opencode_prompt.md','PR=$KD/dispatch/m0b8_aros_guest_opencode_prompt.md')
hdr=rep(hdr,'B=$KR/build/m0b\n','B=$KR/build/m0b8\n')
sx=rep(bl[2],"'B=$KR/build/m0a\\n','B=$KR/build/m0b\\n'","'B=$KR/build/m0a\\n','B=$KR/build/m0b8\\n'")
s14=rep(bl[3],'grep -q "out/run_$r.log" "$S" 2>/dev/null && grep -q "out/rc_$r.txt" "$S" 2>/dev/null','grep -q "out/run_$r.log" "$K/run_$r.aros" 2>/dev/null && grep -q "out/rc_$r.txt" "$K/run_$r.aros" 2>/dev/null')
s12=rep(bl[5],'git -C "$VK" status --porcelain -- \'src/abi/v1/AROS*\' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal | diff "$B/vk_prot_start.txt" - || { echo "VK protected paths changed in the working tree"; f=1; }',
 'vd=$(git -C "$VK" status --porcelain -- \'src/abi/v1/AROS*\' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal | diff "$B/vk_prot_start.txt" - | grep \'^[<>]\'); [ -z "$vd" ] || { echo "INFO VK protected paths changed in the working tree (the owner works there in parallel) — listed for attribution:"; echo "$vd"; }\n'
 'vs=$(cd "$VK" && { git ls-files --others --exclude-standard -- scripts; git diff --name-only HEAD -- scripts; } | sort -u | while read -r p; do [ "$p" -nt "$B/baseline.done" ] && echo "$p"; done); [ -z "$vs" ] || { echo "untracked or uncommitted files under VK scripts/ newer than script 0 (committed owner work is not listed):"; echo "$vs"; f=1; }\n'
 'nl=$(ss -ltnpH 2>/dev/null | sort | comm -13 "$B/listen_start.txt" - | grep -E \':(9099|9100|4479)[[:space:]]\'); [ -z "$nl" ] || { echo "listeners on the lane ports 9099/9100/4479 opened during the run and still open:"; echo "$nl"; f=1; }')
s14=rep(s14,'else out=$(bash tests/aros/lane_run.sh nvk','else n0=$(grep -c . "$B/boots.txt"); out=$(bash tests/aros/lane_run.sh nvk')
s14=rep(s14,'&& [ "$(grep -c . "$B/boots.txt")" = 0 ]; } && g S14.runner-refuses PASS','&& [ "$(grep -c . "$B/boots.txt")" = "$n0" ]; } && g S14.runner-refuses PASS')
s12=rep(s12,'echo "--- files changed vs 38fc422"','echo "--- files changed vs the reviewed M0b tree"')
s12=rep(s12,'sed "s|$B/pre|38fc422|"','sed "s|$B/pre|m0b-tree|"')
open(f'{out}/hdr.sh','w').write(hdr)
for n,s in (('sX',sx),('s14',s14),('s15',bl[4]),('s12',s12)): open(f'{out}/{n}.sh','w').write(hdr+s+'\n')
print('extracted sX, s14, s15, s12 from the M0b prompt; 10 patches applied')
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
# tests/aros/lane_run.sh --dry-boot <outdir>   (LR_STUBS=<dir under build/m0b8>)
#
# The only host-side door to a guest boot in this round (W1). Steps, in order:
#
#   1. D35: resolve <outdir> with `realpath -m` and refuse (exit 3,
#      "outdir outside build/m0b8") unless it lies under $KR/build/m0b8/.
#      Nothing is created before this check passes.
#   2. Read BOOTS GRANTED / LANE from
#      Docs/Development/dispatch/m0b8_aros_guest_opencode_prompt.md and count
#      the lines already in build/m0b8/boots.txt; refuse (exit 3, "no boots
#      granted") on a wrong lane, no grant, or an exhausted grant. This
#      refusal comes before the boots.txt write, so a refused call never
#      consumes a boot.
#   3. D41: before the ledger line, count the lane's boot records newer than
#      build/m0b8/baseline.done; if count != boots.txt lines, refuse (exit 3,
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
#      authority and the host wall ends when it appears (D36).
#
# The per-run guest work lives in run_<name>.aros, the same fragments
# m0b-run.aros executes, so the two paths cannot drift apart.
set -u

KR="$(cd "$(dirname "$0")/../.." && pwd)"
VK="$HOME/Work/projects/Vulkan4Aros"
SPIKE="$VK/scripts/spike_server.py"
B="$KR/build/m0b8"
PKG="$KR/build/aros-pkg"
PROBES="p pt ptr"
RUNS="w h fifo a lp0 lp1 lp4 od2 od4 b1 soak"
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

# ---- 1. D35: the output directory must live under build/m0b8 ----
if [ -z "$OUT_ARG" ] || ! under_b "$OUT_ARG"; then
    echo "outdir outside build/m0b8: ${OUT_ARG:-none}" >&2
    exit 3
fi
OUT="$(realpath -m "$OUT_ARG")"

if [ "$MODE" = transport ]; then
    preflight "$OUT" "$OUT/tspool"
    exit 0
fi

# ---- 2. the mode's values; nothing below tests the mode ----
if [ "$MODE" = dry ]; then
    if [ -z "${LR_STUBS:-}" ] || ! under_b "$LR_STUBS"; then
        echo "LR_STUBS outside build/m0b8" >&2
        exit 3
    fi
    GRANT=1
    AUTH=l0025
    LANE_ARG=l0025
    BOOTS="$OUT/boots.txt"
    LANES="$OUT/lanes"
    LAUNCH="$LR_STUBS/launch.sh"
    SPOOL="$OUT/spool"
else
    PR="$HOME/Work/Kitty/Docs/Development/dispatch/m0b8_aros_guest_opencode_prompt.md"
    GRANT=$(sed -n 's/^ *BOOTS GRANTED: *\([0-9][0-9]*\) *$/\1/p' "$PR" 2>/dev/null | head -1)
    AUTH=$(sed -n 's/^ *LANE: *\([a-z0-9]*\) *$/\1/p' "$PR" 2>/dev/null | head -1)
    BOOTS="$KR/build/m0b8/boots.txt"
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

echo "$(date -Is) $LANE_ARG M0b.8 guest: deploy, prelude, p/pt/ptr, ladder" >> "$BOOTS"
echo "lane_run: boots.txt now has $(grep -c . "$BOOTS") of $GRANT granted lines"
LAUNCHED=1
bash "$LAUNCH" "M0b.8 guest ladder (build/m0b8)" || { echo "launcher failed" >&2; exit 5; }


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

# ---- 5c. the out/ tree, then the D32 prelude, one exec per line (D58) ----
submit --exec "MakeDir RAM:kitty/out" --wait 60 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/out/p" --wait 60 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/out/w" --wait 60 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/out/h" --wait 60 >/dev/null 2>&1
submit --exec "Avail >RAM:kitty/out/avail_before.txt" --wait 120 >/dev/null 2>&1

: > "$OUT/exec_prelude.log"
pn=0
while IFS= read -r pl || [ -n "$pl" ]; do
    case "$pl" in ''|\;*) continue ;; esac
    pn=$((pn + 1))
    pout=$(submit --exec "$pl" --wait 120 2>&1 < /dev/null)
    echo "$pout" >> "$OUT/exec_prelude.log"
    echo "$pout" | sed -n '1,4p'
    wedged "prelude line $pn ($pl)" "$pout"
    if ! echo "$pout" | grep -q -- '-> rc=0 ('; then
        echo "prelude failed at line $pn: $pl"
        getf "assigns.txt" "assigns.txt"
        getf "avail_before.txt" "avail_before.txt"
        exit 10
    fi
done < "$PKG/prelude.aros"
echo "lane_run: prelude sent as $pn execs"

# ---- 5d. one --exec per run fragment; wall time is host-measured ----
run_one() {
    r="$1"; budget="$2"
    s=$(date +%s)
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
        tmo=$(( s + 2600 ))
        while [ "$(date +%s)" -lt "$tmo" ]; do
            q=$(submit --exec "Type RAM:kitty/out/rc_$r.txt" --wait 120 2>&1)
            if echo "$q" | grep -qE '^[[:space:]]+rc=[0-9]+[[:space:]]*$'; then
                break
            fi
            sleep 10
        done
        echo "$q" | sed -n '1,3p'
    fi
    e=$(date +%s)
    echo "$r start=$s end=$e" >> "$OUT/host_wall.txt"
    echo "lane_run: run $r exec rc=$rc wall=$((e - s)) s"
}

run_one p 600
run_one pt 600
run_one ptr 600

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
    getf "assigns.txt" "assigns.txt"
    getf "avail_before.txt" "avail_before.txt"
    exit 8
fi

# Each run's evidence comes home as it ends: a later wedge must not cost it.
for r in $RUNS; do
    run_one "$r" 2400
    getf "run_$r.log" "run_$r.log"
    getf "rc_$r.txt" "rc_$r.txt"
    case "$r" in
        w) getf "w/present_150.bgra" "w/present_150.bgra"
           getf "w/present_150.inner.bgra" "w/present_150.inner.bgra" ;;
        h) for n in 000 150 299; do getf "h/frame_$n.bgra" "h/frame_$n.bgra"; done ;;
    esac
done
submit --exec "Avail >RAM:kitty/out/avail_after.txt" --wait 120 >/dev/null 2>&1

# ---- 7. fetch RAM:kitty/out ----
for r in $PROBES $RUNS; do
    getf "run_$r.log" "run_$r.log"
    getf "rc_$r.txt" "rc_$r.txt"
    getf "sh_$r.log" "sh_$r.log"
done
getf "assigns.txt" "assigns.txt"
getf "avail_before.txt" "avail_before.txt"
getf "avail_after.txt" "avail_after.txt"

echo "lane_run: wrote $OUT/host_wall.txt"
cat "$OUT/host_wall.txt"
echo "lane_run: lane stops now (LANE_KEEP=1 keeps it up)"
exit 0
RUNNER
chmod 755 tests/aros/lane_run.sh; h=$(sha256sum < tests/aros/lane_run.sh | cut -d' ' -f1)
[ "$h" = e2cf0786bdad92dc208282574960c232e0b04ad4351cc580f65ed129d2a2615a ] && echo "installed the supplied runner, sha256 $h" || { echo "INSTALL FAIL: sha256 $h"; exit 1; }
````

**Script X3: apply the supplied fixes (D61).** `bash "$B/scripts/x3.sh"`, after X2, every pass. It
applies the patch once, and answers "already applied" on later passes.

````bash
# script x3
cd "$KR" || exit 1; cat > "$B/fix.patch" <<'FIXPATCH'
--- a/src/app/main.cpp
+++ b/src/app/main.cpp
@@ -283,8 +283,7 @@
 }
 
 [[gnu::noinline]] static int run_engine(Plat& plat, const Opts& o) {
-    TierPick pick;
-    memset(&pick, 0, sizeof pick);
+    TierPick pick = {};
     RendCtx* r = 0;
     if (o.headless) r = r_init(plat_alloc_fn(plat), TIER_LAVAPIPE, &pick);
     else r = r_init_wsi(plat_alloc_fn(plat), TIER_LAVAPIPE, &pick, plat);
@@ -303,8 +302,7 @@
 
     GuruCtx guru;
     guru_init(guru, &plat);
-    Audio au;
-    memset(&au, 0, sizeof au);
+    Audio au = {};
     au.p = &plat;
     au.g = &guru;
     au.rate = 48000;
@@ -337,6 +335,7 @@
         b.p = &plat;
         bench_lite(b, *r, (u32)(o.frames > 0 ? o.frames : 300));
         LOGF(plat, "validation_messages=%u", r_validation_count(*r));
+        r_shutdown(*r);
         pl_shutdown(plat, 0);
     }
 
@@ -385,6 +384,7 @@
 #ifdef KITTY_DEBUG
     if (vc) code = 3;
 #endif
+    r_shutdown(*r);   // AROS frees nothing at exit: the window and the device go now
     pl_shutdown(plat, code);
     return code;
 }
--- a/src/platform/aros/aros_wsi.c
+++ b/src/platform/aros/aros_wsi.c
@@ -61,6 +61,38 @@
     return found;
 }
 
+/* Open a window flagged as the helper's (vkpal_surface_create_sized) long enough
+ * to read the borders Intuition gives it on this screen. */
+int cf_aros_window_borders(const char* title, unsigned* bl, unsigned* bt, unsigned* br, unsigned* bb)
+{
+    if (!IntuitionBase) {
+        IntuitionBase = (struct IntuitionBase*)OpenLibrary((CONST_STRPTR)"intuition.library", 0);
+    }
+    if (!IntuitionBase) return 0;
+    struct NewWindow nw;
+    memset(&nw, 0, sizeof nw);
+    nw.LeftEdge = 10;
+    nw.TopEdge = 10;
+    nw.Width = 320;
+    nw.Height = 256;
+    nw.BlockPen = 1;
+    nw.Flags = WFLG_CLOSEGADGET | WFLG_DEPTHGADGET | WFLG_DRAGBAR | WFLG_SMART_REFRESH | WFLG_GIMMEZEROZERO;
+    nw.Title = (STRPTR)title;
+    nw.MinWidth = 64;
+    nw.MinHeight = 64;
+    nw.MaxWidth = 4096;
+    nw.MaxHeight = 4096;
+    nw.Type = WBENCHSCREEN;
+    struct Window* volatile w = OpenWindowTags(&nw, TAG_DONE);
+    if (!w) return 0;
+    *bl = w->BorderLeft;
+    *bt = w->BorderTop;
+    *br = w->BorderRight;
+    *bb = w->BorderBottom;
+    CloseWindow(w);
+    return 1;
+}
+
 void close_titled_windows(const char* title)
 {
     for (;;) {
--- a/src/platform/aros/plat.cpp
+++ b/src/platform/aros/plat.cpp
@@ -49,6 +49,7 @@
                                            VkSurfaceKHR* out);
 extern "C" int cf_aros_poll(void* base, VkSurfaceKHR surf);
 extern "C" VkResult cf_aros_destroy_surface(void* base, VkInstance inst, VkSurfaceKHR surf);
+extern "C" int cf_aros_window_borders(const char* title, u32* bl, u32* bt, u32* br, u32* bb);
 
 Alloc plat_alloc_fn(Plat& p) { return p.alloc; }
 void plat_free(void* p) { FreeVec(p); }
@@ -124,7 +125,10 @@
     if (!path || !a || !data || !n_out) return false;
     BPTR fh = Open((CONST_STRPTR)path, (LONG)MODE_OLDFILE);
     if (!fh) return false;
-    LONG sz = Seek(fh, 0x7fffffffL, 0L);
+    /* Seek returns the position BEFORE the seek, -1 on error: the size is what
+     * the seek back to the start returns. */
+    if (Seek(fh, 0L, OFFSET_END) < 0) { Close(fh); return false; }
+    LONG sz = Seek(fh, 0L, OFFSET_BEGINNING);
     if (sz < 0) { Close(fh); return false; }
     u8* d = (u8*)a((size_t)sz ? (size_t)sz : 1u);
     if (!d) { Close(fh); return false; }
@@ -158,7 +162,7 @@
     BPTR fh = Open((CONST_STRPTR)path, (LONG)MODE_NEWFILE);
     if (!fh) return false;
     bool ok = true;
-    if (n > 0) ok = (Write(fh, (CONST_APTR)data, (LONG)n) != FALSE);
+    if (n > 0) ok = (Write(fh, (CONST_APTR)data, (LONG)n) == (LONG)n);   /* byte count, -1 on error */
     Close(fh);
     return ok;
 }
@@ -252,8 +256,7 @@
 }
 
 void plat_exit_watchdog_cancel(void) {
-    g_wd_stop = true;
-    if (g_wd_proc) Signal(&g_wd_proc->pr_Task, SIGBREAKF_CTRL_C);
+    g_wd_stop = true;   /* it sleeps in DoIO and may already be gone: never Signal it */
 }
 
 /* ---- D25 WSI ---- */
@@ -264,9 +267,16 @@
     return 1u;
 }
 
+/* The helper's window is GIMMEZEROZERO, its size is the outer size, and Mesa's
+ * wsi_aros_blit writes at (BorderLeft, BorderTop) of the inner RastPort. So 320x256
+ * of picture needs an inner size of 320+BorderLeft x 256+BorderTop. */
 bool pl_wsi_create_surface(Plat& p, VkInstance inst, VkSurfaceKHR* out) {
     if (!out) return false;
-    const VkResult r = cf_aros_create_surface(p.vk_lib, inst, 320u, 256u, aros_window_title, 0, out);
+    u32 bl = 0u, bt = 0u, br = 0u, bb = 0u;
+    if (!cf_aros_window_borders(aros_window_title, &bl, &bt, &br, &bb))
+        alog_trap(__FILE__, __LINE__, "window border probe failed");
+    const VkResult r = cf_aros_create_surface(p.vk_lib, inst, 320u + bl + bl + br, 256u + bt + bt + bb,
+                                              aros_window_title, 0, out);
     if (r != VK_SUCCESS) {
         char line[128];
         rt_fmt(line, sizeof line, "surface_create failed result=%d", (int)r);
@@ -314,9 +324,10 @@
 
 u32 pl_poll_events(Plat& p, EventSink* sink) {
     p.events_drained = p.events_drained + 1ull;
-    const int keep = cf_aros_poll(p.vk_lib, (VkSurfaceKHR)(size_t)p.surf);
+    if (!p.surf) return 0u;   /* headless: no window, and the helper answers 1 for no surface */
+    const int keep = cf_aros_poll(p.vk_lib, (VkSurfaceKHR)(size_t)p.surf);   /* non-zero: close */
     u32 n = 0u;
-    if (keep == 0 && !p.quit) {
+    if (keep != 0 && !p.quit) {
         p.quit = true;
         if (sink) {
             const u32 next = (sink->head + 1u) % 64u;
@@ -364,10 +375,28 @@
     return find_titled_window(title);
 }
 
+/* The whole inner area, so one boot shows where the picture landed. */
+[[gnu::noinline]] static void present_dump_inner(Plat& p, struct Window* w, u32 frame,
+                                                 const char* out_dir, u32 iw, u32 ih) {
+    u8* buf = (u8*)p.alloc(iw * ih * 4u);
+    if (!buf) return;
+    ReadPixelArray((APTR)buf, 0, 0, (UWORD)(iw * 4u), w->RPort, 0, 0, (UWORD)iw, (UWORD)ih, RECTFMT_BGRA32);
+    char path[512];
+    rt_fmt(path, sizeof path, "%s/present_%u.inner.bgra", out_dir ? out_dir : ".", frame);
+    pl_write_file(p, path, buf, iw * ih * 4u);
+    FreeVec(buf);
+    char line[96];
+    rt_fmt(line, sizeof line, "present_inner=%ux%u border=%u,%u", iw, ih, (u32)w->BorderLeft, (u32)w->BorderTop);
+    pl_log(p, line);
+}
+
 [[gnu::noinline]] static bool present_dump_impl(Plat& p, struct Window* w,
                                                  u32 frame, const char* out_dir) {
-    const u32 cw = (u32)w->Width;
-    const u32 ch = (u32)w->Height;
+    /* The picture starts at (BorderLeft, BorderTop) of the inner RastPort. */
+    const u32 iw = (u32)(w->Width - w->BorderLeft - w->BorderRight);
+    const u32 ih = (u32)(w->Height - w->BorderTop - w->BorderBottom);
+    const u32 cw = iw - w->BorderLeft;
+    const u32 ch = ih - w->BorderTop;
     const u32 depth = cyber_depth_of(w->RPort);
     if (cw != 320u || ch != 256u || depth < 24u) {
         present_blocked(p, cw, ch, depth);
@@ -385,10 +414,15 @@
     rt_fmt(path, sizeof path, "%s/present_%u.bgra", out_dir ? out_dir : ".", frame);
     const bool ok = pl_write_file(p, path, buf, 320u * 256u * 4u);
     FreeVec(buf);
+    present_dump_inner(p, w, frame, out_dir, iw, ih);
     return ok;
 }
 
 bool pl_present_dump(Plat& p, u32 frame, const char* out_dir) {
+    if (!CyberGfxBase) {
+        pl_log(p, "present_dump blocked reason=\"no cybergraphics\"");
+        return false;
+    }
     struct Window* w = present_find_window(aros_window_title);
     if (!w) {
         pl_log(p, "present_dump blocked reason=\"no window\"");
@@ -407,6 +441,7 @@
 static bool g_ahi_open = false;
 static u64 g_ahi_submitted = 0;
 static u64 g_ahi_dropped = 0;
+static int g_ahi_last = -1;   /* the request submitted last */
 
 static u32 ahi_inflight(void) {
     u32 n = 0u;
@@ -442,7 +477,6 @@
         rq->ahir_Std.io_Data = b;
         rq->ahir_Std.io_Length = (LONG)kAhiBytes;
     }
-    for (int i = 0; i < 4; i++) g_ahi_req[i]->ahir_Link = g_ahi_req[(i + 1) & 3];
     if (OpenDevice((CONST_STRPTR)"ahi.device", AHI_DEFAULT_UNIT,
                    (struct IORequest*)g_ahi_req[0], 0) != 0) {
         rt_fmt(line, sizeof line, "audio_device=none reason=\"%d\"",
@@ -489,7 +523,12 @@
     ahi_copy_frames(g_ahi_buf[idx], (const u8*)frames, n * 4u);
     rq->ahir_Std.io_Length = (LONG)(n * 4u);
     rq->ahir_Std.io_Command = CMD_WRITE;
+    /* Each request names the one it follows, or NULL: AHI queues it behind that
+     * one if it is still sounding, and clears a link to anything else. */
+    struct AHIRequest* prev = g_ahi_last >= 0 ? g_ahi_req[g_ahi_last] : 0;
+    rq->ahir_Link = (prev && CheckIO((struct IORequest*)prev) == 0) ? prev : 0;
     SendIO((struct IORequest*)rq);
+    g_ahi_last = idx;
     g_ahi_submitted = g_ahi_submitted + 1ull;
     p.queued_frames = p.queued_frames + n;
 }
@@ -511,6 +550,7 @@
     }
     if (g_ahi_port) { DeleteMsgPort(g_ahi_port); g_ahi_port = 0; }
     g_ahi_open = false;
+    g_ahi_last = -1;
 }
 
 void plat_audio_close(Plat& p) {
@@ -525,7 +565,6 @@
 static void wd_release(void) {
     if (!g_wd_proc) return;
     g_wd_stop = true;
-    Signal(&g_wd_proc->pr_Task, SIGBREAKF_CTRL_C);
     for (int i = 0; i < 500; i++) {
         if (!FindTask((CONST_STRPTR)"cf-watchdog")) break;
         timer_sleep_us(5000u);
--- a/src/render/render_frame.cpp
+++ b/src/render/render_frame.cpp
@@ -1,5 +1,6 @@
 #include <cstring>
 #include "render_internal.h"
+#include "wsi.h"
 #include "render_util.h"
 
 static void update_ubo(RendCtx& r) {
@@ -149,6 +150,14 @@
     black.float32[3] = 1.0f;
     r.api.CmdClearColorImage(r.cb, sc, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &black, 1, &range);
     u32 w = r.sc_ext.width, h = r.sc_ext.height;
+    /* The picture is centred in the platform's area, at the swapchain's top left:
+     * on AROS the swapchain is larger than the area (D61). */
+    if (r.plat) {
+        u32 aw = w, ah = h;
+        pl_wsi_extent(*r.plat, &aw, &ah);
+        if (aw && aw < w) w = aw;
+        if (ah && ah < h) h = ah;
+    }
     u32 sx = w / 320u, sy = h / 256u;
     u32 s = sx < sy ? sx : sy;
     if (s < 1u) s = 1u;
FIXPATCH
cat > "$B/fix_pre.txt" <<'SHAS'
448eda147c2f07569cf188372ce43dbcbd9339f01d1761587467d553cffb768f  src/app/main.cpp
8099849991aa1157363242a2aecdc0cf889afd332654128d72d94395f064eda3  src/platform/aros/aros_wsi.c
afea3f707ce31002b4aa41a6552b27375a491c17163d4830f1150ab83384bebf  src/platform/aros/plat.cpp
3cc8e8a63233c1347645d9e250003b65f40008690ad840122546db65d44715d7  src/render/render_frame.cpp
SHAS
cat > "$B/fix_post.txt" <<'SHAS'
7345ffa1b386628559f1f5f73fa1e08eaaad39f7466ef102cee0074f11e5ea51  src/app/main.cpp
071c0f706b8aefb89710e2b7666328c4249e43296f26c6e37c84ab46140c0926  src/platform/aros/aros_wsi.c
be5210d0d5597ae245407fbf10a35cdf22ffca4f253ccb714aa4430050646f4e  src/platform/aros/plat.cpp
a916838e3b42c9eb63490beae3ffe231771396bcaa7db67eb58813d6ea939e40  src/render/render_frame.cpp
SHAS
if sha256sum -c "$B/fix_post.txt" >/dev/null 2>&1; then echo "fixes already applied"
elif sha256sum -c "$B/fix_pre.txt" >/dev/null 2>&1; then
  patch -p1 --no-backup-if-mismatch -s < "$B/fix.patch" || { echo "X3 FAIL: patch did not apply"; exit 1; }
  sha256sum -c "$B/fix_post.txt" >/dev/null 2>&1 || { echo "X3 FAIL: patched files differ from the pins"; exit 1; }
  echo "applied the supplied fixes to $(wc -l < "$B/fix_post.txt") files"
else echo "X3 FAIL: the four files are neither the pre-patch nor the patched state"; sha256sum -c "$B/fix_pre.txt" 2>/dev/null | grep -v ': OK$'; exit 1; fi
````

**Scripts 1–11, 13, 14, 15, 12:** `bash "$B/scripts/s<n>.sh"` each (scripts 1–11 and 13 in one loop as
in M0b; paste the whole output). They are M0b's gates, unchanged except for the patches above.

**Script 14b: the M0b.1–M0b.8 host gates.** `bash "$B/scripts/s14b.sh"`, after 14.

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
chk run-scripts "$f"
f=""; c=$(grep -vE '^[[:space:]]*(;|$)' "$A/prelude.aros" 2>/dev/null)
[ "$c" = "$(printf 'Assign ENVARC: DH1:W\nAssign LIBS: DH1:Libs\nAssign LIBS: ADD AROS:Libs\nAssign >RAM:kitty/out/assigns.txt')" ] || f="prelude.aros is not exactly the four D32 lines:
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
grep -q 'dispatch/m0b8_aros_guest_opencode_prompt\.md' "$R" && grep -q 'build/m0b8/boots\.txt\|m0b8/boots' "$R" || f="runner does not read this prompt and build/m0b8/boots.txt"
grep -qE 'm0b[1-6]([^0-9]|$)' "$R" && f="$f
runner still names m0b1 … m0b6"
grep -q 'probe failed; ladder not run' "$R" || f="$f
runner has no D33 probe exit"
grep -q 'realpath -m' "$R" || f="$f
runner does not resolve <outdir> with realpath -m"
n0=$(grep -c . "$B/boots.txt"); o=$(bash "$R" l0025 /tmp/m0b8-outside 2>&1); rc=$?
{ [ $rc = 3 ] && echo "$o" | grep -q 'outdir outside build/m0b8' && [ "$(grep -c . "$B/boots.txt")" = "$n0" ] && [ ! -e /tmp/m0b8-outside ]; } || f="$f
runner did not refuse an outdir outside build/m0b8 (rc=$rc): $(echo "$o" | head -2)"
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
f=""; PIN=e2cf0786bdad92dc208282574960c232e0b04ad4351cc580f65ed129d2a2615a; h=$(sha256sum < "$R" | cut -d' ' -f1); [ "$h" = "$PIN" ] || f="tests/aros/lane_run.sh is not the supplied runner (D58): sha256 $h"
[ "$(sha256sum < "$B/pre_files.txt" 2>/dev/null | cut -d' ' -f1)" = 34613b67b18a25d86046a699769f56f025eaf8967357ee8b8adbb3fe7450e38b ] || f="$f
build/m0b8/pre_files.txt is not script 0's record of the pinned tree (D55)"
ch=$(grep -vE '  (tests/aros/lane_run\.sh|src/app/main\.cpp|src/platform/aros/aros_wsi\.c|src/platform/aros/plat\.cpp|src/render/render_frame\.cpp)$' "$B/pre_files.txt" | sha256sum -c 2>/dev/null | grep -v ': OK$' | sed 's/: FAILED.*$//')
ad=$(comm -13 <(sed 's/^[0-9a-f]*  //' "$B/pre_files.txt" | sort) <(find .gitignore Makefile src tools shaders tests -type f | sort))
for p in $ch $ad; do
  [ -e "$p" ] || { f="$f
$p removed since script 0"; continue; }
  f="$f
$p changed since script 0: no edits this round (D63)"
done
chk pinned "$f"
f=""; [ -s "$B/fix_post.txt" ] || f="X3 has not run"
x=$(printf '%s\n' '7345ffa1b386628559f1f5f73fa1e08eaaad39f7466ef102cee0074f11e5ea51  src/app/main.cpp' '071c0f706b8aefb89710e2b7666328c4249e43296f26c6e37c84ab46140c0926  src/platform/aros/aros_wsi.c' 'be5210d0d5597ae245407fbf10a35cdf22ffca4f253ccb714aa4430050646f4e  src/platform/aros/plat.cpp' 'a916838e3b42c9eb63490beae3ffe231771396bcaa7db67eb58813d6ea939e40  src/render/render_frame.cpp' | sha256sum -c 2>/dev/null | grep -v ': OK$'); [ -z "$x" ] || f="$f
files are not X3's (D61): $x"
chk aros-fix "$f"
f=""; lm=$(grep -n 'ledger mismatch' "$R" | grep -vE '^[0-9]+:[[:space:]]*#' | head -1 | cut -d: -f1)
x=$(awk -v z="${lm:-0}" 'NR>z' "$R" | grep -nE 'MODE|LR_STUBS' | grep -vE '^[0-9]+:[[:space:]]*#'); { [ -n "$lm" ] && [ -z "$x" ]; } || f="mode tested after the ledger check (D49), or no 'ledger mismatch' line:
$x"
[ -n "$lm" ] && for v in $(awk -v z="$lm" 'NR<z' "$R" | grep -vE '^[[:space:]]*#' | grep -oE '(^|[[:space:];&|(])[A-Za-z_][A-Za-z0-9_]*=' | grep -oE '[A-Za-z_][A-Za-z0-9_]*' | sort -u); do
  case " KR VK SPIKE B PKG PROBES RUNS SERVER_PID LAUNCHED MODE OUT_ARG LANE_ARG OUT GRANT AUTH BOOTS LANES LAUNCH SPOOL PR used lane_boots " in *" $v "*) continue ;; esac
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
mkdir -p "$LR_DRY_LANES/l0025"; : > "$LR_DRY_LANES/l0025/boot-stub-$$.txt"
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
dry ok: the prelude is not sent as its own four command lines, in order (D58): $(echo "$pe" | tr '\n' '|')"
ne=$(grep -c '^submit --exec Execute ' "$D/calls.log"); ns=$(grep -cE '^submit --exec Execute RAM:kitty/run_([a-z0-9]+)\.aros >RAM:kitty/out/sh_\1\.log ' "$D/calls.log")
{ [ "$ne" = 14 ] && [ "$ns" = 14 ]; } || f="$f
dry ok: want 14 Execute calls, each with its sh_ redirect (D58) (Execute=$ne with_redirect=$ns)"
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
a dry boot wrote under ~/Work/lanes or to build/m0b8/boots.txt"
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
if [ "$(sha256sum < "$R" | cut -d' ' -f1)" != e2cf0786bdad92dc208282574960c232e0b04ad4351cc580f65ed129d2a2615a ]; then g S14b.realpath BLOCKED "tests/aros/lane_run.sh is not the supplied runner (S14b.pinned); its real-mode path is not run"
elif held 9099 || held 9100; then g S14b.realpath BLOCKED "9099/9100 held before the check"
else
  F=$B/fakehome; rm -rf "$F" "$B"/real-*; P=$F/Work/Kitty/Docs/Development/dispatch; L=$F/Work/lanes/l0025; V=$F/Work/projects/Vulkan4Aros/scripts
  mkdir -p "$F/bin" "$P" "$L" "$V"
  printf '#!/bin/bash\necho "systemctl $*" >> "$LR_CALLS"\n' > "$F/bin/systemctl"
  printf '#!/bin/bash\necho "real launcher $*" >> "$LR_CALLS"\nexit 1\n' > "$V/lane_l0025_boot.sh"
  chmod +x "$F/bin/systemctl" "$V/lane_l0025_boot.sh"
  lw(){ { find ~/Work/lanes/l0025 ~/Work/lanes/kitty-* -newer "$B/baseline.done" 2>/dev/null; ls ~/Work/lanes/*/boot-*.txt 2>/dev/null; } | sort -u; }
  lw0=$(lw); nb0=$(grep -c . "$B/boots.txt")
  fp(){ printf '    BOOTS GRANTED: %s\n    LANE: l0025\n' "$1" > "$P/m0b8_aros_guest_opencode_prompt.md"; }
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
a real-mode check wrote under ~/Work/lanes or to build/m0b8/boots.txt"
  rm -rf "$F"; chk realpath "$f"
fi
````

**Script 16: guest staging and commands (read-only, no boot).** `bash "$B/scripts/s16.sh"`, after 14b, before the runner.

````bash
# script s16
cd "$KR" || exit 1; NAV=$VK/scripts/sfs_nav.py; IM=~/Work/vms; S=$B/staging; mkdir -p "$S"
if pgrep -f '^[^ ]*qemu-system.*aros_l0025' >/dev/null; then g S16.guest-staging BLOCKED "lane l0025 is running: images are not read while a guest has them open"; g S16.commands BLOCKED "no DH0 tree"; else
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
    else: print(f'loader DH1:Libs/vulkan_loader.library size={len(ld)} sha256={hashlib.sha256(ld).hexdigest()}')
print('\n'.join(bad)); sys.exit(1 if bad else 0)
PY
c=$( { grep -oE 'submit --exec "[A-Za-z][A-Za-z0-9_]*' tests/aros/lane_run.sh | sed 's/.*"//'; grep -hvE '^[[:space:]]*(;|$)' tests/aros/*.aros | awk '{print $1}'; } | grep -v '^RAM:' | sort -u)
f=""; for x in $c; do grep -qiE "^/C/$x size=" "$S/tree-dh0.txt" 2>/dev/null || f="$f $x"; done
echo "guest commands used: $(echo $c)"; [ -n "$c" ] && [ -z "$f" ] && g S16.commands PASS || g S16.commands FAIL "not in DH0:C (tree-dh0.txt):${f:- none found}"
fi
````

S16 FAIL or BLOCKED means **don't boot**: S15 and S15b are BLOCKED, and the report says what the
staging lacks. The owner may change the staging in parallel; S16 reads it as it is when you run it.

**Boots: 1 granted on l0025** (D63). No TCG, no other lane, never more boots than the block grants;
a refused or failed launch keeps its `boots.txt` line and counts.

**Runner** (only when `BOOTS GRANTED` > 0, `LANE` is `l0025`, and S14, S14b and S16 are all PASS in
the current pass — S14b.transport, S14b.bootpath, S14b.realpath and S16.commands included): start it
as D57 says, with `n` = 1. Paste its whole log (`build/m0b8/logs/runner-boot1.log`).

- Boot 1 runs the whole sequence: make the package's directories, deploy (one `--put` per file,
  then `List RAM:kitty` checked), the `out/` tree, the prelude as four execs, `p`, `pt`, `ptr`,
  then — only if `p` passed (D33) — the eleven M0b runs. The soak alone takes over 30 minutes; the
  whole boot about an hour.
- When the runner has exited, `cp -a build/m0b8/guest-boot1 build/m0b8/guest` (in the repo), then
  run S15, S15b, 12, 12b, 12c. No fix and no second boot, whatever boot 1 shows (D63).

**Script 15b: the M0b.1 guest gates.** `bash "$B/scripts/s15b.sh"`, after script 15.

````bash
# script s15b
cd "$KR" || exit 1; G=$B/guest
N="wiring probe trap-clean plant-release output-captured"
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
gate('wiring', bool(re.search(r'^\s*LIBS:?\s+(DH1|L25DATA):Libs\b',a,re.M|re.I)) and bool(re.search(r'^\s*ENVARC:?\s+(DH1|L25DATA):W\b',a,re.M|re.I)), 'assigns.txt: '+(' | '.join(l.strip() for l in a.splitlines() if re.search(r'LIBS|ENVARC|DH1|L25DATA|AROS:Libs',l,re.I))[:300] or 'missing'))
p=rd('run_p.log')
gate('probe', rc('p')==0 and 'vk_loader="vulkan_loader.library"' in p and 'Software Failure' not in p and re.search(r'(^|\s)frames=[1-9]',p) is not None, f"rc={rc('p')} {(re.findall(r'vk_loader=\S+',p) or ['no vk_loader line'])[-1]}")
t=rd('run_pt.log'); un=[os.path.basename(x) for x in sorted(glob.glob(f'{G}/exec_*.log')) if 'unfreed' in open(x,errors='replace').read()]
gate('trap-clean', rc('pt')==4 and re.search(r'^Software Failure\. src/platform/aros/[A-Za-z0-9_/]+\.(cpp|c):[1-9][0-9]*: vulkan loader open failed',t,re.M) is not None and not un, f"rc={rc('pt')} unfreed_in={un}")
gate('plant-release', rc('ptr')==2, f"rc={rc('ptr')}")
emp=[r for r in re.findall(r'^(\w+) start=',rd('host_wall.txt'),re.M) if rc(r) is not None and not rd(f'run_{r}.log').strip()]
wr=[r for r in re.findall(r'^(\w+) start=',rd('host_wall.txt'),re.M) if rc(r) is not None]
gate('output-captured', bool(wr) and not emp, f'runs with an rc: {len(wr)}; runs with an rc but an empty log: {emp}')
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
once=['S2.interfaces','S3.budgets','S5.build','S13.frames-unchanged','S13.hash10s-unchanged','S14.aros-build','S14.aros-flags','S14.aros-package','S14.runner-refuses','S14b.run-scripts','S14b.wiring','S14b.runner','S14b.plant-and-package','S14b.preflight','S14b.makefile','S14b.pinned','S14b.aros-fix','S14b.bootpath','S14b.transport','S14b.realpath','S16.guest-staging','S16.commands','S12.end-state','S12b.ledger']
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
if sh(f'{B}/pre_files.txt')!='34613b67b18a25d86046a699769f56f025eaf8967357ee8b8adbb3fe7450e38b': bad.append('pre_files.txt is not script 0\'s record of the pinned tree (D55)')
PIN={
    'hdr.sh': 'dc5accd835bd9a72332584786842a9f4e0ca0479244d6039b01d383b3d19858a',
    's0.sh': '6db5644b18c495aa3bfe0ba85e00d5a9efdfa9cf3eb219b5902839fbae9934a2',
    's10.sh': '65cdcc763a4d640057e7fb099c8bccfbf8e3bbf813dba21095b9f0685d130142',
    's11.sh': 'c5e7b5eabbd66f4e87772358a9145b635741e56d56105352ab4b3cb993b21bd1',
    's12b.sh': '5756d139f55f47601a02c9aad3ec2995baf24b452fb5ffac116a5562764ba3ce',
    's12.sh': '09ab5257cd116647ae221294d47a9bf578b692bf902bc3634b5a5a5c1dc01c0a',
    's13.sh': 'b3d63b4cabdf7eabdc86e622f13e0d6339f8f9e5a9917fcf17fe34eb5c8ecf0a',
    's14b.sh': '2c89e5256a7400160ecb3b6cd941c319bbf2aa23799044bd2d0c88a477ba776f',
    's14.sh': 'fb72283af8af606e804a8f12e93d28e0b298428cd13eb6877f22177801ace23f',
    's15b.sh': 'e807ff3eae7070713bdc085584f40dbf9680d8e3cf67caaea365ff546ae3feeb',
    's15.sh': '9f1e38bf47d6c057bc39a2a797587dce7ce172ff4cd6e244112a7f3d901326ff',
    's16.sh': '3a64a1e4eb323ec012306b9d712efb5a25bace0690557c4ce923b334265084d5',
    's1.sh': 'a253821ecd337b17b520bc7a0874ad516ef6b5fa15568b47ed21862e58717325',
    's2.sh': 'fc7bbb3f53dc96c783ac35001828a9fa1482a42a6d180ef88ac16ceca6bafda2',
    's3.sh': '1087a269a53691bd17edb57e23bf3e62800e5b00e52de2943173877fc2f20f59',
    's4.sh': '6df414b9e8985543bc0d9605f92f2979e4aef8dd521f42a37065adee909f259c',
    's5.sh': '3ab37385208a199ebd9962e418ed526856c44dc4e8f96ca2305ead286dd4e0a6',
    's6.sh': '6a95368f15435380dc111a9899cebe853e980bbf64561303976116abe1b0ed1e',
    's7.sh': '04fecce64deba8b31bb9df89d9a7a8d866e05ee15e2c5d4d30403faf384b8cfe',
    's8.sh': '436d06f589416dc5047857a8102ea52c4c6f9e7f534e156b0a1f93b89292af7c',
    's9.sh': '7728b3843b235cfa724a1ef63fd07ee7dcd4fbab656ee82481b62af1c04f7b7e',
    'sX.sh': 'b2f636891139ea1248a97deae4c4a33f70fa8eb199b9d06149c35ecac5019006',
    'x0.sh': '0f08e4104b94baff3fd6a89f3269d4b4a0ed71463de4760bba7436823cdb4149',
    'x1.sh': '96a91a00cb2ead1ead9f648fb27419f00aa6a92d49e4a23b1bd40a498b5257e8',
    'x2.sh': 'aafbc290a852a396bc998241b51be5337d250ad5ecca0e10b95de9eebd163dba',
    'x3.sh': '59e983e3be06f558bb88b8170f4d01d3c140b67bf1dc752dabab1f2fe884e41d',
}
for n,h in sorted(PIN.items()):
    if sh(f'{B}/scripts/{n}')!=h: bad.append(f'scripts/{n} is not the script this prompt and the M0b, M0a and M0a.1 prompts produce (D55)')
x=sorted(set(os.listdir(f'{B}/scripts'))-set(PIN)-{'s12c.sh'})
if x: bad.append(f'files in scripts/ no prompt produces: {x}')
print('\n'.join(bad) if bad else f'census: {len(L)} gate lines, {len(sec)} sections in order'); sys.exit(1 if bad else 0)
PY
sha256sum "$B/census.txt" > "$B/census.sealed"; cat "$B/census.sealed"
````

`census.sealed` is the hash of the final census; the report quotes it, and nothing may append to
`census.txt` after 12c.

S12b counts every lane's boot records, so a boot the owner takes in parallel also makes it FAIL:
report the records as they are and let the owner attribute them; never explain a mismatch away.

**Expected census:** host gates — every gate PASS except `S7.golden BLOCKED` (the pinned host
lavapipe ICD is not loadable) and `S15.nvk-smoke BLOCKED`. Measured before dispatch, on a copy of
this tree, running this prompt's scripts as extracted: 91 PASS, 0 FAIL, 22 BLOCKED (S7.golden and
the 21 guest gates, with no boot), 113 census lines, `S12c.complete` PASS, all seven dry boots with
the exit codes D59 names, both AROS binaries at `r12=0`. X3 run twice answers "already applied". An
edit to another file under `src/` FAILs `S14b.pinned`; putting M0b.7's `Seek` line back FAILs
`S14b.aros-fix` and X3; M0b.7's runner under this prompt's names FAILs `S14b.pinned`, with
`S14b.realpath` BLOCKED; an edited `scripts/s0.sh` FAILs `S12c.complete`; M0b.7's own `assigns.txt`
PASSes `S15b.wiring`. The guest gates are **not measured**: the fixes have never run on a guest, and
no guest has yet run this code past `r_init`. Windows, frames, presented pixels, AHI, `r_shutdown` and
the soak are all first runs, so expect new findings. Whatever S15 and S15b print is the round's
result; report it as it is. With a prelude failure (exit 10), a wedged agent (exit 6) or a D33 probe
failure (exit 8), the eleven-run S15 gates are FAIL with "no data" values; say so. If a ladder run
wedges the agent, the runs before it have their files in `guest-boot1/`.
`S14b.transport`, `S14b.bootpath` and `S14b.realpath` are BLOCKED only if
something already listens on 9099/9100 when they start; then don't boot. Three Linux gates are
BLOCKED only if there is no display.

**Loop rule:** a host-gate FAIL before boot 1 stops the round: edit nothing, report the gate's
output and its cause. A pass is re-run (X0, X1, X2, X3, X, 1–11, 13, 14, 14b, 16, then the runner only
within the boot rules, 15, 15b, 12, 12b, 12c) only when the cause was outside the tree and is gone
(a listener that was already up, a missing build), and never after the boot. Script 0 is never re-run.
Report the final pass only, with its full output, plus every runner log. BLOCKED is not FAIL and is
never reported as PASS. A FAIL is never explained as a "known conflict" without the gate output and
the file and line that cause it. Never word code, log lines or file names just to slip past a
pattern.

## Final report (write it to `build/m0b8/report.md` and reply with the same text only, no narration)

1. Skills loaded.
2. **The verbatim output of scripts X0, 0, X1, X2, X3, X, 1–11, 13, 14, 14b, 16, 15, 15b, 12, 12b and
   12c from the final pass**, census included, each script's exit code; `census.sealed`.
3. D55, D58, D59, D62: X0's, X2's and X3's output; S14b.aros-fix's line; S14b.pinned's, S14b.bootpath's (the seven dry boots' exit codes
   and the last line of each `calls.log`) and S14b.realpath's output; S16.commands' command list;
   the `aros-gates:` line of both AROS builds, quoted from the final pass's `make_aros.log`;
   `sha256sum "$B"/scripts/*.sh`.
4. S16's output: what DH1 carries, the loader's size and sha256, the ICD path.
5. Per boot: its `boots.txt` line, its `~/Work/lanes/l0025/boot-*.txt` record and serial log name,
   the runner log's last 30 lines, how far the sequence got, `exec_prelude.log` verbatim, the rc and
   the last 10 lines of `run_p.log`, `run_pt.log` and `run_ptr.log`, `sh_p.log`, `assigns.txt`
   verbatim, and `run_w.log`'s `surface=`, `present_dump`, `present_inner=` and `surface_destroy=` lines; if the guest faulted, the serial log's lines from the last `[atcp] exec:` to its end.
6. The M0b measurements list (M0b prompt, final report item 6), each with its log line or BLOCKED.
7. D63: "one boot, no edits", with `git status --short` and S14b.pinned's line from the final pass.
8. Every departure from this prompt or the M0b prompt, with file and line. "none" only if none.
9. Every process you started that is still running (there should be none), and anything needed
   outside the authorization block.
