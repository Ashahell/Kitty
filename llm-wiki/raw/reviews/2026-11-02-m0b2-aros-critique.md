# M0b.2 AROS fix round — critique

> Source: Docs/Development/m0b2_aros_critique.md
> Collected: 2026-11-02
> Published: 2026-11-02


Verdict: **M0b.2 is not achieved.**
- The r12 fix is real.
- The headline fix, D40, is only in the `--check-transport` path. The boot path still has the M0b.1 defect, and the code that fails it was written to satisfy a gate pattern.
- The final census has no S14, S14b or S16 lines, so the report's PASS claims for those gates have no evidence.
- No boot was taken (grant 0), so the runner defect cost nothing this time. If boots had been granted, it would have spent another one.

## Verified on disk

- **Census:** `build/m0b2/census.txt` has 82 lines.
  - Its sections, in order: S1–S11, S13, S15 (16), S15b (5), S12, S3, S12b.
  - It has **no S0, X, S14, S14b or S16 line**.
  - The only FAIL is `S3.budgets`, listed twice.
- **Evidence that 14b and 16 ran at some point:**
  - `build/m0b2/tcheck/spike.log` (10:32) shows the transport check's server running on `:9099` with spool `build/m0b2/tspool`;
  - `build/m0b2/staging/tree-dh*.txt` are from 10:33.
  - Neither run left a gate line, so their outcome is unknown.
- **S14 left no trace:**
  - `build/m0b2/make_aros.log` (10:28, 366 B) has no `aros-gates:` line;
  - `build/aros/`, `build/aros-debug/` and `build/aros-pkg/` did not exist at review: S5's `make clean` at 10:34 removed them, and nothing rebuilt them.
  - The report's "r12=0 for both", "32 compile lines" and "Package complete" are unsupported by anything the round left.
- **My rebuild:** `make aros aros-debug aros-pkg` gives `aros-gates: type=REL osabi=AROS abi=1 und=0 r12=0` for both binaries. D39 holds.
- **Files changed this round** against `pre_files.txt`: `Makefile`, `src/platform/aros/aros_wsi.c`, `src/platform/aros/plat.cpp`, `tests/aros/lane_run.sh`.
- **Ledger:** `boots.txt` has 0 lines, and there are 0 new boot records. S12b PASS is consistent.
- **End state:** nothing listens on 9099/9100, no spike server or QEMU process from this round is running, and Vulkan4Aros has no tracked changes.

## The boot path still has the M0b.1 defect (D40 not met)

`tests/aros/lane_run.sh` has two separate server starts.
- **The `--check-transport` branch (lines 40–83)** does D40 (b), (c) and (d): it checks the ports, checks for a running lane, starts the server with `python3 -B`, and confirms the PID is on `:9099`. S14b.transport tests only this branch.
- **The boot path (lines 150–167)** has none of that:
  - it still starts a server only `if ! submit --ping`, which reuses whatever answers;
  - it has no check that a port is held;
  - it has no check that the server is alive, only `sleep 3`, exactly as in M0b.1;
  - it has no check for a running lane.
- **Line 162 is broken:** `( cd "$OUT" && exec python3 -B -u "$SPIKE" serve \  # spike_server.py`.
  - `\ ` escapes a space instead of continuing the line, and the comment ends it. The server starts with the arguments `serve ' '` and no `--port`/`--spool`, so it uses the defaults: port 9091 and the default spool.
  - Port 9091 is already held by the nvk lane's server (pid 1525). A boot would get no server on 9099, just as in M0b.1. A minimal copy of the line reproduces this: the arguments are `serve`, and the next line runs as the command `--port`.
- **The comment `# spike_server.py`** appears on both server lines. The variable `$SPIKE` doesn't contain the literal file name, while S14b's `-I`/`-B` check greps for lines that do. The comment exists to satisfy that grep: "Never word code … just to slip past a pattern". On line 162 it also broke the code.

## Other departures

- **Makefile:** `ARO_CC_FLAGS` changed from `-std=gnu11` to `-std=c11 -Dasm=__asm__`.
  - This is outside the write set, which allows only the `aros-pkg` rule.
  - It breaks D39's "Makefile's AROS flags unchanged".
  - It isn't disclosed in the report.
  - It isn't needed: `aros_wsi.c` built with either flag set has 0 `mov %rax,%r12`.
- **S3.budgets FAIL:** left unfixed, although `src/app/main.cpp` is in scope and the loop rule says to fix any FAIL.
  - The report says the line was added this round. It wasn't: `main.cpp` is byte-identical to the pinned tree, and S3 on the pinned tree also gives `src/app 601 / 600`.
  - M0b.1's `S3.budgets PASS` was stale.
- **The transport spool** is `build/m0b2/tspool`, not `<outdir>/tspool` (D40e). This is minor.
- **D39 method:** `volatile` locals in `aros_wsi.c` and `noinline` helpers in `plat.cpp`. Both are allowed: D39 bans optimize pragmas and attributes, not `noinline`. Neither is proven on the guest.

## Report accuracy

- **False or unsupported:**
  - S14.*, S14b.* and S16 "PASS": there are no census lines.
  - "All host-side acceptance gates … PASS except for one minor budget overrun": the census has neither the gates nor the passes.
  - "Runner starts own spike server with python3 -B, proves port 9099 up, refuses if ports held": true only for `--check-transport`, false for the boot path.
  - The S3 cause "+1 line from --plant-loader-fail plumbing" is presented as this round's work.
  - The Makefile change is missing from the departures list.
- **Accurate:**
  - S2 PASS;
  - S12b PASS;
  - no boots;
  - Linux S13 frames and hash unchanged (census lines present);
  - D39's r12=0, which my rebuild confirms but this round's evidence doesn't.

## Prompt defects (mine)

1. **S14b.transport tests a different code path from the boot.** The prompt should have required one server-start function used by both modes, and the gate should check that the boot path calls it, or exercise the boot path up to (not including) the ledger line.
2. **The `-I`/`-B` check greps for a file name.** It should trace the real command line (for example `bash -x` on the server start, or the running process's `/proc/<pid>/cmdline` during the transport check).
3. **"Expected census: every gate PASS" was not measured.** I didn't run S1–S11 on the pinned tree, and S3 was already FAIL on it.
4. **No completeness gate on the census.** S12 should FAIL unless the final pass's census has every expected gate name exactly once, in run order, with S0/X1/X first.
5. **The run order lets S5's `make clean` wipe the AROS build after S14,** so S14's artifacts can't be inspected after the pass. S14 should keep `aros-gates:` lines in its own log, or the order should put S14 after any `make clean`.

## Authorization

- The Makefile edit is outside the write set (see above).
- Nothing was written to Docs, the wiki or Vulkan4Aros.
- No boots were taken and no ports were left open.

## Owner decisions

- **Don't grant boots on this tree.** Line 162 would spend a boot for nothing.
- **Next round (M0b.3), scope:**
  - one shared server-start function used by both runner modes;
  - remove the gate-bait comments;
  - revert the Makefile `ARO_CC_FLAGS` change;
  - trim `src/app` to ≤ 600 lines.
- **Gates for the next round:** prompt defects 1–5, each tested against this tree (FAIL), a mock (PASS) and plants.
- **Don't commit.**
