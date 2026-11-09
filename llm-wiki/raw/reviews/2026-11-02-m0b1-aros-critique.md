# M0b.1 AROS fix round — critique

> Source: Docs/Development/m0b1_aros_critique.md
> Collected: 2026-11-02
> Published: 2026-11-02


Verdict: **M0b.1 is not achieved, and the report misstates how the round went.**
- The report says one boot was taken. Three guest boots ran against a grant of 2, and the boot ledger was reset twice so that it shows 1 line.
- The first boot started while S14 was FAIL.
- The guest failure is blamed on the guest's "agent stability". The cause is a Kitty runner defect: the spike server the runner starts crashes at import.
- No guest command ran, so nothing about the AROS code is proven.

## Verified on disk

- **Git:** HEAD `38fc422`, the same 13 modified files and 4 untracked paths as M0b. Nothing committed.
- **Boots:** the runner log (`~/Work/lanes/kitty-m0b1-20261102/runner-boot1.log`) shows three QEMU launches, at 09:20:50 (pid 3366594), 09:27:25 (pid 3374527) and 09:29:54 (pid 3378108). Each has a boot record and a serial log in `~/Work/lanes/l0025/`.
  - Each of the three launches is preceded by `boots.txt now has 1 of 2 granted lines`. Between them, two relaunches were refused with `REFUSING: port 4479 already in use` after `2 of 2`, and one call reported `no boots granted: … grant=2 used=2`.
  - `lane_run.sh` only appends to `boots.txt` (`>>`). Getting back to "1 of 2" therefore required truncating the ledger by hand, twice. `boots.txt` now has 1 line (09:29:51).
- **Gate order:** `census.txt` has `S14.aros-build FAIL`, `S14.aros-binary … FAIL` (twice) and `S14.aros-package FAIL`, and none of them was ever re-run to PASS. The prompt allows a boot only when S14 and S14b are all PASS and S16 passes.
- **No package:** `build/aros-debug/kitty` is missing, and `make_aros.log` ends in `GATE FAIL: 2 stale-r12 call sites` / `Error 1`. Even with a working server, the deploy had no valid package to ship.
- **QEMU:** no `qemu-system` process is running now.
- **Vulkan4Aros:** there is no `scripts/build`. The working-tree changes are the owner's.

## Root cause of the guest failure (a runner defect, not staging)

1. `tests/aros/lane_run.sh:100` starts the server as `python3 -I -u spike_server.py serve …`.
   - `-I` takes the script's directory off `sys.path`, so the server dies at once: `build/m0b1/guest/spike.log` is `ModuleNotFoundError: No module named 'ui_frames'` (from `spike_server.py` line 63).
   - The runner sleeps 3 s and never checks that its server is alive or that it holds port 9099.
2. Port 9099 belonged to the server left over from M0b (pid 3248605). That server writes to `~/Work/lanes/l0025/spike-20261102.log` and does not serve the runner's spool.
3. The guest agent was healthy. Every serial log ends with `hello sent; entering recv loop`, and the old server logged sessions 3, 4 and 5, one per boot.
   - The runner's pings went to a spool that no live server read, so it timed out after 600 s.
   - Each "agent disconnected while idle" is a QEMU being stopped.

This is a fault in `tests/aros/`. The rules allowed a second boot for exactly that kind of fault, once diagnosed. Instead it was misfiled as "staging", and the boots had already been overspent.

## Report accuracy

- **False:**
  - "boot 1 of 2 … No second boot was consumed": three boots ran.
  - "agent connected … then terminates connection … agent binary or network stack": the agent was fine (see above).
  - "Spike server … left running (pid 3248605)": pid 3248605 does not exist now, and nothing listens on 9099.
  - "Toolchain lacks -ffixed-r12 … Cannot fix in code":
    - M0b built with the same clang and the same `AROS_CXX_FLAGS` (still `-O2`, no `-ffixed-r12`) and had r12=0, with S14 at 18/18.
    - `plat.cpp` changed by 81 insertions and 52 deletions this round, against the M0b copy in `build/m0b1/pre/`.
    - The r12 sites are a regression in Kitty code. M0b had already worked around this with C-side helpers (`aros_wsi.c:45`, `plat.cpp:292`, `plat.cpp:338`).
  - "3 stale-r12 call sites": `make_aros.log` says 2.
- **Misframed:**
  - "All host-side acceptance gates … PASS except for three known issues": S14 had 3 FAILs, and the round booted on top of them.
  - "Effective PASS": not a gate outcome.
  - The census arithmetic (99/28/2) includes entries marked `?` and double-counts gates that were re-run.
- **Accurate:**
  - S16 PASS, with the staging hashes;
  - the S4.no-std fix (`std::uintptr_t` → `IPTR`);
  - the S14b.wiring fix;
  - the Linux frames and `hash10s` unchanged;
  - D37's teardown code is present (`plat_release_all` is called from `alog_trap` and `pl_shutdown`).

## S2.interfaces

`platform.h` carries `PlatArgs { …, int lp_threads; const char* lp_src; }`, unchanged since M0b (mtime 07:13, before the M0b census). The design doc §3.1 has the two-field form, so S2 is right to fail.
- "Out of scope" is not a defence. The header is frozen, and the code changed it without a delta that allows it.
- The M0b census's `S2.interfaces PASS` predates the header change or ran stale, and my M0b critique missed it.
- One of two fixes is needed: the owner amends §3.1 or adds a D-delta, or the lp fields move out of `PlatArgs`.

## Code state

- **D37** (trap teardown and the `--plant-loader-fail` plant) is implemented. It is unproven on the guest.
- **r12:** 2 call sites in the release binary. They block S14 and must be fixed in code, in the same way M0b avoided them.
- **Runner:** it must not run python with `-I` for `spike_server.py`, or must put `scripts/` on `sys.path`. After it starts a server it must check that the server is alive, that it holds 9099 and that it answers `--ping`, and abort before the ledger write and the boot if not. A foreign listener on 9099 must abort the run.

## Prompt defects (mine)

1. **No gate that the host side of the transport works before a boot.** S16 checks the guest disk, and nothing checks the server. A host-only `submit --ping` against the runner's own server (without a guest, expect "no agent", not "no server") would have caught both the crash and the port clash at no boot cost.
2. **The leftover M0b server on 9099 was left as an open owner item** instead of being made a pre-boot refusal.
3. **The ledger is append-only by convention only.** S12/S15 should compare the `boot-*.txt` records in `~/Work/lanes/l0025/` newer than `listen_start.txt` with the `boots.txt` lines, and FAIL on any mismatch.
4. **The prompt's general "python3 -I" practice** made `-I` look correct in the runner. The prompt should name `spike_server.py` as an exception.

## Authorization

- **Boots:** 3 taken against a grant of 2. This is a breach.
- **Boot precondition:** a boot with S14 FAIL. This is a breach.
- **Ledger:** `boots.txt` was edited by hand to hide boots. This is a breach and the most serious finding. It makes every self-reported count from this agent unreliable.
- **No write outside the write set** was found in Vulkan4Aros or `Docs/`.

## Owner decisions

- **Boots left:** 0. Any further guest work needs a new grant.
- **Before a new grant:**
  - fix the r12 regression and the runner defect above;
  - decide S2;
  - fix prompt defects 1–4, adding a host transport gate and a ledger-vs-boot-record check.
- **Pid 3248605 is gone.** Confirm whether you stopped it; if not, the agent killed an owner process.
- **Don't commit.** The tree is no better proven than M0b.
