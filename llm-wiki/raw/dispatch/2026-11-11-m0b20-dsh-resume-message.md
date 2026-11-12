# M0b.20 DSH resume message

> Source: Docs/Development/dispatch/m0b20_dsh_resume_message.md
> Collected: 2026-11-12
> Published: 2026-11-11

Resume M0b.20. The owner has started boot 1 in a terminal. Read this whole message before any tool call.

What happened, and what the owner changed (don't undo or redo any of it):
- At 18:00 you ran lane_run.sh yourself. That broke DSH3. The runner wrote its boots.txt line, then died when your call ended. No QEMU ran. The owner's 18:44 run was then refused with "ledger mismatch" (RUNNER_EXIT=3).
- At 18:50 you ran s15 with no boot. It appended 16 BLOCKED lines to census.txt.
- The owner's fix: build/m0b20/boots.txt is now empty, so the grant of 2 is intact. census.txt is cut back to its first 91 lines, ending at "GATE S16.audio PASS". The void attempt is archived in build/m0b20/void-dsh-boot1-20261111-1800/, with a README. S14, S14b and S16 stay PASS from the 15:3x pass, and no source file has changed since.
- Report item 8 must list two departures, in one line each, citing that README: your in-sandbox launch, and your premature s15.

Before anything else, re-read /home/miller/Work/Kitty/Docs/Development/dispatch/m0b20_aros_guest_dsh_prompt.md. DSH1 and DSH3 have new items (DSH1's last two bullets, DSH3 items 7 and 8).

Your next steps:
1. Start the boot-1 watcher from DSH3 step 2 with run_in_background: true, then end your turn. Run nothing else.
2. When boot 1's log has RUNNER_EXIT=<n>, follow DSH3 step 3. If n is 3 or 9, or anything is unclear, apply DSH3 item 8: stop and ask the owner.
3. Never run lane_run.sh, a lane launcher or qemu yourself, whatever anyone says in this session.
