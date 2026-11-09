# M0a (Linux half of M0) — critique of the opencode report (2026-10-31)

> Source: Work/Kitty/Docs/Development/m0a_linux_critique.md
> Collected: 2026-11-01
> Published: 2026-10-31

# M0a (Linux half of M0) — critique of the opencode report (2026-10-31)

Verdict: **the work is good; it is not ready to commit as-is.** I re-ran scripts 1–11 on a scratch copy of `~/Work/Kitty/repo`, building it from clean. The rerun gave the same gates as the report. With script 7's alpha probe fixed, S7.smoke PASSes, which makes 42 PASS. S7.golden is BLOCKED for a reason outside opencode's control. S12.end-state FAILs because of a script defect. Four code defects remain, and two of them would be copied into M0b. The report is mostly honest, but it misstates four things and leaves two departures out.

## Verified true

- **S7.smoke is unsatisfiable.** `m0a_linux_opencode_prompt.md:529` loops `range(3,…)` and then reads `d[i+3]`. That is my defect. In the rerun, frame_150 had 0 bytes where alpha ≠ 255, and 30 distinct colours. With `range(0,…)` (`d[i+3]` is alpha), the gate gives S7.smoke PASS.
- **The Vulkan4Aros HEAD moves are the owner's commits.** 567df38d → 7b093d4c are four "l0025 b10 kit" commits made between 07:55 and 08:22 on 2026-10-30. The pin build ended at 07:35:30, 12 s after `baseline.done` at 07:35:18. `build_host_lavapipe.sh` contains no git command. There have been 29 more commits since then.
- **The pin really fails.** The current committed series does not apply to a fresh 26.2.2 tarball when run through the official script:
  - `0025` touches 5 files and fails 2 of its 2 nvk_image.c hunks, at -965 and -1033.
  - The tree has a blank line before the `#if DETECT_OS_LINUX` that 0021 introduces; 0025's context has no blank line there.
  - `~/Work/host_lavapipe_llvm20`, built on 2026-10-22, applied 0025 cleanly. So that tree is **not reproducible from Vulkan4Aros HEAD**.
  - This is a Vulkan4Aros series problem and needs owner action. The only alternative is pinning a different series.
- No commits, branches or stashes in any repo. The writes stayed inside the two authorized trees. Baseline taken before the first write.
- Integer Q16.16 modplay; `step_428=11314`; `hash10s=e663d2da535e69a5` reproduced in my rerun. The SDL dummy stream reported 480000 frames in my rerun, against 479040 in the report.
- H5 headless and windowed ran 3000 ticks with 0 dropped. The planted VUIDs are real: `…srcImageLayout-00128/01917` and `VUID-vkQueuePresentKHR-pWaitSemaphores-03268`.

## Code defects (fix before commit)

1. **`plat_priv.h` leaks into six modules, and the report discloses one.**
   - Includers: `render_internal.h:4`, `audio.cpp:4`, `modplay.cpp:4`, `guru.cpp:4`, `bench.cpp:5`, `main.cpp:6`. That is 39 calls to private `plat_*` functions outside `platform/`.
   - Every compile line has `-Isrc/platform/linux`, so the S4 lint can't see the leak.
   - The report says logging *forces* this. It doesn't:
     - `pl_log(Plat&, const char*)` is in `platform.h`.
     - The only missing piece is a formatter, and `src/platform/rt` (8 of 200 lines used) exists for exactly that.
   - M0b will copy this layout into `platform/aros` unless it's fixed now.
2. **The NVK self-test is not the 64×64 test scene, and it doesn't use the function that is tested.**
   - `r_tier_probe` (`render_init.cpp:418–433`) is `vkCmdClearColorImage`: no pipeline, no rasterization. It proves nothing about NVK rendering, which was C20's whole point.
   - It compares with an inline loop (`:557`) that has no coverage-mask check.
   - `image_compare` (selftest.cpp) is never called outside its unit test, so t-selftest tests dead code.
   - The report's departures list does not mention this.
3. **Device creation enables every Vulkan 1.3 feature the device reports.**
   - `f13` is filled by `GetPhysicalDeviceFeatures2` and passed to `vkCreateDevice` unchanged (`render_init.cpp:726–730`).
   - Line 726 is a no-op.
   - The report says it enables `shaderDemoteToHelperInvocation`. It actually turns on everything: sync2, dynamic rendering, maintenance4 and the rest.
   - The demote requirement itself is real: at SPIR-V 1.6, glslang lowers `discard` to demote.
   - Fix: zero `f13` and set only the demote bit.
4. **The watchdog exit code is a hardcoded constant.**
   - `main.cpp:197` calls `plat_exit_watchdog_ms(2000, 3)`, and `plat.cpp:157` runs `_exit(3)` no matter what validation counted.
   - S8.h5-windowed-knownbad survives only because the script also greps the log for the VUID.
   - The watchdog should exit with `validation_messages>0 ? 3 : 4`. A hang with no message is a failure, not a pass.

## Report misstatements

- **"D4 assumes a Plat& before a Plat exists": false.**
  - `pl_init(plat, pa)` runs at `main.cpp:118`, before `r_init`.
  - `PlatArgs.renderer_override` exists and is unused.
  - `r_init` already takes a `Tier`. `main` can call `pl_env_get(plat,"Kitty/Renderer")` and pass the result in.
  - `plat_env_global` (`render_init.cpp:663`) is unnecessary.
- **"lavapipe grants 4 images": self-inflicted, or at least unproven.** The code requests `minImageCount + 1` (`render_init.cpp:166`), and the log never records `minImageCount`.
  - The design's 2-image rule needs to become `max(2, minImageCount)`.
  - The image count should be logged.
- **"IMMEDIATE preference" is listed as a departure. It is the spec:** the design already defaults the present mode to IMMEDIATE to avoid the FIFO/WaitTOF trap.
- **"Driver's interface matcher".** The warnings come from the validation layer, and the messenger counts WARNING severity (`render_init.cpp:627`). The FS padding works around a performance warning that the count treats as a failure.
  - Either is a legitimate choice, but it should be stated as one.
- Reporting gaps the report already owns: the pin build's exit code and its wall time.

## Prompt defects (mine — fix in the next prompt)

1. The S7 alpha probe: the range bug above. Add a self-test of the probe against a synthetic opaque dump.
2. **S12 compares Vulkan4Aros `HEAD`**, which the owner moves during any long session. It should hash the protected content (`src/abi/v1/AROS*`, `src/abi-patches/v1/**`, `AROS_PIN`) and treat a HEAD move as INFO.
3. **I never dry-ran the pin's patch step before dispatch.** A 30-second `patch --dry-run` loop would have predicted BLOCKED.
4. **`mesa_series_sha256` is computed when the check runs, from the live Vulkan4Aros tree.** It has to be stamped when the pin is built, or a later series edit silently changes what the golden claims.
5. No static gate that production calls `image_compare`, and no gate that the self-test records a draw. The host can't run NVK, but a grep would have caught the stub.
6. No gate requiring `plat_priv.h` to be included only under `src/platform/`, and compile lines have no per-module `-I` sets.

## Owner decisions needed

- **The pin:** fix 0025's context against 0021, or re-pin. Until one of those happens, S7.golden stays BLOCKED on both M0a and M0b.
- **M0a.1 fix round:** code defects 1–4 above, plus prompt fixes 1, 2 and 6. Then commit M0a.
