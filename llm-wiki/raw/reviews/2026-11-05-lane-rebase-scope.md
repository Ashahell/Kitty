# Lane l0025 rebase scope, measured for M0b.12

> Source: read-only measurements on the Vulkan4Aros tree, lane images and upstream AROS
> Collected: 2026-11-05
> Published: 2026-11-05

- `boot/pc/kernel.xz` on `aros_l0025_dh0.img`: 104,588 B, sha `c16ba5e029620b06`. Vulkan4Aros's 2026-10-20 rebase article lists the same sha and size as the non-SMP kernel dated 2026-10-05.
- Chain-tip build, `core-pc-x86_64/bin/pc-x86_64/AROS/boot/pc/kernel.xz`: 106,068 B, sha `1c8ead7292efa6d9`, never deployed. So rebasing the tree alone changes nothing the lane runs.
- Vulkan4Aros's deploy manifest: the built set links stdc 0.52 / posixc 0.41 and pthread stubs while the image runs 0.49; C6 crt group first; the repacked boot packages are an owner decision.
- `bf196e7dc1...6fabae71cb` on upstream: 104 commits, 300 files. It includes `29019867d8`, which replaces the 64-bit DOS structures.
- Vulkan4Aros's `nvk_upstream_assess_opencode_prompt.md` says the owner still has to choose "stay forked, rebase, or adopt upstream's route". Its STOP 2 ends the round if the guard digest differs. S0-S2 are committed; the last commit is 2026-11-05 00:17; no build was running at 07:06.
- Vulkan4Aros's `S7-upstream-watch.md`: `a5e2ee7e` applies with `git apply --check` rc 0 on the chain tree, is a pick candidate, and "the pick call is the owner's".
- `BAD handler port` appears in 0 of the 40 newest `serial-*.log` files under `~/Work/lanes/l0025`, M0b.11's boot among them. That doesn't prove the hazard is absent.
- No Vulkan4Aros file was written and no rebase prompt was written.
