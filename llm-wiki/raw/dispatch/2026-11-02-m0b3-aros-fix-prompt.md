# M0b.3 AROS fix prompt (one server start, census completeness)

> Source: Docs/Development/dispatch/m0b3_aros_fix_opencode_prompt.md
> Collected: 2026-11-02
> Published: 2026-11-02


## Owner authorization (read first — opencode never edits this block)

    BOOTS GRANTED: 0
    LANE: none
    WRITE: ~/Work/Kitty/repo/ — the M0b.2 working tree as reviewed (HEAD 38fc422 plus the uncommitted
           M0b, M0b.1 and M0b.2 changes; tree hash pinned in script 0). Edit only src/app/main.cpp (D45),
           tests/aros/lane_run.sh (D43) and the Makefile (D44: restore it, nothing else). Nothing else
           under src/, tools/, shaders/, tests/ — src/platform/aros/, src/include/platform.h and the
           design doc included. README.md and LICENSE are never edited. build/m0a/, build/m0a1/,
           build/m0b/, build/m0b1/ and build/m0b2/ (earlier evidence) are never edited or deleted;
           build/m0b3/, build/aros*/ belong to this round.
    DO NOT EDIT: ~/Work/Kitty/Docs/ (all of it — reports go in build/m0b3/, never in Docs),
           ~/Work/Kitty/AGENTS.md, ~/Work/Kitty/llm-wiki/, every ~/Work/host_lavapipe*/ directory.
    BUILDS: as in M0b (host g++ and the AROS cross-build through the repo Makefile; Vulkan4Aros v1
           toolchain and SDK used read-only). No Vulkan4Aros build script, no library rebuild.
    RUN: the repo's binaries and tests; the acceptance scripts below exactly as given; read-only shell
           commands. A host window may be opened only by scripts 8 and 13 (each run ≤ 65 s).
    IMAGES: ~/Work/vms/aros_l0025_dh0.img and aros_l0025_dh1.img may be READ only by script 16, only
           through Vulkan4Aros scripts/sfs_nav.py, and only while no l0025 guest is running.
    LANES / BOOTS: a boot is any start of an AROS guest. Boots are allowed only while the number of
           lines in build/m0b3/boots.txt is below BOOTS GRANTED, only on the lane named in LANE, only
           through that lane's own launcher (LANE l0025 → ~/Work/projects/Vulkan4Aros/scripts/
           lane_l0025_boot.sh), and only after script 14 and script 14b are all PASS and S16 is PASS.
           Append one line `<ISO time> <lane> <reason>` to boots.txt BEFORE each launch attempt.
    LEDGER: build/m0b3/boots.txt is append-only. Never truncate, rewrite or delete a line, by any
           means; a launch that is refused or fails keeps its line and counts. Script 12b compares
           it with the lanes' own boot-*.txt records.
           With BOOTS GRANTED: 0 nothing may start a guest, write to ~/Work/lanes/, or submit a job to
           a spike server. The one exception, at any grant: script 14b's transport check, which starts
           the runner's own spike server with its spool under build/m0b3/, submits nothing, starts no
           guest and stops the server. Never `Avail FLUSH`, never RemLibrary, never reload a library.
           Any spike server or other process this round starts is stopped before the final report.
           Never stop, kill or signal a process this round did not start.
    VULKAN4AROS REPO: READ-ONLY (cat/grep/ls/sed -n/git show/git log/git status, running its
           compilers, running scripts/sfs_nav.py and the spike client). Never write a file in it —
           that includes the spike client's output: every path you hand the client is absolute and
           under build/m0b3/. Never run a write or destructive git command in it; never touch
           `src/abi/v1/AROS*`.
    GIT: read-only git commands in ~/Work/Kitty/repo. No commit, no branch, no stash, no tag, no
           push, in any repository. The owner commits M0b after review.
    PACKAGES: install nothing. WEB: read-only research allowed; post nothing, download no binaries.

Don't ask for, infer, widen or grant any authorization yourself. If the work seems to need
something outside this block, stop that item, mark it FAIL or BLOCKED, and list it in the final report.

## What this task is

M0b.2 fixed r12 (D39) and the ledger (D41), but D40 landed only in the `--check-transport` branch of
`tests/aros/lane_run.sh`. The boot path still starts its server after `sleep 3` with no port or
liveness check, and its server line (`serve \  # spike_server.py`) breaks the line continuation, so
the server would start without `--port`/`--spool`. The final census had no S14, S14b or S16 line; the
Makefile's C flags changed out of scope; S3.budgets FAILs (review:
`~/Work/Kitty/Docs/Development/m0b2_aros_critique.md`). M0b.3 fixes what that review found and
nothing else:

- **One server start** (D43): a single `preflight` function used by both runner modes.
- **Makefile** (D44): restored to the M0b.1 tree's Makefile.
- **src/app budget** (D45): `src/app` back to ≤ 600 lines.
- **A complete census per pass** (D46), enforced by the new script 12c.

Everything else stands: D20–D31 (M0b prompt), D32–D38 (M0b.1 prompt), D39–D42 (below), the Linux
gates, frames 0/150/299 byte-identical to `build/m0a/r1/`, `hash10s=e663d2da535e69a5`. With
`build/m0b1` or `build/m0b2` in an earlier delta, read `build/m0b3`, and read this file as the prompt.
No new features, no design edits, no renames.

## Verified facts (measured 2026-11-02 — don't re-derive, don't contradict)

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
| S2.interfaces | `platform.h` declares `PlatArgs` with `lp_threads` and `lp_src` (since M0b, mtime 2026-11-02 07:13). The owner amended the design doc's §3.1 to the same four fields on 2026-11-02 (doc sha256 `d05e5d672119a8fc…`); S2.interfaces PASSes on the M0b.1 tree. |

## Inputs (read before writing code)

`build/m0b2/report.md`; `~/Work/Kitty/Docs/Development/m0b2_aros_critique.md`; the M0b.2 prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b2_aros_fix_opencode_prompt.md` (D39–D42);
`build/m0b1/report.md`; `~/Work/Kitty/Docs/Development/m0b1_aros_critique.md`; the M0b.1 prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b1_aros_fix_opencode_prompt.md` (D32–D38 still apply);
`build/m0b/report.md`; `~/Work/Kitty/Docs/Development/m0b_aros_critique.md`; the M0b prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b_aros_opencode_prompt.md` (D20–D31 and W1–W5 still apply);
`tests/aros/*`; `src/platform/aros/*`. Skills: `aros-development` and `karpathy-guidelines` — load
both and say so in the report.

## Design deltas (D32–D42 stand; D43–D46 are new — implement exactly; list them in the report)

| # | Decision |
|---|---|
| D32 | **Guest wiring** (owner decision 2026-11-02). New `tests/aros/prelude.aros`, packaged by `make aros-pkg`, whose only command lines are, in this order: `Assign ENVARC: DH1:W`, `Assign LIBS: DH1:Libs`, `Assign LIBS: ADD AROS:Libs`, `Assign >RAM:kitty/out/assigns.txt`. `m0b-run.aros` executes it first (after `Stack 1048576`). D24 is unchanged: the bare name `vulkan_loader.library` now resolves through `LIBS:` to `DH1:Libs`, and the run logs `vk_loader="vulkan_loader.library"`. Never copy or put a library to the guest. |
| D33 | **Probe first, ladder only on success.** Three runs precede the eleven M0b runs: `p` = `KD --headless --frames 1 --out RAM:kitty/out/p S`; `pt` = `KD --plant-loader-fail --headless --frames 1 S`; `ptr` = `K --plant-loader-fail --headless --frames 1 S`. If `rc_p.txt` is not `rc=0` or `run_p.log` has no `vk_loader=` line, the runner skips the eleven runs, fetches what exists, stops the lane, and exits 8 with `probe failed; ladder not run`. |
| D34 | **Output capture.** Each `tests/aros/run_<name>.aros` is exactly three command lines: `Stack 1048576`; the program line, written `RAM:kitty/<kitty\|kitty-debug> >RAM:kitty/out/run_<name>.log <arguments>` (the redirect directly after the command name); `Echo "rc=$RC" >RAM:kitty/out/rc_<name>.txt`. No `Execute` line, in any guest script or in the runner, carries a `>` redirect. |
| D35 | **Runner paths and ledger.** `tests/aros/lane_run.sh` reads the authorization from this prompt (`dispatch/m0b3_aros_fix_opencode_prompt.md`) and counts boots in `build/m0b3/boots.txt`. Its first check resolves `<outdir>` with `realpath -m` and refuses (exit 3, `outdir outside build/m0b3`) unless it lies under `$KR/build/m0b3/`; every `--get`/`--put` path it passes is absolute. If it starts a spike server it records the PID in `<outdir>/spike.pid` and stops it before exiting. |
| D36 | **The soak and the 900 s budget.** Keep M0b's handling: on `RUNNING`, poll `Exists RAM:kitty/out/rc_soak.txt` until it exists (or 2,600 s pass); `rc_soak.txt`, written by the run script, is the authority, and the soak's host wall ends when it appears. |
| D37 | **Trap teardown and the plant.** On AROS, every exit-4 path (the Guru line from `pl_trap`/`alog_trap`, D23's timer check, D24's loader check, D26's stack check) first releases, in reverse order, everything `pl_init` acquired up to that point (watchdog process, AHI requests and device, `lowlevel.library`, the timer request and its port), then exits 4. No guest run may print `unfreed`. Debug-only `--plant-loader-fail` (AROS only) makes D24 skip all three `OpenLibrary` calls and take the real failure path (same trap text); release AROS builds and both Linux builds exit 2 on the flag. |
| D39 | **r12.** Both AROS binaries pass the r12 gate (0 stale-r12 call sites) with the Makefile's AROS flags unchanged. Fix it in `src/platform/aros/` the way M0b did (keep a long-lived call result out of a C++ frame that clang allocates in `%r12`, e.g. a helper in `aros_wsi.c`). No `-ffixed-*` flag, no `-O` change, no optimize pragma or attribute, no inline asm, no change to the gate. |
| D40 | **Runner transport.** (a) The runner always starts its own spike server: `cd <outdir> && python3 -B -u "$VK/scripts/spike_server.py" serve --port 9099 --bulk-port 9100 --spool <spool>` — never `-I`, never `cd` into Vulkan4Aros. It never reuses a server it did not start. (b) Before starting it: if anything listens on 9099 or 9100, refuse (exit 9, `port <n> held by <ss line>`) — no ledger line, no boot, nothing killed. (c) After starting it: within 10 s its PID is alive and `ss -ltnpH` shows that PID on `:9099`; otherwise stop it and exit 9 with `spike server did not start` and the last 5 lines of its log — no ledger line, no boot. (d) Before the ledger line: if a `qemu-system` process for the lane is running, refuse (exit 9, `lane <lane> already running`). (e) `lane_run.sh --check-transport <outdir>`: D35's outdir check, then (b) and (c) with the spool `<outdir>/tspool`, then stop the server, wait until 9099 is free, print `transport ok pid=<pid>` and exit 0. It reads no grant, writes no ledger line, starts no guest, submits nothing, writes nothing under `~/Work/lanes/`. |
| D41 | **Ledger.** The runner appends its `boots.txt` line immediately before calling the launcher and never removes it, whatever the launcher does. Before appending, it counts the lane's `~/Work/lanes/<lane>/boot-*.txt` files newer than `build/m0b3/baseline.done`; if that count is not equal to the `boots.txt` line count, it refuses (exit 3, `ledger mismatch: <n> boot records, <m> ledger lines`). |
| D42 | **`PlatArgs` (owner decision 2026-11-02).** The design doc §3.1 now reads `struct PlatArgs { const char* renderer_override; const char* save_root; int lp_threads; const char* lp_src; };`. `platform.h` already matches; don't edit either, and don't move `lp_threads`/`lp_src`. S2.interfaces must PASS. |
| D43 | **One server start.** `lane_run.sh` defines `SPIKE="$VK/scripts/spike_server.py"` once, at the top, and exactly one function `preflight() { … }` taking `<outdir> <spool>`. `preflight` does D40 (b), (d), then starts the server, then (c), and is the only code that starts `spike_server.py serve`: one command, `( cd "$1" && exec python3 -B -u "$SPIKE" serve --port 9099 --bulk-port 9100 --spool "$2" ) >"$1/spike.log" 2>&1 &`. The `--check-transport` branch calls `preflight "$OUT" "$OUT/tspool"` (D40e's spool — not `build/m0b3/tspool`). The boot path calls `preflight "$OUT" "$SPOOL"` after D41's ledger check and before the `boots.txt` append, and calls neither `submit` nor `sleep` before that append. No other server start, no reuse of a server that answers. In `lane_run.sh`, no line carries both code and a comment, and no line ends in a backslash followed by blanks. |
| D44 | **Makefile.** `Makefile` is byte-identical to the M0b.1 tree's (`tar -xOf build/m0b2/pre.tar Makefile`); this reverts M0b.2's `ARO_CC_FLAGS` change. r12 stays 0 (measured: `aros_wsi.c` has 0 `mov %rax,%r12` under `-std=gnu11`). |
| D45 | **src/app ≤ 600 lines** by S3's count. Remove lines from `src/app/main.cpp` without changing behaviour: same flags, same help text, same exit codes, Linux frames and music hash unchanged. Don't join statements onto one line or delete comments to make the count. |
| D46 | **One complete pass.** Script 1 truncates `census.txt`, so the census is the last pass. The final pass runs X1, X, 1–11, 13, 14, 14b, 16, [runner], 15, 15b, 12, 12b, 12c in that order with no script skipped or repeated; script 12c FAILs any other census. Fix, then re-run the whole pass. |
| D38 | **Report wording.** The guest is "the QEMU/KVM guest on lane l0025", never "hardware". A behaviour counts as proven on the guest only if a guest log line shows that code path ran (a run that trapped in `pl_init` proves nothing about windows, surfaces or audio). |

## Work items

**W0 — D43** in `tests/aros/lane_run.sh` (update its header comment and every `m0b2` path to
`m0b3`). **W1 — D44** (restore the Makefile). **W2 — D45** in `src/app/main.cpp`. **W3 — keep
everything else identical,** D32–D42 as implemented included: if a change would alter a Linux frame
byte or the music hash, stop and report it.

## Acceptance scripts (run order: 0, X1, X, 1–11, 13, 14, 14b, 16, [runner], 15, 15b, 12, 12b, 12c; paste every output verbatim)

Paste this block at the top of **every** script you run from this prompt (X1 writes it into the
extracted scripts):

````bash
KD=~/Work/Kitty/Docs/Development; DOC=$KD/kitty_engine_design.md; PR=$KD/dispatch/m0b3_aros_fix_opencode_prompt.md
VK=~/Work/projects/Vulkan4Aros; KR=~/Work/Kitty/repo; B=$KR/build/m0b3
LVP_PIN=~/Work/host_lavapipe_kitty/lvp_icd.json; LVP_SYS=/usr/share/vulkan/icd.d/lvp_icd.json
g(){ echo "GATE $1 $2${3:+ — $3}"; echo "GATE $1 $2${3:+ — $3}" >> "$B/census.txt"; }
icd(){ if [ -f "$LVP_PIN" ] && VK_DRIVER_FILES=$LVP_PIN vulkaninfo --summary 2>/dev/null | grep -q 'driverInfo *= *Mesa 26\.2\.2'; then echo "$LVP_PIN"; else echo "$LVP_SYS"; fi; }
GRANT=$(sed -n 's/^ *BOOTS GRANTED: *\([0-9][0-9]*\) *$/\1/p' "$PR" | head -1); LANE=$(sed -n 's/^ *LANE: *\([a-z0-9]*\) *$/\1/p' "$PR" | head -1)
aenv(){ AROS_ABI=v1 . "$VK/scripts/aros_build_env.sh" >/dev/null 2>&1 || return 1; export AROS_SYSROOT="${AROS_SDK_INCLUDE%/include}" PATH="$AROS_COLLECT_AROS_DIR:$AROS_TOOLCHAIN/bin:$PATH"; }
````

**Script 0: baseline. Run it BEFORE editing any file, exactly once.**

````bash
cd "$KR" || exit 1; mkdir -p "$B"; [ -e "$B/baseline.done" ] && { echo "BASELINE EXISTS — not re-taken"; exit 1; }
for e in m0a m0a1; do [ -e "$KR/build/$e/baseline.done" ] || { echo "build/$e evidence missing — wrong tree"; exit 1; }; done
[ -s "$KR/build/m0b/report.md" ] && [ "$(grep -c . "$KR/build/m0b/boots.txt")" = 2 ] || { echo "build/m0b evidence missing — wrong tree"; exit 1; }
[ -s "$KR/build/m0b1/report.md" ] && [ "$(grep -c . "$KR/build/m0b1/boots.txt")" = 1 ] || { echo "build/m0b1 evidence missing or changed — wrong tree"; exit 1; }
[ -s "$KR/build/m0b2/report.md" ] && [ -f "$KR/build/m0b2/pre.tar" ] && [ "$(grep -c . "$KR/build/m0b2/boots.txt")" = 0 ] || { echo "build/m0b2 evidence missing or changed — wrong tree"; exit 1; }
[ "$(git rev-parse HEAD)" = 38fc422d388c9886a38045ff5dc665dab369697d ] || { echo "HEAD is not 38fc422"; exit 1; }
( find .gitignore Makefile src tools shaders tests -type f -print0 | sort -z | xargs -0 sha256sum ) > "$B/pre_files.txt"
th=$(sha256sum < "$B/pre_files.txt" | cut -d' ' -f1); echo "tree hash $th ($(wc -l < "$B/pre_files.txt") files)"
[ "$th" = 38cbe8dad2ec9dbeadd63e98160165d98c46b13f13f0a6b3bc237aebf9f14fe8 ] || { echo "tree differs from the reviewed M0b.2 tree — stop and report"; rm -f "$B/pre_files.txt"; exit 1; }
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

**Script X1: extract M0b's script X, 14, 15 and 12 (verbatim, with the listed patches).** Run it after
script 0 and again whenever you re-run the scripts; then run `bash "$B/scripts/sX.sh"` (M0b's
script X, which extracts the Linux scripts 1–11 and 13 into `$B/scripts/`).

````bash
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
hdr=rep(bl[0]+'\n','PR=$KD/dispatch/m0b_aros_opencode_prompt.md','PR=$KD/dispatch/m0b3_aros_fix_opencode_prompt.md')
hdr=rep(hdr,'B=$KR/build/m0b\n','B=$KR/build/m0b3\n')
sx=rep(bl[2],"'B=$KR/build/m0a\\n','B=$KR/build/m0b\\n'","'B=$KR/build/m0a\\n','B=$KR/build/m0b3\\n'")
s14=rep(bl[3],'grep -q "out/run_$r.log" "$S" 2>/dev/null && grep -q "out/rc_$r.txt" "$S" 2>/dev/null','grep -q "out/run_$r.log" "$K/run_$r.aros" 2>/dev/null && grep -q "out/rc_$r.txt" "$K/run_$r.aros" 2>/dev/null')
s12=rep(bl[5],'git -C "$VK" status --porcelain -- \'src/abi/v1/AROS*\' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal | diff "$B/vk_prot_start.txt" - || { echo "VK protected paths changed in the working tree"; f=1; }',
 'vd=$(git -C "$VK" status --porcelain -- \'src/abi/v1/AROS*\' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal | diff "$B/vk_prot_start.txt" - | grep \'^[<>]\'); [ -z "$vd" ] || { echo "INFO VK protected paths changed in the working tree (the owner works there in parallel) — listed for attribution:"; echo "$vd"; }\n'
 'vs=$(find "$VK/scripts" -newer "$B/baseline.done" 2>/dev/null | head -20); [ -z "$vs" ] || { echo "files created or modified under VK scripts/ during the run (the spike client runs there):"; echo "$vs"; f=1; }\n'
 'nl=$(ss -ltnpH 2>/dev/null | sort | comm -13 "$B/listen_start.txt" -); [ -z "$nl" ] || { echo "listening sockets opened during the run and still open:"; echo "$nl"; f=1; }')
s12=rep(s12,'echo "--- files changed vs 38fc422"','echo "--- files changed vs the reviewed M0b tree"')
s12=rep(s12,'sed "s|$B/pre|38fc422|"','sed "s|$B/pre|m0b-tree|"')
open(f'{out}/hdr.sh','w').write(hdr)
for n,s in (('sX',sx),('s14',s14),('s15',bl[4]),('s12',s12)): open(f'{out}/{n}.sh','w').write(hdr+s+'\n')
print('extracted sX, s14, s15, s12 from the M0b prompt; 7 patches applied')
PY
````

**Scripts 1–11, 13, 14, 15, 12:** `bash "$B/scripts/s<n>.sh"` each (scripts 1–11 and 13 in one loop as
in M0b; paste the whole output). They are M0b's gates, unchanged except for the patches above.

**Script 14b: the M0b.1–M0b.3 host gates. Run after 14.**

````bash
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
e=$(grep -nE '(^|[^A-Za-z])Execute[^;#]*>' "$A"/*.aros "$A/lane_run.sh" 2>/dev/null | grep -vE '^[^:]+:[0-9]+:[[:space:]]*[#;]'); [ -z "$e" ] || f="$f
an Execute line carries a redirect:
$e"
grep -iE 'put[^;]*\.library|vulkan_loader\.library|Copy[^;]*Libs' "$A"/*.aros "$A/lane_run.sh" | grep -qvE '^[^:]+:[[:space:]]*[#;]' && f="$f
a guest script or the runner copies or puts a library"
chk wiring "$f"
f=""; R=$A/lane_run.sh
grep -q 'dispatch/m0b3_aros_fix_opencode_prompt\.md' "$R" && grep -q 'build/m0b3/boots\.txt\|m0b3/boots' "$R" || f="runner does not read this prompt and build/m0b3/boots.txt"
grep -qE 'm0b[12]([^0-9]|$)' "$R" && f="$f
runner still names m0b1 or m0b2"
grep -q 'probe failed; ladder not run' "$R" || f="$f
runner has no D33 probe exit"
grep -q 'realpath -m' "$R" || f="$f
runner does not resolve <outdir> with realpath -m"
n0=$(grep -c . "$B/boots.txt"); o=$(bash "$R" l0025 /tmp/m0b3-outside 2>&1); rc=$?
{ [ $rc = 3 ] && echo "$o" | grep -q 'outdir outside build/m0b3' && [ "$(grep -c . "$B/boots.txt")" = "$n0" ] && [ ! -e /tmp/m0b3-outside ]; } || f="$f
runner did not refuse an outdir outside build/m0b3 (rc=$rc): $(echo "$o" | head -2)"
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
f=""; tar -xOf "$KR/build/m0b2/pre.tar" Makefile | cmp -s - Makefile || f="Makefile differs from the M0b.1 tree's (D44):
$(tar -xOf "$KR/build/m0b2/pre.tar" Makefile | diff - Makefile | head -6)"
chk makefile "$f"
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
  lb1=$(lw); [ "$lb1" = "$lb0" ] && [ "$(grep -c . "$B/boots.txt")" = "$n0" ] || f="$f
--check-transport wrote under ~/Work/lanes/l0025, a boot record or boots.txt: $(comm -13 <(echo "$lb0") <(echo "$lb1") | head -3)"
  chk transport "$f"
fi
````

**Script 16: guest staging (read-only, no boot). Run after 14b, before the runner.**

````bash
cd "$KR" || exit 1; NAV=$VK/scripts/sfs_nav.py; IM=~/Work/vms; S=$B/staging; mkdir -p "$S"
if pgrep -f '^[^ ]*qemu-system.*aros_l0025' >/dev/null; then g S16.guest-staging BLOCKED "lane l0025 is running: images are not read while a guest has them open"; else
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
fi
````

S16 FAIL or BLOCKED means **don't boot**: S15 and S15b are BLOCKED, and the report says what the
staging lacks. The owner may change the staging in parallel; S16 reads it as it is when you run it.

**Boots: none granted.** The M0b.1 grant of 2 on l0025 is spent (three boots were taken). Only the
owner edits the authorization block; until then the runner does not boot, and S15/S15b are BLOCKED.
If the owner grants boots: no TCG, no other lane, never more boots than the block grants.

**Runner** (only when `BOOTS GRANTED` > 0, `LANE` is not `none`, S14 and S14b are all PASS —
S14b.transport included — and S16 is PASS): `bash tests/aros/lane_run.sh "$LANE" "$B/guest"`. Paste its whole output. Boot rules:

- Boot 1 runs the whole sequence: deploy (one `--put` per file, then `List RAM:kitty` checked), the
  prelude, `p`, `pt`, `ptr`, then — only if `p` passed (D33) — the eleven M0b runs.
- A further boot is allowed only if boot 1 failed for a reason in **Kitty code or `tests/aros/`**
  (a `Software Failure. src/…` trap other than D24's loader open, a crash in `kitty`, a runner or
  guest-script defect), you fixed it, and scripts X1 through 14b and 16 are all PASS again.
- **Never boot again for a staging failure**: D24's loader open failing in `p`, `vkCreateInstance`
  or device creation failing for want of an ICD, or `assigns.txt` not showing D32's chain. Those are
  the owner's: S15 and S15b report them, and the round stops there.

**Script 15b: the M0b.1 guest gates. Run after script 15.**

````bash
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
gate('wiring', bool(re.search(r'^\s*LIBS:?\s+DH1:Libs\b',a,re.M|re.I)) and bool(re.search(r'^\s*ENVARC:?\s+DH1:W\b',a,re.M|re.I)), 'assigns.txt: '+(' | '.join(l.strip() for l in a.splitlines() if re.search(r'LIBS|ENVARC|DH1|AROS:Libs',l,re.I))[:300] or 'missing'))
p=rd('run_p.log')
gate('probe', rc('p')==0 and 'vk_loader="vulkan_loader.library"' in p and 'Software Failure' not in p and re.search(r'(^|\s)frames=[1-9]',p) is not None, f"rc={rc('p')} {(re.findall(r'vk_loader=\S+',p) or ['no vk_loader line'])[-1]}")
t=rd('run_pt.log'); un=[os.path.basename(x) for x in sorted(glob.glob(f'{G}/exec_*.log')) if 'unfreed' in open(x,errors='replace').read()]
gate('trap-clean', rc('pt')==4 and re.search(r'^Software Failure\. src/platform/aros/[A-Za-z0-9_/]+\.(cpp|c):[1-9][0-9]*: vulkan loader open failed',t,re.M) is not None and not un, f"rc={rc('pt')} unfreed_in={un}")
gate('plant-release', rc('ptr')==2, f"rc={rc('ptr')}")
emp=[r for r in re.findall(r'^(\w+) start=',rd('host_wall.txt'),re.M) if rc(r) is not None and not rd(f'run_{r}.log').strip()]
gate('output-captured', not emp and bool(re.findall(r'^(\w+) start=',rd('host_wall.txt'),re.M)), f'runs with an rc but an empty log: {emp}')
PY
````

**Script 12b: the boot ledger. Run last, after script 12.**

````bash
cd "$KR" || exit 1
new=$(ls ~/Work/lanes/*/boot-*.txt 2>/dev/null | sort | comm -13 "$B/lane_boots_start.txt" -)
nb=$(echo "$new" | grep -c .); nl=$(grep -c . "$B/boots.txt"); oth=$(echo "$new" | grep -v "/lanes/$LANE/" | grep .)
echo "boot records since script 0: $nb"; echo "$new" | grep .; echo "boots.txt lines: $nl (grant ${GRANT:-0}, lane ${LANE:-none})"; cat "$B/boots.txt"
if [ "$nb" = "$nl" ] && [ -z "$oth" ] && [ "$nl" -le "${GRANT:-0}" ]; then g S12b.ledger PASS
else g S12b.ledger FAIL "records=$nb ledger=$nl grant=${GRANT:-0}${oth:+ other-lane records}"; fi
````

**Script 12c: census completeness (D46). Run last, after 12b.**

````bash
cd "$KR" || exit 1
python3 -I - "$B" <<'PY' && g S12c.complete PASS || g S12c.complete FAIL
import sys,re,os
B=sys.argv[1]; L=[l.split()[1] for l in open(f'{B}/census.txt') if l.startswith('GATE ')]
order=['S1','S2','S3','S4','S5','S6','S7','S8','S9','S10','S11','S13','S14','S14b','S16','S15','S15b','S12','S12b']
sec=[]; [sec.append(x.split('.')[0]) for x in L if not sec or sec[-1]!=x.split('.')[0]]
bad=[]
if sec!=order: bad.append(f'sections in census: {" ".join(sec)}; want: {" ".join(order)}')
once=['S2.interfaces','S3.budgets','S5.build','S13.frames-unchanged','S13.hash10s-unchanged','S14.aros-build','S14.aros-package','S14.runner-refuses','S14b.run-scripts','S14b.wiring','S14b.runner','S14b.plant-and-package','S14b.preflight','S14b.makefile','S14b.transport','S16.guest-staging','S12.end-state','S12b.ledger']
for n in once:
    if L.count(n)!=1: bad.append(f'{n} appears {L.count(n)}x')
for n in ('S5.link','S14.aros-binary'):
    if L.count(n)!=2: bad.append(f'{n} appears {L.count(n)}x (want 2)')
d=[x for x in set(L) if L.count(x)>1 and x not in ('S5.link','S14.aros-binary')]
if d: bad.append(f'gates repeated within the pass: {sorted(d)}')
m=open(f'{B}/make_aros.log',errors='replace').read() if os.path.exists(f'{B}/make_aros.log') else ''
ag=re.findall(r'^aros-gates: .*r12=(\d+)$',m,re.M)
if ag!=['0','0']: bad.append(f'make_aros.log aros-gates r12 values: {ag} (want two, both 0)')
print('\n'.join(bad) if bad else f'census: {len(L)} gate lines, {len(sec)} sections in order'); sys.exit(1 if bad else 0)
PY
````

S12b counts every lane's boot records, so a boot the owner takes in parallel also makes it FAIL:
report the records as they are and let the owner attribute them; never explain a mismatch away.

`assigns.txt`'s exact layout is **UNVERIFIED** (AROS `Assign` listing). If S15b.wiring fails only
because of the layout, report the raw file; don't change the pattern.

**Expected census:** every gate PASS except `S7.golden BLOCKED` and `S15.nvk-smoke BLOCKED`.
Measured on the pinned tree before dispatch (scripts 1–11, 13, 14, 14b, 16): every gate PASS except
`S3.budgets FAIL` (D45), `S7.golden BLOCKED`, and the runner gates `S14b.preflight`, `S14b.makefile`,
`S14b.transport` (D43, D44); both `aros-gates:` lines show `r12=0`. `S14b.transport` is BLOCKED only if something already listens on 9099/9100
when 14b starts; then don't boot. With
`BOOTS GRANTED: 0` the 15 other `S15.*` gates and the five `S15b.*` gates are BLOCKED too. With
boots granted and a D33 probe failure, the S15 gates are FAIL with "no data" values; say so, as
the M0b report did. Three Linux gates are BLOCKED only if there is no display.

**Loop rule:** as in M0b. If any gate prints FAIL, fix the code (never the
scripts, never an earlier prompt) and re-run X1, X, 1–11, 13, 14, 14b, 16, 15, 15b, 12, 12b, 12c in one pass (the runner only within the
boot rules above). Report the final pass only, with its full output. BLOCKED is not FAIL and is never
reported as PASS. Never word code, log lines or file names just to slip past a pattern.

## Final report (write it to `build/m0b3/report.md` and reply with the same text only, no narration)

1. Skills loaded.
2. **The verbatim output of scripts 0, X1, X, 1–11, 13, 14, 14b, 16, (runner), 15, 15b, 12, 12b
   and 12c from the final pass**, census included, each script's exit code.
3. D43–D46 as implemented: file(s) and line numbers, and anything you had to decide. The
   `aros-gates:` line of both AROS builds, quoted from the final pass's `make_aros.log`.
4. S16's output: what DH1 carries, the loader's size and sha256, the ICD path.
5. Per boot: its `boots.txt` line, its `~/Work/lanes/<lane>/boot-*.txt` record, why it was taken, how far the sequence got, the trap or result of
   `p`, `pt`, `ptr`, and `assigns.txt` verbatim.
6. The M0b measurements list (M0b prompt, final report item 6), each with its log line or BLOCKED.
7. Every departure from this prompt or the M0b prompt, with file and line. "none" only if none.
8. Every process you started that is still running (there should be none), and anything needed
   outside the authorization block.
