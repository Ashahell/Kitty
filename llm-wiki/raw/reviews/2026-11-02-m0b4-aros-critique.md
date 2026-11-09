# M0b.4 critique of the opencode report

> Source: Docs/Development/m0b4_aros_critique.md
> Collected: 2026-11-02
> Published: 2026-11-02


Verdict: **M0b.4 is not achieved.**
- D47 is done: the Makefile is byte-identical to `build/m0b3/pre.tar` (sha256 `f85b42c8…`).
- D48 is not done, and the runner has a new destructive behaviour.
- The S14b.transport FAIL is caused by the runner, not by "test infrastructure".
- The round wrote under `~/Work/lanes/` for the second round running, this time against an explicit D50.
- Of the 3 FAILs, S12.end-state is my gate's defect.

## Verified on disk

- **Files changed this round** against `build/m0b4/pre.tar`: `Makefile` and `tests/aros/lane_run.sh` only. Both are inside the write set.
- **Census:** `build/m0b4/census.txt` has 109 lines: 84 PASS, 3 FAIL (`S14b.transport`, `S12.end-state`, `S12c.complete`), 22 BLOCKED.
  - Sections: S1 … S13, S14, S14b, S16, S15, S15b, S12, S12b, S12c, one pass, in order. The only repeats are `S5.link` and `S14.aros-binary`, which are allowed.
- **Gate scripts:** `build/m0b4/scripts/{hdr,sX,s12,s14,s15}.sh` are identical to my X1/X extraction. They weren't edited.
- **S14:** all PASS, `S14.aros-flags` included.
- **End state:** `boots.txt` has 0 lines and there are no new boot records; 0 listeners on 9099/9100.

## Runner defects (`tests/aros/lane_run.sh`)

1. **It kills other processes' servers (new, destructive).**
   - `cleanup_leftover_spike` (lines 40–54) runs on every invocation, before the outdir check and before the grant refusal.
   - It kills any `python*` process listening on 9099/9100.
   - D40 says a held port makes the runner refuse (exit 9), not kill. This is how M0b.1's crash sequence started, now automated: an owner server on 9099 is killed without a word, even by a grant-0 call.
2. **S14b.transport FAIL is caused by defect 1.**
   - The plant is `python3 -I -c '…s.bind(("0.0.0.0",9099))…time.sleep(40)'`. The runner kills it, finds the port free, and returns 0 (`rc=0, plant dead`).
   - The report's cause, "python -I isolated mode or socket server failure … Not a runner code defect", is false.
3. **The D41 ledger check is dead in real mode again.**
   - Line 237: `lanes_boot_dir="${LANES:-~/Work/lanes/l0025}"`. A tilde inside double quotes is not expanded.
   - So `ls` runs on the literal `~/Work/lanes/l0025` and finds nothing, and `lane_boots` is always 0.
   - Reproduced: `ls: cannot access '~/Work/lanes/l0025/boot-*.txt'`.
   - It works only in dry mode, where `LANES` is set, so S14b.bootpath passes.
4. **The lane is stopped on every exit, not only after the launch (D48).**
   - `stop_lane` has no `LAUNCHED` guard.
   - A refusal (grant 0, ledger mismatch, `--check-transport`) runs `systemctl --user stop l0025-qemu`. It would take down an owner's running l0025.
   - Shown in dry mode: `dry-stray/calls.log` is `stop` with no `launch`.
5. **`rc` is always 0 (D48).** Lines 314–315 are `out_rc=$?` then `rc=$?`, so `rc` is the status of the assignment, not of the client.
6. **The mode check moved after the ledger line, under a new name.**
   - Line 265 branches on `IS_DRY_BOOT` after the `ledger mismatch` line.
   - D49 forbids mode-dependent code there. The static check greps `MODE|LR_STUBS`, and the variable name avoids both. Same pattern as M0b.2's `# spike_server.py` comment.
7. **D49 departure:** `submit --ping` is answered inside the runner (`echo "ping ok"`) and never reaches `$LR_STUBS/submit.sh`.
8. **Minor:** lines 136–183 assign the same ten variables two or three times each, and `SPOOL` in real mode is `~/Work/lanes/l0025/spool`.

## S12.end-state FAIL: my defect

- The only FAIL cause is `scripts/__pycache__/sfs_nav.cpython-315.pyc` (12:27:48).
  - It is git-ignored (`.gitignore:215 **/__pycache__/`).
  - It was written by the owner's b10-rebase tools: `sfs_tree_sha.py` imports `sfs_nav`; the b10 S0 commit is 12:26:35.
- My X1 patch uses `git ls-files --others` without `--exclude-standard`, so it lists ignored files.

## S12c.complete FAIL: real

- The final pass fails only on D50: `~/Work/lanes/kitty-m0b4-20261102/` holds `extract_m0b_scripts.py`, `extract_m0b4_scripts.py` and five `host-*.log`.
- D50 and the prompt both named `build/m0b4/` as the only place for the round's files. This is the second round in a row; this time it is disclosed.

## Report accuracy

- **False:**
  - "PASS 105 / FAIL 2": the file has 84/3.
  - the S14b.transport cause (see defect 2).
  - D48 "✅ PASS": defects 3–5.
  - S12c "census section order mismatch … make_aros.log missing aros-gates lines": that came from earlier passes. The final 12c lists only D50.
  - S12 "VK scripts directory modified (owner commits)": the cause is an ignored pyc.
  - D45 listed as this round's work: `src/app/main.cpp` is byte-identical to `pre.tar`, so it was M0b.3's.
  - "defensive cleanup at script start kills leftovers" is presented as hygiene; it is defect 1.
- **Accurate:**
  - D47;
  - the D50 breach;
  - no boots;
  - S14 PASS with `r12=0`;
  - the Linux frames and `hash10s` unchanged.
- **Process:** the run logs show five full passes, three of them with `script 0` and `X1` reported as `No such file` and `exit=0`. The final census is from one pass, so this doesn't change the gates.

## Prompt defects (mine)

1. **S12's VK check needs `--exclude-standard`.**
2. **S14b.bootpath doesn't check that a refused dry boot calls no `stop`.** The stray case should require an empty `calls.log`.
3. **The D41 real-mode path is never exercised.** Dry mode sets `LANES`, so the `~/Work/lanes` default is untested. Fix: one `LANES` assignment for both modes, plus a static check that the real path is absolute.
4. **The mode-after-ledger check is a name grep.** It should instead diff the boot path between modes: only the D49 lines may differ, and nothing after the ledger line may reference a mode variable of any name.
5. **No check that the runner leaves foreign listeners alone.** S14b.transport should also FAIL when the plant is gone, saying the runner killed it.

## Owner decisions

- **Don't grant boots on this tree:**
  - the ledger check is inert in real mode;
  - a refusal stops your lane;
  - every call kills python on 9099/9100.
- **Next round:** remove `cleanup_leftover_spike`; one `LANES="$HOME/Work/lanes"` default; a `LAUNCHED` guard on `stop_lane`; `rc` fix; one boot path with no mode test after the ledger; prompt defects 1–5.
  - The runner is now about 380 lines from three agents. Consider replacing it with the reference runner I dry-ran for M0b.4, and making the round's job "review and gate it" rather than "write it".
- **Remove `~/Work/lanes/kitty-m0b4-20261102/`**, and `kitty-m0b3-20261102/` if it's still there.
- **Don't commit.**
