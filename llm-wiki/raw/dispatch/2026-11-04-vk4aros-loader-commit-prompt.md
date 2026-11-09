# Vulkan4Aros loader commit dispatch prompt

> Source: Docs/Development/dispatch/vk4aros_loader_commit_opencode_prompt.md
> Collected: 2026-11-04
> Published: 2026-11-04

# opencode prompt — Vulkan4Aros: commit the loader fix fixv3 (2026-11-04)

## Owner authorization (read first — opencode never edits this block)

    BOOTS GRANTED: 0
    LANE: none. Never start, boot, or connect to any AROS guest.
    COMMIT: exactly one local commit on master in ~/Work/projects/Vulkan4Aros, of exactly these files,
      by the script below: scripts/aros_audit.sh, src/vulkan/loader/loader_libdefs.h,
      src/vulkan/loader/vk_aros_loader.c. The owner asked for this commit on 2026-11-04.
    NO PUSH. No other git write: no add, stash, reset, checkout, restore, clean, rebase, merge, amend.
    WRITE: ~/Work/lanes/l0025/loader-fixv3/evidence/commit/ (the script creates it) and
      /tmp/opencode/l0025-loader-commit-input/. Nothing else, apart from what the audit writes by itself.
    NEVER RUN: scripts/apply_proof_serie.sh. It adds a git worktree inside src/abi/v1/AROS*, which the
      owner keeps out of this round.
    NEVER TOUCH: src/abi/, any disk image, ~/Work/lanes/l0025/next-boot/, ~/Work/lanes/l0025/loader-fixv3/
      outside evidence/commit/, ~/Work/Kitty/.

## What this task is

The loader fix is applied but uncommitted in a repository where another session is committing. This round commits it. The review is `~/Work/Kitty/Docs/Development/vk4aros_loader_fixv3_critique.md`.

Two traps the script handles:

- **The index isn't empty.** `docs/evidence/2026-11-01-aros-crt-picks/b3-tables.txt` is staged and isn't part of this. The script uses `git commit --only`, so it stays staged and uncommitted. Don't unstage it.
- **`AROS_ABI` must be unset for the audit.** The last round's script exported it, and that caused a false error. An audit run on the read-only tree without it gave 0 errors, 0 warnings and the ATCPGUI skip `AGENTS.md` expects.

`AGENTS.md` also lists `apply_proof_serie.sh` and the guard digest as gates before a record commit. They aren't run here: this commit touches none of the AROS tree. Say so in the report.

## What to do

1. Extract the script and check it:

```bash
PR=~/Work/Kitty/Docs/Development/dispatch/vk4aros_loader_commit_opencode_prompt.md
X=/tmp/opencode/l0025-loader-commit-input
mkdir -p "$X"
sed -n '/^<<<LOADER_COMMIT.SH$/,/^>>>LOADER_COMMIT.SH$/p' "$PR" | sed '1d;$d' > "$X/loader_commit.sh"
sha256sum "$X/loader_commit.sh"
```

The hash must be `190673913fb53282df4512202a07813036667e3d809f5c703cdcbfdac2c2d6a2`. If not, stop and report.

2. Run it once, unchanged:

```bash
bash /tmp/opencode/l0025-loader-commit-input/loader_commit.sh
```

3. Report, then stop. On any STOP or failed gate, don't retry, repair, amend or revert anything. A STOP before the commit leaves the tree as it was.

If another session commits at the same moment, `C.parent` can FAIL with the commit in place: report it and leave it.

## Tested

On a copy with an unrelated staged file and a recorded clean audit log: the commit took exactly the three files and the staged file stayed staged, with all gates passing. A rerun, a changed tree, a staged target file and an audit with errors each stopped with no commit. The audit step itself wasn't run in that test.

## Final report (reply with this only)

1. `DONE`, or the line the script stopped at, and the exit code.
2. `evidence/commit/result.txt`, whole.
3. `git log -1 --stat`, and `git status --short -- src/vulkan/loader scripts/aros_audit.sh`.
4. `git diff --cached --name-only`, and whether anything was pushed.
5. Every departure from this prompt, or "none".

Say plainly: the fix hasn't run on a guest.

## File

```
<<<LOADER_COMMIT.SH
#!/bin/bash
# Commits the three fixv3 files, and only those. No push. Takes no arguments.
set -u
R=$HOME/Work/projects/Vulkan4Aros
E=$HOME/Work/lanes/l0025/loader-fixv3/evidence
L=$E/commit
FILES="scripts/aros_audit.sh src/vulkan/loader/loader_libdefs.h src/vulkan/loader/vk_aros_loader.c"
[ ! -e "$L" ] || { echo "STOP: $L exists"; exit 2; }
mkdir -p "$L" || exit 2
stop() { echo "STOP: $1" | tee -a "$L/result.txt"; exit 3; }
cd "$R" || exit 2

[ "$(git branch --show-current)" = master ] || stop "not on master"
git diff --cached --quiet -- $FILES || stop "one of the three files is staged"
[ "$(git status --short -- $FILES | wc -l)" = 3 ] || stop "the three files are not all modified"
git diff -- $FILES > "$L/diff_before.txt"
cmp -s "$L/diff_before.txt" "$E/landed.diff" || stop "the working tree differs from landed.diff"
git diff --cached --name-only > "$L/index_before.txt"

# AROS_ABI stays unset: the audit picks its own toolchains, and v1 here gives a false error
env -u AROS_ABI bash scripts/aros_audit.sh > "$L/audit.log" 2>&1
sed 's/\x1b\[[0-9;]*m//g' "$L/audit.log" > "$L/audit.txt"
grep 'Audit complete' "$L/audit.txt" | tee -a "$L/result.txt"
grep -q 'Audit complete: 0 error(s), 0 warning(s)' "$L/audit.txt" || stop "audit is not 0 errors, 0 warnings"
grep -q 'OK: boundary does NOT look up vkEnumerateDeviceExtensionProperties by name' "$L/audit.txt" || stop "the new audit gate did not pass"

P=$(git rev-parse HEAD)
cat > "$L/message.txt" <<'MSG'
loader: stop stripping VK_KHR_swapchain from every device (fixv3)

boundary_r7_create_device re-adds VK_KHR_swapchain only if the device
advertises it, and asked through vkGetInstanceProcAddrStock(VK_NULL_HANDLE,
"vkEnumerateDeviceExtensionProperties"). That command is not global, so the
lookup returned NULL and every device lost the extension. Added as an NVK fix
in c0b005ce. Call the stock trampoline symbol directly, as vkCreateDevice does
below it. A device that does not advertise the extension still does not get it.

Stamp fixv3. New aros_audit.sh gate forbids the name lookup.

Control rebuild byte-exact to the deployed 19a3143e; fixed build 90237ed2,
staged in lanes/l0025/loader-fixv3 and not deployed by this commit. Not run on
a guest: the swapchain diagnosis is from the source and the binary.

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
MSG
git commit --only -F "$L/message.txt" -- $FILES > "$L/commit.txt" 2>&1 || { cat "$L/commit.txt"; stop "git commit failed; not retried"; }

FAILED=0
chk() { if [ "$2" = "$3" ]; then echo "GATE $1 PASS" | tee -a "$L/result.txt"; else echo "GATE $1 FAIL want=$3 got=$2" | tee -a "$L/result.txt"; FAILED=1; fi; }
chk C.parent "$(git rev-parse HEAD^)" "$P"
chk C.files "$(git show --name-only --format= HEAD | sort | xargs)" "$FILES"
# content only: the header lines depend on git config (diff.mnemonicPrefix) and hash length
body() { grep -Ev '^(index |diff --git |--- |\+\+\+ )'; }
chk C.diff "$(git diff HEAD^ HEAD -- $FILES | body | sha256sum)" "$(body < "$L/diff_before.txt" | sha256sum)"
chk C.clean "$(git status --short -- $FILES | wc -l)" 0
git diff --cached --name-only > "$L/index_after.txt"
chk C.index-unchanged "$(sha256sum < "$L/index_after.txt")" "$(sha256sum < "$L/index_before.txt")"
git rev-parse HEAD | tee -a "$L/result.txt"
[ "$FAILED" = 0 ] || exit 4
echo DONE | tee -a "$L/result.txt"
>>>LOADER_COMMIT.SH
```
