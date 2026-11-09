# Game design v3

> Sources: Kitty GDD v3, 2026-10-31; Kitty art bible v2, 2026-10-31
> Raw: [GDD v3 rev a](../../raw/game-design/2026-10-31-gdd-v3a.md); [GDD v3](../../raw/game-design/2026-10-31-gdd-v3.md); [Art bible v2 rev a](../../raw/game-design/2026-10-31-art-bible-v2a.md)
> Updated: 2026-10-31

## Overview

GDD v3 (`Docs/Development/kitty_core_awakening_design_v3.md`) is the current game design document. It is GDD v2 with the presentation rules rewritten. The owner decided on 2026-10-31 that graphics quality improves as the player goes through the game, which restores v1's "the engine visibly evolves with the story". Everything else is v2: see [Game design v2](gdd-v2.md).

## What changed from v2

- **§3.2:** each chapter has a look level: Ch1 L1, Ch2 L2, Ch3 L3, Ch4 L4. Ch5 shows every earlier look at once, and the warm reboot resolves them all to L4.
- **§4:** the look level comes from the chapter and the device tier from the startup self-test. The two are independent.
  - Both tiers show the same level in each chapter.
  - NVK only adds resolution, frame rate and anti-aliasing.
  - Every level must run on lavapipe.
- **Look levels:**
  - L1: 320×256, 4 core colours, flat shading;
  - L2: 640×256 (2× vertical), palette ≤ 16;
  - L3: 640×512, palette ≤ 24;
  - L4: true colour, per-pixel light, alpha blend, up to 1920×1080 on NVK.
- **§4.1 budgets:**
  - triangles: L1/L2 3,000; L3 6,280; L4 17,000;
  - frame time on every level: ≤ 40 ms on lavapipe, ≤ 20 ms on NVK;
  - shaders: a pipeline set per look level, never created inside a room. L1 keeps v2's "≤ 6 (1 VS, ≤ 5 FS)".
  - There is a separate L4 fallback order. It never cuts readability and never makes NVK required.
- **New §4.2:** cutscenes and movie clips.
  - In-engine cutscenes are the default.
  - Clips: each ≤ 60 s, all together ≤ 32 MB in 1.0.
- **Milestones:**
  - M3 adds the cutscene player and an L4 test room measured on lavapipe, which gates the start of Ch4 art;
  - M5 adds Ch4 at L4, the Ch5 composite and movie clips;
  - M6 must hit the budgets on every level.
- **§8.1, engine name:** the engine is **CopperForge**, an owner decision of 2026-10-31. It is game-agnostic so other projects can reuse it:
  - no Kitty names, assets, chapters, verbs or story in engine code;
  - the game layer depends on the engine, never the reverse;
  - Kitty data (look profiles, palettes, sequences, clips) reaches the engine as data;
  - the top-level namespace is `cf`, and the existing module prefixes stay;
  - where both share a repository, a lint gate stops engine sources from including game headers.
- **§12:** the art load is several times v2's, because every asset needs a mesh variant per level.
- **New open decisions:**
  - D8: L4 aspect ratio;
  - D9: codec for L4 movie clips.

## Unchanged

- Gameplay, rooms and puzzles are identical on both tiers.
- NVK is never required.
- The fixed camera and flip-screen rooms.
- The snapshot system (§5.3–§5.4).

## See Also

- [Game design v2](gdd-v2.md)
- [Art bible](art-bible.md)
- [Renderer](../engine/renderer.md)
- [Open owner questions](../process/open-questions.md)
