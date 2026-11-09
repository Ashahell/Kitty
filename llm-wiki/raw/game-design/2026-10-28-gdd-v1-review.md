# Review of GDD v1

> Source: Work/Kitty/Docs/Development/kitty_core_awakening_review.md
> Collected: 2026-10-30
> Published: 2026-10-28

# Kitty: Core Awakening — Design Review

*Review of `kitty_core_awakening_design.md`. The revised document is `kitty_core_awakening_design_v2.md`. The original is unchanged.*

---

## Verdict

The tone, the theme and about a third of the mechanics are excellent. **As a plan, though, the document can't be built.** It describes four to six games: a 3D puzzle-platformer, ten arcade mini-games, a life sim, a moderated user-generated-content platform, a networked social layer, and a speedrun/benchmark suite. It sets this against a deliberate "cut nothing" policy. It targets "the team's Vulkan port" without saying which of the three ports it means. The one most players can run is a **CPU rasterizer (lavapipe)**, and the hardware driver (NVK) has so far run on exactly one card under passthrough. Its central teaching promise ("the story teaches the OS") breaks at the climax, because the climax teaches the opposite of the correct concurrency lesson.

The document reads like several brainstorming sessions concatenated without an editing pass. The leftover `godot --headless --check-only` in §11, in a document that rejects Godot in §8, is the clearest sign of that.

If it were built as written, the most likely outcome is a technically impressive engine, one polished room, and a stalled project.

---

## 1. Fatal problems (fix before writing code)

### F1. "Cut nothing" is the core failure
§11 lists five major systems and tells you to "cut nothing, but gate hard". That isn't scope management. Count what the document actually commits to:

| System | Realistic size on its own |
|---|---|
| 3D puzzle-platformer, 5 worlds, ~13 verbs | A full game |
| 10 mini-games (3 at launch) | 3–10 small games, each with its own physics, AI and tuning |
| Sims-style playground (needs, routines, build mode, zoning, economy) | A full game |
| UGC pipeline: editor, packaging, validation, sandboxed scripting, review/voting, reputation, revocation | A platform |
| Network visits, shared clones | A backend |
| Shell, AREXX macros, benchmark mode, leaderboard, 4 localizations, Second-Kitty arc | Each is weeks of work |

The ~10–20k-line engine estimate (§8) is plausible for the engine alone. The game on top of it would be 5–10× that. **The v2 document cuts to one shippable game. Everything else becomes a numbered post-launch update with entry criteria.**

### F2. The platform reality is missing, and it changes everything
The document says "the team's Vulkan port" as if it were one finished thing. As of 2026-10-28 there are **three Vulkan paths on AROS, at very different maturity**, and the document names none of them:

| Path | What it is | State (Vulkan4Aros wiki, 2026-10) | Who can run it |
|---|---|---|---|
| **Interpreter ICD** | Own SPIR-V VM + software rasterizer | Most CTS-tested; slowest by far | Any AROS x86-64 |
| **Lavapipe** | Mesa, LLVM-JIT, multithreaded CPU rasterizer | Renders `conformance_cube` on a KVM guest (300 frames in 320 ms); ~130 MB library; boot lane dies in a force-expunge double free; CTS lists still owed | Any AROS x86-64 with enough RAM |
| **NVK** | Mesa hardware Vulkan on AROS's Linux 6.18 nouveau + GSP-RM | RTX 5070 Ti (GB203) **enumerates**, pipelines create, depth-bias oracle passes and matches the Linux oracle bit for bit. Tested on **one card, under QEMU+VFIO passthrough only**; GSP release wedges; every boot is a gated resource | AROS on bare metal with an NVIDIA **Turing or newer** card, once it's stable |

As a result:

- **Hardware Vulkan is real, but it's not a target yet.** No one has run a game loop on NVK on AROS, no bare-metal boot is recorded, and the audience of AROS users who also have a Turing+ NVIDIA card on bare metal is a fraction of a few thousand. NVK is the *enhanced* path. **Lavapipe is the baseline**: it runs everywhere AROS x86-64 runs, including QEMU, which is how most people run AROS.
- **M0 ("triangle on AROS") is already done** on all three paths, so the milestone validates nothing new.
- **The real M0 is a performance spike on lavapipe**, plus a smoke run on NVK. The whole design depends on the result: how many triangles, at what resolution and fill rate, with how many animated Kitties on screen at what frame rate. Free 3D cameras, "fuller lighting on later cores", and "many Kitties in parallel" in the finale are all unbudgeted.
- **Design for the CPU baseline, then scale up on hardware.** 320×256 (PAL lo-res), integer-scaled, flat or Gouraud shading, nearest-filtered textures, and 25/50 fps (PAL field rates) are the baseline. On NVK the same scenes can render at higher internal resolution and 50 fps. The low-res look is now a style choice that hardware keeps, not a crutch.
- **CI gets much stronger than the document realizes.** Lavapipe and NVK are upstream Mesa, and the project already proves the Linux oracle of the same source matches the AROS guest bit for bit. Run the engine's render tests on **host lavapipe built from the same Mesa version as the AROS build**, with golden images. That is a near-exact proxy for the AROS baseline. A generic Linux GPU-driver pass still proves nothing about AROS.
- **Ship-blocking platform bugs belong in the game's risk register.** The lavapipe boot lane currently dies when a resident 130 MB library is force-expunged. The game must keep its Vulkan library open for its whole lifetime and must never trigger `Avail FLUSH`. Until that crash is fixed upstream, it's a release blocker that the game team doesn't control.
- **AROS SMP is still experimental.** Lavapipe rasterizes on pthreads, so it benefits from SMP, but a game whose theme is SMP, and whose "M6 champagne rule" is an SMP job system, must be correct and fast enough on one core. More cores are a bonus.
- **Headless or remote QEMU has no vertical blank, so `WaitTOF()` hangs forever.** Frame pacing must use `timer.device`.

### F3. The concurrency teaching is wrong where it matters most
The thesis is "the story teaches the operating system". Amiga and AROS developers are exactly the audience that will notice the mistakes:

| Doc says | Reality | Consequence |
|---|---|---|
| Before the Panic, citizens "queued at the Scheduler, one task at a time"; Exec is "single-threaded" | Exec has been a **preemptive multitasking** kernel since 1985. That is the Amiga's founding boast. Multitasking ≠ multiprocessing. | It insults the platform's proudest feature in the opening premise |
| FORK = a clone that replays your recorded actions | fork() copies the current state and both continue independently | Acceptable as a game abstraction, but it needs honest framing |
| JOIN = merge with a clone, keeping its position and cargo | join = **wait** for a thread to finish | The verb teaches the wrong meaning |
| SEMAPHORE = "order clones' actions; sequence switches rhythmically" | A semaphore is a **counter** that admits N holders. Ordering is a barrier or condition variable. | Wrong |
| **Finale: Kitty beats Deadlock by forking across every core, overwhelming the wait cycle** | You can't resolve a deadlock by adding threads. More contenders make a circular wait *worse*. Deadlock is broken by lock ordering, timeouts or preemption. | **The climax teaches the opposite of the lesson.** This is the most important fix in the document. |
| 68k Core grants "basic movement" | Not a power, and nothing is learned | Chapter 1 has no concept to teach |

The v2 document maps every verb to the real concept and rewrites the finale: brute-force forking visibly makes the deadlock worse, and the player wins by imposing lock ordering.

### F4. The control scheme contradicts the moveset
"Joystick-first: one button + directions" (§8) sits beside more than 13 verbs: run, jump, crouch-hold, double-tap pounce, contextual defrag, fork, join, mutex, semaphore, grab/drag window, send to back or front, drag between Screens, choose requester buttons, subclass. That doesn't fit on one button. Double-tap pounce also conflicts with digital-joystick running. Either drop the claim or design for it. v2 cuts the verb count, makes most verbs **places** (pads, pedestals, gates) instead of buttons, and puts thread commands on a hold-fire pie menu.

### F5. Legal exposure is much larger than §2 admits
- **The ReBoot layer isn't "texture-level".** Binomes (ReBoot's coined term), descending Game Cubes, "GAME OVER — USER WINS", and a robed archivist who shouts **"Alpha-numeric!"** (Phong's catchphrase, verbatim) together form a recognizable derivative of a commercial property. It also adds nothing the Amiga theme doesn't already supply better. **Replace it wholesale** with Amiga-native equivalents: Guru Meditation alerts instead of Game Cubes, and original citizens.
- **Tetris:** a falling-tetromino game invites trade-dress enforcement (*Tetris Holding v. Xio*, 2012). Cut it.
- **"Turbo Lotus"** uses a live trademark. **"Crown Defender"** inverts *Defender of the Crown*, which is the kind of near-name courts dislike. Rename all homages so the names point at no specific title.
- **Fred Fish was a real person** who died in 2007. Making him an in-game character who is "missing since the Great Panic" and "always on the dock" raises right-of-publicity and plain-decency questions. Either get the family's blessing or keep the person off-screen: a dedication, the disks, and an empty chair on the dock. v2 defaults to off-screen.
- §2 states that Kitty and the AROS name **are trademarks**. Verify that before asserting it. Get permission from both the AROS team **and** the Kitty artist, in writing, before any public build that uses the artwork.
- Real Fred Fish disk numbers (FF #001–#1120) had real contents. Labelling them with fake contents is mildly misleading to the exact audience that remembers them. Use numbers above 1120 ("the disks Fred never got to release"), which also makes a better story.

### F6. The reward economy has no destination
Story-mode Game Cubes pay out "cycles, bytes, hardware-trinket furniture" into a playground economy. The playground is post-story (§7), and §11 says it "can slip to post-launch". If it slips, the story's whole reward loop pays into nothing. The fail state ("nullified binomes until restored from backup") is also a punitive meta-penalty in a game that wants cozy crossover appeal. v2 makes story rewards self-contained: disks become playable cabinets in a hub. The playground can be added later without breaking the story.

### F7. The UGC plan is impossible as written on AROS
- **AROS has no memory protection.** A contributed native mini-game runs with the same privileges as the whole system, so a "hard sandbox" for native code doesn't exist on this OS. Sandboxing is only possible inside an interpreter you write, with no FFI and with step and memory limits. "Whole mini-games" as contributions must be data plus interpreted script, never native code.
- `checksum.sig` conflates a checksum (integrity) with a signature (authenticity, which needs key infrastructure). Revocation needs a server. Voting needs a server. Reputation needs a server. Each of these contradicts "single-player never depends on the network" in spirit, and AROS networking (AROSTCP) is fragile: non-blocking connect panics, IPv6 ND crashes.
- **The community already has its distribution channel: Aminet.** The document celebrates Fred Fish and never mentions Aminet, the living successor of that culture. Use it: `.lha` archives, `.readme` with `Short:/Author:/Type:` headers, curated index files. That costs nothing and needs no server.

---

## 2. Serious problems

**Two competing structures.** §3.2 has four chapters named after Kickstart versions, and §3.4 has five Cores named after CPU architectures. They don't map onto each other, and the art-direction progression is defined twice: per Kickstart era, and per Core ("engine visibly evolves"). v2 uses one spine with five chapters, one per Core and concept, each with its own era palette.

**Historical errors the audience will catch:**
- "Kickstart 1.0 — near-monochrome, hardware beeps" and "1.3 — sampled audio". The Amiga had 4-channel 8-bit **sampled** audio (Paula) from the A1000 in 1985, which was its other founding boast. The 1.x Workbench was 4 colors (blue, white, black, orange) in both 1.0 and 1.3.
- AROS isn't Kickstart. In-world that's fine (AROS m68k does ship as a Kickstart-replacement ROM), but say it knowingly.

**The PPC Core as "the lost path" is community politics.** The surviving Amiga-like world is split between AmigaOS 4, MorphOS (both PPC) and AROS. Casting PPC as a "fractured mirror-world, the lost path" alienates part of an already tiny audience. Reframed, it's a gift: Amiga history literally **forked**, both branches are still alive, and the PPC Core is where Kitty learns FORK. v2 does this.

**Too many verbs, and they interact badly.** Recorded clones interact with time-pause ("cat on keyboard"), 9-second rewind, Defrag (which moves tiles that recordings depend on), window depth toggling, Screen transfers and requester resets. Each pair needs defined behavior, or clones desync and puzzles break. Nothing in the document addresses this. v2 cuts Pounce (combat isn't needed) and Defrag (it overlaps window manipulation and wrecks recordings), defines an interaction matrix, and gives every verb a real OS name: cat-on-keyboard becomes `Forbid()`, which is the actual Exec call that stops task switching. That alone teaches more than the original's entire moveset.

**Camera.** A free 3D camera is the main failure mode of 3D puzzle-platformers. With several clones on screen it gets worse, and on the CPU baseline (lavapipe) it gets worse again. The document doesn't mention cameras. v2 uses fixed-angle isometric flip-screen rooms, in the Amiga isometric tradition. Readability improves, performance improves, puzzle design gets easier, and each room can literally be an Intuition **Screen**.

**Determinism is underspecified.** "Fixed timestep, seeded RNG" isn't enough when recordings are shareable files and the game runs on both Linux and AROS. The libm implementations (glibc vs AROS) return different `sinf` results, and `-ffp-contract` and FMA differences between host and cross compilers will diverge float simulations. Gameplay simulation must use fixed-point or integer arithmetic with no libm calls, and a CI test must prove identical state hashes for the same input log on both platforms.

**Tech-stack errors:**
- **miniaudio has no AHI backend.** On AROS you'd write a custom backend or go through SDL2, which uses AHI. libopenmpt is a large C++ dependency for playing 4-channel MODs. A ~1–2k-line ProTracker replayer (your own or micromod-class) is smaller, deterministic, and makes per-channel muting trivial.
- **volk loads via `dlopen`, which AROS doesn't have.** Use `volkInitializeCustom(vkGetInstanceProcAddr)` with the existing `vk_aros_loader.c` shim, or skip volk and keep a small dispatch table.
- **Vulkan-Hpp** adds heavy template instantiation and compile time, and no LLM-verifiability, for a project that will use about 40 Vulkan entry points. Drop it.
- **SDL2 and the Vulkan surface both want to own the window.** On AROS, `vkCreateArosSurfaceKHR` (in `vulkan_loader.library`) opens its own Intuition window, and input comes through `vkPollArosEventsKHR`. SDL2's window would be a second window. The AROS platform layer should own the window through the loader, using SDL2 only for audio and joystick if at all.
- **C++20 on the AROS gcc-16 toolchain works but is young.** Exceptions only recently stopped crashing because of an unwinder bug, and r12 reservation miscompiles have occurred. Use a conservative subset: no exceptions, no RTTI, no `std::thread`, no iostreams, no static constructors with side effects. The mandatory flag set must be enforced in the build (see v2 Appendix A).
- "C++ has the most verifiable LLM workflow that exists" is an unsupported claim. Leave it out. The real argument, a small codebase with strong tests, is enough.

**Milestones put fun last and contain no human gates.** The first moment anyone learns whether fork/join puzzles are *fun* is M3. Every acceptance criterion is machine-verifiable, but fun isn't. v2 builds a greybox puzzle prototype on Linux in M1, adds human playtest gates, and makes the M0 go/no-go numeric.

**"LLM-first" is silent on the hard parts.** LLMs are strong at engine code, tooling, tests, data formats and level scripting. They're weak at 3D models, animation, pixel art, MOD composition and puzzle *quality*. The document never says who makes the art or the music. v2 states it, and adds something that plays to LLM strengths: a **puzzle solver** (search over the deterministic sim at grid level) that machine-verifies every room is solvable and has no unintended shortcut.

**Audience honesty.** "AROS only" combined with "crossover appeal via cozy-Sims and puzzle-platformer audiences" is a contradiction. Those audiences don't run AROS, and the active AROS user base is a few thousand people at most. That's fine for a love-letter project, but pick one. Because a Linux build has to exist for CI anyway, shipping it is nearly free. v2 makes AROS the lead platform and leaves the Linux release as an explicit open decision.

**Internal inconsistencies:**
- Moveset "frozen until story ships" (§4.1) vs "frozen until M3" (§11).
- `godot --headless --check-only` in a custom-engine plan (§11).
- Two climaxes: the fork swarm wins (§3.4) and the Contribs arrive and win (§3.5).
- Game Cubes are random district events (§5) but also the contents of collectible disks (§6).
- "Screen" means a tile screen in Defrag (§4.1) and an Intuition Screen (§4.3).
- M7 requires "one community-authored disk accepted end-to-end", which implies a community before launch.
- Moderation and revocation are "required before launch" while the community repository is "optional".
- The `.adf` demo "fitting on one floppy": an ADF is an 880 KB Amiga floppy image. An x86-64 AROS binary plus engine plus assets won't fit, and x86 AROS can't boot it. The joke works as a `.lha` on Aminet, or as an m68k teaser. It doesn't work as stated.
- "Localization: small cost". Not for pun-dense writing in four languages. Use AROS `locale.library` catalogs (`.cd`/`.ct`), which is the native mechanism and a nice touch, and budget it honestly.

---

## 3. What's genuinely good (kept in v2)

- **The design thesis**: teach the OS, celebrate the culture, built by many hands.
- **Requester gates** (`[Retry]` / `[Cancel]`, with Cancel always costing something). This is the strongest original mechanic in the document. Promote it.
- **Window depth as a 3D verb** (back = ghost, front = solid). Second strongest, and readable from an isometric camera.
- **Fred Fish disks as collectibles, where collecting the disk is collecting the arcade.** A perfect loop.
- **Guru Meditation as the build log** for contributions. A perfect joke that's also a feature.
- **Nine Lives as diegetic rewind.** It shares a snapshot system with clones, replays, demo mode and saves: one system powering six features.
- **Determinism at M2**, before anything else is built on top. Correct instinct; in v2 it moves even earlier.
- **Swappable placeholder assets from day one.** Correct.
- **ENV:/ENVARC: as settings, the Shell as fast travel, saves as a visible directory tree.** Cheap, native, delightful.
- **Scarf stripes, the AUTHORS scroll, READMEs written by a previous Kitty.** Great texture.
- **MOD soundtrack with per-channel dynamic muting.** Authentic and cheap.

---

## 4. What v2 changes

1. **One game ships.** Five chapters, eight verbs, three mini-game disks, a hub. Playground, contributions, subclassing and the arcade expansion become numbered post-launch updates. Networking and leaderboards are cut.
2. **Platform-first design.** Lavapipe is the baseline and NVK the enhanced tier. 320×256 internal resolution, isometric flip-screen rooms, explicit performance budgets, and a lavapipe feasibility spike plus NVK smoke run as M0.
3. **Correct concurrency.** The Exec premise is fixed, every verb maps to its real concept, and the finale is won by lock ordering, with the Contribs as the agents that make ordering possible. One climax, and the correct lesson.
4. **ReBoot layer replaced** with Amiga-native fiction: Guru alerts, original citizens, an Aminet-voiced Archivist.
5. **Legal cleanup.** Renamed homages, no tetrominoes, Fred Fish off-screen by default, and trademark claims turned into verification tasks.
6. **Real tech stack.** Window ownership through the AROS loader, AHI audio, own MOD player, fixed-point simulation, host-lavapipe golden-image CI, enforced AROS compiler flags, `timer.device` pacing.
7. **Milestones with go/no-go numbers and human playtest gates.** A puzzle solver verifies every room.
8. **Open decisions listed explicitly** instead of hidden in assumptions.
