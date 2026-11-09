# Vulkan4Aros lane audio prompt

> Source: Docs/Development/dispatch/vk4aros_lane_audio_opencode_prompt.md
> Collected: 2026-11-04
> Published: 2026-11-04

# opencode prompt — Vulkan4Aros: optional sound device on the l0025 launcher (2026-11-04)

## Owner authorization (read first — opencode never edits this block)

    BOOTS GRANTED: 0
    LANE: none. Never start, boot, or connect to any AROS guest.
    COMMIT: exactly one local commit on master in ~/Work/projects/Vulkan4Aros, of exactly one file,
      scripts/lane_l0025_boot.sh, by the script below. The owner asked for this on 2026-11-04.
    NO PUSH. No other git write: no add, stash, reset, checkout, restore, clean, rebase, merge, amend.
      The one exception: if a check fails before the commit, the script reverses its own edit.
    WRITE: scripts/lane_l0025_boot.sh, by the script; ~/Work/lanes/l0025/lane-audio-evidence/ (the
      script creates it); /tmp/opencode/l0025-lane-audio-input/. The launcher's dry runs also leave
      empty files under ~/Work/lanes/l0025/logs/. Nothing else.
    NEVER TOUCH: src/abi/, any disk image, ~/Work/lanes/l0025/next-boot/, ~/Work/Kitty/.

## What this task is

Kitty's M0b.11 needs a sound device on lane l0025, and the launcher has no way to add one. This round adds an optional `LANE_AUDIO` setting. Unset, the QEMU command line stays byte-identical, and the script checks that with `LANE_DRYRUN`. Another session may be committing in this repository, and the index may hold staged files: the script uses `git commit --only`, so they stay staged.

You run one script. You write no code and start no guest.

## What to do

1. Extract the script and check it:

```bash
PR=~/Work/Kitty/Docs/Development/dispatch/vk4aros_lane_audio_opencode_prompt.md
X=/tmp/opencode/l0025-lane-audio-input
mkdir -p "$X"
sed -n '/^<<<LANE_AUDIO.SH$/,/^>>>LANE_AUDIO.SH$/p' "$PR" | sed '1d;$d' > "$X/lane_audio.sh"
sha256sum "$X/lane_audio.sh"
```

The hash must be `ca7bcbe1eda0e1874f6dd1ecb76f211a9cefa7d1c50eab4ebb328e76652db600`. If not, stop and report.

2. Run it once, unchanged:

```bash
bash /tmp/opencode/l0025-lane-audio-input/lane_audio.sh </dev/null
```

3. Report, then stop. On any STOP or failed gate, don't retry, repair, amend or revert anything.

If another session commits at the same moment, `C.parent` can FAIL with the commit in place: report it and leave it.

## Tested

On a throwaway repository holding a copy of the launcher and an unrelated staged file: the commit took only the launcher, the staged file stayed staged, all gates passed. A launcher that broke the dry run stopped the script with the file unchanged. The knob hasn't run on a guest.

## Final report (reply with this only)

1. `DONE`, or the line the script stopped at, and the exit code.
2. `lane-audio-evidence/result.txt`, whole, and `diff.txt`.
3. `git log -1 --stat`, and `git status --short -- scripts/lane_l0025_boot.sh`.
4. `git diff --cached --name-only`, and whether anything was pushed.
5. Every departure from this prompt, or "none".

## File

```
<<<LANE_AUDIO.SH
#!/bin/bash
# Adds the optional LANE_AUDIO knob to the l0025 launcher and commits it. No push. Takes no arguments.
set -u
R=$HOME/Work/projects/Vulkan4Aros
L=$HOME/Work/lanes/l0025/lane-audio-evidence
F=scripts/lane_l0025_boot.sh
[ ! -e "$L" ] || { echo "STOP: $L exists"; exit 2; }
mkdir -p "$L" || exit 2
stop() { echo "STOP: $1" | tee -a "$L/result.txt"; exit 3; }
cd "$R" || exit 2

[ "$(git branch --show-current)" = master ] || stop "not on master"
[ -z "$(git status --short -- "$F")" ] || stop "$F is not clean"
systemctl --user is-active --quiet l0025-qemu 2>/dev/null && stop "the lane is running"
git diff --cached --name-only > "$L/index_before.txt"
LANE_DRYRUN=1 bash "$F" lane-audio-before 2>&1 | grep '^cmdline=' | sed 's/serial-[0-9-]*\.log/serial.log/' > "$L/cmdline_before.txt"
[ -s "$L/cmdline_before.txt" ] || stop "the dry run printed no cmdline"

python3 -I - "$F" <<'PY' || stop "the launcher did not take the edit; it is unchanged"
import sys
p=sys.argv[1]; s=open(p).read()
a='''if [ -n "$GDBPORT" ]; then
    CMDLINE="$CMDLINE -gdb tcp'''
if s.count(a)!=1: sys.exit(1)
b='''# OPTIONAL virtual sound (added 2026-11-04 for Kitty M0b.11). Set LANE_AUDIO to a host
# WAV path to attach an HDA controller and codec; QEMU writes what the guest plays there.
# Unset LANE_AUDIO = the lane's boot identity is exactly as before.
AUDIOWAV="${LANE_AUDIO:-}"
if [ -n "$AUDIOWAV" ]; then
    CMDLINE="$CMDLINE -audiodev wav,id=snd,path=$AUDIOWAV -device intel-hda -device hda-duplex,audiodev=snd"
    echo "audio: intel-hda, writing $AUDIOWAV (LANE_AUDIO was set)"
fi

'''
open(p,'w').write(s.replace(a,b+a))
PY

FAILED=0
chk() { if [ "$2" = "$3" ]; then echo "GATE $1 PASS" | tee -a "$L/result.txt"; else echo "GATE $1 FAIL want=$3 got=$2" | tee -a "$L/result.txt"; FAILED=1; fi; }
back() { git diff -- "$F" > "$L/diff_failed.txt"; git apply -R "$L/diff_failed.txt"; stop "$1; the edit is reversed, nothing committed"; }
bash -n "$F" || back "bash -n failed"
LANE_DRYRUN=1 bash "$F" lane-audio-after 2>&1 | grep '^cmdline=' | sed 's/serial-[0-9-]*\.log/serial.log/' > "$L/cmdline_unset.txt"
LANE_DRYRUN=1 LANE_AUDIO=/tmp/x.wav bash "$F" lane-audio-after 2>&1 | grep '^cmdline=' > "$L/cmdline_set.txt"
cmp -s "$L/cmdline_before.txt" "$L/cmdline_unset.txt" || back "unset, the cmdline changed"
grep -q ' -audiodev wav,id=snd,path=/tmp/x.wav -device intel-hda -device hda-duplex,audiodev=snd' "$L/cmdline_set.txt" || back "set, the cmdline has no device"
git diff -- "$F" > "$L/diff.txt"
[ "$(grep -c '^-[^-]' "$L/diff.txt")" = 0 ] || back "the edit removed a line"

P=$(git rev-parse HEAD)
cat > "$L/message.txt" <<'MSG'
lane_l0025_boot: optional LANE_AUDIO knob for a virtual HDA sound device

Set LANE_AUDIO to a host WAV path to attach intel-hda and hda-duplex; QEMU
writes what the guest plays there. Unset, the command line is byte-identical,
checked with LANE_DRYRUN. For Kitty M0b.11. Not run on a guest by this commit.

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
MSG
git commit --only -F "$L/message.txt" -- "$F" > "$L/commit.txt" 2>&1 || { cat "$L/commit.txt"; stop "git commit failed; not retried; the edit is still in the tree"; }
chk C.parent "$(git rev-parse HEAD^)" "$P"
chk C.files "$(git show --name-only --format= HEAD | xargs)" "$F"
chk C.clean "$(git status --short -- "$F" | wc -l)" 0
git diff --cached --name-only > "$L/index_after.txt"
chk C.index-unchanged "$(sha256sum < "$L/index_after.txt")" "$(sha256sum < "$L/index_before.txt")"
git rev-parse HEAD | tee -a "$L/result.txt"
[ "$FAILED" = 0 ] || exit 4
echo DONE | tee -a "$L/result.txt"
>>>LANE_AUDIO.SH
```
