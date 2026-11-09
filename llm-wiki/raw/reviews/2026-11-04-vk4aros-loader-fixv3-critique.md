# Vulkan4Aros loader fixv3 critique

> Source: Docs/Development/vk4aros_loader_fixv3_critique.md
> Collected: 2026-11-04
> Published: 2026-11-04

# Vulkan4Aros loader fixv3 critique (2026-11-04)

**Verdict: the round is done and was run correctly. The fix is built, gated and staged, and not committed. It has not run on a guest, so the swapchain diagnosis is still unconfirmed.**

Dispatch: `Docs/Development/dispatch/vk4aros_loader_swapchain_opencode_prompt.md`, sha256 `4aef7f07f54189a5b279ff2ad50fc7809fd3507495bc6e7efb9fed9a86479ead`. Boots: 0.

## Verified

- **Scripts:** `fix.patch` `7887d5ce…` and `loader_fixv3.sh` `9820bf4c…` are byte-identical to the ones I wrote.
- **Library:** `~/Work/lanes/l0025/loader-fixv3/vulkan_loader.library` is 850,472 B, sha256 `90237ed20bf7527782253e6853ee66bd442f16247c9463df2a22fa0a1fb536f3`, and byte-identical to my dry-run build. The control copy is `19a3143e…`, the deployed library.
- **The fix in the binary:** `boundary_r7_create_device` references `vkEnumerateDeviceExtensionProperties` and `vkCreateDevice`, and no longer `vkGetInstanceProcAddrStock`. The stamp reads `fixv3`.
- **Tree:** three files changed, and the live diff equals the patch and `landed.diff`. `src/abi` has no dirty entries. HEAD moved from `9a122db9` to `66b26f60` during and after the round through the parallel session's commits; neither carried these files.
- **Left alone:** `next-boot/` still dated 2026-10-26, DH1's image unchanged, 54 boot records, no backup image, no guest running.
- **Audit, rerun by me on a read-only mount without `AROS_ABI`:** 0 errors, 0 warnings, the expected ATCPGUI skip, and the new gate OK. That matches `AGENTS.md`.

## Report errors

- **Wrong cause.** It attributes the audit's one error to "the gcc core toolchain src/abi/v1/toolchain-core-x86_64 not provisioned". That tree is llvm by design. The error comes from my script (below). The prompt told opencode not to chase it, so the report should have stopped at the measurement.

Everything else checked is accurate: hashes, gates, audit lines, status, HEAD, procedure.

## Owner defects

- **`loader_fixv3.sh` exports `AROS_ABI=v1` before running the audit.** The audit then looks for a gcc in the v1 tree and reports 1 false error. L4 compared before and after, so the result stood, but the round's audit output isn't the clean one. Corrected here by the rerun.
- **I blamed that variable for the skipped phase as well.** That was wrong: the ATCPGUI skip is the one `AGENTS.md` expects.
- **The gates can't test behaviour.** They prove the code path changed, in the binary. Nothing on the host calls it.

## Open

1. **Commit.** The fix is only in the working tree, in a repository where another session is committing. A careless `git add -A` there would sweep it in, and a checkout or reset would lose it. The staged library and `landed.diff` survive either way.
2. **Dirty tree against B10.** A loader rebuilt from this tree is now `fixv3`, so a byte-exact control against `19a3143e…` fails there until `loader_libdefs.h` and `vk_aros_loader.c` are at their old state.
3. **M0b.9 is the first guest test.** Its S16 now finds the staged library. Dispatching it changes DH1 and breaks B10's pins.
