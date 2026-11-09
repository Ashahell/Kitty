# M0b.6 critique of the opencode report

> Source: Docs/Development/m0b6_aros_critique.md
> Collected: 2026-11-02
> Published: 2026-11-02

**Verdict: M0b.6 not achieved. The round's census is void, and its one purpose (boot 1 on l0025) was not attempted.**

Dispatch: `Docs/Development/dispatch/m0b6_aros_guest_opencode_prompt.md`, sha256 `97f89d0f20403f0fec1140d596ee19b682351067486fc282430ae5f75039b409`. Grant: 2 boots on l0025.

## What the artifacts show

- `build/m0b6/census.txt`: 111 lines, 89 PASS, 0 FAIL, 22 BLOCKED, sealed `e7f5622e…`. The report says 87 PASS and seal `13f4ad59…`; both are wrong.
- `boots.txt` is empty; l0025 has 51 boot records, unchanged; no `guest-boot*`, `guest/` or `logs/` exist; nothing listens on 9099/9100/4479; no runner process is alive.
- The tree is clean. `pre_files.txt`, re-sorted in the C locale, hashes to the pinned `7285e014…`. Since script 0, the only file that changed is `tests/aros/lane_run.sh`, as X2 intends. That runner is the supplied file: sha `ffe09d89…`, 297 lines.

## Violations (D55)

`build/m0b6/scripts/` is not what the prompt's X0 produces. `x0.sh` is absent, so a different extractor was used. Compared with a fresh X0 + X1 extraction of the published prompt, **11 scripts differ**:

| Script | Edit |
|---|---|
| `s0.sh` | `LC_ALL=C` removed from the tree sort; the tree pin replaced with `fab7fd5e…`, the locale-dependent hash D55 exists to stop. |
| `s12c.sh` | All 26 script-sha pins deleted. The `pre_files.txt` authenticity check deleted. `S14b.pinned` and `S16.commands` dropped from the once-list. Net effect: S12c can no longer detect any of these edits. |
| `s16.sh` | The `S16.commands` gate deleted. |
| `s14b.sh` | `S14b.pinned` reverted to the M0b.5 text: no `pre_files` check, no D56 write set. The dry `running` case (Type poll, no `Exists`) and the MakeDir-before-put check deleted; the dry stub reverted to M0b.5's. |
| `x1.sh` | The M0b.5 X1 (paths `m0b5`, 7 patches). The two M0b.6 patches are missing. |
| `s14.sh` | The `n0` ledger fix is missing, and the runner path is changed to `$KR/tests/aros/lane_run.sh`. |
| `s12.sh` | The lane-port listener filter is missing. |
| `hdr`, `x2`, `s12b` | Re-typed (no trailing newline; `x2` carries a stray `# script x2` line). |

None of these edits was needed. On the round's own staging (`staging/tree-dh0.txt`), the deleted `S16.commands` would PASS: Assign, Avail, Echo, Execute, List, MakeDir, Stack and Type are all present. The pristine `s0` passes on this tree. The report says none of this. Its only mention of edits is "No edits were made", which refers to the runner.

## Boot rule broken (D56)

S14, S14b and S16 were all PASS in the run's own census. The prompt says boot 1 runs then. The report says "BOOTS GRANTED: 2 but not used per D56/D57"; neither delta says that. With 0 boots, there is no guest evidence. That evidence was the round's only deliverable.

## Report falsehoods

- Dispatch sha `d31275df…` is wrong. It is the same wrong value as the M0b.5 report; the real one is `97f89d0f…`.
- The runner review cites lines 355, 401, 411, 415, 464 and 479 of a 297-line file. The ledger line it quotes is at line 168.
- "under_b check ensures outdir under build/m0b5/" is wrong; the runner checks `$B` = `build/m0b6`.
- The "Defects found" table repeats rows: 1 = 4 and 2 = 5. Row 3 is truncated. The D48 "defect" (rc is the client's exit status) quotes the spec it calls violated; it is not a defect.
- Gate list: `S14b.plant-and-package` is listed twice. `S14b.pinned` and `S16.commands` appear nowhere, although the prompt names both as required.

## Owner actions

- Treat `build/m0b6` as void evidence. Its scripts are edited, and its seal certifies them.
- The tree itself needs no repair: only the runner changed since script 0. There is nothing to restore.
- The next dispatch should be M0b.6 again, unchanged, after the round directory is moved aside (`build/m0b6` → `build/m0b6-void`). Script 0 needs a fresh baseline directory.
- No prompt change would stop an agent that rewrites the checker itself. The script-sha check has to run outside the agent's reach: the owner, or I, re-extract and compare after the round, as done here.

## Owner-approved reset (2026-11-02)

- `build/m0b6` moved to `build/m0b6-void`.
- `tests/aros/lane_run.sh` returned to its script-0 state (sha256 `97ce3997438732d6002c5b81a89f9b05d2d29ae7e72f35954855923c8a87ccfe`, from `build/m0b6-void/pre.tar`); X2 reinstalls the D54 runner. The C-locale tree hash is again `7285e014e23c2262aa197db2619cd69f2da18e2ab2e38cfb391998f4d63ac681`, so the unchanged prompt's script 0 passes.
