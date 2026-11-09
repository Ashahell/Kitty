# Art bible

> Sources: Kitty art bible v2, 2026-10-31; Kitty GDD v3, 2026-10-31; Kitty art bible v1, 2026-10-31; GDD v2, 2026-10-28; GDD v1, 2026-10-28
> Raw: [Art bible v2](../../raw/game-design/2026-10-31-art-bible-v2.md); [Art bible v1](../../raw/game-design/2026-10-31-art-bible-v1.md); [GDD v2](../../raw/game-design/2026-10-28-gdd-v2.md); [GDD v1](../../raw/game-design/2026-10-28-gdd-v1.md); [GDD v3](../../raw/game-design/2026-10-31-gdd-v3.md)
> Updated: 2026-10-31

## Overview

The art bible (`Docs/Development/kitty_art_bible.md`, v2) fixes the visual rules for every asset: models, rooms, UI, animation, cutscenes and movie clips. The owner is the human art director (GDD D5). Values marked "proposed" need sign-off.

## Owner decision: the look improves as the game goes on

Owner decision of 2026-10-31: graphics quality "should improve" as the player progresses (art bible v2 §0).

- It restores GDD v1 §8: "the engine visibly evolves with the story".
- GDD v3 (2026-10-31) now carries it. It overrides:
  - GDD v2 §4: one 320×256 look for the whole game, set by the CPU baseline, dithered rather than blended;
  - the shader cap in GDD v2 §4.1;
  - the engine rev 5 non-goal "per-core art divergence".
- It does not change:
  - identical gameplay on both device tiers;
  - NVK never being required;
  - the fixed camera.

> **Status: Outdated** (2026-10-31)
> Art bible v1 fixed one canvas for the whole game: 320×256, palette only, flat per face, dither and never blending. Art bible v2 replaces this with per-chapter look levels.

## Look levels (proposed)

| Level | Chapter | Baseline internal res | Colour | Shading | Transparency |
|---|---|---|---|---|---|
| L1 Dawn | Ch1 | 320×256 | palette, 4 core colours | flat per face | 4×4 Bayer dither |
| L2 Bevel | Ch2 | 640×256 (2× vertical) | palette ≤ 16 | flat, geometry bevels | dither |
| L3 Foundry | Ch3 | 640×512 | palette ≤ 24 | Gouraud dithered back to the palette | dither |
| L4 Modern | Ch4 | 640×512; NVK up to 1920×1080 | true colour (RGBA8, sRGB) | per-pixel: 1 key light + ≤ 4 point lights | alpha blend |

Also part of the look levels:

- **Ch5 regions:** each region of the screen draws in an earlier level, composited into an L4 frame.
- **The hub:** uses the highest completed level.
- **Device tiers:** both show the same level in each chapter. NVK only adds resolution, frame rate and anti-aliasing.
- **Triangle totals per frame (proposed):**
  - 3,000 on L1/L2;
  - 6,280 on L3;
  - 17,000 on L4.
- **L4 on lavapipe:** if it misses 40 ms, the cut order is:
  1. bloom and fog;
  2. point lights;
  3. resolution, down to 480×384;
  4. triangles.

## Rules that hold on every level

- **Camera:** fixed dimetric camera, flip-screen rooms, 5:4 puzzle safe area.
- **Simulation:** fixed at 50 Hz.
- **Light:** from the top-left. The top/left/right value relationship must still read in greyscale on L4.
- **Palette slot map (L1–L3):**
  - 0–23: chapter;
  - 24/25: Guru red/black;
  - 26–30: clone stripes;
  - 31: UI highlight.
  - On L4 the same eight system colours become fixed sRGB constants.
- **Assets:** one silhouette per asset, with a variant per level. The L4 silhouette must match the L1 one.
- **Kitty and Process 1:** they share one rig. The mesh is ≤ 120 tris on L1/L2 and ≤ 900 on L4.
- **Clones:** dithered on L1–L3; on L4, alpha-blended at 50 % with a stripe rim light.
- **Guru box:** identical on every level.
- **Characters:**
  - Deadlock is a ring of links;
  - Fred Fish is never depicted.

## Cutscenes and movie clips

The engine must support both:

- **In-engine cutscenes** (the default) play through the replay path at the chapter's level.
  - Each chapter opens with an in-engine transition that upgrades the level.
- **Movie clips:**
  - L1–L3 clips are palette-indexed and ANIM-style delta-compressed;
  - L4 clips are true colour in an open codec, still to be chosen (Q5).
  - Proposed limits: each clip ≤ 60 s, all clips together ≤ 32 MB.
  - A clip that shows a branded character needs a Process 1 variant.

> **Status: Outdated** (2026-10-31)
> The Gouraud dispute is resolved by level in art bible v2. Before that, GDD v2 §4 allowed "flat or Gouraud from vertex colors", while art bible v1 Q2 allowed flat shading only. Now L1–L2 are flat, L3 uses dithered index interpolation, and L4 uses real shading.

## Engine changes it asks for (rev 6)

- **E1:** a 32-entry palette uniform, for L1–L3 only. M0 uses 16.
- **E2:**
  - a sequence player and a clip decoder, covering both the indexed and the true-colour clip formats;
  - in-engine cutscenes by M3, clips by M5.
- **E3:** look profiles per chapter, loaded as data:
  - an indexed and a true-colour path;
  - render targets per level plus a composite pass;
  - pipeline sets per level, never created inside a room;
  - goldens per level.
  - **Gate:** an L4 test room is measured on lavapipe before Ch4 art starts.

## See Also

- [Game design v3](gdd-v3.md)
- [Game design v2](gdd-v2.md)
- [Renderer](../engine/renderer.md)
- [Open owner questions](../process/open-questions.md)
