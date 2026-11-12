# Knowledge Base Index

## project

What Kitty: Core Awakening is, where its files live, and its standing working rules.

| Article | Summary | Updated |
|---------|---------|---------|
| [Kitty: Core Awakening — project overview](project/kitty-project-overview.md) | Scope, status (design-only), file locations, working rules; licence conflict MIT vs GPL-3.0-or-later marked Disputed | 2026-10-30 |

## game-design

The game design documents (GDD v1, its review, and v2).

| Article | Summary | Updated |
|---------|---------|---------|
| [Game design v3](game-design/gdd-v3.md) | Current GDD: v2 plus per-chapter look levels L1–L4, budgets per level, cutscenes/clips, D8–D9; engine named CopperForge (game-agnostic) | 2026-10-31 |
| [Game design v2](game-design/gdd-v2.md) | GDD v2 (superseded by v3 for presentation): story, verbs, chapters, decisions and milestones | 2026-10-31 |
| [Art bible](game-design/art-bible.md) | Visual rules v2: per-chapter look levels L1–L4, palette slot map, characters, UI, cutscenes/movie clips; asks engine rev 6 for E1/E2/E3 | 2026-10-31 |

## engine

The custom C++20 engine as specified by engine design rev 5 (patched).

| Article | Summary | Updated |
|---------|---------|---------|
| [Engine architecture](engine/engine-architecture.md) | Module layout, 23k line cap, dependency stance, milestone mapping | 2026-10-30 |
| [Determinism and snapshots](engine/determinism-and-snapshots.md) | Fixed tick, InputLine layout, keyframe ring, FORK snapshots and memory budget | 2026-10-30 |
| [Renderer](engine/renderer.md) | Vulkan 1.3 two-tier (lavapipe/NVK), instance-extension rules, VkResult classes, golden images | 2026-10-31 |
| [Audio: AHI device path and modplay](engine/audio.md) | AHI_DEFAULT_UNIT device path; modplay facts; on the guest: AHI links double-buffer only (4-deep chain stalls), fixed by two requests in flight (D89) and 40 ms requests (D91): no xruns in M0b.21; D93 audio-time bound | 2026-11-12 |
| [Room format](engine/room-format.md) | Text room grammar, kitty-cc/kitty-solve, TICKS_PER_CELL = 8, example room rules | 2026-10-30 |
| [Saves and settings](engine/saves-and-settings.md) | Save and settings file formats, atomic write and rename rules | 2026-10-30 |

## platform

AROS and Vulkan4Aros facts the engine depends on, checked against source.

| Article | Summary | Updated |
|---------|---------|---------|
| [AROS platform facts the engine depends on](platform/aros-platform-facts.md) | Loader WSI (LVO 64/72/80), v1 clang lane, SDK gaps, GetUpTime vs C14, r12 pattern, AHI, SetVar; l0025: `DRIVERS:` on a freed lock (cliinit.c) and the HIDD preload, `PREPEND` inserts second, guest timer ~3% slow, `Assign` listing fault and hangs | 2026-11-12 |

## process

How the engine design was produced, reviewed and verified, and what is still open.

| Article | Summary | Updated |
|---------|---------|---------|
| [Engine design revision history (rev 1–5)](process/engine-design-revision-history.md) | Verdicts and headline defects per revision; rev 5 accepted and hand-patched | 2026-10-30 |
| [opencode dispatch lessons](process/opencode-dispatch-lessons.md) | What went wrong in dispatches to opencode and the prompt rules that fixed it; M0b gate-pattern lessons; M0b.1 ledger and transport lessons; M0b.2 census-completeness lessons; M0b.3 reference-pinning and runtime-gate lessons; M0b.12/M0b.13 boot-grading and log-what-loaded lessons; M0b.17–M0b.21 report-rule lessons, DSH sandbox (owner boots, phantom ledger line, report in pieces), no owner dispatch edits mid-round | 2026-11-12 |
| [M0 implementation status](process/m0-implementation.md) | M0a/M0a.1 results, commit 38fc422, M0b result (loader trap, 2 boots spent), the M0b.1 fix prompt (D32–D38) its round (3 boots on a grant of 2, runner server crash) the M0b.2 prompt (D39–D42) and its round (r12 fixed; boot-path server start still broken) the M0b.3 prompt (D43–D46) and its round (ledger check dead, guest not stopped) the M0b.4 prompt (D47–D50, dry boot) and its round (runner kills port-9099 listeners, D41 dead in real mode) the M0b.5 prompt (owner-supplied runner, real-mode gates) and its round (tree restored mid-round, script 0 hash edited, D47 Makefile reverted), the M0b.6 guest prompt (2 boots, corrected runner, script pins) and its round (11 scripts edited, checks deleted, 0 boots) and re-dispatch (clean round, guest faulted in the prelude, S12c pins wrong), the M0b.7 prompt (1 boot, prelude as separate execs, wedge exit) and its round (reached Vulkan on the guest, file reads broken by Seek misuse); the M0b.8 audit and round (first correct guest frames, NULL CreateSwapchainKHR, blank frame 0); the Vulkan4Aros loader fix prompt; the M0b.9 prompt and round (fixed loader on DH1, first swapchain and correct presented pixels, audio fault, no soak); the M0b.10 prompt (D69, 1 boot granted) and its round (AHI driver fault, soak gate); the M0b.11 prompt (D70, rebuilt AHI) and its round (stock ahi.device loaded); the M0b.12 prompt (D71, PROGDIR open, 0 boots granted) and its round (0 boots, D72 device-id logging); the M0b.13 prompt (2 boots, boot 2 graded) and its round (boot 1 near-pass, AHI stalls after 2 returns, boot 2 guest fault on the `Assign` listing); the M0b.14 prompt (D74 AHI return logging, D75 listing last, D76 better boot graded); M0b.14 result; M0b.15–M0b.21 prompts and rounds (VOID driver → `DRIVERS:` freed lock → HIDD preload → AHI link stall → two in flight → 40 ms requests, audio clean in M0b.21, one FAIL from the listing hang); the M0b.22 prompt (D92 tiebreak, D93 audio-time bound) | 2026-11-12 |
| [Verification and CI plan](process/verification-and-ci.md) | Acceptance scripts, CI gates, golden images and guest proofs | 2026-10-30 |
| [Open owner questions](process/open-questions.md) | Unresolved decisions the owner still has to make | 2026-10-31 |
