# opencode prompt: M0b.1 (M0b fix round), revision with boot grant

> Source: Work/Kitty/Docs/Development/dispatch/m0b1_aros_fix_opencode_prompt.md
> Collected: 2026-11-02
> Published: 2026-11-02

# opencode prompt — Kitty: Core Awakening, **M0b.1** (M0b fix round: guest wiring and runner, CopperForge engine) (2026-11-02)

## Owner authorization (read first — opencode never edits this block)

    BOOTS GRANTED: 2
    LANE: l0025
    WRITE: ~/Work/Kitty/repo/ — the M0b working tree as reviewed (HEAD 38fc422 plus the uncommitted M0b
           changes; tree hash pinned in script 0). Edit only src/platform/aros/, src/app/main.cpp,
           tests/aros/ and the Makefile's aros-pkg rule. Nothing else under src/, tools/, shaders/,
           tests/. README.md and LICENSE are never edited. build/m0a/, build/m0a1/ and build/m0b/
           (earlier evidence) are never edited or deleted; build/m0b1/, build/aros*/ belong to this round.
    DO NOT EDIT: ~/Work/Kitty/Docs/ (all of it — reports go in build/m0b1/, never in Docs),
           ~/Work/Kitty/AGENTS.md, ~/Work/Kitty/llm-wiki/, every ~/Work/host_lavapipe*/ directory.
    BUILDS: as in M0b (host g++ and the AROS cross-build through the repo Makefile; Vulkan4Aros v1
           toolchain and SDK used read-only). No Vulkan4Aros build script, no library rebuild.
    RUN: the repo's binaries and tests; the acceptance scripts below exactly as given; read-only shell
           commands. A host window may be opened only by scripts 8 and 13 (each run ≤ 65 s).
    IMAGES: ~/Work/vms/aros_l0025_dh0.img and aros_l0025_dh1.img may be READ only by script 16, only
           through Vulkan4Aros scripts/sfs_nav.py, and only while no l0025 guest is running.
    LANES / BOOTS: a boot is any start of an AROS guest. Boots are allowed only while the number of
           lines in build/m0b1/boots.txt is below BOOTS GRANTED, only on the lane named in LANE, only
           through that lane's own launcher (LANE l0025 → ~/Work/projects/Vulkan4Aros/scripts/
           lane_l0025_boot.sh), and only after script 14 and script 14b are all PASS and S16 is PASS.
           Append one line `<ISO time> <lane> <reason>` to boots.txt BEFORE each boot. With BOOTS
           GRANTED: 0 nothing may start a guest, write to ~/Work/lanes/, or submit a job to a spike
           server. Never `Avail FLUSH`, never RemLibrary, never reload a library. Any spike server or
           other process this round starts is stopped before the final report.
    VULKAN4AROS REPO: READ-ONLY (cat/grep/ls/sed -n/git show/git log/git status, running its
           compilers, running scripts/sfs_nav.py and the spike client). Never write a file in it —
           that includes the spike client's output: every path you hand the client is absolute and
           under build/m0b1/. Never run a write or destructive git command in it; never touch
           `src/abi/v1/AROS*`.
    GIT: read-only git commands in ~/Work/Kitty/repo. No commit, no branch, no stash, no tag, no
           push, in any repository. The owner commits M0b after review.
    PACKAGES: install nothing. WEB: read-only research allowed; post nothing, download no binaries.

Don't ask for, infer, widen or grant any authorization yourself. If the work seems to need
something outside this block, stop that item, mark it FAIL or BLOCKED, and list it in the final report.

## What this task is

M0b (the AROS half of M0) built, linked and packaged cleanly, but every guest run trapped at
`src/platform/aros/plat.cpp:702: vulkan loader open failed`, and both boots were spent (review:
`~/Work/Kitty/Docs/Development/m0b_aros_critique.md`). M0b.1 fixes what the review found and nothing else:

- **The guest's Vulkan wiring** (D32): the lane stages the loader, the lavapipe ICD and the ICD
  manifest on `DH1:`; the guest scripts make them reachable with assigns. D24 does not change.
- **A probe inside the boot** (D33): one headless frame runs first; the ladder runs only if it passes,
  so a wiring failure costs one boot, not the whole round.
- **Run output capture** (D34), **runner paths** (D35), **the soak's exec budget** (D36).
- **Trap teardown** (D37): exit 4 releases what `pl_init` acquired; no `unfreed signal`.
- **Report wording** (D38).

Everything else in M0b stands: D20–D31, the Linux gates, frames 0/150/299 byte-identical to
`build/m0a/r1/`, `hash10s=e663d2da535e69a5`. No new features, no design edits, no renames.

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

## Inputs (read before writing code)

`build/m0b/report.md`; `~/Work/Kitty/Docs/Development/m0b_aros_critique.md`; the M0b prompt
`~/Work/Kitty/Docs/Development/dispatch/m0b_aros_opencode_prompt.md` (D20–D31 and W1–W5 still apply);
`tests/aros/*`; `src/platform/aros/*`. Skills: `aros-development` and `karpathy-guidelines` — load
both and say so in the report.

## Design deltas (owner decisions for M0b.1 — implement exactly; list them in the report)

| # | Decision |
|---|---|
| D32 | **Guest wiring** (owner decision 2026-11-02). New `tests/aros/prelude.aros`, packaged by `make aros-pkg`, whose only command lines are, in this order: `Assign ENVARC: DH1:W`, `Assign LIBS: DH1:Libs`, `Assign LIBS: ADD AROS:Libs`, `Assign >RAM:kitty/out/assigns.txt`. `m0b-run.aros` executes it first (after `Stack 1048576`). D24 is unchanged: the bare name `vulkan_loader.library` now resolves through `LIBS:` to `DH1:Libs`, and the run logs `vk_loader="vulkan_loader.library"`. Never copy or put a library to the guest. |
| D33 | **Probe first, ladder only on success.** Three runs precede the eleven M0b runs: `p` = `KD --headless --frames 1 --out RAM:kitty/out/p S`; `pt` = `KD --plant-loader-fail --headless --frames 1 S`; `ptr` = `K --plant-loader-fail --headless --frames 1 S`. If `rc_p.txt` is not `rc=0` or `run_p.log` has no `vk_loader=` line, the runner skips the eleven runs, fetches what exists, stops the lane, and exits 8 with `probe failed; ladder not run`. |
| D34 | **Output capture.** Each `tests/aros/run_<name>.aros` is exactly three command lines: `Stack 1048576`; the program line, written `RAM:kitty/<kitty\|kitty-debug> >RAM:kitty/out/run_<name>.log <arguments>` (the redirect directly after the command name); `Echo "rc=$RC" >RAM:kitty/out/rc_<name>.txt`. No `Execute` line, in any guest script or in the runner, carries a `>` redirect. |
| D35 | **Runner paths and ledger.** `tests/aros/lane_run.sh` reads the authorization from this prompt (`dispatch/m0b1_aros_fix_opencode_prompt.md`) and counts boots in `build/m0b1/boots.txt`. Its first check resolves `<outdir>` with `realpath -m` and refuses (exit 3, `outdir outside build/m0b1`) unless it lies under `$KR/build/m0b1/`; every `--get`/`--put` path it passes is absolute. If it starts a spike server it records the PID in `<outdir>/spike.pid` and stops it before exiting. |
| D36 | **The soak and the 900 s budget.** Keep M0b's handling: on `RUNNING`, poll `Exists RAM:kitty/out/rc_soak.txt` until it exists (or 2,600 s pass); `rc_soak.txt`, written by the run script, is the authority, and the soak's host wall ends when it appears. |
| D37 | **Trap teardown and the plant.** On AROS, every exit-4 path (the Guru line from `pl_trap`/`alog_trap`, D23's timer check, D24's loader check, D26's stack check) first releases, in reverse order, everything `pl_init` acquired up to that point (watchdog process, AHI requests and device, `lowlevel.library`, the timer request and its port), then exits 4. No guest run may print `unfreed`. Debug-only `--plant-loader-fail` (AROS only) makes D24 skip all three `OpenLibrary` calls and take the real failure path (same trap text); release AROS builds and both Linux builds exit 2 on the flag. |
| D38 | **Report wording.** The guest is "the QEMU/KVM guest on lane l0025", never "hardware". A behaviour counts as proven on the guest only if a guest log line shows that code path ran (a run that trapped in `pl_init` proves nothing about windows, surfaces or audio). |

## Work items

**W0 — D37** in `src/platform/aros/` and the flag in `src/app/main.cpp` (help text and exit-2 rule
like `--present-dump`). **W1 — D32–D36** in `tests/aros/` and the `aros-pkg` rule (the package gains
`prelude.aros` and `run_p.aros`, `run_pt.aros`, `run_ptr.aros`). **W2 — keep everything else
identical:** if a change would alter a Linux frame byte or the music hash, stop and report it.

## Acceptance scripts (run order: 0, X1, X, 1–11, 13, 14, 14b, 16, [runner], 15, 15b, 12; paste every output verbatim)

Paste this block at the top of **every** script you run from this prompt (X1 writes it into the
extracted scripts):

````bash
KD=~/Work/Kitty/Docs/Development; DOC=$KD/kitty_engine_design.md; PR=$KD/dispatch/m0b1_aros_fix_opencode_prompt.md
VK=~/Work/projects/Vulkan4Aros; KR=~/Work/Kitty/repo; B=$KR/build/m0b1
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
[ "$(git rev-parse HEAD)" = 38fc422d388c9886a38045ff5dc665dab369697d ] || { echo "HEAD is not 38fc422"; exit 1; }
( find .gitignore Makefile src tools shaders tests -type f -print0 | sort -z | xargs -0 sha256sum ) > "$B/pre_files.txt"
th=$(sha256sum < "$B/pre_files.txt" | cut -d' ' -f1); echo "tree hash $th ($(wc -l < "$B/pre_files.txt") files)"
[ "$th" = 4f0aec2dd4ee9f555f67dc756c2e2f05635193384be7f820ebf8ec7a21623cef ] || { echo "tree differs from the reviewed M0b tree — stop and report"; rm -f "$B/pre_files.txt"; exit 1; }
tar -cf "$B/pre.tar" .gitignore Makefile src tools shaders tests
git branch --list --format='%(refname:short) %(objectname)' > "$B/branches_start.txt"
git -C "$VK" rev-parse HEAD > "$B/vk_head.txt"
git -C "$VK" status --porcelain -- 'src/abi/v1/AROS*' src/abi-patches/v1 scripts src/vulkan/loader src/vulkan/pal > "$B/vk_prot_start.txt"
( cd ~/Work/Kitty && find Docs llm-wiki AGENTS.md -type f -print0 | sort -z | xargs -0 sha256sum ) > "$B/protected_start.txt"
( cd ~/Work/Kitty && find Docs llm-wiki AGENTS.md -type f | sort ) > "$B/protected_list_start.txt"
pgrep -f '^[^ ]*qemu-system' | sort > "$B/qemu_start.txt"; : > "$B/boots.txt"
ss -ltnpH 2>/dev/null | sort > "$B/listen_start.txt"
date +%s > "$B/baseline.done"; : > "$B/census.txt"
echo "GRANT=$GRANT LANE=$LANE"; [ -n "$GRANT" ] && [ -n "$LANE" ] || { echo "authorization block unreadable"; exit 1; }
echo "VK HEAD $(cat "$B/vk_head.txt")"; echo "VK protected status lines: $(wc -l < "$B/vk_prot_start.txt")"; echo "protected Kitty files: $(wc -l < "$B/protected_start.txt")"; echo "qemu processes at start: $(wc -l < "$B/qemu_start.txt")"; echo "listening sockets at start: $(wc -l < "$B/listen_start.txt")"
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
hdr=rep(bl[0]+'\n','PR=$KD/dispatch/m0b_aros_opencode_prompt.md','PR=$KD/dispatch/m0b1_aros_fix_opencode_prompt.md')
hdr=rep(hdr,'B=$KR/build/m0b\n','B=$KR/build/m0b1\n')
sx=rep(bl[2],"'B=$KR/build/m0a\\n','B=$KR/build/m0b\\n'","'B=$KR/build/m0a\\n','B=$KR/build/m0b1\\n'")
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

**Script 14b: the M0b.1 host gates. Run after 14.**

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
grep -q 'dispatch/m0b1_aros_fix_opencode_prompt\.md' "$R" && grep -q 'build/m0b1/boots\.txt\|m0b1/boots' "$R" || f="runner does not read this prompt and build/m0b1/boots.txt"
grep -q 'probe failed; ladder not run' "$R" || f="$f
runner has no D33 probe exit"
grep -q 'realpath -m' "$R" || f="$f
runner does not resolve <outdir> with realpath -m"
n0=$(grep -c . "$B/boots.txt"); o=$(bash "$R" l0025 /tmp/m0b1-outside 2>&1); rc=$?
{ [ $rc = 3 ] && echo "$o" | grep -q 'outdir outside build/m0b1' && [ "$(grep -c . "$B/boots.txt")" = "$n0" ] && [ ! -e /tmp/m0b1-outside ]; } || f="$f
runner did not refuse an outdir outside build/m0b1 (rc=$rc): $(echo "$o" | head -2)"
chk runner "$f"
f=""; grep -q -- '--plant-loader-fail' src/app/main.cpp || f="no --plant-loader-fail in main.cpp"
VK_DRIVER_FILES=$(icd) build/linux/kitty --plant-loader-fail --headless --frames 1 > /dev/null 2>&1; r1=$?
VK_DRIVER_FILES=$(icd) build/linux-debug/kitty --plant-loader-fail --headless --frames 1 > /dev/null 2>&1; r2=$?
[ $r1 = 2 ] && [ $r2 = 2 ] || f="$f
Linux must exit 2 on --plant-loader-fail (release=$r1 debug=$r2)"
for x in prelude.aros run_p.aros run_pt.aros run_ptr.aros; do cmp -s "$A/$x" "$K/$x" || f="$f
$K/$x missing or differs from $A/$x"; done
chk plant-and-package "$f"
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

**Boots: 2 on lane l0025 (owner grant 2026-11-02).** No TCG, no other lane, never a third boot.

**Runner** (only when `BOOTS GRANTED` > 0, `LANE` is not `none`, S14 and S14b are all PASS and S16 is
PASS): `bash tests/aros/lane_run.sh "$LANE" "$B/guest"`. Paste its whole output. Boot rules:

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

`assigns.txt`'s exact layout is **UNVERIFIED** (AROS `Assign` listing). If S15b.wiring fails only
because of the layout, report the raw file; don't change the pattern.

**Expected census:** every gate PASS except `S7.golden BLOCKED` and `S15.nvk-smoke BLOCKED`. With
`BOOTS GRANTED: 0` the 15 other `S15.*` gates and the five `S15b.*` gates are BLOCKED too. With
boots granted and a D33 probe failure, the S15 gates are FAIL with "no data" values; say so, as
the M0b report did. Three Linux gates are BLOCKED only if there is no display.

**Loop rule:** as in M0b. If any gate prints FAIL, fix the code (never the scripts, never an earlier
prompt) and re-run X1, X, 1–11, 13, 14, 14b, 16, 15, 15b, 12 in one pass (the runner only within the
boot rules above). Report the final pass only, with its full output. BLOCKED is not FAIL and is never
reported as PASS. Never word code, log lines or file names just to slip past a pattern.

## Final report (write it to `build/m0b1/report.md` and reply with the same text only, no narration)

1. Skills loaded.
2. **The verbatim output of scripts 0, X1, X, 1–11, 13, 14, 14b, 16, (runner), 15, 15b and 12 from
   the final pass**, census included, each script's exit code.
3. D32–D38 as implemented: file(s) and line numbers, and anything you had to decide.
4. S16's output: what DH1 carries, the loader's size and sha256, the ICD path.
5. Per boot: its `boots.txt` line, why it was taken, how far the sequence got, the trap or result of
   `p`, `pt`, `ptr`, and `assigns.txt` verbatim.
6. The M0b measurements list (M0b prompt, final report item 6), each with its log line or BLOCKED.
7. Every departure from this prompt or the M0b prompt, with file and line. "none" only if none.
8. Every process you started that is still running (there should be none), and anything needed
   outside the authorization block.
