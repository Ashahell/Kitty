# Engine design document, revision 5 (with 2026-10-30 patches)

> Source: Work/Kitty/Docs/Development/kitty_engine_design.md
> Collected: 2026-10-30
> Published: 2026-10-30

# Kitty: Core Awakening — Engine Design Document (revision 5)

*Design for the engine that serves `kitty_core_awakening_design_v2.md` [S1]. This document decides architecture; a later session implements milestones M0–M1 from it without making architecture decisions of its own. Rationale for game design lives in [S1]/[S2]; this doc designs the machine under them. Citations `[S#]` resolve in `kitty_engine_design_sources.md`. Claims that could not be verified are marked **UNVERIFIED:** with the M0 probe that settles them. Every decision this revision fixed is in the body section that owns it; Appendix R is a one-row-per-finding index to those sections, not a place where a decision lives.*

---

## 1. Scope and non-goals

**In scope (1.0 only):** the subsystems v2 requires — a deterministic 50 Hz fixed-point simulation with snapshot ring + input log; isometric flip-screen rendering through the AROS Vulkan loader chain on two tiers (lavapipe baseline / NVK enhanced); ProTracker MOD audio with per-channel mute; joystick-first input with pie menu; text-first room format with compiler, validator and puzzle solver; Guru-style line-numbered error reporting; ENVARC settings, Shell fast-travel, visible saves; EN+DE catalogs; replay/demo mode; benchmark mode.

v2 §8.1 targets at most 20k lines; on 2026-10-29 the owner raised the engine cap to 23k so that `shell/`, `replay/` and `benchmark/` stay in 1.0. [S1 §8.1]

**Non-goals — explicitly not in the engine:**
- Networked anything (v2 cuts network visits/voting/leaderboards; AROS networking is known-fragile — no game feature may open a socket).
- Native-code contributions and any interpreter (v2 §9.2: data-only packs; AROS has no memory protection [S2]).
- The playground/Sims layer, BOOPSI chapter, extra arcade disks (v2 post-launch items 1.1–1.4; their entry criteria are design-doc changes, not engine hooks. The engine ships the level format and validator so 1.1 is additive only.)
- A free camera, blend-based transparency as default, >6 shaders, per-core art divergence (v2 §4 style rules).
- A Godot/Bevy/Fyrox port (§14 with [S1 §8.1], [S2]).
- The Vulkan4Aros *interpreter ICD* as a target: slowest path, ~160–197 ms/frame measured [S17]; v2 names lavapipe/NVK [S1 §1].
- Linux-only conveniences that would fork behaviour: the Linux build is the same codebase through `platform/linux` ([S1 §8]; shipping it publicly is v2 D1).

## 2. Constraints table

Every binding constraint, with its source. C29 is the allocation rule, C30 the fixed-timestep/no-interpolation rule; both are enforced in §4, §9 and `kitty-lint`.

| # | Constraint | Source |
|---|---|---|
| C01 | Platform lead AROS x86-64; baseline tier Mesa **lavapipe**, enhanced **NVK** on Turing+; interpreter ICD not a target | [S1 §1, §8.2] |
| C02 | Engine is small custom C++20 subset; port nothing | [S1 §8.1], [S2 F1/F2] |
| C03 | Flags: `-fno-exceptions -fno-rtti`; no `std::thread`, `<iostream>`, `<filesystem>`, `<regex>`; no global non-trivial-ctor objects | [S1 §8.1]; startup facts [S40][S41] |
| C04 | Appendix A AROS flag set is mandatory for every AROS artifact | [S1 Appendix A], [S60] |
| C05 | Cross-compile against the same SDK tree as the target image (ABI-patched v1 SDK, never stale build-pc/core-linux SDKs); gate `mov %rax,%r12` per Appendix A | [S1 Appendix A], [S53]; nuance for libcall glue binaries [S46] |
| C06 | Sim: Q16.16/integer only, no libm in `sim/`; floats only in `render/`+`audio/`; `-ffp-contract=off` both platforms, no `-ffast-math` | [S1 §8.1, Appendix A], [S2] |
| C07 | All OS access via `platform/` (one interface, two impls) | [S1 §8.1] |
| C08 | AROS window owned by `vkCreateArosSurfaceKHR` in `vulkan_loader.library`; SDL2 must not open a second window on AROS | [S1 §8.2], [S10][S11][S31]; rejection of SDL2-on-AROS windowing [S2], [S12] blit facts |
| C09 | Per frame: acquire → record → submit → present, no exceptions; silent ordering failure mode with no error return on the AROS ICD paths | [S1 §8.3], [S48 2026-07-08] |
| C10 | Zero-init every Vulkan create-info; always supply optional state structs explicitly (permissive-defaults rule on young AROS paths) | [S1 §8.3], [S60] |
| C11 | Handle arrays 8-byte stride; NDC z∈[0,1] | [S1 §8.3] |
| C12 | Open the Vulkan loader/library once at startup, keep resident until exit; never call/trigger `Avail FLUSH`; reboot (not reload) after any library rebuild | [S1 §8.3], [S13][S14], [S4] (newer rule), [S42]–[S44] |
| C13 | Frame pacing via `timer.device`; **never `WaitTOF()`** (hangs forever headless QEMU) | [S1 §8.2, Appendix A], [S29], [S48 2026-07-07] |
| C14 | On pc-x86-64 AROS there is no clock API (`Clock_Get`/`GetSystemTime`/`ReadEClock` absent; EClock not an AROS API); the only time units are `Delay()` ticks (20 ms) and `timer.device` `TR_ADDREQUEST` wakeups (`UNIT_MICROHZ`) | [S29], log 2026-10-17/19 |
| C15 | Audio AROS path = AHI device API (`CMD_WRITE`, full `struct AHIRequest`, `ahir_Link` chain field, filled and `SendIO`'d from the main loop; queue depth and service cadence in §4.6) | [S1 §8.2], [S33], [S80], [S81] |
| C16 | MOD replayer is the engine's own (~1.5k lines), deterministic, per-channel mute | [S1 §8.2], [S2 tech-errors note] |
| C17 | Dear ImGui in Linux debug builds only; never compiled for AROS | [S1 §8.2] |
| C18 | Golden ↔ host CI must be bit-exact; guest ↔ golden is a proxy check whose tolerance M0 sets, and the guest's LLVM version need not match the host's. Goldens are built from the **same Mesa commit + `LP_*` env + `AROS_PIN` hash** as the AROS build; the on-AROS state hash must equal the Linux hash | [S1 §8.4]; proxy validity [S50], [S56], [S85], [S25], [S47] |
| C19 | Pin the Mesa version per release; AROS build and CI build from the same commit | [S1 §8.3]; pin mechanics [S47][S49], upstream drift fact [S6] |
| C20 | NVK enhanced tier may never be required: auto-select via startup self-test, else lavapipe; `ENV:Kitty/Renderer` override | [S1 §4], D7 [S1 §13] |
| C21 | ≤ 6 shaders whole game (1 VS, ≤ 5 FS); back-face culling explicit per pipeline; ordered-dither transparency default | [S1 §4.1, §8.3] |
| C22 | Solver verifies every room solvable within par and no cheese/verb-shortcut | [S1 §8.4 item 3, §11] |
| C23 | CI gate discipline: "a check that cannot fail is not a check"; every gate names a known-bad input; print return codes; zero counters prove nothing; BLOCKED is a distinct state, never counted as PASS | [S1 §8.4], [S19], [S20] |
| C24 | No `Avail FLUSH` in any dev loop; deploy fix = reboot | [S4] 2026-10-26, [S42]–[S44] (supersedes pre-June "flush between tests" records) |
| C25 | Never strip an AROS binary beyond `--strip-debug`; large-module ET_REL GOT64 gap → `-fno-pic -fno-pie` on app binaries | [S13 addendum 2026-09-22] |
| C26 | Community content (1.1): manifest schema-validated, license field mandatory (CC-BY-4.0 / CC-BY-SA-4.0 / GPL-3.0-or-later); failures print Guru with file+line | [S1 §9.2] |
| C27 | Determinism proof = same input log → identical state hash on Linux and AROS | [S1 §8.4 item 7, §5.4] |
| C28 | `Forbid()`-themed verb is gameplay fiction, not the real AROS `Forbid()` call; nothing in the engine may call the kernel Forbid/Permit | derived from [S1 §5.1] fiction mapping — engine keeps its own task-scheduling fiction in `sim/` |
| C29 | **Allocation:** no STL containers and no exceptions in engine code; fixed arrays + per-room arenas; `operator new`/`delete` are traps only — §9 defines them and the link-time check | dispatch rev 2 #3 |
| C30 | **No interpolation:** render draws the latest fully-stepped `World`; at 25 fps the sim steps 2 ticks per frame; max catch-up cap N ticks then drop wall-time debt (logged) | dispatch rev 2 #14 |

## 3. Architecture overview

```
                 ┌────────────────────────────────────────────────┐
                 │ game/ (chapter content, rooms, story glue)     │
                 └───────┬──────────────────────────┬─────────────┘
                         │                          │
        ┌────────────────▼───────┐      ┌───────────▼─────────────┐
        │ sim/ (Q16.16, 50 Hz)   │◄────►│ render/ (Vulkan, floats)│
        │ snapshot/ (ring+hash)  │      │ audio/ ── modplay       │
        └───────▲────────────────┘      └───────────▲─────────────┘
                │ inputs (ticks)                    │ draw/audio packets
        ┌───────┴────────┐      ┌───────────────────┴─────────────┐
        │ input/ (verbs,  │      │ platform/{aros,linux}           │
        │         pie)    │      │ window, WSI surface, pacing,    │
        └─────────────────┘      │ files, AHI|SDL2, IDCMP|POSIX    │
                                 └─────────────────────────────────┘
        level/ → (compiled rooms) → sim+, render+   ui/ ← render pass-through
        locale/ (strings)   save/ (files)   assets/ (packages)
        settings/ (ENV/ENVARC)  shell/ (fast-travel)  guru/ (error reporter)
        replay/ (demo files)  benchmark/ (fixed replay + camera)
        tools/ (host only: kitty-cc, kitty-solve, kitty-validate,
                              kitty-pal, kitty-hash, kitty-shaders, kitty-lint)
```

Dependency direction is downward only; `platform/` is the single seam for OS differences ([S1 §8.1]). `game/` is thin data glue in 1.0. `sim/` never sees floats, never sees a Vulkan type; `render/` never sees sim mutable state outside the snapshot serialization + per-frame render state (positions are read-only at frame time). `tools/` reuse `sim/`+`level/` libraries directly — this is how the puzzle solver [S1 §8.4 #3] and state-hash checker [C27] get their proofs. `guru/` is one module shared by the runtime (Guru-style error screen) and the tools' output format [rev 2 #23].

**Threading model — one-core-correct.** Engine code is single-threaded end to end; no engine thread, no `std::thread`, no atomics requirement in our code. The only threads that may exist are lavapipe's internal rasterizer workers inside the ICD, controlled by `LP_NUM_THREADS` [S64][S25 semantics: `=1` still one worker, `=0` none]. Because AROS SMP is experimental and upstream-owned [S22][S48 smp census 2026-10-15/19], the engine default pins `LP_NUM_THREADS=0` (render in the caller's thread) subject to an M0 probe of `0` vs `1` [rev 2 #20]; enabling more is a *render-only* bonus that must not change state (proven by hash equality [C27]). v2 §4.1 fallback ladder ends at "lavapipe thread count, SMP" — never free camera.

**Tick/frame loop:**
1. `input/` collects channel state for this tick (from `platform/` events);
2. `sim/` advances exactly one 20 ms tick from the input log line (never from wall-time);
3. `snapshot/` appends input, updates keyframe ring + rolling hash every keyframe;
4. `render/` draws the latest fully-stepped state (no interpolation; C30);
5. `audio/` services the AHI/SDL stream; `ui/` draws overlays through the same renderer.

This list is the frame loop; the per-tick order is §5.1's, starting with Step 0.

The accumulator that decides *how many* ticks to step is the only place wall-time touches the machine; on AROS it is driven by `timer.device` `UNIT_MICROHZ` `TR_ADDREQUEST` requests [S29/S32], on Linux by monotonic clock. Sim output never depends on it [C27]. Max catch-up: cap N ticks per frame (default 4), then drop wall-time debt and log it.

### 3.1 Type definitions (written once, before any module spec)

All interface types are defined here. No module redefines them. Shared header `platform.h`:

```cpp
// platform.h — one interface, two implementations (platform/aros, platform/linux)
using u8 = uint8_t; using u16 = uint16_t; using u32 = uint32_t; using u64 = uint64_t;
using i16 = int16_t;
using Alloc = void*(*)(size_t);     // AllocVec-backed on AROS, malloc-backed on Linux
struct Buf;                         // file read buffer; defined in §3.2
struct Plat;                        // opaque, initialized explicitly in main
struct PlatEvent { u8 kind; u8 a; u16 b; u32 tick; };   // normalized platform event
struct EventSink { PlatEvent ev[64]; u32 head, tail; };  // fixed ring, drained per tick
struct PlatEvents { EventSink* sink; };                  // alias used by input/
struct PlatArgs { const char* renderer_override; const char* save_root; };
enum ErrCode : u16 { OK=0, E_IO, E_VK, E_SCHEMA, E_SIM_OVF, E_SIM_DIV0,
                     E_SNAP_UNDERFLOW, E_AUDIO, E_TIER_FALLBACK };
// platform/ interface: one declaration here, two implementations
void pl_init(Plat&, const PlatArgs&);
void pl_step(Plat&, u64& due_us);          // timer.device / monotonic
u32  pl_poll_events(Plat&, EventSink*);    // drains IDCMP / SDL events
bool pl_read_file(Plat&, const char* path, Buf* out);
bool pl_write_file(Plat&, const char* path, const void* data, size_t n);
void pl_env_get(Plat&, const char* key, char* v, size_t n); // ENV:/ENVARC:
void pl_log(Plat&, const char* line);      // T:Kitty.log / host stderr
void pl_shutdown(Plat&, int code);         // keeps libs resident [C12]
```

`sim_types.h`:

```cpp
using Fixed = int32_t;               // Q16.16
using TickId = u32;
using Arg = u8;
using ByteBuf = Buf;                 // alias of the §3.2 buffer type
struct World;                        // POD arena, see §5.5 field list
struct InputLine { u8 dir; u8 fire; };   // 2 B/tick; wire layout in §5.1
struct TickRecord { u32 tick; u8 action; u8 arg; };   // sparse event log (shareable)
struct Event { u32 tick; u8 kind; u8 a; u16 b; };     // logged mutation (R5)
struct HashState { u64 fnv; };                       // rolling FNV-1a state
struct ForkRec { u8 input[500][2]; };  // inputs only: length, cursor and spawn position live in World (R8)
struct SnapRing { u32 keyframes; TickId tick_of[19]; };   // 19 keyframes, see §5.3
enum VerbId : u8 { V_NONE=0, V_JUMP, V_FORBID_HOLD, V_WINDOW_GRAB, V_WINDOW_DEPTH,
                   V_FORK_COMMIT, V_JOIN_WAIT, V_MUTEX_CLAIM, V_SEMA_PASS,
                   V_REQUESTER_RETRY, V_REQUESTER_CANCEL, V_REWIND };
// sim/ interface
void tick(World&, const InputLine&);
u16  verb_commit(World&, VerbId, Arg);   // returns an ErrCode value
void hash_state(const World&, HashState*);
void serialize(const World&, ByteBuf*, size_t*);
// snapshot/ interface
void ring_push(SnapRing&, const TickRecord&);
void keyframe(SnapRing&, const World&);
bool restore_to_tick(SnapRing&, TickId, World&);
void fork_open(ForkRec&, const World&, TickId);
void fork_read(ForkRec&, TickId, InputLine&);
```

`render_types.h`:

```cpp
struct RendCtx;                      // owns all Vulkan objects, destroyed once
struct RenderState { u32 tick; };    // positions read-only at frame time
struct DrawList { u32 items; };      // up to ~240 items
struct UIRects { u32 quads; };       // overlay quads
struct TierPick { bool nvk; const char* reason; };
enum Tier { TIER_LAVAPIPE, TIER_NVK };
struct GoldenImg { u8 rgba[64*64*4]; };              // 64x64 self-test reference
struct MatchResult { bool ok; u32 mismatched_px; };  // tier self-test result
// render/ interface: one declaration here
RendCtx* r_init(Alloc, Tier, TierPick*);
void r_begin(RendCtx&, const RenderState*, u32 frame_index);
void r_draw(RendCtx&, const DrawList&, size_t n);
void r_overlay(RendCtx&, const UIRects&, size_t n);
VkResult r_present(RendCtx&);        // acquire→record→submit→present [C09]
void r_tier_probe(Alloc, GoldenImg*, MatchResult*); // §6.4
```

`Alloc` is a function pointer `void*(*)(size_t)`; on AROS it is `AllocVec`-backed, on Linux `malloc`-backed. No other allocator is used in engine code [C29].

### 3.2 Module interface declarations (every function §4 points to, declared exactly once)

```cpp
struct Buf { u8* data; size_t n; size_t cap; }; // file read buffer
// audio/ (§4.6)
struct Audio;
void aud_open(Audio&, u32 rate, u16 channels);
void aud_fill(Audio&, i16* buf, size_t frames);
void mod_load(Audio&, const u8* mod, size_t n);
void mod_mute(Audio&, u8 channel, bool muted);
void mod_seek(Audio&, u32 position);
// input/ (§4.7)
struct Input; struct PieResult { u8 choice; };
void in_poll(Input&, PlatEvents*);
bool in_next(Input&, InputLine* out);
u32  in_pie(Input&, PieResult* out);
// assets/ (§4.8)
struct Assets; struct AsMesh { u32 vertices; };
bool as_load(Assets&, const char* path);
const AsMesh* as_get(Assets&, u16 mesh_id);
void as_skin_swap(Assets&, const char* path);
// level/ (§4.9)
struct RoomBin; struct InputLog; struct Levels;
void lv_load_room(Levels&, const RoomBin*, size_t n);
void lv_replay(Levels&, u16 room_id, const InputLog*);
// ui/ (§4.10)
struct UiSpec; struct UICtx;
void ui_draw(UICtx&, const UiSpec*);
// locale/ (§4.11)
struct Loc; struct CharBuf { char text[128]; };
void lc_get(Loc&, u32 string_id, CharBuf* out);
// save/ (§4.12)
struct Save; struct Progress; struct SnapSeed;
bool sv_save(Save&, u16 reason);
bool sv_load(Save&, Progress*, SnapSeed*);
// game/ (§4.14)
struct Game;
void game_init(Game&, const Progress*);
void game_tick(Game&, World&, const InputLine&);
bool game_room_complete(const Game&, u16 room_id);
// settings/ (§4.15)
struct Settings;
void settings_load(Settings&, Plat&);
void settings_set(Settings&, Plat&, const char* key, const char* value);
// shell/ (§4.16)
struct Shell;
void shell_exec(Shell&, World&, const char* line);
// guru/ (§4.17)
struct GuruCtx;
void guru_report(GuruCtx&, const char* file, u32 line, const char* msg);
void guru_render(GuruCtx&, RendCtx&);
// replay/ (§4.18)
struct Replay;
void replay_play(Replay&, const InputLog*, World&);
void replay_record(Replay&, const InputLog*);
// benchmark/ (§4.19)
struct Bench;
void bench_run(Bench&, World&, const InputLog*);
```

## 4. Module specs

Fields per module: **R** responsibility · **I** public interface · **D** data/ownership · **T** threading · **E** errors (no exceptions [C03]) · **A** AROS-vs-Linux difference · **Test** strategy · **LB** line budget.

### 4.1 `platform/aros` — LB 1.5k
- **R:** Own the process on AROS: startup entry, window/surface lifecycle via `vulkan_loader.library` boundary (`vkCreateArosSurfaceKHR` opens the Intuition window; the app never opens one itself [C08][S10][S11][S31]), event drain (`vkPollArosEventsKHR` [S31][S30]), pacing, files (DOS SDK), ENV reading, process-exit rules (library left resident; no unload [C12]).
- **I:** §3.1 `platform.h`: `pl_init`, `pl_step`, `pl_poll_events`, `pl_read_file`, `pl_write_file`, `pl_env_get`, `pl_log`, `pl_shutdown` (one `Plat` interface, two implementations).
- **D:** Window/surface/swapchain objects are non-moved opaque handles owned by the loader/ICD per the ownership table (Surface: loader; Swapchain: ICD [S10]); engine holds only IDs + a 16-slot window registry view [S11]. All buffers `AllocVec`/`FreeVec` (no libc malloc in ROMTag-adjacent code [S52][S60]).
- **T:** Single-threaded; `timer.device` request is the heartbeat.
- **E:** Returns `ErrCode u16` from SDK+Vulkan results; a Vulkan result ≠ `VK_SUCCESS` is logged immediately with context (probe discipline: print rc always [S19]). A failed startup path raises the Guru-style error screen with the failing step name — never a silent fallback without a log line [C20].
- **A:** This *is* the AROS side. Contrasts to §4.2 are the difference itself; both implement the same `Plat` interface.
- **Test:** `t-aros-boot`: boots in QEMU lane, asserts window opened, 60 paced ticks, events drained, exit rc 0 with libs resident. Known-bad: forced FIFO present mode → test must *not* hang (proves no `WaitTOF` [C13]); plus a `WaitTOF` source lint and an LVO/import scan (rev 2 #21).

### 4.2 `platform/linux` — LB 1.2k
- **R:** Mirror interface: window via SDL2 (SDL2 *only* here [C08][S1 §8.2]), monotonic clock, POSIX files, SIGTERM clean exit; hosts CI hooks (validation layers, golden-image dumps).
- **I:** the same §3.1 `platform.h` declarations as §4.1: `pl_init`, `pl_step`, `pl_poll_events`, `pl_read_file`, `pl_write_file`, `pl_env_get`, `pl_log`, `pl_shutdown`.
- **D:** SDL handles internal; engine-owned buffers use fixed arrays or per-room arenas [C29]; no STL containers anywhere in engine code, `platform/` and `platform/linux/` included (R7); `tools/` are separate host binaries and may use STL.
- **T:** Single-threaded; monotonic clock drives the tick accumulator.
- **E:** As §4.1.
- **A:** Pacing source differs only; window resize semantics map to the same OUT_OF_DATE contract [S30].
- **Test:** Host for all unit tests; `t-plat-parity` compiles a shared behavior test against both impls where the AROS side runs in the spike agent.

### 4.3 `sim/` — LB 3.5k
- **R:** The entire game logic: entities, platforms/tasks, pads/pedestals/gates, windows depth state, requesters, FORBID starvation, clone replay, verb resolution, win/lose. Runs at fixed 50 Hz; sees only integer/Q16.16.
- **I:** §3.1 `sim_types.h`: `tick`, `verb_commit`, `hash_state`, `serialize`.
- **D:** Fixed-capacity arrays: ≤ 64 entities × 96 B, ≤ 16 clones, ≤ 8 windows, ≤ 8 requesters — no heap after init, whole `World` is one POD arena so snapshot = `memcpy` (this is what makes rewind/clone/fork one system [S1 §5.4]). No virtual dispatch in hot data (tagged union per entity). RNG: per-room `xorshift64*` (exact constants in §5.6) seeded from room `SEED` via splitmix64.
- **T:** Single-threaded; deterministic given identical input order.
- **E:** No error path at runtime for verb resolution — every resolution returns a result code recorded into the tick result, not an abort. Illegal combos are impossible by construction (interaction matrix §5.2) or visible state ("zombie" clone [S1 §5.3]). Arithmetic errors: `SIM_OVF`/`SIM_DIV0` result codes (§5.1.OV).
- **A:** Identical binary source on both platforms; the *only* AROS-vs-Linux sim exposure is the compiler flags, pinned identical by §9.
- **Test:** Golden tick tables: hand-authored worlds × hand-authored input logs → hand-authored hash, checked into `tests/sim/`. The Q16.16 contract (§5.1) unit-tests rounding on negatives and overflow pins (§5.1.OV). Property: `tick` never reads platform headers — enforced by `kitty-lint` include scan.

### 4.4 `snapshot/` — LB 1.2k
- **R:** One system, six features (rewind, FORK record, replay, shareable file, save, CI hash) [S1 §5.4]. Ring of keyframes + per-tick input log; rollback *replay* (GGPO-style synctest [S74]) instead of delta-compressed rewind (Braid [S75] considered in §15 Q07, default full keyframes).
- **I:** §3.1 `sim_types.h`: `ring_push`, `keyframe`, `restore_to_tick`, `fork_open`, `fork_read`.
- **D:** Budgets (§5.3/§5.5): keyframe = `serialize(World)` only = 10296 B (tick and room id live in the World header); keyframe period 25 ticks (0.5 s); rewind window 450 ticks (9 s [S1 §5.1 NINE LIVES]) → ring ≤ 19 keyframes = 195624 B; the fork-moment `World` = one `fork_base` keyframe outside the ring = 10296 B (§5.3); fork record = inputs only = `sizeof(ForkRec)` = 1000 B, ≤ 16 of them = 16000 B; per-room snapshot total = 221920 B (the BUDGET line in §5.3). All fixed arenas.
- **T:** Single-threaded.
- **E:** No failure mode except ring underflow, which is by-design clamped to the oldest keyframe (result code `SNAP_UNDERFLOW`).
- **A:** Identical; save-file encoding (§8) is shared.
- **Test:** `t-snapshot-replay`: 60 s demo → rewind to t−9s → re-tick → hash equality; `kitty-hash` on the recorded log must equal stored final hash on *both* platforms [C27][C23: known-bad input = log with one flipped bit → must FAIL].

### 4.5 `render/` — LB 2.5k
- **R:** Vulkan rendering of both tiers through the loader chain [S10]. Frame graph, pipelines, pass-through of `ui/` quads, palette-indexed vertex colours (no runtime quantize), dither, resize handling, startup tier self-test.
- **I:** §3.1 `render_types.h`: `r_init`, `r_begin`, `r_draw`, `r_overlay`, `r_present`, `r_tier_probe`.
- **D:** Swapchain 2 images (measured min [S26]); command buffer + per-frame uniform buffer (1 set, 1 frame in flight [rev 2 #12]); vertex buffers per room from `level/`, never reallocated mid-room. Ownership: all Vulkan objects owned by `RendCtx`, destroyed exactly once, in reverse order [S31 note: double-destroy of surface → semaphore crash].
- **T:** One submit per frame; fence-synchronised acquire per VUID [S32]. No engine threads.
- **E:** `VkResult` classification per the §6.2 VkResult table. Any result classified *fatal-fallback* logs to `T:Kitty.log` (or host `stderr`) with the call name + result, then triggers tier-fallback to lavapipe *once*; if already lavapipe, Guru-style error screen + exit code (never a loop). A driver `exit()` inside the client is a driver-side bug — the game treats device-lost as "fall back + log" [S7 lane rule].
- **A:** AROS: surface created by the loader boundary [C08][S31]; presented-pixel proof via `ReadPixelArray` from the window RastPort [S82] (LVO −160, signature in §6.6). Before `vkCreateInstance`, `platform/aros` calls `SetVar("LP_NUM_THREADS", "0", -1, GVF_LOCAL_ONLY)`. [S84] **UNVERIFIED:** whether lavapipe in the same process reads a process-local variable; the M0 probe logs the `GetVar` result and lavapipe's worker-thread count. Mesa build pinned by CI [C19]. Linux: same calls, stock WSI instance selection. Golden-image comparison uses the same dump path on both [S50].
- **Test:** Host-lavapipe golden frames (scripted demo, same Mesa commit + `LP_*` env + `AROS_PIN` hash [C18]); validation-layer pass on a 60 s demo [S1 §8.4 #5]; shader compile gate (§9) [S1 §8.4 #4]. Known-bad inputs: (a) planted shader compile error → gate FAIL; (b) golden from a different Mesa pin → pin-stamp check FAIL. Known-bad (c): see H5.

### 4.6 `audio/` (+ `modplay`) — LB 2.0k
- **R:** Streamed audio: mixer feeds a stereo S16 interleaved buffer at 48 kHz to AHI (AROS, device API) or SDL2 (Linux); `modplay` replayer for ProTracker MODs, deterministic, per-channel mute (dynamic music [S1 §3.2, §10]) and tempo (`Fxx`) handling [S68][S69][S70]. `audio/` LB 2.0k = `modplay` 1.5k + AHI/SDL stream and mixer 0.5k (C16).
- **I:** §3.2: `aud_open`, `aud_fill`, `mod_load`, `mod_mute`, `mod_seek`.
- **D:** `ahi.device` is opened on `AHI_DEFAULT_UNIT` (0) with a full `struct AHIRequest`. [S92] `AHI_NO_UNIT` is never used: it leaves `io_Unit` NULL, and `CMD_WRITE` dereferences `io_Unit`. [S92] AHI request queue: four `struct AHIRequest` blocks chained through `ahir_Link` [S80][S81], each 960 frames (20 ms at 48 kHz) = 3,840 B per buffer at 48 kHz S16 stereo (§7). Four chained 960-frame `CMD_WRITE` requests form an 80 ms queue. The queue covers any gap between services shorter than 80 ms; a longer frame underruns, and the underrun is logged with its tick. Requests are serviced in the tick loop (every 20 ms tick, catch-up included), not once per frame. **UNVERIFIED:** what the device does with a chain deeper than the documented pair — inferred from `VK/src/abi/v11/AROS/workbench/devs/AHI/Device/devcommands.c:1245`, where the wait-queue path guards `ioreq->ahir_Link->ahir_Link`; settle it with an M0 xrun probe that prints the pass number.
- **T:** Single-threaded; audio requests are serviced from the tick loop, audio never feeds `sim/`, and it is never hashed.
- **E:** AHI open failure is non-fatal: log + continue silent (QEMU audio lane absent [S33/S34 probe contexts]); missing mixer driver must never `abort()` in our process (the known mixer-side crash was upstream-merged as PR #1518 [S36]). Device-API xrun behaviour is **UNVERIFIED:** (the xrun lessons [S33] were taken on the library API) — M0 probe measures xruns at 960-frame passes with a pass number.
- **A:** AROS: AHI device path with full request block; SDL2 not used for audio on AROS. Linux: `SDL_QueueAudio` from the main thread, no callback, no lock; same 48 kHz S16 stereo stream so `modplay` output is platform-neutral.
- **Test:** `t-modplay-golden`: replayer on fixed MOD → deterministic sample-stream hash (first 10 s), compared Linux↔AROS; CI check that benchmark-mode MOD position matches [S1 §10]. Known-bad: a MOD with an illegal sequence-table entry → parser must reject, not wrap.

### 4.7 `input/` — LB 1.2k
- **R:** Maps physical channels to virtual verbs: joystick (one button + 8 directions), tap-fire=jump, hold-fire-on-node verbs, hold-fire 0.5 s pie menu (4-way: Rewind / window-depth toggle / cycle thread) [S1 §5.2], keyboard/pad enhancement mappings. Emits the per-tick `InputLine` (§5.1) — the input log IS the save/replay currency [S1 §5.4 feature 4].
- **I:** §3.2: `in_poll`, `in_next`, `in_pie`.
- **D:** Fixed ring of platform events (IDCMP `Event` structs on AROS [S31][S30]; SDL events on Linux) → normalized `InputLine` (2 B/tick; wire layout in §5.1).
- **T:** Read-only to `sim/`; single-threaded.
- **E:** Unknown/unsupported physical events are dropped with a rate-limited log line (debug ring rule [S60]).
- **A:** AROS: IDCMP through loader poll [S31] + joystick source: `lowlevel.library ReadJoyPort` availability on pc-x86-64 AROS is **UNVERIFIED:** [S58/S72] (autodoc: ports "not implemented on all platforms") — the M0 spike probes all 4 ports (port numbers **UNVERIFIED** [S58][S72]); fallback input.device path. Linux: SDL joystick+keys.
- **Test:** `t-input-replay`: recorded demo replays identically when fed through `in_next` (no wall-time reading); unit tests for pie-hold boundaries (0.5 s = 25 ticks).

### 4.8 `assets/` — LB 1.0k
- **R:** Content packages: meshes (isometric low-poly), keyframed vertex animation (squash/stretch, verlet tail is *authored keys*, not runtime physics, so it is deterministic-free: render-only), palette tables, ENVARC-referenced skins. **All Kitty/AROS-branded assets are a skin package**; game boots with placeholder "Process 1" skin [S1 §2 architecture rule]. Loading is read-only into `assets/`; no asset reaches `sim/`.
- **I:** §3.2: `as_load`, `as_get`, `as_skin_swap`.
- **D:** Single arena per room, loaded at room entry (flip-screen gives a natural load boundary [S1 §4]); skin swap is legal only at room boundaries and takes effect atomically at the next room load [rev 2 B16].
- **T:** Single-threaded.
- **E:** Failure of skin → keep placeholder + Guru-style log line (skins must never hard-fail the game [S1 §2]).
- **A:** Identical on both platforms (content is platform-neutral files).
- **Test:** Boot with only placeholder skin must pass the full scripted demo (the day-one legal rule as a machine gate); skin-swap mid-hub with no library reload [C12].

### 4.9 `level/` — LB 1.2k
- **R:** The room format ([S1 §8.4 #1]: text-first, schema-validated, solver+validator+error-reporter all read it): parse source text (canonical, with line numbers) → validate → compile to binary room (`RoomBin`) for `sim/`+`render/`. Runtime `level/` loads RoomBin and *validates a checksum*, it does not parse text (parse+compile is tools-side; runtime keeps code small). The runtime validator used for 1.1 packs is the host-side `kitty-validate` [C26]; the runtime never parses text.
- **I:** §3.2: `lv_load_room`, `lv_replay`; tools-side: `kitty-cc`, `kitty-solve`.
- **D:** Room text grammar (§8.1): `ROOM/TILE/SPAWN/ENTITY/GATE/PAR/SEED/` lines; full example in §8. Binary room: flat tables, 8-byte aligned rows, byte-order fixed little-endian; content checksum CRC-32C [S79].
- **T:** Solver is host, single-thread (state-space search — §10 H3; optional worker split is a *host tools* concern only, results order-independent via canonical min-cost tie-break).
- **E:** Every schema failure carries (file, line, token) → Guru-style report [S1 §9.2, §8.4]; parse errors never "best-effort" — a room that fails schema does not exist [S18 discipline].
- **A:** Identical sources on both platforms.
- **Test:** The 12 greybox M1 rooms [S1 §11 M1] are validated by `kitty-solve` (solvable ≤ par, no cheese pattern from `tests/cheese/`, intended verb set == allowed verb set [C22]). Known-bad: a room with goal walled off → solver must report unsolvable, not hang (budgeted search with explicit inconclusive verdict).

### 4.10 `ui/` — LB 1.0k
- **R:** In-game UI drawn by `render/` overlay pass: pie menu (the only modal UI [S1 §5.2]), requester dialogs (doors; `[Retry]`/`[Cancel]` semantics in `sim/`, skin here), Guru Meditation alert boxes (error report screen with file+line [S1 §9.2] and death "Cat Meditation" [S1 §4]), HUD (nine-lives pips, starvation meter [S1 §5.1]).
- **I:** §3.2: `ui_draw`; `ui/` renders a declarative `UiSpec` (rects/text/badges fed from `locale/` strings) and calls `guru_render` — §4.17 `guru/` owns the Guru screen, `ui/` never draws it itself [R18].
- **D:** Pure state-mirror view of `sim/` state for drawing; cannot mutate state directly (verbs already committed in `sim/`).
- **T:** Single-threaded draw pass.
- **E:** Errors impossible.
- **A:** Identical both platforms; all strings via `locale/`.
- **Routing (B15):** requester `[Retry]` → `Event{kind=RETRY}`; requester `[Cancel]` → `Event{kind=CANCEL, a=seal_index}`; pie Rewind → `Event{kind=REWIND}`; pie window-depth toggle → `Event{kind=WINDEPTH, a=window_id, b=new_depth}`; pie cycle thread → `Event{kind=THREADCYCLE, a=thread_id}`. All applied at the next tick boundary per R5.
- **Test:** Golden frames include HUD states (starvation near-overflow frame is a golden case: the bleed-to-Guru ramp [S1 §3.2 Ch1]).

### 4.11 `locale/` — LB 0.8k
- **R:** All user-visible strings go through an id'd string table: `STRMENU_NEWGAME` etc. EN + DE in 1.0 [S1 §10]; community FR/IT/SV as `.ct`-style add-on catalogs post-launch (1.1 [S1 §9.2]).
- **I:** §3.2: `lc_get` + catalog loader from text source.
- **D:** Canonical source = our own `.ct`-style text (one file per language, `msgid "text"` lines, comment headers carry Aminet-style metadata [S1 §10 honest-budget note: 1.5× wordcount per language]).
- **T:** Read-only lookups; catalog loaded once at boot (language switch = file re-read, never a library reload [C12]).
- **E:** Missing StringId → returns the EN string + logs once (a locale miss must never crash or blank text).
- **A:** The *engine* reads the same text catalog on both platforms (byte-identical strings → hashes never include text; saves/replays store only StringIds — a locale change never breaks a replay). **UNVERIFIED:** using the AROS `locale.library` binary catalog (.catalog/.cd/.ct, `InitCatalogA`/`GetMsg`) *at runtime* on x86-64 AROS — portal records describe locale.library constants and "no real locale support by stdc.library" [S59]; the catalog formats were not verified this pass. Settle by loading the EN catalog through `locale.library` in a boot probe at M3; decision in §15 Q06 (default: engine-native reader, `locale.library` as an *offer*, not a dependency).
- **Test:** String-coverage scan: any literal user-visible string outside `locale/` fails `kitty-lint`; DE catalog load test.

### 4.12 `save/` — LB 1.0k
- **R:** Saves are a **visible directory tree** the player can copy/trade [S1 §10]: `PROGDIR:Saves/Kitty/v1/` with two-slot progress files `progress.a`/`progress.b` (story flags, scarf stripes, AUTHORS-scroll lines [S1 §6]), `current/` (last keyframe + room id + room input tail), `replays/` (§8.3 format, shareable), `prefs.envarc` (mirror of ENVARC settings [S1 §10]).
- **I:** §3.2: `sv_save`, `sv_load` — atomic write via the two-slot scheme (`progress.a`/`progress.b` + generation counter + checksum; load the newest valid slot). Inferred from the handler sources: `Rename` onto an existing destination fails with `ERROR_OBJECT_EXISTS`, raised by the filesystem handler, not by `rom/dos/rename.c`; saves therefore use two slots instead of replace-by-rename. [S90]
- **D:** Versioned text files ("KSAVE 1 …") with a slot checksum; a malformed file = Guru-style report + refuse load, never a partial load. A `current/` that fails to load is discarded with a log line; progress loads from the newest valid slot. Total save failure surfaces on the Guru-style screen with file+line.
- **T:** Single-threaded.
- **E:** As above.
- **A:** Path roots differ (`PROGDIR:` vs `~/.local/share` [S1 D1]); content byte-identical (Linux reads AROS saves and vice versa — a "sneakernet" feature test).
- **Test:** Round-trip save→load→same progress hash; cross-load an AROS-written save on Linux CI.

### 4.13 `tools/` (host) — LB 2.0k
- **R:** Seven tools, all reusing engine libraries (that reuse is the proof surface):
  - `kitty-cc` — room compile+validate (line-numbered errors, Guru format).
  - `kitty-solve` — puzzle solver (§10 H3).
  - `kitty-validate` — asset/manifest/license checks (host-side of the runtime validator used for 1.1 packs [C26]).
  - `kitty-pal` — palette quantizer: per-chapter palette tables [S1 §3.2] → engine palette uniform; rejects off-palette art (rev 2 #10).
  - `kitty-hash` — replay→hash checker [C27].
  - `kitty-shaders` — glslangValidator compile of every shader + pipeline-validate dump [S1 §8.3 "non-negotiable"].
  - `kitty-lint` — the C++ subset rule scanner (§9): a rule table (pattern, scope, rationale, must-fail fixture) [rev 2 B13].
- **I:** CLI tools (`kitty-cc in.txt out.bin`, `kitty-solve levels/ --par --cheese dir`, `kitty-validate pack/`, `kitty-pal artdir palette-table`, `kitty-hash log --check`, `kitty-shaders`, `kitty-lint src/`).
- **D:** Tools link `sim/`+`snapshot/`+`level/` objects directly (same code as the shipped app) — the solver's determinism *is* the sim's determinism; data files never mutated in place.
- **T:** Single-threaded; solver may multithread on host only, results order-independent (canonical min-cost tie-break).
- **E:** All errors are text+line (Guru format for the reporter tools).
- **A:** Host-only; never built for AROS except `kitty-hash` gets an AROS twin *as the app itself* (benchmark mode runs replays on-AROS [S1 §8.4 #7]).
- **Test:** Each tool ships with a must-fail fixture (C23): e.g. `kitty-lint` on a file containing `float` in `sim/` → exit ≠ 0.

`kitty-lint` rule table (B13):

| Pattern | Scope | Rationale | Must-fail fixture |
|---|---|---|---|
| `#include <(iostream|filesystem|regex|thread)>` | all | C03 banned headers | a file with `#include <iostream>` |
| `\b(float|double)\b` | `sim/`,`snapshot/`,`input/`,`level/` | C06 no floats in sim | a `sim/` file declaring `float x;` |
| `sinf|cosf|tanf|sqrtf|pow\(|fmod|exp2?` | `sim/`,`snapshot/`,`input/`,`level/` | no libm in sim | a `sim/` file calling `sinf(x)` |
| `virtual ` | engine | C29 no virtual dispatch in hot data | a class with a virtual method in `sim/` |
| function-local static with non-trivial init | engine | no global ctors (C03) | `static Foo x;` with a ctor |
| namespace-scope object with a non-trivial constructor | engine (`platform/` included) | C03/C29: no global constructors; `operator new`/`delete` are traps (§9) | a file with `static Foo g;` at namespace scope |
| `new ` / `delete ` | engine (`platform/` included) | C29: fixed arrays + per-room arenas only; the traps and the link-time check in §9 catch strays | an engine file calling `new Foo()` |

`kitty-shaders` shader table (B13):

| Shader | Stage | Purpose |
|---|---|---|
| `VS_iso` | VS | isometric matrix + keyblend skinning + vertex colour + era fog |
| `FS_flat` | FS | vertex-colour flat/Gouraud, no texture (baseline workhorse) |
| `FS_tex` | FS | nearest-filtered texture × vertex colour |
| `FS_dither` | FS | ordered 4×4 Bayer dither alpha for clones/ghost windows |
| `FS_ui` | FS | overlay quads, no depth test |


### 4.14 `game/` — LB 1.0k
- **R:** Chapter content glue: room sequencing, story flags, freed-module placement in the hub, disk collection, Second-Kitty seeds, finale lock-ordering room. Thin data-driven layer over `sim/`+`level/`+`assets/`.
- **I:** §3.2: `game_init`, `game_tick`, `game_room_complete`.
- **D:** No game logic in `game/` — all rules live in `sim/`. `game/` only sequences and presents.
- **T:** Single-threaded.
- **E:** None at runtime (sequencing is data-driven from room completion flags).
- **A:** Identical.
- **Test:** Scripted demo replay covers the full chapter sequence.

### 4.15 `settings/` (ENV/ENVARC) — LB 0.6k
- **R:** Settings are literally `ENV:`/`ENVARC:` variables [S1 §10, §7]: renderer override, palette override, audio mute, language. Reads/writes through `platform/` ENV API; `prefs.envarc` mirrors them in the save tree (§4.12).
- **I:** §3.2: `settings_load`, `settings_set`.
- **D:** No in-engine settings state beyond a cached map; `ENVARC:` is the source of truth on AROS.
- **T:** Single-threaded.
- **E:** Missing key → default value + log once.
- **A:** `ENV:`/`ENVARC:` on AROS vs `~/.config` on Linux; content byte-identical.
- **Test:** Setting `ENV:Kitty/Renderer` to `lavapipe` forces the tier and is logged.

### 4.16 `shell/` (fast-travel) — LB 0.8k
- **R:** Earned CLI: real AROS commands do real things in-world (`cd` fast-travels between chapter Screens, `list` shows room contents, `loadwb` resets the hub, `assign` creates a shortcut) [S1 §7, §10]. Doubles as a gentle tutorial for actual AROS commands.
- **I:** §3.2: `shell_exec`.
- **D:** Command table (name → handler) fixed at compile time; no scripting/eval.
- **T:** Single-threaded.
- **E:** Unknown command → "command not found" text + Guru-style line if invoked from a script.
- **A:** On Linux, the shell runs the same command table against a POSIX-ish shim (no real AROS commands) so the demo is playable.
- **Test:** `cd` fast-travel changes the current room id in `World`; `list` output matches room entity count.

### 4.17 `guru/` (error reporter) — LB 0.6k
- **R:** One module shared by the runtime (Guru Meditation alert screen with file+line [S1 §9.2]) and the tools' output format. Every failure in the engine/tools prints the same shape: `"Software Failure. <file>:<line>: <message>"`.
- **I:** §3.2: `guru_report`, `guru_render`.
- **D:** Fixed ring of the last N (8) reports; the screen shows the most recent.
- **T:** Single-threaded.
- **E:** Reporting never fails (it is the failure path).
- **A:** Identical.
- **Test:** A deliberately-invalid room produces the expected Guru line; the golden frame includes it.

### 4.18 `replay/` (demo mode) — LB 0.6k
- **R:** Attract mode in the hub plays curated replays [S1 §7, §10]; recordings-as-files shareable by sneakernet [S1 §5.4, §10].
- **I:** §3.2: `replay_play`, `replay_record`.
- **D:** Reuses `snapshot/` ring + input log; no new state.
- **T:** Single-threaded.
- **E:** Malformed replay file → Guru line + skip (never partial play).
- **A:** Identical.
- **Test:** A curated replay reproduces its stored final hash on both platforms.

### 4.19 `benchmark/` — LB 0.8k
- **R:** `SYS:benchmark` plays a fixed replay with a demo-style camera path and prints frame-time stats [S1 §10]. Useful to the AROS ICD team as a real-world workload ("drink your own champagne" [S1 §10]).
- **I:** §3.2: `bench_run`.
- **D:** Fixed replay (checked-in `.krpl`), fixed camera path, no randomness.
- **T:** Single-threaded.
- **E:** None.
- **A:** Every timing log line carries an env stamp (host CPU, KVM/TCG, thread mode) [C23]; numbers without a stamp are meaningless.
- **Test:** Benchmark output is deterministic for a given env stamp (frame-time *values* vary by env; the replay's final state hash does not).

<!-- LB-TABLE -->
| `module` | LB (k) |
|---|---|
| `platform/aros` | 1.5 |
| `platform/linux` | 1.2 |
| `sim/` | 3.5 |
| `snapshot/` | 1.2 |
| `render/` | 2.5 |
| `audio/` | 2.0 |
| `input/` | 1.2 |
| `assets/` | 1.0 |
| `level/` | 1.2 |
| `ui/` | 1.0 |
| `locale/` | 0.8 |
| `save/` | 1.0 |
| `tools/` | 2.0 |
| `game/` | 1.0 |
| `settings/` | 0.6 |
| `shell/` | 0.8 |
| `guru/` | 0.6 |
| `replay/` | 0.6 |
| `benchmark/` | 0.8 |
| **Engine total (excl. `tools/`)** | **22.5** |

Engine modules (excluding `tools/`) sum to 22.5 k; the host-only `tools/` row is excluded from that total. The 23k engine cap is binding.

## 5. Simulation and determinism spec

### 5.1 Types and contracts

**Q16.16** (`using Fixed = int32_t` with 16 fractional bits), widened math only:

```cpp qmath
using u32 = uint32_t;
using Fixed = int32_t;
constexpr u32 SIM_OVF = 1u << 0;
constexpr u32 SIM_DIV0 = 1u << 1;

inline Fixed qmul(Fixed a, Fixed b, u32* flags) {
    int64_t r = (int64_t)a * (int64_t)b;
    int64_t res = r >> 16;
    if (res > INT32_MAX) { if (flags) *flags |= SIM_OVF; return INT32_MAX; }
    if (res < INT32_MIN) { if (flags) *flags |= SIM_OVF; return INT32_MIN; }
    return (Fixed)res;
}

inline Fixed qdiv(Fixed a, Fixed b, u32* flags) {
    if (b == 0) {
        if (flags) *flags |= SIM_DIV0;
        if (a == 0) return 0;
        return (a >= 0) ? INT32_MAX : INT32_MIN;
    }
    int64_t r = ((int64_t)a << 16) / (int64_t)b;
    if (r > INT32_MAX) { if (flags) *flags |= SIM_OVF; return INT32_MAX; }
    if (r < INT32_MIN) { if (flags) *flags |= SIM_OVF; return INT32_MIN; }
    return (Fixed)r;
}
```

- Rounding is **pinned**: `>>` on negative int64 is arithmetic floor in C++20 [S76]; qmul rounds toward −∞; qdiv truncates toward zero (division). Both pinned by unit tests on the four sign quadrants — the divergence trap [S2 tech note]; no "banker's rounding", no per-platform libm.
- **Overflow policy (§5.1.OV):** every widening site is `int64_t`. qmul saturates on overflow (`SIM_OVF` recorded in the tick result) rather than wrapping; `qdiv` saturates and records `SIM_OVF` when the quotient leaves the `int32_t` range (e.g. `INT32_MIN / -1.0`). `sim/`, `snapshot/`, `level/` and `input/` compile with `-fwrapv` as a belt-and-braces backstop (rev 2 #5); CI adds a Linux UBSan job (`-fsanitize=undefined -fno-sanitize-recover`) over H1+H2. The same policy applies in release and CI (no `#ifdef`).
- **Trig:** one shared quarter-period sine table (1025 entries, Q16.16), linear interpolation between entries, angle as a 16-bit wrap; table is generated by `tools/` and checked into the tree as data — bit-exact both platforms [S77]. No `sinf`, no libm in `sim/` [C06].
- **Tick order (fixed list, never data-order):** Step 0 applies the `Event`s queued for this tick, sorted by (kind, a). Then 1 player verb resolution → 2 FORBID task-switch fiction (starvation meter) → 3 clones replay input lines from their `ForkRec` [S1 §5.3: clones replay *inputs*] → 4 entity/task simulation in fixed ID order → 5 locks/gates/window-depth evaluation → 6 win/lose evaluation. Array order is index order, never insertion order. No RNG draws occur in 1.0 (R15); the generator is pinned (§5.6) for future use, and any draw requires a v2 change. `SEED` stays in the room format.

**InputLine wire format (2 B/tick, dense):** `InputLine` is two bytes: byte 0 bits 0–3 are the direction mask and bits 4–7 are reserved; byte 1 bit 0 is fire and bits 1–7 are reserved. Reserved bits must be zero and are checked on load (a non-zero reserved bit → `E_SCHEMA`). Pie and verb commits are *derived deterministically* from hold-ticks (25-tick threshold), not extra bytes — so the dense log is the whole truth. Inputs only: no sim state is recorded (rev 2 #15). Shareable replay file (§8.3) run-length encodes it.

**InputLine bit layout (R14):**

| Byte | Bits | Meaning |
|---|---|---|
| 0 | 0–3 | direction bitmask (N/E/S/W); opposing directions cancel (N+S → neither), normalized in `input/` before logging |
| 0 | 4–7 | reserved (must be 0; checked on load) |
| 1 | 0 | fire (0 up, 1 down) |
| 1 | 1–7 | reserved (must be 0; checked on load) |

All tick counters are `u32` (`TickRecord`, `Event`, `ForkRec`, replay header) [R14].

**Event stream (R5):** every `World` mutation that does not come from an `InputLine` is a logged `Event` (shell `cd`, room transitions, requester choices). Events are stored in the replay file with their target tick and applied as Step 0 of that tick's order (§5.1), sorted by (kind, a). `game/` and `shell/` take `const World&` and emit events; they never mutate `World` directly. "The log is the whole truth" now means inputs + events.

**State hash:** `hash_state` = FNV-1a 64-bit over `serialize(World)` in canonical order [S78 constants]. Hash at every keyframe and at end-of-replay; a host-only CI job may hash every tick (rev 2 #16). A per-room seed line is in the header so replays pin RNG.

TICKS_PER_CELL = 8 (player speed: 6.25 cells/s; PAR ticks must be ≥ shortest path in cells × 8).

### 5.2 Interaction matrix → sim rules

Normative matrix from [S1 §5.3]; each row gets its own test in `tests/sim/` [C23]:

| v2 row | Sim mechanism |
|---|---|
| Rewind × clones | rewind = `restore_to_tick` writes whole `World` incl. clone array (§4.4: clones are `World` state) |
| Forbid × clones | Forbid sets `world.tasks_frozen`; clone tick step consults it — frozen clones consume no tick of their input line (their input clock freezes too; tested) |
| Window depth × clone recording | clones replay input lines against current world → divergence by design; no special case, test proves divergence is deterministic |
| Retry × clones | `retry_room` = restore entry snapshot from ring, kill all clone slots |
| Cancel × clones | Cancel mutates world (closes path); clones see it (no rule needed; test asserts) |
| Fork during Forbid | pad returns "grey" result state in sim; commit impossible |
| Join w/ unfinished clone | gate open-check includes all-live-thread arrival; finished-away clones become `ZOMBIE` idle slots (lesson, not bug) |
| Clone death | despawn + guru flash (render); any waiting gate keeps waiting → retry required (no auto-fix) |

**Event application:** the Step 0 rule of §5.1 is the whole story — the events queued for a tick are applied in one pass, sorted by (kind, a), before any verb resolution, so the order never depends on arrival order. A replay that omits an event produces a different hash (H2 known-bad: drop one event → hash mismatch).

### 5.3 Snapshot contents and budget

Keyframe = full `serialize(World)` only — tick and room id already live in the World header, so one keyframe is exactly the §5.5 total. Ring: 19 keyframes at a 25-tick period (⌈450/25⌉+1 = 19 for a guaranteed 9 s rewind). The fork-moment `World` is kept in one `fork_base` keyframe outside the ring; only one FORK recording is open at a time, so one slot is enough. `ForkRec` holds 500 ticks because v2 caps a FORK recording at 10 s [S1 §5.1]. Fork records hold inputs only; spawn position and cursor live in `World` (R8). The budget is stated once, with the §5.5 total and the measured record size filled in:

BUDGET keyframe=10296 B; ring=19*10296=195624 B; fork_base=10296 B; fork=1000 B; forks=16*1000=16000 B; total=195624+10296+16000=221920 B

(Provisional until M0 measures entity counts [S1 §4.1 rule: budgets set by M0]; §4.4 quotes the same two totals.)

### 5.4 Replay equivalence contract

`kitty-hash <room.txt> <log>` == stored hash is the *same binary code path* on Linux and AROS (the tool and the app link the same `sim/`/`snapshot/` objects). The proof that AROS gcc-16 + crt produce the same machine behaviour as gcc/glibc for this subset is CI item 7 [S1 §8.4]; there is no measured record yet that AROS-libm differs from glibc — **UNVERIFIED:** but irrelevant by construction (no libm in `sim/` [C06]) — the residual divergence risks are exactly `-ffp-contract`/FMA, pinned off both sides [C06, S1 Appendix A, rev 2 #6].

### 5.5 World field list (source of the §4.4/§5.3 budgets)

| Field | Type | Count | Bytes |
|---|---|---|---|
| header (tick, room id, seed, rng state) | struct | 1 | 32 |
| entities (id, kind, pos x/y fixed, vel, state flags) | struct | 64 | 6144 |
| clones (start tick, length, input cursor, active flag, carried items, spawn position) | struct | 16 | 832 |
| windows (id, pos, depth state, owner) | struct | 8 | 256 |
| requesters (id, pos, state, retry/cancel flags) | struct | 8 | 192 |
| gates (semaphore/mutex state, holder counts) | struct | 8 | 128 |
| mutable grid (bitset overlay for Cancel-closed paths) | bitset | 128×128 | 2048 |
| task/forbid state (starvation meter, frozen flag) | struct | 1 | 16 |
| player state (pos, vel, on-node, held-verb) | struct | 1 | 64 |
| pad/pedestal/keyboard-node states | struct | 32 | 512 |
| input hold counters (per verb, per clone) | u32 | 16 | 64 |
| per-tick flags (forbid active, retry pending) | u32 | 1 | 4 |
| event cursor (R5) | u32 | 1 | 4 |
| **Total** | | | **10296 B** |

Snapshot budgets derived from this table: keyframe = the Total row above = 10296 B; ring = 19 keyframes = 195624 B; fork base = one more keyframe outside the ring = 10296 B; fork record = `sizeof(ForkRec)` = 1000 B (inputs only); 16 fork records = 16000 B; per-room total = 221920 B. The §5.3 BUDGET line is the single place those numbers are written.

### 5.6 RNG

`xorshift64*` with the standard constants (multiplier `0x2545F4914F6CDD1D`, shift 12/25/27), seeded from the room `SEED` line via splitmix64, then one draw to advance. The `SEED` hex value is zero-extended to 64 bits and used as the splitmix64 state. No draw occurs in 1.0 (§5.1). Both platforms use the same integer operations; no libm.

## 6. Renderer spec

### 6.1 Feature floor — what the engine may use

Target **core Vulkan 1.3** via the loader chain (interface v7+, truthful; the AROS ICD manifest is `api_version 1.4.0` but requesting 1.3 is legal and keeps the floor honest [S10][S48]) **plus these extensions:**

- **Instance (AROS):** On AROS, `ppEnabledExtensionNames` contains no WSI extension; the loader adds `VK_KHR_surface` and `VK_KHR_xcb_surface` only when the ICD advertises them. [S83] The app itself asks for no surface extension at all.
- `VK_EXT_debug_utils` is enabled only in debug builds and only when `vkEnumerateInstanceExtensionProperties` lists it. [S94]
- **Instance (Linux):** On Linux, the instance extension list is exactly what `SDL_Vulkan_GetInstanceExtensions` returns. [S89]
- **Device:** `VK_KHR_swapchain` (AROS WSI platform is `VK_ICD_WSI_PLATFORM_AROS` [S26]; present modes FIFO mandatory + IMMEDIATE, default IMMEDIATE to avoid the FIFO/WaitTOF trap [C13][S26]).

`VK_KHR_aros_surface` ("VK_KHR_aros_surface") is defined in `vulkan_aros.h` but is obsolete (`FEATURE_NOT_PRESENT`) [S83]; do not use it.

Deliberately **not used, even though Mesa 26.x exposes them on both drivers** [S63]: dynamic rendering (use render passes + framebuffers: simpler state, and CTS measured no losses either way [S56]), synchronization2, timeline semaphores, push descriptors, descriptor indexing (interpreter ICD had to keep indexing off [S14/S15] — we don't need any of it), mesh shaders (tier-2 qualified [S63]), `device_generated_commands`. The Mesa 26.2.2-pin caveat: features must be probed from the *pinned* release's `features.txt` + a runtime probe gate in CI (§10 H4), because [S63] mirrors mesa `main` while we pin 26.2.2 [S47][S6].

No vendor extensions except the loader's own boundary functions (`vkCreateArosSurfaceKHR`, `vkPollArosEventsKHR`), which are loader-private-entrypoints and must be registered in the loader allowlist if new ones are ever added [S55].

Depth bias: **the engine never uses `VkPolygonOffset`** — historically the AROS lavapipe zero-bias behaviour was a real gap [S24] that was reverted with upstream Mesa fix; the engine simply must not depend on bias (isometric painter's algorithm, §6.6).

### 6.2 Frame graph and VkResult classification (both tiers, one path)

`acquire (fence-sync) → record → submit → present`, once per presented frame [C09], 2-image swapchain [S26], one acquire semaphore, one render-finished semaphore *per swapchain image*, one fence (1 frame in flight [rev 2 #12]). On `VK_ERROR_OUT_OF_DATE_KHR` from acquire/present (window resized/zero-sized [S30]) rebuild extent: 320×256 internal always, blit-scaled to window client area [S32] — never a dynamic internal resolution except the user 2× option on NVK [S1 §4].

**VkResult table (§4.5 E uses these classes):**

| Result | Class | Action |
|---|---|---|
| `VK_SUCCESS` | normal | continue |
| `VK_SUBOPTIMAL_KHR` | normal | continue; schedule extent rebuild |
| `VK_ERROR_OUT_OF_DATE_KHR` | recreate-swapchain | rebuild swapchain + command buffers; re-record; do not fall back |
| `VK_TIMEOUT` / `VK_NOT_READY` | retry | acquire timeout is 100 ms (`vkAcquireNextImageKHR` timeout arg = 100000 us); on `VK_TIMEOUT`, skip frame + log; retry once with 16 ms wait |
| `VK_ERROR_DEVICE_LOST` | fatal-fallback | if tier is NVK → fall back to lavapipe once (device rebuild); if already lavapipe → Guru screen + exit code (never a loop) |
| `VK_ERROR_SURFACE_LOST_KHR` | recreate-surface | recreate the surface through the loader once, else Guru + exit |
| any other `VK_ERROR_*` | fatal-exit | Guru screen with file+line + exit code |

A lost surface is a window-lifecycle problem, not a tier problem: it never changes the selected driver.

### 6.3 Pipelines

Shaders (glslangValidator-checked; ≤ 6 shaders, but pipelines ≥ shaders due to variants [rev 2 B7]):

Shaders:
- `VS_iso` — isometric matrix + skinning-by-keyblend (two blends, u8 indices) + vertex colour + era fog factor.
- `FS_flat` — vertex-colour flat/Gouraud-interpolated, no texture (baseline workhorse).
- `FS_tex` — nearest-filtered texture × vertex colour (optional textures [S1 §4]).
- `FS_dither` — ordered 4×4 Bayer dither alpha for clones/ghost windows (pattern is the style [S1 §4]; cheaper than blending on CPU tier).
- `FS_ui` — overlay quads (ui/ + requester skins), no depth test.

Pipelines (count these, not shaders): `P_flat` (cull on), `P_flat_nocull` (cull off, for authored two-sided props), `P_tex`, `P_dither`, `P_ui` = **5 pipelines**. Pipeline-compile cost: every pipeline is created at init and JIT-compiled once; **nothing caches it on AROS** [S21 decision] so the engine creates all 5 pipelines once at startup, never during a room; worst-case init cost measured at M0 (§11; the guest MCJIT finalize hazard is recorded [S48 rt-mcjit row] — M0 gate: init completes in bounded time).

Pipeline rules: optional state structs always supplied explicitly [C10]; back-face culling explicit per pipeline (isometric rooms cull back-faces, overdraw budget ≤ 2× [S1 §4.1]); rasterization samples 1× (MSAA on lavapipe is a silent 1× downgrade on the interpreter [S16] — never advertised as a feature). **Palette:** vertex colours are authored as palette indices (0..N into the chapter palette uniform); `kitty-pal` rejects off-palette art at build time. Textures, if used, are pre-quantized at build time [rev 2 #10]. No per-fragment nearest-colour search.

### 6.4 Tier selection and startup self-test

Startup order ([S1 §4 selection + §8.3 rule]): enumerate devices via loader/manifest → if `ENV:Kitty/Renderer` set honour it [S1 §4]; else: try NVK-class device (`vendorID == 0x10DE` && `driverID == VK_DRIVER_ID_MESA_NVK` [rev 2 #8]) → run **self-test**, which opens a scratch instance first: The NVK self-test instance is created with zero instance extensions. Then create the device, render the 64×64 test scene (one triangle + HUD quad, exercising all shader entry points), read back, compare against a tolerance (per-channel ±ε plus coverage-mask equality, with numbers set at M0 [rev 2 #8]); any error/mismatch → fall back lavapipe with one log line "why" [S1 §8.3]. Lavapipe stays the shipped default unless the NVK gate passes — and per D7 [S1 §13] the 1.0 NVK *tier* additionally requires upstreaming; today NVK is lane-only [S9] — the self-test is the machine-side enforcement.

### 6.5 Library-residency rule (C12)

Open `vulkan_loader.library` once (its manifest opens the ICD; OpenCnt ≥ 1 keeps ICD out of any flush [S48/S43]); never `CloseLibrary`, never `RemLibrary`, never `Avail FLUSH` [S4][S42–S44: the force-expunge double-free family is *loader-side*, ours not the kernel's]; after a rebuilt library must take effect → **reboot the boot** (deploying a fix = new boot [S14]).

### 6.6 Readability, geometry and presented-pixel proof

Fixed dimetric projection, matrix from `tools/` (glm is render-side only [S1 §8.2]). Depth: painter sort by room-authored depth key (flip-screen authored rooms → sort is per-drawable at compile time when static; dynamic entities sorted per frame by grid key, ≤ 240 items) — **not** depth-buffer-dependent for correctness (the isometric readability requirement [S1 §4]; the depth buffer exists but only for intra-mesh self-occlusion). Transparency: dither, never blend [S1 §4].

Presented-pixel proof on AROS: `ReadPixelArray` (cybergraphics, LVO −160, signature `ULONG ReadPixelArray(APTR dst, UWORD destx, UWORD desty, UWORD dstmod, struct RastPort *rp, UWORD srcx, UWORD srcy, UWORD width, UWORD height, UBYTE dstformat)` [S82]) read back from the window RastPort into a host buffer, then compared against the golden. the interpreter-ICD's WPA diagnostic getter is removed from the design (interpreter-ICD-internal [rev 2 #9]). On the lane, the QEMU HMP `screendump` is the external check.
- Linux (R19): render to an offscreen 320×256 `VK_FORMAT_B8G8R8A8_UNORM` image, then `vkCmdCopyImageToBuffer` into a host-visible buffer; compare bytes.
- AROS CI (R19): the window client area is exactly 320×256 (scale 1), the screen depth is ≥ 24-bit, and readback uses `RECTFMT_BGRA32` (value 111 [S12]); otherwise the gate is BLOCKED, never compared.

### 6.7 Golden-image hooks

Every CI render test ends with: render scripted frames headless → dump presented pixels (`ReadPixelArray` readback on AROS; direct framebuffer dump on Linux) → compare (a) host-lavapipe vs stored golden — bit-exact, same Mesa commit + `LP_*` env + `AROS_PIN` hash [C18][S50][S25]; (b) on-AROS guest vs the same golden — the baseline-tier proxy check of C18, its tolerance set at M0, feature dumps compared field by field [S56][S50]. NVK goldens only in lane context (§10 H8, gated by boot grants).

## 7. Audio spec

Covered by §4.6; the AHI-vs-SDL contract:
- AROS: AHI device API: `ahi.device` is opened on `AHI_DEFAULT_UNIT` (0) with a full `struct AHIRequest`. [S92] The `ahir_Link` chain field orders the queue [S80]; the engine queues four 960-frame (20 ms) `CMD_WRITE` requests = 80 ms of buffered audio at 48 kHz S16 stereo = 3,840 B per buffer (§4.6 D); the AHI library-API frequency/player-callback tags are not used [rev 2 #2]. Device-API xrun behaviour is **UNVERIFIED:** (the xrun lessons [S33] were taken on the library API) — M0 probe measures xruns at 960-frame passes with a pass number.
- Linux: `SDL_QueueAudio` from the main thread, no callback, no lock; same 48 kHz S16 stereo stream so `modplay` output is platform-neutral.
- `modplay`: PT4 subset, 4 channels, own code (C16). `Fxx` with xx = 00–1F sets the speed (ticks per row), and xx = 00 is ignored; xx = 20–FF sets the tempo (32–255 BPM). [S93][S68] The CIA timer itself accepts 28–255 [S70][S93]; `modplay` follows the PT2.3A player, which raises a tempo below 32 to 32 (`MOVEQ #32,D0`) [S91]. A note's playback rate is the Paula clock divided by its period (PAL: 3546895 Hz); 50 Hz is the tick rate, not the pitch clock. [S93] Set volume is effect `Cxx`, with volume 0–64. [S93] MOD samples are signed 8-bit, so they have no byte order; `modplay` widens them to `i16` at load. [S68] Per-channel mute = clear the mixer player slot for that channel (trivial in own replayer [S1 §8.2] and why own beats miniaudio/libopenmpt [S2 tech note]).

## 8. Data formats

### 8.1 Room source format (text-first; [S1 §8.4 #1]; prior art: .xsb char-grid + PuzzleScript rule style [S73])

EBNF (the grammar `kitty-cc` implements; field order is fixed — header, seed, par, grid, spawn, objects, exit):

```ebnf
room     = header seed par grid spawn { object } exit ;
header   = "ROOM" id uint uint era NL ;
seed     = "SEED" hex NL ;
par      = "PAR" uint uint uint NL ;      (* ticks, verb-commits, clones-max *)
grid     = "GRID" NL row { row } "ENDGRID" NL ;
row      = tile { tile } NL ;             (* exactly w tiles; exactly h rows *)
tile     = "#" | "." ;                    (* terrain only *)
spawn    = "SPAWN" pos NL ;
object   = entity | gate | pane ;
entity   = "ENTITY" id kind "patrol" pos "->" pos "speed=" uint "phase=" uint NL ;
gate     = "GATE" id gatetype "pos=" pos [ "n=" uint ] { gateflag } NL ;
gateflag = "retry" | "cancel-seals=" pos ;
pane     = "PANE" id "pos=" pos "depth=" depth NL ;
exit     = "WIN" "exit" "pos=" pos NL ;
pos      = uint "," uint ;
kind     = "nybble" ;
gatetype = "semaphore" | "mutex" | "requester" ;
depth    = "front" | "mid" | "back" ;
era      = "ch" digit ;
id       = letter { letter | digit | "_" } ;
uint     = digit { digit } ;
hex      = "0x" hexdigit { hexdigit } ;
comment  = ";" { char } NL ;              (* the only comment form; may follow any line *)
```

Rules: `;` is the only comment marker (and `#` is a wall tile, so a comment can never be confused with a grid row); GRID is followed by exactly `h` rows of exactly `w` terrain tiles; entities, gates and panes are *lines*, never tiles; every keyword's fields are typed with units; `PAR` = ticks (sim ticks at 50 Hz), verb-commits (count), clones-max (count). Every referenced position must be in bounds and on floor (`.`), and `kitty-cc` checks all of it (§4.9 `lv_load_room`).

Valid example (Chapter 1 room, 16×12), fenced as `kittyroom` (R2):

```kittyroom
; ch1_03 — "The requester door"
ROOM ch1_03 16 12 ch1
SEED 0xC0FFEE
PAR 200 1 0
GRID
################
#.......#......#
#.......#......#
#.......#......#
#.......#......#
#..............#
#.......#......#
#.......#......#
#.......#......#
#.......#......#
#.......#......#
################
ENDGRID
SPAWN 1,1
ENTITY pat1 nybble patrol 2,2->6,2 speed=1 phase=0
ENTITY pat2 nybble patrol 10,4->10,8 speed=1 phase=2
GATE req1 requester pos=8,5 retry cancel-seals=11,5
PANE w1 pos=12,8 depth=mid
WIN exit pos=14,10
```

Player speed is `TICKS_PER_CELL = 8` (§5.1), so this example's PAR 200 ticks requires a 22-cell path — checked below.

Hand-check: declared 16×12; GRID has 12 rows of exactly 16 chars; object positions (1,1),(2,2),(6,2),(8,5),(10,4),(10,8),(11,5),(12,8),(14,10) are all inside 0..15 × 0..11 and on floor; 0-verb path: none — with every GATE cell closed, the interior wall at x=8 has no other gap, so WIN (14,10) is unreachable from SPAWN (1,1); shortest path with gates open (1,1)→(14,10) = 22 cells × 8 = 176 ticks = 3.52 s, under PAR 200 ticks = 4.00 s; 1 verb-commit × 25-tick hold = 25 ≤ 200; 0 clones (FORK arrives in Ch2) — solvable under §5.1's rules, and not solvable with zero verbs.

Negative examples — each is a complete room file whose validation must fail with exactly the Guru line shown after it (`kitty-cc` prints `Software Failure. <file>:<line>: <message>`; §4.9):

1. Row shorter than the declared width:

```kittyroom-bad
ROOM r 4 2 ch1
SEED 0x1
PAR 100 1 0
GRID
####
#..
ENDGRID
SPAWN 1,1
WIN exit pos=2,1
```
Expected: `Software Failure. room.txt:6: row width 3 != declared 4`

2. More rows than the declared height:

```kittyroom-bad
ROOM r 4 2 ch1
SEED 0x1
PAR 100 1 0
GRID
####
####
####
ENDGRID
SPAWN 1,1
WIN exit pos=2,1
```
Expected: `Software Failure. room.txt:7: row count 3 != declared 2`

3. Tile outside the closed terrain alphabet:

```kittyroom-bad
ROOM r 4 2 ch1
SEED 0x1
PAR 100 1 0
GRID
####
#X.#
ENDGRID
SPAWN 1,1
WIN exit pos=2,1
```
Expected: `Software Failure. room.txt:6: illegal tile 'X' in row 1`

4. Object position outside the declared grid:

```kittyroom-bad
ROOM r 4 2 ch1
SEED 0x1
PAR 100 1 0
GRID
####
####
ENDGRID
SPAWN 5,1
WIN exit pos=2,1
```
Expected: `Software Failure. room.txt:8: position (5,1) out of bounds (4,2)`

5. Semaphore with `n=0`:

```kittyroom-bad
ROOM r 4 2 ch1
SEED 0x1
PAR 100 1 0
GRID
#..#
#..#
ENDGRID
SPAWN 1,1
GATE g semaphore pos=2,1 n=0
WIN exit pos=2,1
```
Expected: `Software Failure. room.txt:9: semaphore n must be >= 1`

### 8.2 Manifest (content packs, 1.1; schema-validated [C26])

`manifest.json` per [S1 §9.2] — name, author, version, license (one of three [C26]), deps, per-file SHA256 (`SHA256SUMS`, integrity only, never called a signature [S1 §9.2 honesty rule]). Validator failure = Guru with file+line [S1 §9.2]; validator *also* runs `kitty-solve` on every room inside [S1 §9.2].

### 8.3 Save tree and replay file

- `Saves/Kitty/v1/` as §4.12; `prefs.envarc` mirrors `ENV:Kitty/…` variables verbatim [S1 §10 ENVARC settings].
- Replay file: `KRPL 1 <room-id> <seed> <len-ticks>` then RLE of 2-B/tick channel state, then the `Event` stream (R5), then trailer: final state hash (FNV-1a §5.1) + tool stamp (git tag name [S1 §11 "commit after every milestone"]). Shareable by sneakernet [S1 §5.4, §10]; replay demo mode in hub plays curated ones [S1 §7].
- Version rule: format tag after tool name; loader rejects unknown major, tolerates additive trailer fields.

### 8.4 Catalog strings

`locale/en.ct` source: `msgid "hud.lives" "Nine lives: %d"` — `.ct`-style text (engine reads; `.cd`/catalog compile step is a §15 Q06 open item [S59 unverified]). Replay/save files store StringIds, never text [§4.11 rule].

## 9. Build and toolchain

**One flag file** (rev 2 #6) for the determinism-critical modules (`sim/`, `snapshot/`, `level/`, `input/`), identical on both platforms:

```
-std=c++20 -fno-exceptions -fno-rtti -fno-strict-aliasing -fwrapv -ffp-contract=off
-O2 -Wall -Werror=return-type
```

No `-ffast-math`. Host compiler pinned to the gcc-16 major; if that can't be pinned, H2+H7 hash equality is the stated proof and a mismatch blocks M2 [rev 2 #6].

**Common C++ (all modules):** banned includes (`iostream|filesystem|regex|thread`), `float|double` under `sim/`+`snapshot/`+`input/`, banned symbols (`sinf|cosf|tanf|sqrtf|pow(|fmod|exp2?`), virtual-in-hot-data check — all enforced by `kitty-lint` (rule table in `tools/kitty-lint.rules`).

**Linux (CI host / shipped D1 build):** SDL2-dev, Vulkan loader (lavapipe pinned to the same Mesa commit as the AROS build, with matching `LP_*` env and `AROS_PIN` hash [C18]), glm header-only, Dear ImGui (debug builds only [C17]).

**AROS cross (every artifact):** mandatory set verbatim [S1 Appendix A]
```
-mcmodel=large -mno-red-zone -mno-ms-bitfields -fno-strict-aliasing
-ffixed-r12 -fno-stack-protector -fno-exceptions -fno-rtti -ffp-contract=off
```
plus: build against the **ABI-patched v1 SDK** [S53] (verify emitted LVO/proto ABI by objdump, base in last C arg — not r12 [S60 2026-07-06]); `-nostartfiles` + engine's own `__startup_entry` [S40][S41][S52]; `-fno-pic -fno-pie` on app binaries (131 MB ET_REL GOT64 gap [S13 addendum]); link libs: `-ldos -lexec` (self-contained tools avoid libc entirely [S51]); if stdio is needed, link via the shim (`libcrt`/`libstdlib`/`libcrtprog` → `libstdc`/`libstdcio`/`libposixc` [S51], with `-lstdcio -lposixc`).

**CRT/allocator (C29):** no STL containers and no exceptions anywhere in engine code. `platform/rt` provides `memcpy`/`memmove`/`memset`/`memcmp` and `__cxa_pure_virtual`, all freestanding C [R6]. Fixed arrays plus per-room arenas. Global `operator new`/`delete` are defined only as traps that call `guru_report` and exit; a link-time check fails the build if any engine object references them. Logging formats text without libc stdio (a tiny fixed-buffer formatter in `platform/` writing via `RawPutChar`/file sink [S60][S1 Appendix A: long traces rate-gated]); if a lib is needed, name the verified one — none is required by the engine.

Gate after build: `objdump -d kitty | grep -c 'mov %rax,%r12' == 0` per Appendix A [C05]; **nuance:** binaries that include AROS libcall glue legitimately contain `movq %1,%%r12` [S48 2026-08-13, S45/46 healthy counts 225] — the gate is defined for the engine app binary as built above (no libcall glue) and any future exception must be *justified* in the audit, per gate-design [S20].
After installing rebuilt `.library` artifacts on the boot: reboot [C12/C24]. Never `Avail FLUSH` [C24].

**Pin stamp (C18):** every CI run records `Mesa commit + LP_* env + AROS_PIN hash` (the host LLVM version is recorded for provenance, never as a requirement). [S85] measured one depth-bias test with a pixel-count metric (`diff=153600` for both bias signs); it is not a bit-exactness proof. The 2026-10-22 control showed host LLVM 20.1.8 passing 109/109 while the guest is 20.1.0 (same release series, different patch) [S25] — the guest's compiler build need not equal the host's (C18). `AROS_PIN` is an upstream AROS git SHA (pinned 2026-10-28) [S47]; the value is cited from the file, never hardcoded [R20]. The stamp is recorded in H4/H7 and in the risk register.

**Source numbering:** citations `[S#]` resolve in `kitty_engine_design_sources.md`; numbering is contiguous from S1 (rev 3 closed the S80+ gaps). New platform facts cite `VK/<path>:<line>` (read-only) per the evidence rules. No source id appears twice in one citation cluster.

## 10. Verification and CI

Every [S1 §8.4] item → harness, inputs, pass criterion, and the known-bad input it must FAIL on ("a check that cannot fail is not a check" [C23]; gate-design rules [S20][S19]). **BLOCKED** is a distinct state (lane-gated gates) and is never counted as PASS [rev 2 #21].

| # | v2 §8.4 item | Harness (command) | Pass criterion | Known-bad input → must FAIL |
|---|---|---|---|---|
| H1 | Linux build + unit tests | `make linux && t-sim t-snapshot t-modplay t-level` | all tables green; return codes printed always [S19] | planted off-by-one in `qdiv` negative test; planted ring-underflow; UBSan job (`-fsanitize=undefined -fno-sanitize-recover`) over H1+H2 |
| H2 | State-hash replay suite | `kitty-hash tests/replays/*.krpl --check` | every log → stored final hash, on Linux and (H7) on AROS | log with one flipped bit; replay recorded against a different `SEED`; uninitialized padding in `serialize` reaching the hash |
| H3 | Puzzle solver on all rooms | `kitty-solve levels/ --par --cheese tests/cheese/` | solvable ≤ par; intended-verb-set = minimal-verb-set; zero cheese solutions; bounded search exits INCONCLUSIVE (never hangs) | room walled-off goal (unsolvable); a planted cheese solution file; room solvable with fewer verbs than PAR declares |
| H4 | Render CI host-lavapipe, same Mesa commit | `kitty-shaders` (glslangValidator every shader + pipeline validate), then scripted demo → golden compare (bit-exact against the stored golden) | golden frames == stored; features.txt of the *pinned* Mesa reviewed vs §6.1 floor; pin stamp (Mesa commit, `LP_*` env, `AROS_PIN`) matches the run that made the golden | planted shader compile error; golden from a different pin (detected by pin-stamp check, part of the harness) |
| H5 | Validation-layers demo | `VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_validation kitty-demo --seconds 60` | zero validation messages | a deliberately-wrong pipeline: missing `pRasterizationState` is *invalid usage* (not testable as "does not render") — instead use a named VUID the layers report, e.g. an invalid image layout in `vkCmdCopyImage` (layers report `VUID-vkCmdCopyImage-srcImageLayout-01917`); acquire/submit ordering goes here with `VUID-vkQueuePresentKHR-pWaitSemaphores-03268` [S32] |
| H6 | AROS cross-build + ABI gates | §9 recipe + `objdump` r12-count gate + `kitty-lint` | binary links; gate counts == 0; lint clean | building against the stale SDK (historical r12-ABI break [S53]); an include of `<iostream>` |
| H7 | On-AROS smoke run (milestone agent, granted boots) | boot lane: run same replay on lavapipe; upload final state hash, frame-time log, final frame | state hash == Linux hash (H2 pair); frame == host-lavapipe golden (§6.7); 30-min resident stability [S1 M0] | a sim that reads `long`-size-dependent encoding in `serialize`; forced `WaitTOF` path (hang class [C13]) — kept as a no-hang test *and* a `WaitTOF` source lint + LVO/import scan (both cheap, complementary [rev 2 #21]) |
| H7-M0 | On-AROS smoke run (M0, no sim) | boot lane: render the M0 scene on lavapipe; upload frame-time log, final frame, resident memory | frame == host-lavapipe golden; 30-min resident stability; init completes in bounded time | same as H7 minus the hash check (no sim in M0) |
| H8 | NVK card-lane run (from M3, boot-granted only) [S1 §8.4 #8] | same replay on NVK; compare state hash (must match); frame vs host NVK oracle golden | hash equal; frame within tolerance (§6.4) | lane rules enforced first: no signal-teardown with device live [S54][S27]; NVK present never reached on card lane today [S7/S28: has-swapchain=0] → H8 is **BLOCKED** until lane fixes; BLOCKED is its own state, never counted as PASS |

CI meta-gate: every run prints the gate census (how many gates ran) and each gate's rc [S20/S19 rules]; `make` exit code is never trusted alone [S1 §11 "verify by timestamp or hash"]. A BLOCKED gate is reported as BLOCKED in the census, not as PASS.

## 11. Performance budgets

**Anchor measurements (what we actually have):**
- lavapipe on the KVM guest, 4 vCPU, 256×192 window: conformance cube **300 frames in 320 ms** (≈ 1.07 ms/frame minimal scene) [S23]; 5-run variance 320–380 ms [S17]. This is a *minimal* scene, not a game scene — it bounds the plumbing cost, not fill.
- Fill dominates llvmpipe/lavapipe cost (large textured quads, fragment-stage dominant) [S17, S65]; JIT per-pipeline compile is never cached on AROS [S21] and one guest MCJIT finalize hazard is on record [S48] — M0 must time pipeline creation.
- Resident: lavapipe ICD library measured **130,500,536 B** [S43-adjacent bulk put; S6 lane]; guest RAM seen 2 GiB (boots) and 4 GiB w/ largest free ≈ 1.2 GB [S44/S42 records].
- TCG-only (no KVM) interpreter-ICD frames were 160–197 ms — never plan against TCG [S17]; QEMU pacing numbers must state KVM-vs-TCG env [C23].
- LLVM: guest llvmpipe reports LLVM 20.1.0, host oracle LLVM 22.1.8; [S85] recorded one depth-bias test compared equal under its pixel-count metric (`diff=153600` for both bias signs); the 2026-10-22 control showed host LLVM 20.1.8 passes 109/109 while guest is 20.1.0 (same series, different patch) [S25] — the two compilers are allowed to differ (C18).

**Budgets (all provisional — set by M0 [S1 §4.1]; M0 measures: column "M0 must")**:

| Budget (v2 §4.1) | Provisional | M0 must measure |
|---|---|---|
| Triangles/frame | 3,000 [S1] | tri-rate at 320×256 with 6 animated Kitties, lavapipe, KVM reference target [S1 §11.2 M0 def] |
| On-screen Kitties | 6 [S1] | per-anim-keyblend vertex work + dither cost |
| Shaders | ≤ 6, design rule [C21] | pipeline JIT init total ms at startup (uncached [S21]) |
| Overdraw ≤ 2× | design [S1] | scene-queue/tiling cost vs overdraw — **no AROS tile-cost measurement exists** (research gap: NOT FOUND in wiki) → M0 sweep 1×/2×/4× |
| Frame time baseline ≤ 40 ms | D4 ref hardware [S1 §13] | frame-time log from H7 upload [S1 §8.4 #7] |
| Frame time enhanced ≤ 20 ms | D4 card [S1] | NVK lane (when granted; else BLOCKED, not fudged) |
| Resident ≤ 512 MB | lavapipe ~130 MB [S6] | app RSS + library resident (guest `Avail` census at steady state; **never** `Avail FLUSH` [C24]) |
| LP_NUM_THREADS effect | engine default 0 [rev 2 #20] | thread-count sweep 0/1/4 (semantics per [S25]: 0 = none, 1 = one worker) — SMP gain only if AROS SMP proven (bonus, never required [S1 §11.2 M6]) |

Fallback ladder order is [S1 §4.1] verbatim: cut texture sampling → 320×200 → cap clones at 4 → AROS-side work. Never free camera, never make NVK required [C20].

## 12. Milestone mapping

M0 is a feasibility spike plus v2's required NVK smoke run; it is not "pure" [rev 2 #1].

| M | Modules landed | Gates (from §10/§11) |
|---|---|---|
| M0 | `platform/aros` (window+pacing+input-drain+files), `render/` tier probe + minimal golden scene, `audio/` AHI stream + minimal MOD playback, host `platform/linux` mirror, benchmark-lite, **NVK smoke run of the same scene on the card lane (informational, lane-gated, BLOCKED is legal)** | H5, H6, H7 (no sim yet), M0 numeric go/no-go [S1 §11.2], 30-min resident stability [S1 §11.2 M0] |
| M1 | `sim/`, `snapshot/`, `input/`, `level/` + `tools/` (cc, solve, hash), greybox rooms (host only), Linux golden replays | H1, H2 (Linux side), H3 |
| M2 | M1 on AROS unchanged | H7 hash-equality on-AROS [C27], H2 both platforms |
| M3 | vertical slice Ch1: `assets/` skin v0, `ui/`, palette pass, save tree, disk `Boing!` (Boing physics toy reuses `sim/`) | H4 goldens, H7 frame-vs-golden, H8 first NVK attempt (lane-gated), human gate [S1 §11.2] |
| M4–5 | chapters 2–5, WINDOW/MUTEX/SEMAPHORE verbs (all `sim/` rules pre-existing), finale lock-ordering room | H3 per room (solver is per-room from M1 on), goldens |
| M6 | budgets §11 on both tiers, benchmark mode, replay camera, optional SMP render split | §11 measurements on reference target(s), bare-metal data points [S1 D4] |
| M7 | catalogs EN+DE, Aminet demo `.lha`+`.readme` [S1 §10], licensing checklist [S1 §2 tasks], 1.0 tag | locale scan, H1–H8 green on release build, tag+commit [S1 §11.1] |

Post-launch 1.1 hooks (validator+solver already shipped in tools by M1 — 1.1 is data+docs, not engine [S1 §11.3]).

## 13. Risks (platform risks the game does not control)

| Risk | Likelihood / state | Impact | Detection | Fallback |
|---|---|---|---|---|
| Force-expunge crash: boot lane died in TLSF double-free behind `LDFlush → Lddemon RemLibrary → UnLoadSeg → FreeMem → tlsf ud2` [S42][S43]; fix `940c8978` applied; runtime-confirmed **only on the direct RemLibrary path**; under `LDFlush` UNTESTED-INCONCLUSIVE [S44 2026-10-28] | live, upstream-adjacent (our loader-side fix, unproven end-to-end) | a dev-session flush or memhandler fires → boot dies mid-session; a 30-min game session never triggers it if we keep OpenCnt ≥ 1 | `T:Kitty.log` open-count line at startup + boot alert census; H7 30-min stable requirement [S1 M0] | game never flushes/unloads (C12); dev sessions reboot instead [C24]; if the boot still dies: treat boot grants as failed, never ship a flush path |
| NVK enhanced tier lane-only: 0/~30 nouveau patches upstream, upstream builds no NVK ICD [S9 2026-10-28]; D7 gates shipping the tier [S1 §13] | certain today (dated 2026-10-28) | enhanced tier is unshipable as a *player* path until upstreamed; card-lane-only playtesting | startup self-test + manifest enumeration (no `vulkan_nvk.library` in a stock build) | 1.0 ships lavapipe-only; NVK tier = point release [S1 D7]; engine self-test is the enforcement, not prose |
| NVK present-path never exercised on the lane (has-swapchain=0 in card enumeration [S7/S28]; NVK reached pipeline creation + depth oracle, not present) | open, recorded | enhanced-tier frame correctness is unmeasured end-to-end | H8 status line "BLOCKED: lane has no present" (honest state [S19]) | goldens for NVK deferred to when lane reaches present; baseline carries the ship |
| AROS SMP experimental, upstream-owned [S22 decision, 2026-09-26]; `test/smp` suite does not pass on port [S48 2026-10-15]; boot census `master_smp31` vs `master` doesn't boot [S48 2026-10-19]; lavapipe SMP sub-lane deliberately held; `-smp 4` default-mode hang matrix recorded [S6 2026-09-27] | active | multithreaded lavapipe rasterization on AROS is unproven-by-default; our game's SMP *theme* is fiction in `sim/` — safe | thread env pin `LP_NUM_THREADS=0` engine-set (subject to M0 probe); H7 logs thread mode with hashes [C27] | keep 0 workers; SMP gain is M6-bonus [S1 §11.2 M6 "bonus, never required"] |
| Mesa pin drift: ours pinned 26.2.2 [S47][S49]; upstream AROS moved to 26.2.5 with flag deltas [S6 2026-10-27]; upstream-repo Mesa is not ours [S48/S53-adjacent] | managed, real | host-CI vs AROS-guest divergence if pins ever skew | CI pin stamp in H4 (golden carries Mesa stamp; mismatch = FAIL); compare `AROS_PIN` + built-at stamp vs H7 upload | re-pin both sides together; oracle evidence (same-source bit-for-bit [S50]) only valid within a pin |
| GSP/card teardown wedge family [S27][S54][S48 correction] | active, lane ops | card-lane session management, not game | lane rules already [S54]: in-guest reboot, no signals | game never sees it; CI lane runs follow lane doc |
| AHI driver init alignment crash (movaps, `-mno-sse`-class fixes) [S34]; mixer selected-rate contract merged upstream [S36 PR #1518] | driver-side, fixed/known | audio init fails on some guests | AHI failure log + continue silent (§4.6 E) | audio non-fatal by design; benchmark mode runs silent-ok |
| Full-width window on 1280×800 guest desktop limit [S32] | active | window size decoupled from internal 320×256 + blit scale — engine already designed this way (§6.2) | M0 gate "window opens at target size" | 1232-class width trick / blit scaling [S32] |
| `lowlevel.library ReadJoyPort` availability on pc-x86-64 AROS **UNVERIFIED** [S58 autodoc raw, S72 "not implemented on all platforms"] | unknown | joystick input channel may not exist → M0 must probe all 4 ports (port numbers UNVERIFIED) | M0 probe gate: print per-port presence + rc [C23] | IDCMP via loader (already the input path [S31]); keyboard mapping stays legal-but-enhancement [S1 §5.2] |
| QEMU pacing distortion (TCG vs KVM; headless no-vblank) [S29, S17 TCG rows, C13] | managed | benchmark numbers are meaningless without env stamp | every timing log line carries env stamp (host CPU, KVM/TCG, threads) [C23] | sim never depends on wall-time (§3 loop rule) |
| AROS stdio/locale/iostream fragility class (newlocale→C-locale fix, libc++ iostream crash class [S47-adjacent src/abi-patches README records]) | avoided by design [C03/C17/C29] | — | kitty-lint include scan | — |
| `LP_NUM_THREADS` set before `vkCreateInstance`: does lavapipe in the same process read it? | **UNVERIFIED:** inferred from `vkpal_aros.c:720-726` (`SetVar(..., GVF_LOCAL_ONLY)`) and `__env.c:208-222` (`ScanVars` honoring `GVF_LOCAL_ONLY`) [S84] | if the ICD's `getenv` path cannot see the variable, lavapipe's worker-thread default applies instead of our 0 | M0 probe (§4.5): log the `GetVar` result and lavapipe's worker-thread count | attempt `SetVar` anyway (engine default 0); if the probe shows it is ignored, the mechanism is the owner's call (§15 Q12) — never a silent `ENV:` write |

## 14. Rejected alternatives

1. **Port an existing engine (Godot/Bevy/Fyrox/O3DE).** [S2 F1/F2]: porting Godot to AROS is "a second massive project that would consume all effort"; a small codebase with strong tests is the reviewable shape [S1 §8.1]. Kept small-codebase + tests as the actual argument (the v1 "C++ is the most verifiable workflow" claim was rejected [S2]).
2. **volk.** `dlopen` does not exist on AROS [S2 tech-errors note]; the AROS loader *is* the dispatch boundary (stock `vkGetInstanceProcAddr` shim opens `vulkan_loader.library` [S60][S10]); a small static dispatch table is our path [S1 §8.2 "no volk"].
3. **Vulkan-Hpp.** Heavy template instantiation, compile-time and LLM-review cost for ~40 entry points [S2]; plain PFN typedefs + explicit structs (which the AROS chain already bit-validates against stock create-infos [S60]) is enough.
4. **Float simulation.** Divergence class: glibc-vs-AROS libm differences and `-ffp-contract`/FMA drift [S2 determinism note]; **UNVERIFIED:** no measured AROS-vs-glibc `sinf` delta on record in VK (NOT FOUND terms: sinf/libm precision) — the *fix* is by construction (integer sim + hash CI [C27][C06]) and does not depend on that claim.
5. **Free 3D camera.** v1 had no camera decision; free camera is the classic failure mode with clones on screen and is fill-rate hostile on the CPU tier [S2 serious-problems; S1 §4 fixed isometric flip-screen]. Fallback ladder forbids it even under perf pressure [S1 §4.1].
6. **SDL2 on AROS for windowing.** SDL2's window would be a *second* window; the loader's `vkCreateArosSurfaceKHR` opens the one Intuition window and input already flows through its event drain [S10][S11][S31][S2]. SDL2 stays Linux-only for the same interface [S1 §8.2]. (SDL2 AROS port exists [S48 2026-07-17 record] — available, not adopted.)
7. **Native-code mods / community native mini-games.** AROS has no memory protection: contributed native code *is* the machine [S2 F7]; v2 allows data-only packs [S1 §9.2]; an interpreter is a 1.x+ item [S1 §9.2].
8. **Interpreter ICD as baseline.** Most-CTS-tested but ~160–197 ms/frame measured [S17]; v2 names lavapipe baseline [S1 §1] [S2 F2 "three Vulkan paths" analysis].
9. **AREXX macro recording / WHDLoad pack format / `.adf` demo.** Cut upstream by [S2] (macro: replays cover it; WHDLoad: m68k convention nobody can use on x86 AROS; ADF: an x86-64 build cannot boot [S2 internal-inconsistencies]) — v2 keeps replay files and Aminet `.lha` [S1 §10].
10. **Blit-based presentation instead of Vulkan on AROS.** The cybergraphics `WritePixelArray` blit exists [S12] but is the *present step inside* the WSI path, not an engine tier; going blit-native would forfeit the Vulkan skills/hardware tier and the CI oracle match [S50].

## 15. Open questions for the owner

Numbered, each with a recommended default; v2 §13 decisions (D1–D7) are referenced, never re-opened. `superpowers:brainstorming` was unavailable (see the sources file note) — the questions an owner-interview would have asked are recorded here instead. Q11–Q14 carry the engine-cap and mechanism questions this revision fixed in the body; the defaults below now match the body.

| # | Question | Default if undecided |
|---|---|---|
| Q01 | `LP_NUM_THREADS` policy on AROS given the SMP hold [S22] | 0 (render in caller) subject to M0 probe of 0 vs 1; benchmark mode may *test* other values, never default them |
| Q02 | Joystick physical input source on AROS (ReadJoyPort unverified [S72]) | M0 probes all ports; if absent, ship IDCMP-keyboard first pass and mark joystick "enhancement pending hardware" — v2 §5.2 requires joystick sufficiency, so a failing probe *blocks* M3, not M0 |
| Q03 | NVK golden strategy while the lane has no present path [S7/S28] | ship baseline goldens only; NVK goldens recorded from the lane when present works; H8 stays BLOCKED-with-reason |
| Q04 | State hash primitive | FNV-1a 64 [S78] (spec is one page; no collision-free pretence — "same log same hash" not "unique hash"); manifests use SHA256SUMS [S1 §9.2] |
| Q05 | MOD replayer provenance | own ~1.5k-PT4-subset replayer from fetched specs [S68–S70] with the UNVERIFIED-details list (§7) settled against an Amiga spec copy before M3; *do not* import micromod-class code before its licence is verified (licence check UNVERIFIED this pass) |
| Q06 | Catalogs: engine `.ct` reader vs AROS `locale.library` runtime [S59 unverified formats] | engine reader both platforms; a boot probe offering locale.library at M3; never both code paths required |
| Q07 | Snapshot strategy at scale | Default: full keyframes per the §5.3 BUDGET line until M6 says otherwise; Braid-style keyframe+delta compression [S75] only if a measured budget miss |
| Q08 | apiVersion request from loader chain | 1.3 floor even though manifest is 1.4.0 [S48]; CI probe gate re-checks against pinned 26.2.2's `features.txt` (mirror-read caveat [S63]) |
| Q09 | Who owns boot grants for H7/H8 runs (lane budget: SPENT boots recorded [S44]) | spike agent per milestone, budget stated per run; BLOCKED is a legal CI state (honest [S19]) |
| Q10 | Art/music ownership | v2 D5 [S1 §13] — must be named before M3 [S1 §11.2]; not an engine decision, listed for completeness |
| Q11 | 23k engine cap vs 22.5 k of module budgets | Decided 2026-10-29: the owner raised the cap to 23k so `shell/`, `replay/` and `benchmark/` stay in 1.0; §1 records the decision and §4 keeps the cap binding |
| Q12 | `LP_NUM_THREADS` mechanism: which way does the value actually reach lavapipe? | Default: `SetVar("LP_NUM_THREADS", "0", -1, GVF_LOCAL_ONLY)` before `vkCreateInstance` (§4.5) [S84]; the M0 probe reports whether the same process reads it; if not, the owner picks the mechanism (§13 risk row) |
| Q13 | NVK self-test tolerance numbers | Default: per-channel ±4/255 + coverage-mask equality; set at M0 from the first NVK run |
| Q14 | Two-slot save: is slot + generation counter + checksum enough against a torn write? | Default: two-slot scheme regardless; Inferred from the handler sources: `Rename` onto an existing destination fails with `ERROR_OBJECT_EXISTS`, raised by the filesystem handler, not by `rom/dos/rename.c`; saves therefore use two slots instead of replace-by-rename [S90]; the owner decides at M6 whether a third slot is wanted |

## Appendix R — Revision 5 changelog

One row per finding, pointing at the section that owns the decision. The row is an index, not the decision itself.

| id | decision | section |
|---|---|---|
| R1 | Instance extensions: nothing WSI requested on AROS (the loader adds `VK_KHR_surface`/`VK_KHR_xcb_surface` only when the ICD advertises them); the Linux list comes from `SDL_Vulkan_GetInstanceExtensions`; the NVK self-test instance has zero extensions | §6.1 |
| R2 | Room grammar in one EBNF block (header, seed, par, grid, spawn, objects, exit); terrain-only tiles; `TICKS_PER_CELL = 8`; one validating example with a matching Hand-check line; five complete negative rooms with the exact Guru lines | §8.1 |
| R3 | `LP_NUM_THREADS` is set by a process-local `SetVar` before `vkCreateInstance`; whether the same process reads it is UNVERIFIED with an M0 probe | §4.5 |
| R5 | Every `World` mutation that is not an `InputLine` is a logged `Event` carrying its tick, applied as Step 0 of that tick | §5.1 |
| R6 | `platform/rt` provides memcpy/memmove/memset/memcmp and __cxa_pure_virtual as freestanding C; H6's known-bad links without rt and must fail on undefined memcpy | §9 |
| R7 | No STL anywhere in engine code (platform included); `tools/` are separate host binaries and may use STL | §4.2 |
| R8 | `ForkRec` is the append-only input buffer outside `World`; `World` holds each recording's length, cursor and clone spawn position | §3.1 |
| R9 | One LB per module heading, a matching table after the `LB-TABLE` marker, engine total 22.5 k (excl. `tools/`), one binding 23k cap sentence, and one Q row recording that decision | §4 |
| R10 | What [S85] measured is stated as one depth-bias test with a pixel-count metric; golden↔host CI is bit-exact, guest↔golden is a proxy check whose tolerance M0 sets, and the guest's compiler may differ from the host's | §9 |
| R11 | Four 960-frame `CMD_WRITE` requests form an 80 ms queue serviced in the tick loop; underruns are logged with their tick; deeper chains are UNVERIFIED with an M0 xrun probe | §4.6 |
| R12 | `VK_ERROR_SURFACE_LOST_KHR` recreates the surface through the loader once, else Guru + exit; never a tier fallback | §6.2 |
| R13 | §4.5's known-bad (c) defers to H5, which owns the validation-layer ordering case and its named VUID | §4.5 |
| R14 | `InputLine` is inputs only (the two-byte wire layout lives in §5.1); all tick counters are `u32` | §5.1 |
| R15 | No RNG draws in 1.0; the generator stays pinned for future use and any draw needs a v2 change | §5.1 |
| R16 | Keyframe = `serialize(World)` only (N = 10296 B from §5.5); fork record = `sizeof(ForkRec)` (F = 1000 B, inputs only); §5.3 carries the BUDGET line; no KB-style figures | §5.5 |
| R17 | One save format: two slots + generation counter + checksum; `ERROR_OBJECT_EXISTS` is raised in the filesystem handler's rename action, not by `rom/dos/rename.c` | §4.12 |
| R18 | `guru/` owns the Guru screen, `ui/` calls `guru_render`; §4.10 has exactly one I bullet | §4.10 |
| R19 | Linux presents through an offscreen image copied to a host buffer; AROS CI reads back `RECTFMT_BGRA32` at 320×256 scale 1, else the gate is BLOCKED | §6.6 |
| R20 | `AROS_PIN` is cited from the file, never hardcoded | §9 |
| R21 | No source id appears twice in one citation cluster | §9 |
| B1 | Every C++ block under §3 compiles as C++20 with -Wall -Werror -Wredundant-decls | §3.1 |
| B2 | Every module has exactly one R/I/D/T/E/A/Test bullet, and each I bullet names backticked functions that §3 declares | §4 |
| B3 | The tick order starts with Step 0, which applies the tick's `Event`s sorted by (kind, a) | §5.1 |
| B4 | No RNG draw occurs in 1.0 | §5.6 |
| B5 | `operator new`/`delete` are traps that call `guru_report` and exit, caught by a link-time check; the lint table also covers namespace-scope objects with non-trivial constructors, platform included | §9 |
| B6 | Line 1 is the revision-5 title and no earlier-revision leftovers remain | §1 |
| B7 | Shaders are capped at 6; pipelines are counted separately (5 pipelines cover the shader set) | §6.3 |
| B13 | `kitty-lint` keeps its rule table (pattern, scope, rationale, must-fail fixture) inside §4.13 | §4.13 |
| B15 | §4.10 keeps the ui/ → event routing table | §4.10 |
| B16 | Skins swap only at room boundaries, atomically at the next room load | §4.8 |
| A1 | `ahi.device` opens on `AHI_DEFAULT_UNIT` (0) with a full `struct AHIRequest`; `AHI_NO_UNIT` is never used (it leaves `io_Unit` NULL and `CMD_WRITE` dereferences it) | §4.6 |
| A2 | The `InputLine` two-byte wire layout is defined in §5.1 only; §3.1 declares the struct and §4.7 points at §5.1 | §5.1 |
| A3 | v2's engine size target is unchanged; the owner raised the cap to 23k on 2026-10-29 so `shell/`, `replay/` and `benchmark/` stay in 1.0 | §1 |
| A4 | The example room's interior wall forces the requester door: with every GATE cell closed WIN is unreachable from SPAWN, and PAR declares the single verb-commit the solution needs | §8.1 |
| A5 | The fork-moment `World` is one `fork_base` keyframe outside the ring; `ForkRec` holds 500 ticks (v2's 10 s FORK cap); BUDGET totals 221920 B | §5.3 |
| M1 | MOD facts restated from the ProTracker sources: `Fxx` speed 00–1F (00 ignored), tempo 20–FF (32–255 BPM), Paula-clock pitch, `Cxx` volume 0–64, signed 8-bit samples widened at load | §7 |
| M2 | `audio/` is single-threaded and serviced from the tick loop; it never feeds `sim/` and is never hashed | §4.6 |
| M3 | `VK_EXT_debug_utils` is requested only when enumerated, debug builds only | §6.1 |
| M4 | §4.5's `E:` bullet points at the §6.2 VkResult table | §4.5 |
| M5 | The §3 frame list defers to §5.1's per-tick order, starting with Step 0 | §3 |
| N1 | A `current/` that fails to load is discarded with a log line; progress loads from the newest valid slot | §4.12 |
| N2 | `qdiv` saturates and records `SIM_OVF` when the quotient leaves the `int32_t` range | §5.1 |
| N3 | The `SEED` hex value is zero-extended to 64 bits as the splitmix64 state | §5.6 |
| N4 | The LB total row is labeled `excl. tools/` and totals 22.5 k | §4 |
| N5 | No source id appears twice in one citation cluster | §9 |

*Revision 5 — end of document.*
