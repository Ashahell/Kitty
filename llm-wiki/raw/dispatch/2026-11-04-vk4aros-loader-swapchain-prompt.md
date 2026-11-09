# Vulkan4Aros loader swapchain dispatch prompt

> Source: Docs/Development/dispatch/vk4aros_loader_swapchain_opencode_prompt.md
> Collected: 2026-11-04
> Published: 2026-11-04

# opencode prompt — Vulkan4Aros: fix `boundary_r7_create_device` stripping `VK_KHR_swapchain` (2026-11-04)

## Owner authorization (read first — opencode never edits this block)

    BOOTS GRANTED: 0
    LANE: none. Never start, boot, or connect to any AROS guest, on any lane.
    WRITE (these and nothing else):
      - ~/Work/projects/Vulkan4Aros/src/vulkan/loader/vk_aros_loader.c   only through fix.patch
      - ~/Work/projects/Vulkan4Aros/src/vulkan/loader/loader_libdefs.h   only through fix.patch
      - ~/Work/projects/Vulkan4Aros/scripts/aros_audit.sh                only through fix.patch
      - ~/Work/lanes/l0025/loader-fixv3/    (new; the script creates it)
      - /tmp/opencode/l0025-loader-fixv3/ and /tmp/opencode/l0025-loader-fixv3-input/   (new; build
        output and extracted files)
      - what the two scripts below write by themselves: src/vulkan/loader/stock/gen_defines.asm
        (git-ignored; regenerated with identical bytes, and script gate L1.gen-defines-unchanged
        checks it) and .offload/telemetry.jsonl if the audit writes it.
    NEVER WRITE: src/abi/ (all of it, src/abi/v1/AROS* above all), src/vulkan/loader/stock/ apart from
      the generated file above, ~/Work/lanes/l0025/next-boot/, any disk image in ~/Work/vms/,
      DH0: or DH1:, AGENTS.md, docs/, llm-wiki/, ~/Work/Kitty/.
    GIT: read-only commands only (status, diff, log, rev-parse) plus the script's own `git apply`.
      No add, commit, stash, reset, checkout, restore, clean, rebase, merge, or push. The owner lands
      the change. Another session may be committing in this repository: don't touch its files.
    RUN: the extraction and script below, unchanged, once. Read-only shell commands.

If anything here conflicts with Vulkan4Aros `AGENTS.md`, this block wins for this round, and you say so in the report.

## The defect

`boundary_r7_create_device` (`src/vulkan/loader/vk_aros_loader.c`) removes the application's `VK_KHR_swapchain` and adds it back only if the device advertises it. To ask the device, it calls `vkGetInstanceProcAddrStock(VK_NULL_HANDLE, "vkEnumerateDeviceExtensionProperties")`.

That command is not global:

- `stock/gpa_helper.c:270` `globalGetProcAddr` serves 4 names, and this is not one of them.
- `stock/trampoline.c:96` returns NULL for a NULL instance.

So the lookup returns NULL, `want_swapchain` stays 0, and every device loses the extension. `vkGetDeviceProcAddr` then returns NULL for `vkCreateSwapchainKHR`, and the instance trampoline aborts on its NULL slot (`stock/wsi.c:452`).

Commit `c0b005ce` (2026-10-04) added the check as an NVK fix, with a comment that wrongly calls this command global. The same function's comment on `vkCreateDevice` records the same trap and calls the stock symbol directly.

**Measured:**

- The deployed library `19a3143e…` has the lookup in `boundary_r7_create_device`, and no reference to `vkEnumerateDeviceExtensionProperties` there.
- Kitty's M0b.8 guest run jumped to address 0 at its `vkCreateSwapchainKHR` call.

**Not measured:** any of this on a guest with the fixed library. This round cannot measure it, and your report must not claim it.

## The fix

`fix.patch` changes three files:

1. `vk_aros_loader.c`: calls the stock `vkEnumerateDeviceExtensionProperties` symbol directly, and corrects the comment. It keeps the behaviour NVK needs: a device that doesn't advertise the extension still doesn't get it.
2. `loader_libdefs.h`: stamps `(04.11.2026) fixv3`, so `liblist_probe idstring` can tell this build from `fixv2`.
3. `scripts/aros_audit.sh`: adds a `forbid_in` gate, so the name lookup can't come back.

Don't improve, widen or reformat the patch. In particular, leave `boundary_r7_create_instance` and its instance-extension lookup alone: `vkEnumerateInstanceExtensionProperties` really is global.

## What to do

1. Extract and check the two files:

```bash
PR=~/Work/Kitty/Docs/Development/dispatch/vk4aros_loader_swapchain_opencode_prompt.md
X=/tmp/opencode/l0025-loader-fixv3-input
mkdir -p "$X"
sed -n '/^<<<FIX.PATCH$/,/^>>>FIX.PATCH$/p' "$PR" | sed '1d;$d' > "$X/fix.patch"
sed -n '/^<<<LOADER_FIXV3.SH$/,/^>>>LOADER_FIXV3.SH$/p' "$PR" | sed '1d;$d' > "$X/loader_fixv3.sh"
sha256sum "$X/fix.patch" "$X/loader_fixv3.sh"
```

The two hashes must be:

```
7887d5ce74a226c769ac6a5e1a24c3df9df52beed4a9cf3d609da7dc762f992a  fix.patch
9820bf4cb917f8247cdd4b0892b7f7175cce103ab116249fe7fe6ee61415c9a3  loader_fixv3.sh
```

If either differs, stop and report. Don't retype or repair anything.

2. Run the script once. It takes a few minutes:

```bash
bash /tmp/opencode/l0025-loader-fixv3-input/loader_fixv3.sh /tmp/opencode/l0025-loader-fixv3-input/fix.patch
```

3. Report, then stop.

## What the script does

- **L0:** stops if an input file is dirty, the two source files aren't the expected base, the patch doesn't apply, or the llvm toolchain doesn't resolve.
- **L1:** rebuilds the unfixed tree. The stock object must hash to `26cc90b0…` and the library to the deployed `19a3143e…`. If not, it stops with no source changed. A mismatch means the recipe or an input moved: report it, and don't chase it. Runs the audit as a baseline.
- **L2, L3:** applies the patch and rebuilds. The library must hash to `90237ed2…`. Then the ABI stamp, unresolved symbols, relocations, version stamp and warnings; `LibOpen`, `LibClose` and `LibExpunge` byte-identical to the control; and the relocations in `boundary_r7_create_device`: the control has the lookup and no direct call, the fixed build the reverse.
- **L4:** reruns the audit. The new gate must show OK, and the audit's error lines must be the same as the baseline's. This compares against the baseline because other work in this repository can change the audit's result.
- **L5:** stages the library and `MANIFEST.sha256` in `~/Work/lanes/l0025/loader-fixv3/`.

If a gate fails from L3 on, the script takes the patch back out with `git apply -R` and exits non-zero. Don't rerun it or fix anything; `~/Work/lanes/l0025/loader-fixv3/` existing also makes a rerun stop by design. Report the failure.

The build must run at the real repository path: `__FILE__` strings make the library's bytes depend on it, so a worktree or copy can't match the pins.

## Known and expected

- This was dry-run on 2026-11-04 on a copy mounted at the repository's path, at HEAD `49a9b25d`: all gates passed, the failure path reverted the patch, and the dirty-file, moved-base and existing-output stops all fired. The copy held only `scripts/` and `src/vulkan/`, so that audit reported errors from the missing files. In the real tree, AGENTS.md expects 0 errors and 0 warnings; report what you measure.
- HEAD may have moved. That's fine as long as the gates pass.

## Final report (reply with this only)

1. Result: `DONE`, or the line the script stopped at, and the exit code.
2. `cat ~/Work/lanes/l0025/loader-fixv3/evidence/gates.txt`, whole.
3. `evidence/head.txt`, the two extraction hashes, and `git status --short -- src/vulkan/loader scripts/aros_audit.sh`.
4. The audit's `Audit complete` lines, before and after, and any differing lines in `evidence/audit_errs_before.txt` and `evidence/audit_errs_after.txt`.
5. Every departure from this prompt, or "none".

Say plainly what is not verified: the fixed library has not run on any guest, and nothing is committed, deployed, or staged in `next-boot/`.

## Files

```
<<<FIX.PATCH
--- a/src/vulkan/loader/vk_aros_loader.c
+++ b/src/vulkan/loader/vk_aros_loader.c
@@ -501,31 +501,27 @@
      * not have. Khronos requires the implementation to reject an unsupported
      * extension, so the caller was at fault, not the driver.
      *
-     * Khronos: vkEnumerateDeviceExtensionProperties is a global entry point, so
-     * it resolves through the stock trampoline with a NULL instance. */
+     * vkEnumerateDeviceExtensionProperties is NOT a global entry point, so
+     * vkGetInstanceProcAddrStock(NULL, ...) returns NULL for it. Looking it up
+     * that way stripped VK_KHR_swapchain from every device. Call the stock
+     * trampoline symbol directly, as vkCreateDevice does below. */
     int want_swapchain = 0;
     {
-        PFN_vkEnumerateDeviceExtensionProperties enumExt =
-            (PFN_vkEnumerateDeviceExtensionProperties)
-            vkGetInstanceProcAddrStock(VK_NULL_HANDLE,
-                                       "vkEnumerateDeviceExtensionProperties");
-        if (enumExt) {
-            uint32_t cnt = 0;
-            if (enumExt(physicalDevice, NULL, &cnt, NULL) == VK_SUCCESS && cnt) {
-                VkExtensionProperties *props =
-                    (VkExtensionProperties *)AllocVec(cnt * sizeof(*props), MEMF_ANY);
-                if (props) {
-                    if (enumExt(physicalDevice, NULL, &cnt, props) == VK_SUCCESS) {
-                        uint32_t e;
-                        for (e = 0; e < cnt; e++)
-                            if (boundary_streq(props[e].extensionName,
-                                               VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
-                                want_swapchain = 1;
-                                break;
-                            }
-                    }
-                    FreeVec(props);
+        uint32_t cnt = 0;
+        if (vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &cnt, NULL) == VK_SUCCESS && cnt) {
+            VkExtensionProperties *props =
+                (VkExtensionProperties *)AllocVec(cnt * sizeof(*props), MEMF_ANY);
+            if (props) {
+                if (vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &cnt, props) == VK_SUCCESS) {
+                    uint32_t e;
+                    for (e = 0; e < cnt; e++)
+                        if (boundary_streq(props[e].extensionName,
+                                           VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
+                            want_swapchain = 1;
+                            break;
+                        }
                 }
+                FreeVec(props);
             }
         }
     }
--- a/src/vulkan/loader/loader_libdefs.h
+++ b/src/vulkan/loader/loader_libdefs.h
@@ -10,7 +10,7 @@
 #define MAJOR_VERSION    1
 #define REVISION_NUMBER  0
 #define MINOR_VERSION    0
-#define VERSION_STRING   "$VER: vulkan_loader.library 1.0 (26.10.2026) fixv2\n"
+#define VERSION_STRING   "$VER: vulkan_loader.library 1.0 (04.11.2026) fixv3\n"
 #define COPYRIGHT_STRING ""
 #define RESIDENTPRI      0
 #define FUNCTIONS_COUNT  5
--- a/scripts/aros_audit.sh
+++ b/scripts/aros_audit.sh
@@ -1674,6 +1674,9 @@
 require_in "$BND" 'arosPollWindowEvents'         "plain-C event pump present"
 require_in "$BND" 'vkCreateXcbSurfaceKHR'        "surface created via the registered VK_KHR_xcb_surface entry point"
 require_in "$BND" 'vkpal_surface_poll_events'    "boundary polls events via the PAL (no dos.library)"
+# vkEnumerateDeviceExtensionProperties is not a global entry point: a NULL-instance
+# lookup returns NULL, which stripped VK_KHR_swapchain from every device.
+forbid_in "$BND" '"vkEnumerateDeviceExtensionProperties"' "boundary does NOT look up vkEnumerateDeviceExtensionProperties by name"
 
 # The WSI platform must be compiled into every stock component built through the
 # AROS cmake toolchain. A layer built without it is the worst case, not the
>>>FIX.PATCH
```

```
<<<LOADER_FIXV3.SH
#!/bin/bash
# Vulkan4Aros loader fixv3: control rebuild, apply fix.patch, rebuild, gates, stage. 0 boots.
# usage: loader_fixv3.sh <fix.patch>
set -u
R=$HOME/Work/projects/Vulkan4Aros
OUT=${LOADER_OUT:-$HOME/Work/lanes/l0025/loader-fixv3}
W=${LOADER_WORK:-/tmp/opencode/l0025-loader-fixv3}
PATCH=$(realpath "$1")
BIN_CTL=19a3143eb6d792d944d1bcff2d731ded7726f275e6522534c84edb40f05217d3
OBJ_CTL=26cc90b01eb74f23a4c952c2d2eb283c7fc8d645afda14d8519906089b3cdc64
BIN_FIX=90237ed20bf7527782253e6853ee66bd442f16247c9463df2a22fa0a1fb536f3
FILES="src/vulkan/loader/vk_aros_loader.c src/vulkan/loader/loader_libdefs.h scripts/aros_audit.sh"
INPUTS="src/vulkan/loader/stock src/vulkan/pal scripts/build_stock_e2.sh scripts/gen_defines_aros.sh scripts/aros_build_env.sh"
[ ! -e "$OUT" ] || { echo "STOP: $OUT exists"; exit 2; }
mkdir -p "$OUT/evidence" "$W" || exit 2
G=$OUT/evidence/gates.txt; : > "$G"
gate() { echo "GATE $1 $2${3:+ $3}" | tee -a "$G"; }
stop() { echo "STOP: $1" | tee -a "$G"; exit 3; }
cd "$R" || exit 2
git rev-parse HEAD > "$OUT/evidence/head.txt"
git status --short -- $FILES $INPUTS > "$OUT/evidence/status_start.txt"
[ ! -s "$OUT/evidence/status_start.txt" ] && gate L0.clean PASS || { gate L0.clean FAIL; stop "tracked input files are dirty"; }
sha256sum -c --quiet - <<S && gate L0.base PASS || { gate L0.base FAIL; stop "files moved since the patch was written"; }
1719415a1e690fb53f5404a84073a17feeaf6c7ba8acf37a212d209cefb5ad2e  src/vulkan/loader/vk_aros_loader.c
5d1f811d1434768ec165e09ff4a3705d3bd9cae1abce747fc06c6d65b4b092b2  src/vulkan/loader/loader_libdefs.h
S
git apply --check "$PATCH" && gate L0.patch-applies PASS || { gate L0.patch-applies FAIL; stop "patch does not apply"; }

export AROS_ABI=v1
. scripts/aros_build_env.sh
TB=$AROS_TOOLCHAIN/bin
[ "$AROS_TOOLCHAIN_FAMILY" = llvm ] && gate L0.toolchain PASS || { gate L0.toolchain FAIL; stop "v1 llvm toolchain not resolved"; }
[ -d "$AROS_COLLECT_AROS_DIR" ] && export PATH="$AROS_COLLECT_AROS_DIR:$PATH"
export STOCK_E2_OUT=$W/stock_e2
VK=$R/src/vulkan; STOCK=$VK/loader/stock; PAL=$VK/pal; SDK=$AROS_SDK_INCLUDE
INC="-I $STOCK -I $STOCK/include -I $STOCK/generated -I $PAL -I $PAL/xcb -I $SDK -I $SDK/aros/stdc -I $SDK/aros/posixc"
[ -d "${AROS_SDK_INCLUDE_DEV:-}" ] && INC="$INC -I $AROS_SDK_INCLUDE_DEV -I $AROS_SDK_INCLUDE_DEV/aros/stdc -I $AROS_SDK_INCLUDE_DEV/aros/posixc"
DEF="-DVK_AROS_STOCK_BOUNDARY -DVK_AROS_LIBRARY_HOST -DVK_USE_PLATFORM_AROS -DVK_USE_PLATFORM_XCB_KHR -DUNKNOWN_FUNCTIONS_SUPPORTED -DVK_DEBUG=0 -DVK_AROS_LVO_LAST_ARG"
boundary() { # <outdir>: scripts/build_gcc16.sh's v1 arm, boundary + link + stamp
  mkdir -p "$1"
  "$AROS_CC" -c -O2 $AROS_TARGET_CFLAGS -fno-strict-aliasing -fno-stack-protector -Wall $DEF $INC -o "$1/vk_aros_loader.o" "$VK/loader/vk_aros_loader.c" || return 1
  "$TB/ld.lld" -r "$STOCK_E2_OUT/vulkan_loader_stock.o" "$1/vk_aros_loader.o" -o "$1/vulkan_loader.library" || return 1
  printf '\017\001' | dd of="$1/vulkan_loader.library" bs=1 seek=7 conv=notrunc status=none
}
sha() { sha256sum "$1" | cut -d' ' -f1; }
listing() { "$TB/llvm-objdump" -d --disassemble-symbols=LibOpen,LibClose,LibExpunge "$1" | tail -n +3; }
# error lines only, without the colour codes
audit_errs() { sed 's/\x1b\[[0-9;]*m//g' "$1" | grep -E '^ *(ERROR|ERR)\b|^ *ERROR:' | sort; }

# L1: control. The unfixed tree must rebuild byte-exact, or nothing later means anything.
GD=$STOCK/gen_defines.asm; GD0=$(sha "$GD" 2>/dev/null)
bash scripts/build_stock_e2.sh > "$OUT/evidence/build_stock.log" 2>&1 || { gate L1.stock FAIL; stop "stock build failed"; }
[ "$(sha "$GD")" = "$GD0" ] && gate L1.gen-defines-unchanged PASS || { gate L1.gen-defines-unchanged FAIL; stop "generated gen_defines.asm changed"; }
[ "$(sha "$STOCK_E2_OUT/vulkan_loader_stock.o")" = "$OBJ_CTL" ] && gate L1.stock PASS || { gate L1.stock FAIL; stop "stock object is not the deployed one"; }
boundary "$W/ctl" > "$OUT/evidence/build_ctl.log" 2>&1 || { gate L1.control FAIL; stop "control build failed"; }
[ "$(sha "$W/ctl/vulkan_loader.library")" = "$BIN_CTL" ] && gate L1.control PASS || { gate L1.control FAIL; stop "control is not byte-exact"; }
bash scripts/aros_audit.sh > "$OUT/evidence/audit_before.log" 2>&1; echo "audit before rc=$?" | tee -a "$G"

# L2: the fix. From here a failed gate takes the patch back out.
git apply "$PATCH" || { gate L2.apply FAIL; stop "apply failed"; }
gate L2.apply PASS
FAILED=0
chk() { if [ "$2" = "$3" ]; then gate "$1" PASS; else gate "$1" FAIL "want=$3 got=$2"; FAILED=1; fi; }
boundary "$W/fix" > "$OUT/evidence/build_fix.log" 2>&1 || { gate L3.build FAIL; FAILED=1; }
L=$W/fix/vulkan_loader.library
if [ -f "$L" ]; then
  chk L3.sha "$(sha "$L")" "$BIN_FIX"
  chk L3.warnings "$(grep -c 'warning:' "$OUT/evidence/build_fix.log")" "$(grep -c 'warning:' "$OUT/evidence/build_ctl.log")"
  chk L3.elf "$(od -A n -t x1 -N 4 "$L" | tr -d ' ')" 7f454c46
  chk L3.abi "$(od -A n -t u1 -j 7 -N 2 "$L" | xargs)" "15 1"
  chk L3.und "$("$TB/llvm-readelf" -sW "$L" | awk '$5=="GLOBAL"&&$7=="UND"' | wc -l)" 0
  chk L3.32s "$("$TB/llvm-readelf" -rW "$L" | grep -c R_X86_64_32S)" 0
  chk L3.stamp "$(strings -a "$L" | grep -c 'vulkan_loader.library 1.0 (04.11.2026) fixv3')" 1
  listing "$W/ctl/vulkan_loader.library" > "$OUT/evidence/contract_ctl.asm"; listing "$L" > "$OUT/evidence/contract_fix.asm"
  [ -s "$OUT/evidence/contract_fix.asm" ] && cmp -s "$OUT/evidence/contract_ctl.asm" "$OUT/evidence/contract_fix.asm" && gate L3.contract PASS || { gate L3.contract FAIL; FAILED=1; }
  # the broken build references GIPA and not the enumerate call; the fixed one the reverse
  for t in ctl fix; do "$TB/llvm-objdump" -dr --disassemble-symbols=boundary_r7_create_device "$W/$t/vulkan_loader.library" > "$OUT/evidence/r7_$t.asm"; done
  c() { grep -cE "R_X86_64.*[[:space:]]$2\b" "$OUT/evidence/r7_$1.asm"; }
  chk L3.r7-control-has-lookup "$(c ctl vkGetInstanceProcAddrStock)/$(c ctl vkEnumerateDeviceExtensionProperties)" 1/0
  chk L3.r7-fixed-direct-call "$(c fix vkGetInstanceProcAddrStock)/$(c fix vkEnumerateDeviceExtensionProperties)" 0/1
  chk L3.r7-create-device "$(c fix vkCreateDevice)" 1
fi
chk L3.stock-pristine "$(git status --short -- $INPUTS | wc -l)" 0
chk L3.only-patch-files "$(git status --short -- $FILES | wc -l)" 3
bash scripts/aros_audit.sh > "$OUT/evidence/audit_after.log" 2>&1; echo "audit after rc=$?" | tee -a "$G"
chk L4.audit-new-gate "$(sed 's/\x1b\[[0-9;]*m//g' "$OUT/evidence/audit_after.log" | grep -c 'OK: boundary does NOT look up vkEnumerateDeviceExtensionProperties by name')" 1
audit_errs "$OUT/evidence/audit_before.log" > "$OUT/evidence/audit_errs_before.txt"; audit_errs "$OUT/evidence/audit_after.log" > "$OUT/evidence/audit_errs_after.txt"
cmp -s "$OUT/evidence/audit_errs_before.txt" "$OUT/evidence/audit_errs_after.txt" && gate L4.audit-no-new-errors PASS || { gate L4.audit-no-new-errors FAIL; FAILED=1; }
sed 's/\x1b\[[0-9;]*m//g' "$OUT/evidence/audit_after.log" | grep 'Audit complete' | tee -a "$G"
git diff -- $FILES > "$OUT/evidence/landed.diff"
if [ "$FAILED" != 0 ]; then git apply -R "$PATCH" && echo "patch reverted" | tee -a "$G"; exit 4; fi

# L5: stage. l0025/next-boot and the images stay as they are.
cp "$L" "$OUT/vulkan_loader.library"
cp "$W/ctl/vulkan_loader.library" "$OUT/evidence/vulkan_loader.control.library"
( cd "$OUT" && sha256sum vulkan_loader.library > MANIFEST.sha256 )
gate L5.staged PASS "$(cat "$OUT/MANIFEST.sha256")"
echo DONE | tee -a "$G"
>>>LOADER_FIXV3.SH
```
