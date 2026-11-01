# Golden frames — M0a — BLOCKED

This directory is intentionally **BLOCKED**: it holds no `STAMP` and no `frame_*.bgra`
files, because the pinned host lavapipe of D7 could not be built on this machine.

The one authorised run of

    bash ~/Work/projects/Vulkan4Aros/scripts/build_host_lavapipe.sh ~/Work/host_lavapipe_kitty

failed while applying the pinned Mesa patch series (`0025-...nvk_image.c`, both hunks
rejected), so `~/Work/host_lavapipe_kitty` contains no `build/` and no `lvp_icd.json`.
Authorization forbids patching Mesa, the script, LLVM or the patch set, so the goldens
could not be rendered with the pinned driver. The host's system lavapipe
(`/usr/share/vulkan/icd.d/lvp_icd.x86_64.json`, Mesa 26.2.5) is *not* the pin and must
never be stored as a golden.

Script 7 therefore records `S7.golden BLOCKED` (BLOCKED ≠ PASS).

## What is here

`check.sh ICD_JSON DUMP_DIR` — the harness from design §6.7 / dispatch W14. It:

1. rebuilds the pin stamp with the same commands as dispatch script 7
   (`mesa_driver_info`, `lvp_sha256`, `mesa_series_sha256`, `lp_env`,
   `aros_pin_upstream_sha`, `aros_pin_merged_by`);
2. compares it byte for byte with `$GOLDEN_DIR/STAMP` (`GOLDEN_DIR` defaults to this
   directory);
3. compares `frame_000.bgra frame_150.bgra frame_299.bgra` byte for byte with the run
   directory.

Output is exactly one line:

| rc | line | meaning |
|----|------|---------|
| 0 | `GOLDEN PASS` | stamp and frames match |
| 1 | `GOLDEN FAIL <reason>` | wrong driver pin, missing/short/edited frame |
| 2 | `GOLDEN BLOCKED <reason>` | no `STAMP` or no golden frames here |

## How to un-block (owner action, later machine)

1. Build the pinned host lavapipe successfully (the same single command above).
2. Render goldens with it:

       VK_DRIVER_FILES=<pin>/lvp_icd.json env -u LP_NUM_THREADS \
           build/linux/kitty --headless --frames 300 --dump 0,150,299 --out /tmp/gold

3. Write `<pin stamp>` to `STAMP` and copy the three `frame_*.bgra` files here
   (they are deliberately untracked; the owner commits them).
4. `tests/golden/check.sh` then returns `GOLDEN PASS` for the pin and `GOLDEN FAIL` for
   any other lavapipe build.
