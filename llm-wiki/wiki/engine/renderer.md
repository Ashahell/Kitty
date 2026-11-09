# Renderer

> Sources: Kitty GDD v3, 2026-10-31; Kitty art bible v2, 2026-10-31; Engine design rev 5, 2026-10-30; Engine design sources rev 5, 2026-10-30; Engine design rev 2 critique, 2026-10-29; Vulkan4Aros source checks, 2026-10-30
> Raw: [GDD v3 rev a](../../raw/game-design/2026-10-31-gdd-v3a.md); [Art bible v2](../../raw/game-design/2026-10-31-art-bible-v2.md); [Engine design rev 5](../../raw/engine-design/2026-10-30-engine-design-rev5.md); [Sources rev 5](../../raw/engine-design/2026-10-30-engine-design-sources-rev5.md); [Rev 2 critique](../../raw/reviews/2026-10-29-engine-design-rev2-critique.md); [VK source checks](../../raw/platform/2026-10-30-vulkan4aros-source-checks.md); [Rev 5 hand patches](../../raw/reviews/2026-10-30-rev5-hand-patches.md)
> Updated: 2026-10-31

## Overview

The renderer of CopperForge (the engine's name since 2026-10-31, [GDD v3](../game-design/gdd-v3.md)) targets core Vulkan 1.3 through the AROS loader chain. It works at 320×256 internally, blit-scaled to the window. There are two tiers: lavapipe is the shipped default, and NVK is selected only by a startup self-test (C20). Depth ordering is an isometric painter's sort, and transparency is ordered dither, never blending. The engine never uses `VkPolygonOffset`.

> **Status: Outdated** (2026-10-31)
> Art bible v2 asks engine rev 6 for per-chapter look levels (E3): an indexed and a true-colour path, render targets per level, alpha blending and per-pixel light on L4, and up to 1920×1080 on NVK. The single 320×256, dither-only renderer is the L1 case. See [art bible](../game-design/art-bible.md).

## Instance extensions (the R1 decision)

- "On AROS, `ppEnabledExtensionNames` contains no WSI extension; the loader adds `VK_KHR_surface` and `VK_KHR_xcb_surface` only when the ICD advertises them."
- "On Linux, the instance extension list is exactly what `SDL_Vulkan_GetInstanceExtensions` returns."
- "The NVK self-test instance is created with zero instance extensions."
- "`VK_EXT_debug_utils` is enabled only in debug builds and only when `vkEnumerateInstanceExtensionProperties` lists it." This cites [S94]: the loader comment that records `rc=-7 (VK_ERROR_EXTENSION_NOT_PRESENT)` on NVK when the extension list was appended unconditionally.

Why it matters: the loader comment (`vk_aros_loader.c:419`–`:421`) records that NVK advertises 10 instance extensions and neither `VK_KHR_surface` nor `VK_KHR_xcb_surface`, so requesting either of them breaks NVK instance creation. Revisions 2 and 3 got this wrong. See [AROS platform facts](../platform/aros-platform-facts.md).

## VkResult classification (§6.2)

| Result | Class | Action |
|---|---|---|
| `VK_SUBOPTIMAL_KHR` | normal | continue; schedule extent rebuild |
| `VK_ERROR_OUT_OF_DATE_KHR` | recreate-swapchain | rebuild the swapchain and command buffers; never a fallback |
| `VK_ERROR_DEVICE_LOST` | fatal-fallback | NVK → lavapipe once; already on lavapipe → Guru screen + exit |
| `VK_ERROR_SURFACE_LOST_KHR` | recreate-surface | recreate the surface through the loader once, else Guru + exit |

"A lost surface is a window-lifecycle problem, not a tier problem: it never changes the selected driver."

## Pipelines and frame

- ≤ 6 shaders (`VS_iso`, `FS_flat`, `FS_tex`, `FS_dither`, `FS_ui`) and **5 pipelines**, all created at startup, because nothing caches pipelines on AROS.
  - Art bible v2 E3 proposes pipeline sets per look level, created at startup or chapter load, never inside a room.
- Per frame: acquire (fence-synced) → record → submit → present, with a 2-image swapchain and 1 frame in flight. The present mode defaults to IMMEDIATE, to avoid the FIFO/WaitTOF trap.
- Tier self-test: render a 64×64 test scene and compare against a tolerance. The Q13 default is per-channel ±4/255 plus coverage-mask equality.

## Golden images and proofs

- **Host:** golden ↔ host CI is bit-exact, with the same Mesa commit + `LP_*` env + `AROS_PIN` hash.
- **Guest:** guest ↔ golden is a proxy check whose tolerance M0 sets. The guest's LLVM version need not match the host's.
- **[S85]:** measured one depth-bias test with a pixel-count metric (`diff=153600` for both bias signs); "it is not a bit-exactness proof".
- **AROS presented-pixel proof:** `ReadPixelArray` (cybergraphics, LVO −160) from the window RastPort, at 320×256 with `RECTFMT_BGRA32`; otherwise the gate is BLOCKED.

## See Also

- [Engine architecture](engine-architecture.md)
- [Verification and CI](../process/verification-and-ci.md)
