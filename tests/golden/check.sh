#!/usr/bin/env bash
# Golden-image + pin-stamp harness (design §6.7, C18, D7).
#
#   tests/golden/check.sh ICD_JSON DUMP_DIR
#
# Builds the host-lavapipe pin stamp with the same commands as dispatch script 7,
# compares it with $GOLDEN_DIR/STAMP (GOLDEN_DIR defaults to this directory), then
# compares frame_000.bgra frame_150.bgra frame_299.bgra byte for byte.
#
# Prints exactly one line and exits:
#   0  GOLDEN PASS
#   1  GOLDEN FAIL <reason>
#   2  GOLDEN BLOCKED <reason>     (no STAMP or no golden frames in GOLDEN_DIR)
set -u

ICD=${1:-}
RUN=${2:-}
HERE=$(cd "$(dirname "$0")" 2>/dev/null && pwd)
GOLD=${GOLDEN_DIR:-$HERE}
VK=${VULKAN4AROS:-$HOME/Work/projects/Vulkan4Aros}
FRAMES="000 150 299"

if [ -z "$ICD" ] || [ -z "$RUN" ]; then
    echo "GOLDEN FAIL usage: check.sh ICD_JSON DUMP_DIR"
    exit 1
fi

have_store=1
[ -f "$GOLD/STAMP" ] || have_store=0
for n in $FRAMES; do
    [ -f "$GOLD/frame_$n.bgra" ] || have_store=0
done
if [ "$have_store" = 0 ]; then
    echo "GOLDEN BLOCKED no pin stamp or golden frames in $GOLD (pinned host lavapipe not built)"
    exit 2
fi

if [ ! -f "$ICD" ]; then
    echo "GOLDEN FAIL ICD json not found: $ICD"
    exit 1
fi

so=$(python3 -I -c 'import json,sys;print(json.load(open(sys.argv[1]))["ICD"]["library_path"])' "$ICD" 2>/dev/null)
case "$so" in
    /*) ;;
    *) for d in /usr/lib /usr/lib64 /usr/lib/x86_64-linux-gnu; do
           [ -f "$d/$so" ] && { so="$d/$so"; break; }
       done ;;
esac

tmp=$(mktemp "${TMPDIR:-/tmp}/kitty-stamp.XXXXXX") || { echo "GOLDEN FAIL cannot create temp file"; exit 1; }
{
    echo "mesa_driver_info=$(VK_DRIVER_FILES=$ICD vulkaninfo --summary 2>/dev/null | sed -n 's/^[[:space:]]*driverInfo *= *//p' | head -1)"
    echo "lvp_sha256=$(sha256sum "$so" 2>/dev/null | cut -d' ' -f1)"
    echo "mesa_series_sha256=$(cat "$VK"/src/abi-patches/v1/mesa-26.2.2/*.diff 2>/dev/null | sha256sum | cut -d' ' -f1)"
    echo "lp_env=LP_NUM_THREADS=0"
    echo "aros_pin_upstream_sha=$(awk '$1=="upstream_sha"{print $2}' "$VK/src/abi-patches/v1/AROS_PIN" 2>/dev/null)"
    echo "aros_pin_merged_by=$(awk '$1=="merged_by"{print $2}' "$VK/src/abi-patches/v1/AROS_PIN" 2>/dev/null)"
} > "$tmp"

if ! cmp -s "$tmp" "$GOLD/STAMP"; then
    echo "GOLDEN FAIL pin stamp differs from $GOLD/STAMP"
    rm -f "$tmp"
    exit 1
fi
rm -f "$tmp"

for n in $FRAMES; do
    if [ ! -f "$RUN/frame_$n.bgra" ]; then
        echo "GOLDEN FAIL missing $RUN/frame_$n.bgra"
        exit 1
    fi
    if [ "$(wc -c < "$RUN/frame_$n.bgra")" != 327680 ]; then
        echo "GOLDEN FAIL $RUN/frame_$n.bgra is not 327680 bytes"
        exit 1
    fi
    if ! cmp -s "$GOLD/frame_$n.bgra" "$RUN/frame_$n.bgra"; then
        echo "GOLDEN FAIL frame $n differs"
        exit 1
    fi
done

echo "GOLDEN PASS"
exit 0
