# M0b.6 re-dispatch critique of the opencode report

> Source: Docs/Development/m0b6r2_aros_critique.md
> Collected: 2026-11-03
> Published: 2026-11-03

# M0b.6 re-dispatch critique — boot 1 on l0025 (2026-11-03)

**Verdict: M0b.6 not achieved. The round itself was run correctly. The guest crashed at the prelude before `kitty` ran; both blockers are in owner-supplied material.**

Dispatch: `Docs/Development/dispatch/m0b6_aros_guest_opencode_prompt.md`, sha256 `97f89d0f20403f0fec1140d596ee19b682351067486fc282430ae5f75039b409` (unchanged). Grant: 2 boots on l0025. Used: 1.

## Verified

- **Scripts:** all 26 scripts in `build/m0b6/scripts/` are byte-identical to a fresh X0 + X1 extraction of the published prompt. No edits.
- **Census:** `build/m0b6/census.txt` has 112 lines: 90 PASS, 20 FAIL, 2 BLOCKED. The seal `e92d88d9…` matches the file. The report's figures are right.
- **Tree:** the only file changed since script 0 is `tests/aros/lane_run.sh`, which is the D54 runner (X2).
- **Ledger:** `boots.txt` has one line and the lane has one new boot record, `boot-20261102-2244.txt`.
- **Lane state:** the lane is inactive, nothing listens on 9099/9100/4479, and no runner is alive.
- **D56:** the decision not to take a second boot is correct. No `Software Failure. src/…` line was logged and `kitty` never ran. `prelude.aros` is pinned by `S14b.wiring`, so no fix inside the write set could keep the pass green.

## Blocker 1: S12c pins are wrong (my defect)

`S12c.complete FAIL` covers `hdr.sh`, `sX.sh`, `s14.sh`, `s15.sh` and `s12.sh`, the five files X1 writes. I took their pins from my scratch dry run. There, the header's `PR=` pointed at the scratch copy of the prompt (`scratchpad/m0b6/m0b6_prompt.md`), not at `$KD/dispatch/m0b6_aros_guest_opencode_prompt.md`. The values the real prompt produces are:

| Script | Pinned (wrong) | Produced by the prompt |
|---|---|---|
| `hdr.sh` | `32a669cc…` | `3a4a3050ed74e5a71c0c89e008255ca109644e4cb29b931aca71ffcd6f2ba931` |
| `sX.sh` | `c50e3984…` | `26f9f0679a451ac1b33dab3e4899dfbaa189cccb723a90fcb40d32930be6d4a9` |
| `s14.sh` | `1cd11b1e…` | `62e55c9a3190843da84e742dc6ccb8a3adb9bb0de1606faafd28627da14fc198` |
| `s15.sh` | `591a690b…` | `57340a882911784bcdf0e31f9e3d2aefd1e545b23f8e700a3e05d6797b67b5b4` |
| `s12.sh` | `bc56a069…` | `eb01f83ce6c50c39625bce08660fbcc45d7ac1fefd7120cb700d5ac6de04b212` |

Opencode's diagnosis (departure 1) is correct.

## Blocker 2: the guest faults inside `Execute RAM:kitty/prelude.aros`

Serial log `~/Work/lanes/l0025/serial-20261102-2244.log`, lines 213–226:

- The agent launches `Execute RAM:kitty/prelude.aros`.
- A user-mode read faults: `CR2=0000000000000088 IP=0000000001bf8d25`, `present=protection`. That is consistent with an access at offset `0x88` from a NULL pointer.
- The serial log stops advancing and the agent never answers again.

This is the prelude's first run on any guest: no other l0025 serial log contains `prelude.aros`.

What is known from earlier lane logs:

- **The same four assigns work as separate agent execs.** `serial-20261024-1815.log` runs `Assign ENVARC: DH1:W`, `Assign LIBS: DH1:Libs` and `Assign LIBS: ADD AROS:Libs` one exec each; every one returns `exec done`.
- **`Execute <script> >log` works as an agent exec.** `run_*.aros` were run that way in two earlier boots.
- **What has never run before:** the four assigns inside one `Execute`'d script with no output redirect.

The cause is **not established**. The fault IP is not mapped to a module. Opencode's `Stack 1048576` remark was offered as a lead only. A fault at NULL+0x88 is not a stack-overflow signature.

## Runner defects (mine, D54)

- **No check after the prelude.** The prelude exec timed out after 300 s, but the runner still sent `p`, `pt` and `ptr` to the wedged agent, at 600 s each. That cost 30 minutes of a boot with no possible result. The runner should stop at a failed prelude, as it stops at a failed deploy.
- **D32 says the prelude runs after `Stack 1048576`.** The runner sends no `Stack` first. This is a spec mismatch, not a proven cause.

## Gate defect

`S15b.output-captured PASS` with no data: no `rc_*.txt` were fetched, so the "rc but empty log" list is empty and the gate passes vacuously. It should FAIL or BLOCK when no run has an rc.

## Report accuracy

The report is accurate on everything checked: script hashes, census, seal, tree, ledger, serial excerpt, and the D56 reasoning.

## Owner decision needed before boot 2

Boot 2 of the grant is unused, but under D56 a prelude failure does not qualify for it. Options:

1. **Diagnose first:** one owner boot that runs the four assigns as separate agent execs (the form proven on 2026-10-24) and then `Execute RAM:kitty/prelude.aros >RAM:kitty/out/prelude.log`, to see which form faults.
2. **Fix the runner without diagnosing:** send the four assigns as separate execs, the form proven on 2026-10-24 (dropping `Execute` for the prelude); stop the runner on a failed prelude; correct the five S12c pins; fix `S15b.output-captured`. Then re-dispatch as M0b.7 with the remaining boot.
