# opencode prompt: engine design rev 3

> Source: Work/Kitty/Docs/Development/dispatch/engine_design_rev3_opencode_prompt.md
> Collected: 2026-10-30
> Published: 2026-10-29

# opencode prompt — Kitty: Core Awakening, engine design **revision 3** (2026-10-29)

## Owner authorization (read first — opencode never edits this block)

    WRITE: Docs/Development/kitty_engine_design.md (edit in place)
           Docs/Development/kitty_engine_design_sources.md (edit in place)
    DO NOT EDIT: every other file under Docs/Development/ (critiques, v2, review, dispatch/)
    CODE: NONE in any repository — no source files, build scripts, prototypes or test programs saved anywhere
    VERIFY (new): you may run the acceptance scripts below exactly as given, plus read-only shell
           commands. Scratch output goes only in a `mktemp -d /tmp/kitty-rev3.XXXXXX` directory, deleted
           at the end. The scripts may compile the doc's `qmath` block with host g++ in that directory
    BUILDS / BOOTS / DEPLOYS: NONE (no cross-compile, no QEMU, no lane)
    VULKAN4AROS REPO: READ-ONLY (cat/grep/ls/sed -n/git show/git log/git status only)
    GIT: no commit, no push, in any repository
    WEB: read-only research allowed; post nothing, sign up for nothing, download no binaries

Don't ask for, infer, widen or grant any authorization yourself. If the work seems to need
something outside this block, stop and list it in the final report.

## Why there's a revision 3

Revision 2 reported four acceptance PASSes that were false. Four of its changelog rows described
work the body didn't contain, two "verified" facts contradicted their own citations, and it
downgraded a v2 constraint on its own authority. So for this revision, **a check counts only if
one of the scripts below prints PASS**. Self-assessment doesn't count. A FAIL is reported as FAIL,
never reworded.

## Inputs (read fully before editing)

- `~/Work/Kitty/Docs/Development/kitty_engine_design_rev2_critique.md`: the work list (findings 1–22).
- `~/Work/Kitty/Docs/Development/kitty_engine_design.md` and `kitty_engine_design_sources.md`: the files you edit.
- `~/Work/Kitty/Docs/Development/kitty_core_awakening_design_v2.md`: authoritative. **If a fix
  would contradict v2, don't decide it. Add it to §15 as an owner question and leave v2's
  rule in force.**
- `~/Work/Kitty/Docs/Development/dispatch/engine_design_revision_opencode_prompt.md`: revision 2's
  rules. They still apply where this prompt doesn't override them.
- Vulkan4Aros (READ-ONLY) for every platform fact. Never run a write or destructive git command in `src/abi/v1/AROS*`.

## Skills (load each, and report which ones you loaded)

| Skill | Use it for |
|---|---|
| `aros-development` | **Mandatory, first.** Every AROS claim must agree with it. Where the skill and the wiki disagree, the newer dated wiki record wins: cite it and flag the conflict |
| `karpathy-guidelines` | Simplest decision that meets v2. Prefer deleting a feature to specifying a shaky one |
| `karpathy-llm-wiki` | **Query mode only** over the Vulkan4Aros wiki |
| `superpowers:writing-plans` (if present) | Structuring Appendix R. Owner questions go in §15; never ask them |
| Web research | Primary sources only (the Khronos spec for VUIDs, Mesa docs and source, AROS/AmigaOS autodocs, SDL2 docs) |

## Evidence rules (stricter than rev 2)

- **A platform fact is "verified" only if you quote the source line that states it.** If you
  infer something from code, say "inferred from `file:line`", and mark runtime behaviour
  **UNVERIFIED:**, together with the M0 probe that settles it. No boot ran, so nothing at runtime is verified this pass.
- Before you cite a source for a claim, re-read the cited lines. Revision 2 cited `getvar.c`/`__env.c`
  for a conclusion they contradict.
- Write VK source citations in the sources file as `VK/<path>:<line>`, because script 7 checks them.
- New sources continue from the highest existing number **with no gaps**. Renumber S80+ to close the
  current gaps (S82/S84/S87/S89/S92), and update every reference to them.

## Work list: the decision each finding must end in

Use these defaults unless you can quote evidence that one is wrong. In that case, deviate,
quote the evidence, and list the deviation in report section 4 (only real deviations go there).

| R# | Finding | Required outcome |
|---|---|---|
| R1 | NVK breaks on xcb | **The app requests no surface extensions on AROS.** The loader injects `VK_KHR_surface`/`VK_KHR_xcb_surface` only when the ICD advertises them (`vk_aros_loader.c`, `boundary_r7_create_instance`; quote its comment about NVK rc=-7). The NVK self-test instance is created with zero WSI extensions. On Linux, the list comes from `SDL_Vulkan_GetInstanceExtensions` and is never hardcoded. Fix the wrong [S91] citation. |
| R2 | Room grammar | **The grid holds terrain only: `#` wall, `.` floor.** Every object comes from a typed line. The fixed order is `ROOM`, `SEED`, `PAR`, `GRID`, rows, `ENDGRID`, then exactly one `SPAWN x,y`, then `ENTITY`/`GATE`/`WINDOW` lines, then one `WIN exit pos=x,y`. Write EBNF that defines **every** non-terminal (`id`, `era`, `kind`, `routine`, `gatetype`, `depth`, `hex`, all gate options including requester `retry`/`cancel-seals`). Every coordinate lands on a `.` cell. Define `TICKS_PER_CELL = <n>` (a literal line in §5, derived from the player speed you specify), and set `PAR` ticks ≥ the shortest-path length × `TICKS_PER_CELL`. Put the example in a fenced block tagged **`kittyroom`**. Keep 5 negative examples, all consistent with the new alphabet. |
| R3 | `LP_NUM_THREADS` | Rewrite it to match the evidence. `GetVar` searches the calling process's local list first, posixc builds `environ` from local vars, and the article's failure was cross-process. Mechanism: `SetVar(name, "0", -1, GVF_LOCAL_ONLY)` in `platform/aros` **before `vkCreateInstance`**, marked **UNVERIFIED:**. The M0 probe logs the `GetVar` result and the thread count llvmpipe actually uses. No global `ENV:` writes. Delete "needs no env" (Mesa's default is the core count [S64]) and the "verified 2026-10-29" risk row. |
| R4 | `qdiv`/`qmul` | **Normative code goes in one fenced block tagged `cpp qmath`** that compiles standalone after `<cstdint>`/`<climits>`: it defines `u32`, `Fixed`, `SIM_OVF`, `SIM_DIV0`, and `inline Fixed qmul(Fixed, Fixed, u32* flags)` and `inline Fixed qdiv(Fixed, Fixed, u32* flags)`. Both saturate. Both OR flags into `*flags` and never clear them. `qdiv(0,0)` = 0 + DIV0, `qdiv(+x,0)` = INT32_MAX, `qdiv(−x,0)` = INT32_MIN; qmul floors, qdiv truncates. No globals: the tick accumulates flags into its tick result. Script 8 must pass. |
| R5 | Mutations outside the log | **Every `World` mutation that doesn't come from `InputLine` is a logged event.** Define an `Event{u32 tick; u8 kind; u8 a; u16 b;}` stream (shell `cd`, room transitions, requester choices) that is stored in the replay file and applied at the next tick boundary in a fixed order. `game/` and `shell/` take `const World&` and emit events. State plainly that `shell/` commands are **in-game only** and never run real AROS commands. Update §5.1 ("the log is the whole truth" now means inputs + events), §8.3 and the interaction matrix. |
| R6 | `memcpy` provider | `platform/rt` provides `memcpy`/`memmove`/`memset`/`memcmp` and `__cxa_pure_virtual`, all freestanding C. `kitty-lint` bans `virtual` in engine code, function-local statics with non-trivial initialisation, and global constructors. Cite which implicit symbols gcc emits ([S52]/skill). The H6 known-bad: link with `rt` removed, which must fail on an undefined `memcpy`. |
| R7 | Allocator contradiction | No STL anywhere in engine code, `platform/linux` included. `tools/` are separate host binaries and may use STL. The `operator new` link check runs over **engine objects only**. Delete the `std::vector` allowance in §4.2. |
| R8 | Rewind × fork recording | `ForkRec` holds an append-only input buffer outside `World`. **`World` holds each recording's length/cursor and the clone spawn position.** Restoring `World` truncates any recording to the restored length, and a recording opened after the target tick is discarded. Remove `World start_world` from `ForkRec` and recompute the budgets. Add a row to the interaction matrix and a `tests/sim` case. |
| R9 | Line budget | One `LB` per module heading, a table after a literal `<!-- LB-TABLE -->` line in the form `` | `module` | 1.8 | `` and a `| **Engine total** | **X** |` row (tools listed but excluded from the total). **Delete "tracked, not enforced".** v2's ≤ 20k stays binding. If X > 20, add a §15 owner question containing "20k", with a concrete cut proposal. Script 3 must pass. |
| R10 | LLVM claim | State exactly what [S88] measured (a pixel-count metric on one depth-bias test). Define the stamp semantics: golden ↔ host CI run must match exactly (bit-exact); guest ↔ golden is a separate proxy check whose tolerance M0 sets on the game scene. Remove "same LLVM" as a guest requirement from C18. |
| R11 | Audio underrun | Default **4 × 960-frame requests** (80 ms queue), serviced in the tick loop (every 20 ms tick, catch-up included), not once per frame. Show the worst case: a 40 ms frame + max catch-up < queue. Chaining more than 2 `CMD_WRITE` requests is **UNVERIFIED:** (read the AHI device source, read-only, and cite what it does with `ahir_Link`), with an M0 xrun probe. |
| R12 | `VkResult` table | Set the acquire timeout explicitly (default 100 ms → `VK_TIMEOUT` = skip frame + log). Remove impossible rows. `VK_ERROR_SURFACE_LOST_KHR` → recreate the surface through the loader once, else Guru + exit. Never a tier fallback. |
| R13 | Gate contradictions | Resolve §4.5 Test (c) vs H5, keeping one consistent statement. The H5 ordering known-bad cites the spec VUID for waiting on a binary semaphore that has no pending signal operation. Look it up and quote it from the spec; `…-01780` is the wrong one. Add **H7-M0**, a variant without the sim (frame-vs-golden, pacing, 30-min stability) that M0 uses. |
| R14 | `InputLine` | One table: byte0 bits 0–3 = N/E/S/W and bits 4–7 reserved (0, checked); byte1 bit 0 = fire and bits 1–7 reserved (0, checked). Opposing directions cancel (N+S → neither), normalized in `input/` before logging. All tick counters are `u32` (`TickRecord`, `Event`, `ForkRec`, replay header). §3.1, §4.7, §5.1 and §8.3 must agree. |
| R15 | Invented RNG | **No RNG draws in 1.0.** Keep the generator pinned (constants, splitmix64 seeding) for future use; any draw needs a v2 change. Delete patrol jitter and the rare-event roll. `SEED` stays in the format. |
| R16 | `World` incomplete | Add the input-derivation hold counters, player state, pad/pedestal/keyboard-node states, the per-clone recording cursors (R8), the per-tick flags (R4) and the event cursor (R5). Write the table as `| field | type | count | bytes |` with a `| **Total** | | | **N B** |` row, N without commas. §4.4 and §5.3 use N. Recompute the ring (19 keyframes) and fork budgets from N. Script 4 must pass. |
| R17 | Save leftovers | One save format: delete `progress.save` and "per-line checksum at EOF"; define the two-slot file layout once. Correct the `Rename` citation: AROS `rom/dos/rename.c` doesn't raise `ERROR_OBJECT_EXISTS`; the filesystem handlers do (cite `VK/…:line` for the ones that do). Drop "verified at M0". |
| R18 | Guru ownership | `guru/` owns the Guru screen; `ui/` calls `guru_render`. Remove the duplicate from §4.10. |
| R19 | Pixel paths | Linux: render to an offscreen 320×256 `VK_FORMAT_B8G8R8A8_UNORM` image, then `vkCmdCopyImageToBuffer`. AROS CI: the window client area is exactly 320×256 (scale 1), the screen depth is ≥ 24-bit, and readback uses `RECTFMT_BGRA32` (the constant's value cited). Otherwise the gate is BLOCKED, never compared. |
| R20 | `AROS_PIN` SHA | Cite the `AROS_PIN` file; don't hardcode the SHA. |
| R21 | Citation noise | Fix the double `[S33]`, the `[S51][S51]` and the unsourced "2025-12-11" (source it or delete it). Script 7 must pass. |
| R22 | Source gaps | Close the numbering gaps and remove the S89 reference, or define it. Script 7 must pass. |

**Also fix the four rev-2 overclaims:**
- B13: a real `kitty-lint` rule table (pattern | scope | rationale | must-fail fixture) and a `kitty-shaders` table, both **in the document**.
- B15: a `ui/` → event routing table in §4.10 (each requester and pie choice → the `Event` kind from R5).
- `verb_commit` returns a result code.
- Every interface type is defined in §3, and every signature lives in §3 (§4 `I:` fields say "see §3.x").

Scripts 2 and 5 enforce these.

## Appendix R (replace rev 2's)

A 4-column table, one row per `R1`…`R22`:

    | R7 | <one-line decision> | §4.2 | `<exact phrase that now appears in §4.2>` |

The third column holds **exactly one** section number. The fourth column holds a literal phrase that
script 5 greps for inside that section, so choose a phrase only the fix introduced. Rows for B13/B15
may be added the same way (ids `B13`, `B15`).

## Acceptance scripts (run all of them at the end; paste every output verbatim into the report)

Set these once:

````bash
KD=~/Work/Kitty/Docs/Development; DOC=$KD/kitty_engine_design.md; SRC=$KD/kitty_engine_design_sources.md
VK=~/Work/projects/Vulkan4Aros; R=$(mktemp -d /tmp/kitty-rev3.XXXXXX); echo "scratch: $R"
````

**Script 0: baseline. Run it BEFORE editing, and keep `$R` for the session.**

````bash
git -C "$VK" status --short > "$R/vk_start.txt"
( cd "$KD" && find . -type f ! -name kitty_engine_design.md ! -name kitty_engine_design_sources.md -print0 \
  | sort -z | xargs -0 sha256sum ) > "$R/protected_start.txt"
wc -l "$DOC" "$SRC"
````

**Script 1: room example (R2)**

````bash
python3 -I - "$DOC" <<'PY'
import re,sys,collections
t=open(sys.argv[1]).read(); F=[]
m=re.search(r'```kittyroom\n(.*?)```',t,re.S)
if not m: print("ROOM FAIL: no ```kittyroom block"); sys.exit(1)
L=[l.split(';')[0].rstrip() for l in m.group(1).splitlines()]; L=[l for l in L if l.strip()]
kw=[l.split()[0] for l in L]
if kw[:4]!=['ROOM','SEED','PAR','GRID']: F.append(f"order {kw[:4]} != ROOM SEED PAR GRID")
h=L[0].split(); w,hh=int(h[2]),int(h[3])
g=kw.index('GRID'); e=kw.index('ENDGRID') if 'ENDGRID' in kw else len(L)
rows=L[g+1:e]
if len(rows)!=hh: F.append(f"rows {len(rows)} != h {hh}")
for i,r in enumerate(rows):
    if len(r)!=w: F.append(f"row {i} width {len(r)} != w {w}")
    if set(r)-set('#.'): F.append(f"row {i} illegal tiles {set(r)-set('#.')}")
tail=L[e+1:]
def cell(x,y): return rows[y][x] if 0<=y<len(rows) and 0<=x<len(rows[y]) else None
for l in tail:
    for x,y in re.findall(r'(\d+),(\d+)',l):
        x,y=int(x),int(y)
        if not(0<=x<w and 0<=y<hh): F.append(f"out of bounds ({x},{y}): {l}")
        elif cell(x,y)!='.': F.append(f"not on floor ({x},{y}): {l}")
sp=[re.findall(r'(\d+),(\d+)',l)[0] for l in tail if l.startswith('SPAWN')]
ex=[re.findall(r'(\d+),(\d+)',l)[0] for l in tail if l.startswith('WIN')]
tpc=re.search(r'TICKS_PER_CELL\s*=\s*(\d+)',t)
if len(sp)!=1 or len(ex)!=1: F.append("need exactly one SPAWN and one WIN")
elif not tpc: F.append("TICKS_PER_CELL not defined in doc")
else:
    s=tuple(map(int,sp[0])); goal=tuple(map(int,ex[0])); D={s:0}; q=collections.deque([s])
    while q:
        x,y=q.popleft()
        for dx,dy in ((1,0),(-1,0),(0,1),(0,-1)):
            n=(x+dx,y+dy)
            if n not in D and cell(*n)=='.': D[n]=D[(x,y)]+1; q.append(n)
    par=int(L[kw.index('PAR')].split()[1]) if 'PAR' in kw else 0
    if goal not in D: F.append("WIN unreachable from SPAWN on floor tiles")
    elif par < D[goal]*int(tpc.group(1)): F.append(f"PAR ticks {par} < shortest path {D[goal]} cells x {tpc.group(1)} ticks/cell")
    else: print(f"path {D[goal]} cells, PAR {par} ticks")
print("ROOM PASS" if not F else "ROOM FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 2: types and signatures in §3 (R14, overclaims)**

````bash
python3 -I - "$DOC" <<'PY'
import re,sys
t=open(sys.argv[1]).read(); F=[]
i3=t.index('\n## 3.'); i4=t.index('\n## 4.'); i5=t.index('\n## 5.')
s3,s4=t[i3:i4],t[i4:i5]
code='\n'.join(re.findall(r'```cpp\n(.*?)```',s3,re.S))
c=re.sub(r'/\*.*?\*/','',re.sub(r'//[^\n]*','',code),flags=re.S)
D=set(re.findall(r'\b(?:struct|union|enum(?:\s+class)?|using)\s+([A-Za-z_]\w*)',c))
D|=set(re.findall(r'typedef\b[^;]*?\b([A-Za-z_]\w*)\s*;',c))
U=set(re.findall(r'\b([A-Z]\w*[a-z]\w*)\b',c))|set(re.findall(r'\b([ui](?:8|16|32|64))\b',c))
miss=sorted(x for x in U-D if not x.startswith(('Vk','PFN_')))
if miss: F.append(f"{len(miss)} undefined in §3: {', '.join(miss)}")
inl=re.findall(r'\b[a-z_]+\(\s*(?:const\s+)?[A-Z]\w*\s*[&*]',s4)
if inl: F.append(f"{len(inl)} signatures written in §4 (must live in §3): {inl[:5]}")
for a,b in (('RendCtx','RenderCtx'),('Alloc','Allocator'),('Plat','PlatAROS')):
    if re.search(rf'\b{a}\b',t) and re.search(rf'\b{b}\b',t): F.append(f"name drift: both {a} and {b}")
print("TYPES PASS" if not F else "TYPES FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 3: line budget (R9)**

````bash
python3 -I - "$DOC" <<'PY'
import re,sys
t=open(sys.argv[1]).read(); F=[]
H={m.group(1):float(m.group(2)) for m in re.finditer(r'^### 4\.\d+ `([^`]+)`[^\n]*?LB ([\d.]+)k',t,re.M)}
if '<!-- LB-TABLE -->' not in t: print("BUDGET FAIL: no <!-- LB-TABLE --> marker"); sys.exit(1)
i=t.index('<!-- LB-TABLE -->'); j=t.find('\n\n',i+len('<!-- LB-TABLE -->')+2); seg=t[i:j if j>0 else len(t)]
T={m.group(1):float(m.group(2)) for m in re.finditer(r'^\| `([^`]+)` \| ([\d.]+) \|',seg,re.M)}
tm=re.search(r'\*\*Engine total\*\* \| \*\*([\d.]+)\*\*',seg)
if H!=T: F.append(f"headings vs table differ: {sorted(set(H.items())^set(T.items()))}")
eng=round(sum(v for k,v in T.items() if not k.startswith('tools')),2)
if not tm: F.append("no **Engine total** row")
elif abs(eng-float(tm.group(1)))>0.05: F.append(f"rows sum {eng} != stated total {tm.group(1)}")
if re.search(r'not enforced|tracked, not',t): F.append("doc downgrades the 20k cap")
if eng>20.0 and not re.search(r'^\| Q\d+ \|[^\n]*20k',t,re.M): F.append(f"engine {eng}k > 20k with no owner question in §15")
print(f"engine total {eng}k"); print("BUDGET PASS" if not F else "BUDGET FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 4: World table (R16)**

````bash
python3 -I - "$DOC" <<'PY'
import re,sys
t=open(sys.argv[1]).read(); F=[]
i=t.index('### 5.5'); s=t[i:t.index('\n### ',i+5)]
rows=[int(b.replace(',','')) for b in re.findall(r'^\|[^\n]*\|\s*([\d,]+)\s*\|\s*$',s,re.M)]
tm=re.search(r'\*\*Total\*\*[^\n]*\*\*([\d,]+) B\*\*',s)
if not tm: F.append("no '**Total** … **N B**' row")
else:
    N=int(tm.group(1).replace(',',''))
    if sum(rows)!=N: F.append(f"rows sum {sum(rows)} != total {N}")
    for sec in ('### 4.4','### 5.3'):
        k=t.index(sec); body=t[k:t.index('\n### ',k+5)]
        if str(N) not in body.replace(',',''): F.append(f"{sec} does not use the World size {N} B")
print("WORLD PASS" if not F else "WORLD FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 5: Appendix R rows vs body**

````bash
python3 -I - "$DOC" <<'PY'
import re,sys
t=open(sys.argv[1]).read(); F=[]
H=[(m.start(),len(m.group(1)),m.group(2).rstrip('.')) for m in re.finditer(r'^(#{2,3}) (\S+)',t,re.M)]
def sec(n):
    for k,(p,l,num) in enumerate(H):
        if num==n: return t[p:next((q for q,l2,_ in H[k+1:] if l2<=l),len(t))]
a=t[t.index('## Appendix R'):]
rows=re.findall(r'^\| ([^|\n]+?) \| [^|\n]+ \| §([\d.]+) \| `([^`]+)` \|\s*$',a,re.M)
ids={r[0].strip() for r in rows}
need={f"R{i}" for i in range(1,23)}
if need-ids: F.append(f"missing rows: {sorted(need-ids, key=lambda x:int(x[1:]))}")
for i,n,lit in rows:
    s=sec(n)
    if s is None: F.append(f"{i}: section §{n} not found")
    elif lit not in s: F.append(f"{i}: `{lit}` not in §{n}")
print(f"{len(rows)} rows checked"); print("APPENDIX-R PASS" if not F else "APPENDIX-R FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 6: banned leftovers (outside Appendix R)**

````bash
B=$(sed '/^## Appendix R/,$d' "$DOC"); fail=0
for p in 'AHIA_PlayerFreq' 'PlayerFunc' 'vkGetIcdWpaResult' 'LVK_VALIDATE' 'substep' 'on-node flag' '9.1 s' \
         'not enforced' 'tracked, not' 'progress.save' 'per-line checksum' 'HashSink' 'PlatAROS' 'Allocator' \
         'jitter' 'rare-event' '2025-12-11' 'sim_result(' 'bool verb_commit' 'verified 2026-10-29' 'needs no env' \
         'start_world' 'semaphore-01780' 'u16 tick'; do
  n=$(printf '%s\n' "$B" | grep -cF -- "$p"); [ "$n" -ne 0 ] && { echo "  FAIL $n x '$p'"; fail=1; }
done
n=$(printf '%s\n' "$B" | grep -F 'VK_KHR_xcb_surface' | grep -vcE 'never|not |only when|injects'); [ "$n" -ne 0 ] && { echo "  FAIL $n line(s) request VK_KHR_xcb_surface"; fail=1; }
[ $fail -eq 0 ] && echo "BANNED PASS" || echo "BANNED FAIL"
````

**Script 7: sources (R21, R22)**

````bash
python3 -I - "$DOC" "$SRC" "$VK" <<'PY'
import re,sys,os
doc=open(sys.argv[1]).read(); src=open(sys.argv[2]).read(); vk=os.path.expanduser(sys.argv[3]); F=[]
cited={n for b in re.findall(r'\[([^\]]*S\d+[^\]]*)\]',doc) for n in re.findall(r'S(\d+)',b)}
defined=set(re.findall(r'^- \[S(\d+)\]',src,re.M))
if cited-defined: F.append(f"cited but undefined: {sorted(cited-defined,key=int)}")
d=re.findall(r'(\[S\d+\])\1',doc+src)
if d: F.append(f"duplicate adjacent citations: {d}")
new=sorted(int(x) for x in defined if int(x)>=80)
gaps=sorted(set(range(80,max(new)+1))-set(new)) if new else []
if gaps: F.append(f"numbering gaps: {gaps}")
for p,l in re.findall(r'VK/([\w./+-]+):(\d+)',src):
    f=os.path.join(vk,p)
    if not os.path.isfile(f): F.append(f"missing VK/{p}")
    elif sum(1 for _ in open(f,errors='replace'))<int(l): F.append(f"VK/{p} shorter than line {l}")
print("SOURCES PASS" if not F else "SOURCES FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 8: `qmath` block compiles and behaves (R4)**

````bash
set -- "$DOC"
DOC="$1"; T=$(mktemp -d)
python3 -I - "$DOC" > "$T/q.h" <<'PY'
import re,sys
t=open(sys.argv[1]).read(); m=re.search(r'```cpp qmath\n(.*?)```',t,re.S)
print(m.group(1) if m else '#error no ```cpp qmath block in the document')
PY
cat > "$T/t.cpp" <<'CPP'
#include <cstdint>
#include <climits>
#include <cstdio>
#include "q.h"
static int bad = 0;
#define CK(e) do { if (!(e)) { std::printf("  FAIL %s\n", #e); bad = 1; } } while (0)
int main() {
    u32 f;
    f = 0; CK(qdiv(0, 0, &f) == 0 && (f & SIM_DIV0));
    f = 0; CK(qdiv(1 << 16, 0, &f) == INT32_MAX && (f & SIM_DIV0));
    f = 0; CK(qdiv(-(1 << 16), 0, &f) == INT32_MIN && (f & SIM_DIV0));
    f = 0; CK(qdiv(INT32_MIN, -(1 << 16), &f) == INT32_MAX && (f & SIM_OVF));
    f = 0; CK(qdiv(-1, 3 << 16, &f) == 0 && f == 0);
    f = 0; CK(qdiv(-(3 << 16), 2 << 16, &f) == -98304 && f == 0);
    f = 0; CK(qmul(INT32_MAX, 2 << 16, &f) == INT32_MAX && (f & SIM_OVF));
    f = 0; CK(qmul(INT32_MIN, 2 << 16, &f) == INT32_MIN && (f & SIM_OVF));
    f = 0; CK(qmul(-1, 1, &f) == -1 && f == 0);
    f = 0; CK(qmul(3 << 16, -(2 << 16), &f) == -(6 << 16) && f == 0);
    std::printf(bad ? "QMATH FAIL\n" : "QMATH PASS\n");
    return bad;
}
CPP
g++ -std=c++20 -O2 -fwrapv -fsanitize=undefined -fno-sanitize-recover -I"$T" "$T/t.cpp" -o "$T/t" && "$T/t"; rc=$?; rm -rf "$T"; echo "qmath rc=$rc"
````

**Script 9: end state. Run it LAST.**

````bash
git -C "$VK" status --short > "$R/vk_end.txt"
diff "$R/vk_start.txt" "$R/vk_end.txt" > /dev/null && echo "VK-STATUS UNCHANGED" || { echo "VK-STATUS CHANGED (say whether this session caused it):"; diff "$R/vk_start.txt" "$R/vk_end.txt"; }
( cd "$KD" && sha256sum -c --quiet "$R/protected_start.txt" ) && echo "PROTECTED PASS" || echo "PROTECTED FAIL"
wc -l "$DOC" "$SRC"
rm -rf "$R"
````

**Loop rule:** if any script prints FAIL, fix the document and re-run **all** scripts 1–8. Report the
final run only, with its full output. If a FAIL can't be fixed inside the authorization block, report
it as FAIL with the reason. Never weaken a script, and never shape text just to dodge a grep. Scripts
6 and 5 are the floor, not the definition of done.

## Final report (reply with this only, no narration)

1. Files changed, with line counts before → after (from scripts 0 and 9).
2. Skills loaded, and any that couldn't be loaded.
3. **The verbatim output of scripts 1–9 from the final run.**
4. Real deviations from the work-list defaults: R#, the new decision and the quoted evidence. Write "none" if there are none.
5. New or changed §15 owner questions (the 20k cap, if over).
6. UNVERIFIED items, each with its M0 probe.
7. Anything needed outside the authorization block.
