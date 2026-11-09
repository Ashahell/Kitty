# Kitty: Core Awakening Art Bible v2 (rev a)

> Source: Work/Kitty/Docs/Development/kitty_art_bible.md
> Collected: 2026-10-31
> Published: 2026-10-31

# Kitty: Core Awakening — Art Bible v2 (2026-10-31)

*The visual rules every asset follows: models, rooms, UI, animation, cutscenes and movie clips.
Sources: GDD v3 (§3.2, §4, §4.1, §4.2) and GDD v2 (§2, §3.4, §6, §7, §12), GDD v1 §8 "Art & audio direction", engine
design rev 5 (§4.8, §4.13, §6.3, §6.6), and the owner decision of 2026-10-31 (§0). Owner: the human
art director (GDD D5, to be named before M3). Values marked **proposed** need art-director sign-off.*

---

## 0. Owner decision (2026-10-31) and what it overrides

> "the engine needs to support modern graphics as well. The graphics quality should improve when the
> user progresses in the game."

- **Graphics quality steps up chapter by chapter**, from period low resolution in Ch1 to modern
  rendering in Ch4. This restores GDD v1 §8: "68k Core flat-shaded retro → later cores fuller
  lighting — the engine visibly evolves with the story".
- **It overrides (GDD v3 of 2026-10-31 now carries the change):**
  - GDD v2 §4: the art direction set by the CPU baseline, 320×256 for the whole game, dither instead of
    blending;
  - GDD v2 §4.1: "≤ 6 (1 VS, ≤ 5 FS)" shaders and one triangle budget for the whole game;
  - engine rev 5 non-goals: "per-core art divergence", "blend-based transparency as default", ">6
    shaders".
- **It does not override:**
  - gameplay, rooms and puzzles are identical on both device tiers (lavapipe and NVK);
  - NVK is never required, so every look must run on lavapipe;
  - the fixed camera and flip-screen rooms.
- Engine work this needs: §11 E3.

## 1. Pillars

1. **The look evolves with the story.** Each chapter has a *look level* (§2): resolution, colour
   depth, shading, transparency and effects. The level only goes up as the player progresses. Each
   step is a story beat, felt on the chapter's first screen.
2. **Readable at a glance, at every level.** Each room fits one fixed view, so the whole puzzle is
   visible at once, clones included. Silhouette first, colour second, detail last. A higher level
   adds detail, never clutter.
3. **The OS is the world.** Rooms are Screens, alerts are Guru boxes, settings are `ENV:`. Every
   visual metaphor comes from real AROS/Amiga concepts, drawn in our own hand.
4. **Each era uses only what its era had.** L1–L3 are limited to their era's palettes. L4 is modern,
   but it stays inside the chapter's colour script.
5. **Original and swappable.** Every branded asset is a skin. Nothing is copied from another work.

## 2. Look levels

The look level comes from the **chapter**. The device tier is chosen at startup by the GDD v2 §4
self-test. The two are independent:

- both device tiers show the same look level in each chapter;
- NVK spends its headroom on resolution, frame rate and anti-aliasing, never on a different look.

### 2.1 Levels (proposed)

| | **L1 Dawn** | **L2 Bevel** | **L3 Foundry** | **L4 Modern** |
|---|---|---|---|---|
| Chapter | Ch1 (68k) | Ch2 (PPC) | Ch3 (x86) | Ch4 (ARM) |
| Era | Kickstart 1.3 | Workbench 2.x | MagicWB-style | Modern AROS (Zune/Wanderer) |
| Internal res, baseline (lavapipe) | 320×256 | 640×256, non-square pixels (2× vertical) | 640×512 | 640×512 |
| Internal res, enhanced (NVK) | 320×256 | 640×256 | 640×512 | window resolution, up to 1920×1080 |
| Upscale to window | integer, nearest | integer, nearest | integer, nearest | linear |
| Colour | palette, 4 core colours | palette, ≤ 16 | palette, ≤ 24 | true colour (RGBA8, sRGB) |
| Shading | flat, one index per face | flat, geometry bevels | Gouraud, ordered-dithered back to the palette | per-pixel: 1 key light + ≤ 4 point lights |
| Textures | none, or ≤ 4 per room, nearest | ≤ 4 per room, nearest | ≤ 8 per room, nearest | mipmapped, linear, ≤ 256×256 |
| Transparency | 4×4 Bayer dither | dither | dither | alpha blend |
| Shadows | none | baked contact darkening | baked | blob shadow per entity, baked room AO |
| Anti-aliasing | none | none | none | none on baseline; 4× MSAA on enhanced |
| Effects | none | none | heat shimmer as palette cycling | light pools, soft fog, optional bloom (enhanced only) |
| Frame rate | 25 fps baseline, 50 fps enhanced; simulation fixed at 50 Hz on every level | | | |

**Ch5 (SMP Heart)** is the composite: the screen is split into regions, and each region draws in an
earlier level.

- The full frame is output at L4 resolution.
- Each L1–L3 region is rendered at its own internal resolution and nearest-upscaled into place.
- The seams between regions are where the finale's visual drama happens.
- The final warm reboot resolves every region to L4.

**The hub** uses the level of the highest completed chapter. It upgrades in place each time a chapter
is finished.

### 2.2 Fixed across all levels

| Rule | Value | Source |
|---|---|---|
| Camera | fixed dimetric 2:1 isometric, flip-screen rooms, never player-controlled | GDD v2 §4 |
| Aspect | 5:4 puzzle safe area on every level. L4 may widen to 16:9 with decoration only (proposed, §11 Q4) | proposed |
| Depth order | painter's sort by grid key; never rely on the depth buffer for readability | eng §6.6 |
| Overdraw | ≤ 2×, back faces culled; two-sided only through `P_flat_nocull` | GDD v2 §4.1, eng §6.3 |
| Kitties on screen | 6 (player + 5 clones) | GDD v2 §4.1 |
| Light direction | top-left key light on every level; L4 makes it a real light, L1–L3 bake it into the index | §3.1 |

### 2.3 Triangle budgets per frame (proposed; L3 and L4 must be measured on lavapipe, §11 E3)

| Item | L1 / L2 | L3 | L4 |
|---|---|---|---|
| Static room (floor, walls, blocks, set dressing) | ≤ 1,800 | ≤ 3,600 | ≤ 9,000 |
| Kitty mesh × 6 | ≤ 120 each = 720 | ≤ 300 each = 1,800 | ≤ 900 each = 5,400 |
| Entities (gates, pedestals, Nybbles, props) | ≤ 400 | ≤ 800 | ≤ 2,400 |
| HUD / UI quads | ≤ 80 | ≤ 80 | ≤ 200 |
| **Total** | **3,000** (GDD v2 §4.1) | **6,280** | **17,000** |

If L4 misses the 40 ms baseline frame on lavapipe, cut in this order:

1. bloom and fog;
2. point lights down to 1;
3. internal resolution down to 480×384;
4. triangles down to the L3 budget.

Never cut readability, and never make NVK required.

## 3. Colour

### 3.1 Shading rule (all levels)

- **Light comes from the top-left**, the Workbench icon convention:
  - top faces take the lightest tone;
  - left faces the middle tone;
  - right faces the darkest.
- **L1–L2:** one palette index per face, with lighting baked by choosing the index. No runtime light.
- **L3:** vertex indices are interpolated along one ramp and ordered-dithered back to palette
  entries, so no off-palette colour is ever produced.
- **L4:** real lighting with authored normals. The baked top/left/right value relationship must
  still read in a greyscale screenshot.

### 3.2 Palette slot map, L1–L3 (proposed; needs §11 E1)

| Slots | Use |
|---|---|
| 0–23 | the chapter palette (era colours plus their shading ramps) |
| 24 | Guru red (alert border) |
| 25 | Guru black (alert field) |
| 26–30 | five clone-stripe colours, fixed for the whole game, so clone 1 is always the same colour |
| 31 | UI focus highlight |

- Slots 24–31 are "the System's own colours" and the only colours allowed outside an era.
- In L4 the same eight colours are fixed sRGB constants, so the Guru box and the clone stripes look
  identical in every chapter.

### 3.3 Chapter colour

All hex values are **proposed** and are our own values in the era's spirit.

| Ch | Level | Colours | Notes |
|---|---|---|---|
| 1 | L1 | 4: blue `#0055AA`, white `#FFFFFF`, black `#000022`, orange `#FF8800` | Blue fields, white edges, black shadow, orange accents. Shade with those 4 only |
| 2 | L2 | 4 core: grey `#AAAAAA`, black `#000000`, white `#FFFFFF`, blue `#6688BB`, plus ramps up to 16 | 3D bevels: white top-left edge, black bottom-right edge |
| 3 | L3 | 8 core: greys `#959595 #7B7B7B #AFAFAF`, black, white, blue `#3B67A2`, beige `#AA907C`, rose `#FFA997`, plus ramps up to 24 | Industrial. Palette in the style only; no MagicWB icons copied |
| 4 | L4 | true colour inside a **colour script**: greens, soft greys, warm lamp light, a sky gradient | Real gradients and soft light. The colour script is a hue/value range per room, checked at build time |
| 5 | composite | each region uses its era's palette or colour script | The seams use the system slots only |

## 4. Shape language and modelling

- **Faceted and clean on every level.**
  - Big planar faces with a clear top, left and right.
  - L1–L3 bevels are geometry (one chamfer), not textures.
  - L4 may add rounding, normal maps and surface detail, but the silhouette must still match the
    L1 version of the same object.
- **One asset, several level variants.** Each gameplay object has a variant per level it appears
  in, built from one silhouette. A pedestal seen in Ch1 and in Ch4 is recognisably the same
  pedestal.
- **Grid.** One room cell is one world unit. Props sit on the grid, gameplay objects fill whole
  cells, and nothing important straddles a cell edge.
- **Minimum readable size:** every gameplay object is at least 8 px in both dimensions at 320×256
  equivalent, and identifiable as a one-colour silhouette, on every level.
- **Textures:** for signage, disk labels and screens on L1–L3, never for surface noise. Surface
  detail is allowed on L4 only.
- **Two-sided geometry** only for thin props such as banners and leaves (`P_flat_nocull`).

### 4.1 Chapter environments

| Ch | Motif | Shape language | Materials |
|---|---|---|---|
| 1 | The Ancestor: one Core, many tasks | blocky, orthogonal, chunky platforms that tick in turn | flat enamel, blue and white |
| 2 | The Mirror: two living worlds | mirrored layouts, bevelled panels, window frames | brushed grey plates, blue glass |
| 3 | The Foundry | heavy machines, pipes, gantries, coolant channels; scale ×2 | grey metal, warm beige rust, rose heat glow (palette cycling) |
| 4 | The Garden: low power | sparse, rounded, open sky; every clone costs light, so the light pools *are* the energy budget | foliage greens, soft greys, warm lamps, real soft light |
| 5 | The SMP Heart | regions in each era's level, meeting at seams | every chapter's colour, each in its own region |
| Hub | Workbench neighbourhood | small houses for the modules, arcade cabinets, Shell kiosk | the highest completed level; it fills up and upgrades as chapters complete |

## 5. Characters

- **Kitty (licensed skin) and Process 1 (placeholder generic cat) share one rig.** Same
  proportions, silhouette, joint layout and animations. Every animation is authored once and
  plays on both skins and every level. The game must look finished with Process 1 (GDD §2).
- **Mesh per level** (proposed): L1/L2 ≤ 120 tris, L3 ≤ 300, L4 ≤ 900 with normals. The same rig and
  keys drive all of them.
- **Kitty proportions** (proposed):
  - about 1 cell long and 0.75 cell tall; the head is a third of the height;
  - big ears and a long tail carry the expression;
  - the face is 3–5 polys on L1–L3; L4 may add eyes and fur cards in the same silhouette.
- **The scarf** is the identity carrier:
  - it holds the clone stripe (system slots 26–30);
  - it shows the scarf stripes earned from collected disks.
- **Clones** use the same mesh plus their stripe colour. A clone must never be confusable with
  Kitty on any level:
  - L1–L3: drawn with `P_dither`;
  - L4: alpha-blended at 50 % with a stripe-coloured rim light; the stripe always shows.
- **Modules** (Exec, Intuition, Graphics, DOS, Layers) are characters of architectural scale,
  each read through one prop:
  - Exec: a clock tower;
  - Intuition: windows / an eye motif with no eyes;
  - Graphics: a painter's easel and blitter;
  - DOS: shelves of disks;
  - Layers: overlapping panes.
  - Each one is frozen until freed: dithered and stopped on L1–L3, desaturated and stopped on L4.
- **Nybbles:** tiny, about 0.4 cell, always in pairs (two make a byte), simple 4-pose loops.
- **Deadlock:** a shape, not a monster. A closed ring of links, one per held resource, that
  tightens. No teeth, no face beyond the cycle itself.
- **The Archivist:** visualised through `.readme` panels, not as a face.
- **The User:** never shown. Only the sky flickers.
- **Fred Fish:** never depicted (GDD D2). The disks, the credits dedication and the empty chair on
  the dock are enough.

## 6. Animation

- Keyframed vertex animation, blended two poses at a time by `VS_iso`; the weight comes from the
  tick in integer arithmetic. Identical keys on every level.
- The tail and scarf are **authored keys, not physics**, so they look identical on every run
  (eng §4.8).
- **Timing is in 50 Hz ticks.** One cell of walking takes 8 ticks (`TICKS_PER_CELL = 8`).
  Anticipation takes 2–4 ticks, and squash-and-stretch goes up to ±20 % (proposed).
- **Snappy over smooth.** Hold poses, then move. At 25 fps the strong poses carry the motion.
- **Ch1 time-slicing must be visible.** Tasks move in turn, one at a time.
- **Frozen things stop dead.** Never slow-motion.

## 7. UI and screens

- **Every room is a Screen**, with a title bar showing the Screen name and a depth gadget in the
  corner that returns to the hub (GDD §4).
- **Gadget style follows the level:**
  - L1: flat;
  - L2: bevelled;
  - L3: MagicWB-style;
  - L4: Zune-style, anti-aliased;
  - hub: the highest completed level.
- **Font:**
  - an original 8×8 bitmap font in the spirit of the Amiga system font on L1–L3 (Topaz glyphs are
    not copied);
  - an original high-resolution version of the same design on L4.
- **Text:** locale catalogs only (EN and DE in 1.0). Design for 1.5× the English length.
- **Guru / Cat Meditation box:** the same on every level, which makes it the one constant.
  - slot 24 red border on a slot 25 black field, with the border blinking at 1 Hz (50 ticks per
    state);
  - text in the system font, e.g. "Software Failure. Press left mouse button to continue.";
  - used for deaths, Guru Alerts and validation errors.
- **HUD:** at most one line plus verb icons. Everything goes through `P_ui`.

## 8. Cutscenes and movie clips (engine requirement)

The engine must play both:

| Type | Use for | How |
|---|---|---|
| **In-engine cutscene** (default) | story beats, module freed, chapter intros, level upgrades, the finale | A scripted sequence played through the replay/input-log path and the normal renderer, at the chapter's look level. Deterministic, the same on both device tiers, re-skins automatically, almost no disk cost |
| **Movie clip** | what rooms can't show: boot intro, warm-reboot flash, credits | Pre-rendered frames decoded on the CPU and shown as one full-screen quad. No new shader |

### 8.1 Rules for both

- Skippable with fire or Esc. Never the only place where gameplay information appears.
- Subtitles come from the locale catalogs and are drawn in the system font of the current level.
- A cutscene or clip uses the look level of the chapter it belongs to. Letterboxing is done only by
  framing (no bars beyond the Screen).
- **Level-upgrade cutscenes:** each chapter opens with an in-engine transition from the previous
  level to the new one, rendered in the engine (e.g. the Ch3 → Ch4 sky resolving from dithered
  palette bands to a real gradient).

### 8.2 Movie-clip formats (proposed)

- **L1–L3 clips:**
  - palette-indexed, using the chapter palette plus the system slots;
  - delta-compressed frames, Amiga ANIM-style (only changed spans per frame).
- **L4 clips:** true colour in an open, royalty-free codec decoded on the CPU (choice open, §11
  Q5). Prefer in-engine cutscenes on L4, because the renderer can already show the modern look.
- Sound is a MOD or samples, synced by tick.
- Limits:
  - each clip ≤ 60 s;
  - all clips together ≤ 32 MB in 1.0;
  - decoding stays inside the 40 ms baseline frame time on lavapipe.

### 8.3 Skins

- In-engine cutscenes re-skin for free.
- A movie clip may show Kitty or AROS branding only if a Process 1 variant of the same clip
  ships too. Otherwise the clip shows no branded character, or the beat is done in-engine.

## 9. Licensing guard-rails (from GDD §2)

- No ReBoot elements at all.
- No other work's names, catchphrases, logos or coined terms.
- Homage is through genre and mechanics only.
- The Boing! disk uses our own checkered sphere model. Disk numbers start at FF #1121.
- No falling-tetromino imagery.
- Every branded asset lives in a skin package. Assets stay gated until the skin licence clears.
- Post-launch community art needs a licence field: CC-BY-4.0, CC-BY-SA-4.0 or GPL-3.0-or-later.

## 10. Pipeline and checks

- **Build-time checks:**
  - L1–L3: `kitty-pal` rejects off-palette vertex colours and textures;
  - L4: the colour-script check rejects hues and values outside the room's script;
  - the validator checks the §2.3 triangle budgets and the texture count per asset, per room and
    per level.
- **Model sources:** one folder per asset, with one mesh per level, its palette-index map (L1–L3)
  or materials (L4), and its keys. Skin assets go under `skins/<skin>/`, with Process 1 as the
  default.
- **Review gate for every asset:**
  - silhouette readable at 1× on 320×256 equivalent;
  - the L4 variant's silhouette matches the L1 variant;
  - correct era palette or colour script;
  - top-left light;
  - within budget;
  - Process 1 variant present.
- **Room review:** a 1× screenshot of the full room next to the solver's PAR path, on the room's
  level and on L1. The puzzle must be readable on both without moving the camera.

## 11. Open items

| ID | Item | Proposal |
|---|---|---|
| E1 | The palette uniform is 16 entries (M0a prompt W6; M0 code `UboData.pal[16]`); the design leaves N open. L3, Ch5 and the system slots need 32 | Engine rev 6: 32-entry palette for L1–L3 (§3.2); L4 does not use it |
| E2 | Cutscene playback and movie clips are not in engine design rev 5 | Engine rev 6: a sequence player (on the replay path) and a clip decoder (indexed ANIM-style for L1–L3, true colour for L4); in-engine cutscenes by M3 (vertical slice), clips by M5 |
| E3 | Look levels L1–L4 (§2) are not in engine design rev 5, which lists "per-core art divergence" as a non-goal and caps shaders at 6 | Engine rev 6: a look profile per chapter, loaded as data (internal resolution, pixel aspect, colour mode, shading, transparency, texture filter, light count, AA); an indexed and a true-colour path; one render target per level plus an upscale/composite pass (Ch5 regions); pipeline sets per level, created at startup or chapter load, never inside a room; the look is render-only, so state hashes stay identical; goldens per level. **Gate:** an L4 test room is measured on lavapipe at the D4 reference before Ch4 art starts (proposed: at M3, next to the Ch1 slice) |
| Q1 | Exact era hex values, the Ch4 colour script and the per-level budgets | Art director signs off at M3 |
| Q2 | GDD v2 §4 allows Gouraud shading, but palette-index vertex colours make interpolated colours off-palette | Resolved by level: flat on L1–L2, dithered index interpolation on L3, real shading on L4 |
| Q3 | Who makes the art (GDD D5) | Must be named before M3 |
| Q4 | L4 aspect: keep 5:4, or widen to 16:9 with decorative margins | Keep the 5:4 puzzle area; margins only on L4 |
| Q5 | The true-colour clip codec for L4 | Owner picks an open, royalty-free codec with a CPU decoder that builds for AROS |
| Q6 | GDD v3: rewrite GDD v2 §4 and §4.1 to match §0 | **Done 2026-10-31:** `kitty_core_awakening_design_v3.md`; Q4 and Q5 are GDD v3 D8 and D9 |
