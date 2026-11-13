# M0b.23 AROS guest DSH prompt

> Source: Docs/Development/dispatch/m0b23_aros_guest_dsh_prompt.md
> Collected: 2026-11-13
> Published: 2026-11-12

# DSH prompt — Kitty: Core Awakening, **M0b.23** in DSH Desktop (owner runs the two boots) (2026-11-12)

This file adapts the M0b.23 round to DSH Desktop. **The round itself is defined by
`~/Work/Kitty/Docs/Development/dispatch/m0b23_aros_guest_opencode_prompt.md` (the base prompt).**
Read it in full before doing anything. Every rule, script, gate and report item in it applies
to you as written, with "opencode" read as "you". Its "Owner authorization" block binds you
exactly as written. This file only adds the DSH amendments DSH1–DSH7 below. Where an amendment
and the base prompt differ, the amendment wins. Nowhere else does anything change.

## Owner checklist (before the first message to DSH)

1. Start DSH normally, not in safe mode (a harness crash restarts it in safe mode; quit it and
   start it again). Open a **new** session with **`~/Work/Kitty/repo` as its workspace**, in the
   normal sandbox (`workspace-write`, the default). Don't use `danger-full-access`.
2. Confirm no lane and no runner is up, in a normal terminal:

   ```bash
   pgrep -af 'qemu-system|lane_run\.sh'; ss -ltnp | grep -E ':(4479|9099|9100)\s'
   ```

   Both must print nothing.
3. Paste this whole file into DSH as the first message.

## Why the boots are the owner's

The lane launcher (`~/Work/projects/Vulkan4Aros/scripts/lane_l0025_boot.sh`) starts QEMU with
`systemd-run --user`. Inside DSH's sandbox that call fails with `Failed to connect to user scope
bus via local transport` (measured 2026-11-11 with DSH's exact bwrap profile). So DSH cannot boot
the lane. Everything else runs in DSH: the base prompt's host pass under DSH's bwrap profile
rehearsed at 93 PASS / 0 FAIL / 29 BLOCKED (2026-11-12, M0b.23 prompt), every script exit 0, with each script in its own sandboxed call and no
read-only-filesystem errors.

## Amendments

**DSH1 — your environment.**
- **Every `bash` call is a fresh shell:** no variable, function or `cd` carries over. Give every
  command its own `workdir` (`/home/miller/Work/Kitty/repo`) and full paths.
- **`/tmp` is private to each call and empty at the start.** Never leave anything in `/tmp` for a
  later call.
- **Writable:** only `~/Work/Kitty/repo`, `~/Work/vms` and `~/Work/lanes`. Everything else is
  read-only. A `Read-only file system` or `Permission denied` error is a result: report it, and
  don't work around it.
- **You see only your own processes** (separate PID namespace). `pgrep`, `ps` and `kill` cannot see
  or reach the host's QEMU, runner or spike server. DSH4 covers what that means for the gates.
- **There is no systemd user bus.** Never run `systemctl` or `systemd-run`.
- **Nothing you start outlives its call.** When a `bash` call ends, the sandbox kills every process
  it started, including ones started with `setsid`, `nohup` or `&`. Only `run_in_background` jobs
  persist.
- **Never run `tests/aros/lane_run.sh` in any mode that boots** (`lane_run.sh l0025 …`), nor any
  `lane_*boot*.sh`, nor `qemu-system-*`. This holds even if the owner says you may use bash, says
  "go ahead", or the boot seems late. Only the owner boots (DSH3). On 2026-11-11 a DSH run broke
  this rule: the runner wrote its ledger line, then was killed when the call ended, with no boot
  behind it, which made the runner refuse every later boot.
- **Never request sandbox escalation** (`sandbox_permissions`, `justification`) and never change the
  permission mode. A command the sandbox refuses is reported, not retried with more access.
- **Never run `kill`, `pkill`, `killall` or `job_kill`** on anything except a job you started in
  this session.
- **Web:** none needed. Post nothing and download nothing.

**DSH2 — running the scripts.** Never retype a script from the base prompt; always extract it.
- **Script X0:** the base prompt says to paste the header and X0. Run this exact command instead.
  It extracts both blocks from the base prompt and runs them as one script. Workdir is the repo:

  ```bash
  P=/home/miller/Work/Kitty/Docs/Development/dispatch/m0b23_aros_guest_opencode_prompt.md
  ex(){ python3 -I - "$P" "$1" <<'PY'
  import sys
  t=open(sys.argv[1]).read().split('\n'); i=t.index('# script '+sys.argv[2]); j=t.index('````',i)
  print('\n'.join(t[i+1:j]))
  PY
  }
  L=/home/miller/Work/Kitty/repo/build/m0b23/logs; mkdir -p "$L"
  bash -c "$(ex hdr)
  $(ex x0)" > "$L/x0.out" 2>&1; echo "SCRIPT x0 exit=$?"; cat "$L/x0.out"
  ```

- **Every other script** (`s0`, `x1`, `x2`, `x3`, `x4`, `sX`, `s1`–`s11`, `s13`, `s14`, `s14b`,
  `s16`, then after the boots `s15`, `s15b`, `s12`, `s12b`, `s12c`), in the base prompt's order.
  Run each as its own command, with its output in a file:

  ```bash
  B=/home/miller/Work/Kitty/repo/build/m0b23; mkdir -p "$B/logs"; bash "$B/scripts/<name>.sh" > "$B/logs/<name>.out" 2>&1; echo "SCRIPT <name> exit=$?"
  ```

  Then read `$B/logs/<name>.out` with the file tool. That file is the verbatim output the report
  pastes. Run `s1`–`s11` and `s13` one at a time in this form, not as one loop.
- **Long scripts:** `s14b`, `s8`, `s13` and the builds can take several minutes. Start them with
  `run_in_background: true` and wait with `job_output` (`wait: true`, `timeout_ms: 600000`)
  until the job settles. If a foreground call times out, it becomes a job: wait for it the same
  way. Never start the same script twice.
- **D95 inside DSH.** The base prompt's detached launch command and wait command don't survive here:
  the sandbox kills a `setsid` job when its call ends (DSH1). DSH2's forms replace them, with the
  same output files `$B/logs/<name>.out`. D95's rules stand as written. A script is started once per
  pass. A job that hasn't settled is still running: wait for it with `job_output`, never start it
  again. A script cut off before boot 1 voids its pass, which restarts at X0.

**DSH3 — the boots (amends D57).** You never start `lane_run.sh` and never boot anything. The
owner runs each boot in a normal terminal. You hand it over, watch for the end, then continue.

1. **When S14, S14b and S16 are all PASS in the current pass** (D73's condition), stop and tell the
   owner, in a code block, to run exactly:

   ```bash
   cd ~/Work/Kitty/repo && mkdir -p build/m0b23/logs && setsid nohup bash -c 'bash tests/aros/lane_run.sh l0025 build/m0b23/guest-boot1; echo "RUNNER_EXIT=$?"' > build/m0b23/logs/runner-boot1.log 2>&1 < /dev/null &
   ```

   The runner writes the `boots.txt` line itself, before it launches. A boot takes about an hour.
2. **In the same turn, start one watcher job** with `run_in_background: true`:

   ```bash
   L=/home/miller/Work/Kitty/repo/build/m0b23/logs/runner-boot1.log; until grep -q '^RUNNER_EXIT=' "$L" 2>/dev/null; do sleep 60; done; tail -n 25 "$L"
   ```

   Then end your turn. The job's completion notice wakes you. If the owner says the boot is done
   first, read the log yourself.
3. **When boot 1's log has `RUNNER_EXIT=<n>`:**
   - If `n` is 3 or 9, stop and write the report (D73).
   - Otherwise, with nothing edited in between, give the owner the boot-2 command: the same line
     with `guest-boot1` and `runner-boot1` changed to `guest-boot2` and `runner-boot2`. Start the
     matching watcher, and end your turn.
4. **When boot 2's log has `RUNNER_EXIT=<n>`:** grade per D76 as amended by D92 (the base prompt's `c()` and `f()` block, run
   in the repo). Then run `s15`, `s15b`, `s12`, `s12b`, `s12c` in DSH2's form, and write the report.
5. **The runner's exit code** is the `RUNNER_EXIT=` value. Quote it in report item 5.
6. **If the owner reports** that a boot didn't start, or that the runner is still alive after 2 hours,
   don't retry it: grade and report what exists.
7. **While a boot runs, do nothing else.** If the log is empty or hasn't changed, that is normal:
   the runner writes little until the guest finishes. Don't run any script and don't touch
   `boots.txt`, `census.txt` or `guest-boot*`. Don't start the boot yourself. Wait for the watcher,
   or for the owner.
8. **If `RUNNER_EXIT` is 3 or 9, or the log has no `RUNNER_EXIT=` but the owner says the runner
   ended:** stop and tell the owner. Quote the log. Run no further script until the owner answers;
   the owner decides between a report and a retry.

**DSH4 — checks that are blind inside the sandbox.**
- **Some gates rely on `pgrep`.** These are S16's "lane l0025 is running" guard, the `qemu_start`
  and "stray process" lines of script 0 and script 12, and D57's "no other `lane_run.sh`" rule.
  Inside DSH they see only your own processes, so they pass whether or not a lane runs on the host.
- **Before script 16, and before script 15b,** run
  `ss -ltnH | grep -E ':(4479|9099|9100)[[:space:]]'`. The network isn't namespaced, so this does
  see the host. If it prints anything, a lane or spike server is up on the host: stop and tell the
  owner. Don't run S16 or S15b until it prints nothing.
- **In report item 8,** list one line: "process checks (pgrep) ran inside DSH's PID namespace and
  saw only DSH's processes; the lane ports were checked with ss before S16 and S15b". This is a
  known limit, not a gate result. Change no gate line because of it.

**DSH5 — skills (report item 1).** DSH doesn't load the base prompt's two skills itself. Before
script 0, read `/home/miller/.claude/skills/aros-development/SKILL.md` and
`/home/miller/.claude/skills/karpathy-guidelines/SKILL.md` in full. Report item 1 says "read
aros-development and karpathy-guidelines SKILL.md (DSH5)".

**DSH6 — long outputs and the report.** If a tool result is cut short (`[output truncated; full
output: …]`), read the full file it names, or the `$B/logs/*.out` file. Never paste a truncated
output as if it were whole.
- **Write the report in pieces.** M0b.20's DSH run ended with `max-tokens` while writing the
  whole report in one call, and no report existed. Write `build/m0b23/report.md` one report item
  per call: create it with item 1, then append items 2–9 one call each.
- **Item 2 is not pasted into the report.** For each script, the report gives its exit code and its
  output file `build/m0b23/logs/<name>.out` (X0's: `logs/x0.out`, see DSH2), and then the census
  verbatim, read from `census.txt`. The `.out` files are the verbatim outputs. Item 5's long
  files likewise: name the file and quote only the lines item 5 asks for.
- **Collect boot details one boot at a time,** and write each boot's item 5 section before
  collecting the next.
- **"Verbatim" means copied bytes.** M0b.21's report labelled one-line summaries "verbatim" and
  quoted a `loader_id.txt` value (`open=1`) that no file holds. Copy a quoted line from the file
  with the file tool or `sed -n`, never from memory. A shortened line is marked "summary".
- **Your final reply** is short: the census line counts copied from `census.txt`, the FAIL and
  BLOCKED gate lines verbatim, and the path `build/m0b23/report.md`. Never repeat the report's text.

**DSH7 — what doesn't change.**
- The authorization block, the boot grant (2), the ledger rules, the IMAGES rule, D73 (no edits,
  before, between or after the boots), the report format and its rules. The rules are: gate lines
  verbatim, nothing appended to a gate line, counts copied from `census.txt`, no "key result"
  paragraph, no explanations of FAILs.
- **D94 and D96 bind you exactly as written.**
  - Never write `census.txt` or `census.chain` except by running a script.
  - If S12c FAILs, its line stands in the report.
  - If script 0 fails, stop and report.
  - Git stays read-only: never reset, checkout, restore, stash or commit.
- Applying DSH1–DSH7 is not a departure. Report item 8 lists real departures only, plus DSH4's
  one line.
- **Before the final report,** confirm with `job_list` that none of your jobs is still running. The
  watchers end by themselves when `RUNNER_EXIT=` appears.

## Order of work (summary; the base prompt has the detail)

1. Read the base prompt in full, then the two SKILL.md files (DSH5).
2. X0 (DSH2's command), then `s0` once.
3. `x1`, `x2`, `x3`, `x4`, `sX`, `s1`–`s11`, `s13`, `s14`, `s14b`. A FAIL stops the round
   (base prompt's loop rule).
4. The `ss` port check (DSH4), then `s16`.
5. All of S14, S14b and S16 PASS: hand boot 1 to the owner, and start the watcher (DSH3).
6. Boot 1 has ended: hand boot 2 over, unless its exit code was 3 or 9.
7. Boot 2 has ended: grade (D76), the `ss` check, then `s15`, `s15b`, `s12`, `s12b`, `s12c`.
8. Write `build/m0b23/report.md` item by item (DSH6; base prompt's final report, with DSH5 and
   DSH4's notes). Reply with DSH6's short reply.
