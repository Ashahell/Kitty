#!/bin/bash
# tests/aros/lane_run.sh <lane> <outdir>
# tests/aros/lane_run.sh --check-transport <outdir>
# tests/aros/lane_run.sh --dry-boot <outdir>   (LR_STUBS=<dir under build/m0b23>)
#
# The only host-side door to a guest boot in this round (W1). Steps, in order:
#
#   1. D35: resolve <outdir> with `realpath -m` and refuse (exit 3,
#      "outdir outside build/m0b23") unless it lies under $KR/build/m0b23/.
#      Nothing is created before this check passes.
#   2. Read BOOTS GRANTED / LANE from
#      Docs/Development/dispatch/m0b23_aros_guest_opencode_prompt.md and count
#      the lines already in build/m0b23/boots.txt; refuse (exit 3, "no boots
#      granted") on a wrong lane, no grant, or an exhausted grant. This
#      refusal comes before the boots.txt write, so a refused call never
#      consumes a boot.
#   3. D41: before the ledger line, count the lane's boot records newer than
#      build/m0b23/baseline.done; if count != boots.txt lines, refuse (exit 3,
#      "ledger mismatch").
#   4. preflight: D40 (b) port check, (d) lane running check, then start the
#      server, then (c) liveness check. One function, used by both runner modes.
#      Write one boots.txt line, then boot through the lane's own launcher.
#   5. Make every directory of build/aros-pkg/ under RAM:kitty, then deploy
#      it file by file with one --put each (absolute paths), verify with
#      `List RAM:kitty`, make the out/ tree, then send each command line of
#      prelude.aros as its own exec (D58); a line that does not answer rc=0
#      prints "prelude failed at line <n>", fetches what exists, stops the
#      lane and exits 10. Then the D33 probes p -> pt -> ptr, each as
#      `Execute RAM:kitty/run_<name>.aros >RAM:kitty/out/sh_<name>.log`.
#      An exec that gets no answer at all (the client's TIMEOUT) means the
#      agent is wedged: print "guest agent wedged at <what>", send nothing
#      more, stop the lane and exit 6.
#   6. Only when the probe passed (rc_p.txt is rc=0 and run_p.log has a
#      vk_loader= line) run the eleven M0b runs; otherwise print
#      "probe failed; ladder not run", fetch what exists, stop the lane and
#      exit 8.
#   7. Fetch RAM:kitty/out into <outdir> and write <outdir>/host_wall.txt as
#      "<name> start=<epoch s> end=<epoch s>" per run. On RUNNING (the guest
#      agent answers at its 900 s exec budget) poll `Type` on the run's own
#      rc_<name>.txt (the image has no Exists command); that file is the
#      authority and the host wall ends when it appears (D36). A run with no
#      rc goes in dead.txt, sooner if it faulted on serial; three dead start
#      no more runs (D65).
#   8. D75: the prelude's `Assign >…` listing is held back from step 5 and
#      sent last, after the fetch and two reads (`Type ENV:Sys/ahi.prefs
#      HEX`, `List DEVS:AudioModes`), each one exec. A TIMEOUT on any of the
#      three prints "guest agent wedged at <what>; the run evidence is
#      already home", sends nothing more and exits 11. Exits 10 and 8 make
#      the same reads, then exit with their own code.
#   9. D78: after the AHI set, `MakeDir RAM:kitty/Devs/AudioModes` and
#      `Copy AROS:Devs/AudioModes/HDAUDIO` into it, one exec each, so AHI's
#      `DEVS:AudioModes` scan finds a mode in the first DEVS: directory. A
#      copy that does not answer rc=0 exits 7, before the prelude.
#  10. D81: then `Copy AROS:Prefs/Env-Archive/hdaudio.config` to
#      DH1:W/hdaudio.config, one exec, because hdaudio.audio reads its
#      overrides from ENVARC: and the prelude points ENVARC: at DH1:W.
#      A copy that does not answer rc=0 exits 7, before the prelude.
#
# The per-run guest work lives in run_<name>.aros, the same fragments
# m0b-run.aros executes, so the two paths cannot drift apart.
set -u

KR="$(cd "$(dirname "$0")/../.." && pwd)"
VK="$HOME/Work/projects/Vulkan4Aros"
SPIKE="$VK/scripts/spike_server.py"
B="$KR/build/m0b23"
PKG="$KR/build/aros-pkg"
AHIDIR="$KR/build/m0b23/ahi"
PROBES="p pt ptr"
RUNS="h b1 od2 od4 lp0 lp1 w fifo lp4 a soak"
SERVER_PID=""
LAUNCHED=0
MODE=boot

held() { ss -ltnH "( sport = :$1 )" 2>/dev/null | grep -q .; }

stop_lane() {
    if [ "$MODE" = dry ]; then
        "$LR_STUBS/stop.sh"
    else
        systemctl --user stop l0025-qemu >/dev/null 2>&1 || true
    fi
}

submit() {
    if [ "$MODE" = dry ]; then
        "$LR_STUBS/submit.sh" "$@"
    else
        python3 -B "$SPIKE" submit --spool "$SPOOL" "$@"
    fi
}

on_exit() {
    if [ -n "$SERVER_PID" ]; then
        kill "$SERVER_PID" 2>/dev/null || true
        wait "$SERVER_PID" 2>/dev/null || true
        echo "lane_run: stopped the spike server it started (pid $SERVER_PID)"
    fi
    if [ "$LAUNCHED" = 1 ] && [ "${LANE_KEEP:-0}" != 1 ]; then
        stop_lane
        echo "lane_run: lane stopped"
    fi
}
trap on_exit EXIT

preflight() {
    if held 9099 || held 9100; then
        echo "port 9099 held by $(ss -ltnpH '( sport = :9099 or sport = :9100 )' | head -2 | tr '\n' ' ')" >&2
        exit 9
    fi
    if pgrep -f '^[^ ]*qemu-system.*aros_l0025' >/dev/null; then
        echo "lane l0025 already running" >&2
        exit 9
    fi
    mkdir -p "$1" "$2"
    ( cd "$1" && exec python3 -B -u "$SPIKE" serve --port 9099 --bulk-port 9100 --spool "$2" ) >"$1/spike.log" 2>&1 &
    SERVER_PID=$!
    echo "$SERVER_PID" > "$1/spike.pid"
    for i in 1 2 3 4 5 6 7 8 9 10; do
        if kill -0 "$SERVER_PID" 2>/dev/null && ss -ltnpH '( sport = :9099 )' | grep -q "pid=$SERVER_PID,"; then
            echo "transport ok pid=$SERVER_PID"
            return 0
        fi
        sleep 1
    done
    echo "spike server did not start: $(tail -5 "$1/spike.log")" >&2
    exit 9
}

under_b() {
    case "$(realpath -m "$1")" in "$B"/*) return 0 ;; *) return 1 ;; esac
}

case "${1:-}" in
    --check-transport) MODE=transport; OUT_ARG="${2:-}" ;;
    --dry-boot) MODE=dry; OUT_ARG="${2:-}" ;;
    *) LANE_ARG="${1:-none}"; OUT_ARG="${2:-}" ;;
esac

# ---- 1. D35: the output directory must live under build/m0b23 ----
if [ -z "$OUT_ARG" ] || ! under_b "$OUT_ARG"; then
    echo "outdir outside build/m0b23: ${OUT_ARG:-none}" >&2
    exit 3
fi
OUT="$(realpath -m "$OUT_ARG")"

if [ "$MODE" = transport ]; then
    preflight "$OUT" "$OUT/tspool"
    exit 0
fi

# ---- 2. the mode's values; nothing below tests the mode ----
PROBE="DH1:C/liblist_probe"
if [ "$MODE" = dry ]; then
    if [ -z "${LR_STUBS:-}" ] || ! under_b "$LR_STUBS"; then
        echo "LR_STUBS outside build/m0b23" >&2
        exit 3
    fi
    GRANT=1
    AUTH=l0025
    LANE_ARG=l0025
    GRACE=1
    MON=1
    BOOTS="$OUT/boots.txt"
    LANES="$OUT/lanes"
    LAUNCH="$LR_STUBS/launch.sh"
    SPOOL="$OUT/spool"
else
    PR="$HOME/Work/Kitty/Docs/Development/dispatch/m0b23_aros_guest_opencode_prompt.md"
    GRACE=90
    MON=4479
    GRANT=$(sed -n 's/^ *BOOTS GRANTED: *\([0-9][0-9]*\) *$/\1/p' "$PR" 2>/dev/null | head -1)
    AUTH=$(sed -n 's/^ *LANE: *\([a-z0-9]*\) *$/\1/p' "$PR" 2>/dev/null | head -1)
    BOOTS="$KR/build/m0b23/boots.txt"
    LANES="$HOME/Work/lanes"
    LAUNCH="$VK/scripts/lane_l0025_boot.sh"
    SPOOL="$HOME/Work/lanes/l0025/spool"
fi
used=0
[ -f "$BOOTS" ] && used=$(grep -c . "$BOOTS")

refuse() {
    echo "no boots granted: lane=${LANE_ARG} authorized=${AUTH:-none} grant=${GRANT:-0} used=$used" >&2
    exit 3
}

[ "$LANE_ARG" = "${AUTH:-none}" ] || refuse
case "${GRANT:-x}" in ''|*[!0-9]*) refuse ;; esac
[ "$GRANT" -gt 0 ] || refuse
[ "$used" -lt "$GRANT" ] || refuse
[ "$LANE_ARG" = l0025 ] || refuse
[ -d "$PKG" ] || { echo "no package at $PKG (run make aros-pkg)" >&2; exit 4; }
[ -x "$LAUNCH" ] || { echo "no launcher: $LAUNCH" >&2; exit 4; }

# ---- 3. D41: ledger check ----
lane_boots=$(find "$LANES/$LANE_ARG" -maxdepth 1 -name 'boot-*.txt' -newer "$B/baseline.done" 2>/dev/null | wc -l)
if [ "$lane_boots" -ne "$used" ]; then
    echo "ledger mismatch: $lane_boots boot records, $used ledger lines" >&2
    exit 3
fi

mkdir -p "$OUT" "$OUT/w" "$OUT/h"
: > "$OUT/host_wall.txt"

# ---- 4. preflight: start server, then ledger line, then boot ----
preflight "$OUT" "$SPOOL"

echo "$(date -Is) $LANE_ARG M0b.23 guest: deploy, AHI set, prelude, p/pt/ptr, ladder" >> "$BOOTS"
echo "lane_run: boots.txt now has $(grep -c . "$BOOTS") of $GRANT granted lines"
LAUNCHED=1
LANE_AUDIO="$OUT/lane_audio.wav" bash "$LAUNCH" "M0b.23 guest ladder (build/m0b23)" || { echo "launcher failed" >&2; exit 5; }
DEAD=0
: > "$OUT/dead.txt"
SERIAL=$(sed -n 's/^serial=//p' "$(ls -t "$LANES/$LANE_ARG"/boot-*.txt | head -1)" | head -1)
echo "lane_run: serial log ${SERIAL:-unknown}"

# D69: the guest's screen when a run is dead; the crash requester names the function
shot() {
    ( { printf 'screendump %s\n' "$OUT/dead_$1.ppm"; sleep 2; } > "/dev/tcp/127.0.0.1/$MON" ) 2>/dev/null || \
        echo "lane_run: no screendump for $1"
}

serial_lines() { wc -l < "$SERIAL" 2>/dev/null || echo 0; }
faulted_since() { tail -n +"$(( $1 + 1 ))" "$SERIAL" 2>/dev/null | grep -qE 'core_IRQHandle\((13|14)\)'; }


# ---- wait for the guest agent (ping is agent-side liveness) ----
t0=$(date +%s)
until submit --ping --wait 10 >/dev/null 2>&1; do
    now=$(date +%s)
    if [ $((now - t0)) -gt 600 ]; then echo "guest agent did not answer in 600 s" >&2; exit 6; fi
    sleep 5
done
echo "lane_run: guest agent answered after $(( $(date +%s) - t0 )) s"

# ---- 5b. deploy, one --put per file, absolute on both sides ----
submit --exec "MakeDir RAM:kitty ALL" --wait 120 >/dev/null 2>&1
while IFS= read -r d; do
    submit --exec "MakeDir RAM:kitty/${d#./} ALL" --wait 120 >/dev/null 2>&1
done < <(cd "$PKG" && find . -mindepth 1 -type d | sort)
nput=0
while IFS= read -r f; do
    rel=${f#./}
    if ! submit --put "$PKG/$rel:RAM:kitty/$rel" --wait 300 >/dev/null 2>&1; then
        echo "lane_run: put failed for $rel" >&2
        exit 7
    fi
    nput=$((nput + 1))
done < <(cd "$PKG" && find . -type f | sort)
echo "lane_run: put $nput files to RAM:kitty"

# D70: the rebuilt AHI set, for this boot only; the prelude puts it in front of DEVS:
submit --exec "MakeDir RAM:kitty/Devs ALL" --wait 120 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/Devs/AHI ALL" --wait 120 >/dev/null 2>&1
for f in "$AHIDIR"/*; do
    case "${f##*/}" in *.device) to="Devs/${f##*/}" ;; *) to="Devs/AHI/${f##*/}" ;; esac
    if ! submit --put "$f:RAM:kitty/$to" --wait 300 >/dev/null 2>&1; then
        echo "lane_run: put failed for $to" >&2
        exit 7
    fi
done
echo "lane_run: put the AHI set to RAM:kitty/Devs"

# D78: AHI scans DEVS:AudioModes in the first DEVS: directory only, so one mode file goes there
submit --exec "MakeDir RAM:kitty/Devs/AudioModes" --wait 120 >/dev/null 2>&1
mc=$(submit --exec "Copy AROS:Devs/AudioModes/HDAUDIO RAM:kitty/Devs/AudioModes/HDAUDIO" --wait 120 2>&1)
if ! echo "$mc" | grep -q -- '-> rc=0 ('; then
    echo "lane_run: AudioModes copy failed: $(echo "$mc" | grep -m1 'rc=\|TIMEOUT')" >&2
    exit 7
fi
echo "lane_run: copied AROS:Devs/AudioModes/HDAUDIO to RAM:kitty/Devs/AudioModes"

# D81: hdaudio.audio reads its overrides from ENVARC:hdaudio.config; the prelude points ENVARC: at DH1:W
ec=$(submit --exec "Copy AROS:Prefs/Env-Archive/hdaudio.config DH1:W/hdaudio.config" --wait 120 2>&1)
if ! echo "$ec" | grep -q -- '-> rc=0 ('; then
    echo "lane_run: hdaudio.config copy failed: $(echo "$ec" | grep -m1 'rc=\|TIMEOUT')" >&2
    exit 7
fi
echo "lane_run: copied AROS:Prefs/Env-Archive/hdaudio.config to DH1:W"

lst=$(submit --exec "List RAM:kitty" --wait 120 2>&1)
echo "$lst" | sed -n '1,14p'
for need in kitty kitty-debug prelude.aros run_p.aros run_pt.aros run_ptr.aros run_w.aros run_soak.aros test.mod spv; do
    echo "$lst" | grep -q "$need" || { echo "lane_run: deploy check failed: $need missing" >&2; exit 7; }
done

getf() {
    submit --get "RAM:kitty/out/$1:$OUT/$2" --wait 300 >/dev/null 2>&1 || \
        echo "lane_run: get failed for $1" >&2
}

wedged() {
    if echo "$2" | grep -q 'TIMEOUT: no result'; then
        echo "guest agent wedged at $1; nothing more is sent"
        exit 6
    fi
}

# D75: one late exec; a wedge here costs no run, its evidence is already home
late() {
    q=$(submit --exec "$2" --wait 120 2>&1 < /dev/null)
    echo "$q" > "$OUT/exec_$1.log"
    if echo "$q" | grep -q 'TIMEOUT: no result'; then
        echo "guest agent wedged at $3; the run evidence is already home"
        return 1
    fi
}

endreads() {
    late ahi_prefs "Type ENV:Sys/ahi.prefs HEX >RAM:kitty/out/ahi_prefs.txt" "the AHI prefs read" || return 1
    getf "ahi_prefs.txt" "ahi_prefs.txt"
    late audiomodes "List DEVS:AudioModes >RAM:kitty/out/audiomodes.txt" "the AudioModes list" || return 1
    getf "audiomodes.txt" "audiomodes.txt"
    [ -n "$LISTING" ] || return 0
    late listing "$LISTING" "the Assign listing" || return 1
    getf "assigns.txt" "assigns.txt"
}

# ---- 5c. the out/ tree, then the D32 prelude, one exec per line (D58) ----
submit --exec "MakeDir RAM:kitty/out" --wait 60 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/out/p" --wait 60 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/out/w" --wait 60 >/dev/null 2>&1
submit --exec "MakeDir RAM:kitty/out/h" --wait 60 >/dev/null 2>&1
submit --exec "Avail >RAM:kitty/out/avail_before.txt" --wait 120 >/dev/null 2>&1

LISTING=""
: > "$OUT/exec_prelude.log"
pn=0
while IFS= read -r pl || [ -n "$pl" ]; do
    case "$pl" in ''|\;*) continue ;; esac
    case "$pl" in 'Assign >'*) LISTING=$pl; continue ;; esac
    pn=$((pn + 1))
    pout=$(submit --exec "$pl" --wait 120 2>&1 < /dev/null)
    echo "$pout" >> "$OUT/exec_prelude.log"
    echo "$pout" | sed -n '1,4p'
    wedged "prelude line $pn ($pl)" "$pout"
    if ! echo "$pout" | grep -q -- '-> rc=0 ('; then
        echo "prelude failed at line $pn: $pl"
        getf "avail_before.txt" "avail_before.txt"
        endreads
        exit 10
    fi
done < "$PKG/prelude.aros"
echo "lane_run: prelude sent as $pn execs"
# D71: what a bare device name would be looked up against
submit --exec "CD" --wait 60 > "$OUT/exec_cd.log" 2>&1

# ---- 5d. one --exec per run fragment; wall time is host-measured ----
run_one() {
    r="$1"; budget="$2"; limit="$3"
    s=$(date +%s)
    n0=$(serial_lines)
    out=$(submit --exec "Execute RAM:kitty/run_$r.aros >RAM:kitty/out/sh_$r.log" --exec-timeout "$budget" --wait "$budget")
    rc=$?
    echo "$out" > "$OUT/exec_$r.log"
    echo "$out" | sed -n '1,6p'
    if echo "$out" | grep -q 'TIMEOUT: no result'; then
        echo "$r start=$s end=$(date +%s)" >> "$OUT/host_wall.txt"
    fi
    wedged "run $r" "$out"
    if echo "$out" | grep -q -- '-> RUNNING'; then
        echo "lane_run: run $r answered RUNNING (guest exec budget); waiting for rc_$r.txt"
        tmo=$(( s + limit ))
        got=0
        fault_at=0
        while [ "$(date +%s)" -lt "$tmo" ]; do
            q=$(submit --exec "Type RAM:kitty/out/rc_$r.txt" --wait 120 2>&1)
            if echo "$q" | grep -qE '^[[:space:]]+rc=[0-9]+[[:space:]]*$'; then
                got=1
                break
            fi
            if faulted_since "$n0"; then
                [ "$fault_at" != 0 ] || fault_at=$(date +%s)
                if [ $(( $(date +%s) - fault_at )) -ge "$GRACE" ]; then
                    break
                fi
            fi
            sleep 10
        done
        echo "$q" | sed -n '1,3p'
        if [ "$got" = 0 ]; then
            DEAD=$((DEAD + 1))
            echo "$r" >> "$OUT/dead.txt"
            echo "lane_run: run $r has no rc; dead ($DEAD)"
            shot "$r"
        fi
    fi
    tail -n +"$(( n0 + 1 ))" "$SERIAL" > "$OUT/serial_$r.txt" 2>/dev/null
    e=$(date +%s)
    echo "$r start=$s end=$e" >> "$OUT/host_wall.txt"
    echo "lane_run: run $r exec rc=$rc wall=$((e - s)) s"
}

run_one p 600 1100
run_one pt 600 1100
run_one ptr 600 1100

# D33: p decides; pt and ptr are the trap/plant evidence and have already run.
getf "rc_p.txt" "rc_p.txt"
getf "run_p.log" "run_p.log"
if [ "$(cat "$OUT/rc_p.txt" 2>/dev/null)" != "rc=0" ] || \
   ! grep -q 'vk_loader=' "$OUT/run_p.log" 2>/dev/null; then
    echo "probe failed; ladder not run"
    for r in pt ptr; do
        getf "rc_$r.txt" "rc_$r.txt"
        getf "run_$r.log" "run_$r.log"
    done
    for r in $PROBES; do getf "sh_$r.log" "sh_$r.log"; done
    getf "avail_before.txt" "avail_before.txt"
    endreads
    exit 8
fi

# Each run's evidence comes home as it ends: a later wedge must not cost it.
for r in $RUNS; do
    if [ "$DEAD" -ge 3 ]; then
        echo "lane_run: $DEAD dead, $r not started"
        continue
    fi
    # D70: soak opens AHI too; after a dead `a` it would only wait on the dead task
    if [ "$r" = soak ] && grep -qx a "$OUT/dead.txt"; then
        echo "lane_run: a dead, soak not started"
        continue
    fi
    case "$r" in soak) lim=2600 ;; *) lim=1100 ;; esac
    run_one "$r" 2400 "$lim"
    getf "run_$r.log" "run_$r.log"
    getf "rc_$r.txt" "rc_$r.txt"
    case "$r" in
        w) getf "w/present_150.bgra" "w/present_150.bgra"
           getf "w/present_150.inner.bgra" "w/present_150.inner.bgra" ;;
        h) for n in 000 150 299; do getf "h/frame_$n.bgra" "h/frame_$n.bgra"; done ;;
    esac
done
submit --exec "Avail >RAM:kitty/out/avail_after.txt" --wait 120 >/dev/null 2>&1
# D69: which loader the guest really had open, last, so a fault here costs no run
submit --exec "$PROBE idstring vulkan_loader" --wait 120 > "$OUT/loader_id.txt" 2>&1 < /dev/null

# ---- 7. fetch RAM:kitty/out ----
for r in $PROBES $RUNS; do
    getf "run_$r.log" "run_$r.log"
    getf "rc_$r.txt" "rc_$r.txt"
    getf "sh_$r.log" "sh_$r.log"
done
getf "avail_before.txt" "avail_before.txt"
getf "avail_after.txt" "avail_after.txt"

echo "lane_run: wrote $OUT/host_wall.txt"
cat "$OUT/host_wall.txt"
endreads || exit 11
echo "lane_run: lane stops now (LANE_KEEP=1 keeps it up)"
exit 0
