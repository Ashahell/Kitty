# opencode prompt: engine design rev 4

> Source: Work/Kitty/Docs/Development/dispatch/engine_design_rev4_opencode_prompt.md
> Collected: 2026-10-30
> Published: 2026-10-29

# opencode prompt — Kitty: Core Awakening, engine design **revision 4** (2026-10-29)

## Owner authorization (read first — opencode never edits this block)

    WRITE: Docs/Development/kitty_engine_design.md (edit in place)
           Docs/Development/kitty_engine_design_sources.md (edit in place)
    DO NOT EDIT: every other file under Docs/Development/ (critiques, v2, review, dispatch/)
    CODE: NONE in any repository — no source files, build scripts, prototypes or test programs saved anywhere
    VERIFY: you may run the acceptance scripts below exactly as given, plus read-only shell commands.
           Scratch output goes only in a `mktemp -d /tmp/kitty-rev4.XXXXXX` directory, deleted at the end.
           The scripts compile the doc's §3 code blocks (g++ -fsyntax-only, plus one sizeof probe) and
           the `qmath` block with host g++ inside that directory
    BUILDS / BOOTS / DEPLOYS: NONE (no cross-compile, no QEMU, no lane)
    VULKAN4AROS REPO: READ-ONLY (cat/grep/ls/sed -n/git show/git log/git status only)
    GIT: no commit, no push, in any repository
    WEB: read-only research allowed; post nothing, sign up for nothing, download no binaries

Don't ask for, infer, widen or grant any authorization yourself. If the work seems to need
something outside this block, stop and list it in the final report.

## Why there's a revision 4

Revision 3 printed PASS on all ten scripts, yet 13 of 22 work items failed. Most decisions were
written only in Appendix R while the body kept the old text or its opposite. One citation was
fabricated: [S88] attributes to the `dos.md` autodoc a sentence the autodoc doesn't contain. The
example room invented `TICKS_PER_CELL = 2` so that its own PAR would pass. §3 doesn't compile. The
report said "no deviations".

So in this revision **the decisions are fixed by this prompt as exact sentences**. You paste them
verbatim into the named sections and make the rest of the document agree with them. The scripts
check those sentences, a list of banned old claims, and the arithmetic, and they compile the
interface code. Self-assessment doesn't count; a FAIL is reported as FAIL.

## Inputs (read fully before editing)

- `~/Work/Kitty/Docs/Development/kitty_engine_design_rev3_critique.md`: the work list (§A rows, §B items 1–6).
- `~/Work/Kitty/Docs/Development/kitty_engine_design.md` and `kitty_engine_design_sources.md`: the files you edit.
- `~/Work/Kitty/Docs/Development/kitty_core_awakening_design_v2.md`: authoritative. **If a fix would
  contradict v2, don't decide it. Add it to §15 as an owner question and leave v2's rule in force.**
- `dispatch/engine_design_rev3_opencode_prompt.md` and `dispatch/engine_design_revision_opencode_prompt.md`:
  earlier rules. They still apply where this prompt doesn't override them.
- Vulkan4Aros (READ-ONLY) for every platform fact. Never run a write or destructive git command in `src/abi/v1/AROS*`.

## Skills (load each, and report which ones you loaded)

| Skill | Use it for |
|---|---|
| `aros-development` | **Mandatory, first.** Every AROS claim must agree with it. Where the skill and the wiki disagree, the newer dated wiki record wins: cite it and flag the conflict |
| `karpathy-guidelines` | Simplest decision that meets v2. Delete rather than specify something shaky |
| `karpathy-llm-wiki` | **Query mode only** over the Vulkan4Aros wiki |
| Web research | Primary sources only (the Khronos spec, Mesa docs and source, AROS/AmigaOS autodocs, SDL2 docs) |

## Evidence rules

- **A citation must say only what its source says.** Before writing or keeping any `[S#]`, re-read the
  cited lines. A sources entry describes what the source states, not your conclusion from it.
- Code-derived facts are written "inferred from `VK/<path>:<line>`". Runtime behaviour is **UNVERIFIED:**
  with the M0 probe that settles it. No boot ran, so nothing at runtime is verified this pass. The
  string "verified at M0" is banned.
- VK citations in the sources file use the form `VK/<path>:<line>`; script 7 opens every one.
- New sources continue from the highest number, with no gaps.

## Fixed decisions — paste each sentence verbatim into its section

Each sentence below must appear **exactly** (same backticks, punctuation and Unicode arrows) inside
the named section (`§4` means anywhere under `## 4.`). Then remove or rewrite every other sentence in
the document that contradicts it. Contradictions the scripts can't see will be found in review and
fail the item.

| Item | § | Sentence |
|---|---|---|
| R1 | 6.1 | On AROS, `ppEnabledExtensionNames` contains no WSI extension; the loader adds `VK_KHR_surface` and `VK_KHR_xcb_surface` only when the ICD advertises them. |
| R1 | 6.1 | On Linux, the instance extension list is exactly what `SDL_Vulkan_GetInstanceExtensions` returns. *(cite a new source entry for the SDL2 wiki page of that function on the same line)* |
| R1 | 6.4 | The NVK self-test instance is created with zero instance extensions. |
| R3 | 4.5 | Before `vkCreateInstance`, `platform/aros` calls `SetVar("LP_NUM_THREADS", "0", -1, GVF_LOCAL_ONLY)`. *(cite the source on the same line)* |
| R3 | 4.5 | \*\*UNVERIFIED:\*\* whether lavapipe in the same process reads a process-local variable; the M0 probe logs the `GetVar` result and lavapipe's worker-thread count. *(the `**` bold markers are part of the sentence)* |
| R9 | 4 | The 20k engine cap is binding. |
| R10 | 9 | [S85] measured one depth-bias test with a pixel-count metric (`diff=153600` for both bias signs); it is not a bit-exactness proof. *(script checks from "measured"; the cited source number may differ if your numbering changed)* |
| R10 | 2 | Golden ↔ host CI must be bit-exact; guest ↔ golden is a proxy check whose tolerance M0 sets, and the guest's LLVM version need not match the host's. *(this goes in C18)* |
| R11 | 4.6 | Four chained 960-frame `CMD_WRITE` requests form an 80 ms queue. |
| R11 | 4.6 | The queue covers any gap between services shorter than 80 ms; a longer frame underruns, and the underrun is logged with its tick. |
| R13 | 4.5 | Known-bad (c): see H5. |
| R17 | 4.12 | Inferred from the handler sources: `Rename` onto an existing destination fails with `ERROR_OBJECT_EXISTS`, raised by the filesystem handler, not by `rom/dos/rename.c`; saves therefore use two slots instead of replace-by-rename. *(cite on the same line a source entry that lists ≥ 2 handler lines as `VK/<path>:<line>`, each line containing `ERROR_OBJECT_EXISTS` inside the handler's rename action. Candidates: `rom/filesys/fat/ops.c`, `rom/filesys/afs/filehandles2.c`, SFS. Check which function each line is in.)* |
| B3 | 5.1 | Step 0 applies the `Event`s queued for this tick, sorted by (kind, a). *(add it as step 0 of the tick order; delete the other event order in §5.2)* |
| B4 | 5.6 | No draw occurs in 1.0 (§5.1). |
| B5 | 9 | Global `operator new`/`delete` are defined only as traps that call `guru_report` and exit; a link-time check fails the build if any engine object references them. *(update C29 to match)* |
| B5 | 4.13 | namespace-scope object with a non-trivial constructor *(the pattern cell of a new `kitty-lint` row; the `new`/`delete` row's scope becomes all engine code, `platform/` included)* |

## Structural requirements (no sentence to paste — the scripts check the structure)

- **R2 room format (script 1).** Replace §8.1's EBNF with the grammar inside script 1, verbatim, in the
  only ```` ```ebnf ```` block. `TICKS_PER_CELL = 8` (6.25 cells/s), defined on a line of its own that
  starts with that text; no other value anywhere. The single ```` ```kittyroom ```` example must
  validate. Its `PAR` must satisfy: ticks ≥ shortest path × 8; verb-commits × 25 ≤ ticks;
  clones-max ≤ 16. Rewrite the `Hand-check:` line so it lists exactly the example's coordinates, and so
  that any "N ticks = X s" in it is right at 50 Hz. Rewrite the negative examples as complete rooms
  (ROOM, SEED, PAR, GRID …), each in a ```` ```kittyroom-bad ```` block followed on the next line by
  ``Expected: `Software Failure. room.txt:<line>: <message>` ``. Cover at least the kinds row width,
  row count, illegal tile, out of bounds and semaphore n. Line numbers count every source line,
  comments included. The script's validator is the oracle.
- **B1 §3 compiles (script 2).** All ```` ```cpp ```` blocks under `## 3.`, concatenated in order after
  `<cstdint>`, `<cstddef>` and `<vulkan/vulkan.h>`, must pass
  `g++ -std=c++20 -fsyntax-only -Wall -Werror -Wredundant-decls`. That means declaration before use,
  no duplicate declarations, and no stray text. `ForkRec` must be a complete type.
- **B2 interfaces (script 2).** Every `### 4.N` module except `tools/` has exactly one each of
  `- **R:**`, `- **I:**`, `- **D:**`, `- **T:**`, `- **E:**`, `- **A:**`, `- **Test:**` (R18).
  Its `I:` bullet names its functions in backticks (e.g. `` `aud_open` ``), and every one is declared
  in §3. Declare in §3 every function §4 points to; delete pointers to functions you don't specify.
- **R9 line budget (script 3).** Same table as rev 3. No sentence that mentions 20k may also say
  target/tracked/soft/goal/"enforced at" (§1's "Target ≤ 20k" included). If the engine total exceeds
  20, exactly one §15 Q row mentions 20k. It names the modules to cut in backticks and states
  `→ ~X k`, where X = total − their LB sum, and X ≤ 20. No other budget figure appears in §4 or that row.
- **R16 snapshot budget (script 4).** Keyframe = `serialize(World)` only (tick and room id already live
  in the World header). Fork record = `sizeof(ForkRec)` (inputs only; spawn position and cursor live in
  `World`, per R8). §5.3 contains this line, with the numbers filled in:
  `BUDGET keyframe=N B; ring=19*N=X B; fork=F B; forks=16*F=Y B; total=X+Y=Z B`.
  N is the §5.5 total and F is what the script measures. §4.4 states N and Z. No KB/KiB/MB figure is
  allowed in §4.4, §5.3, §5.5 or the Q07 row.
- **R12 (script 5).** The `VK_ERROR_SURFACE_LOST_KHR` row: class `recreate-surface`; action recreates the
  surface through the loader once, else Guru + exit; no "fallback", "lavapipe" or "device-lost" in that row.
- **R21 (script 7).** No source id appears twice in one citation cluster (e.g. `[S33], [S80], [S33]`).
- **B6.** Line 1 is exactly `# Kitty: Core Awakening — Engine Design Document (revision 4)`. No
  "(revision 2)"/"Revision 2 adds" leftovers.
- **Citation subjects (script 7).** A body line containing `SDL_Vulkan_GetInstanceExtensions`,
  `ERROR_OBJECT_EXISTS`, `GVF_LOCAL_ONLY` or `diff=153600` must cite a source entry that contains that
  same string.

## Banned (script 5 greps the body — the doc minus Appendix R — and, where marked, the sources file)

The `LP_NUM_THREADS` cross-process claims: `not visible`, `S:User-Startup`, `launcher`, `global var`,
`ENV: file` (body + sources). The LLVM overclaims: `+ LLVM`, `LLVM version +`, `same LLVM`,
`held bit-exact`, `bit-exact across`, `bit-exact oracle` (body + sources). The two-buffer audio
leftovers: `two requests`, `two struct AHIRequest`, `two chained`, `40 ms total`, `< 80 ms queue`,
`double-buffer pair`. `same as device-lost`. `missing an explicit rasterization state`. The Rename
fabrication: `verified at M0`, `AmigaDOS returns`, `dos.md §Rename`, `returns DOSFALSE with`
(body + sources). `shell cd → room transition`. `Draws happen in`. `routed to the engine arena`.
`` outside `platform/` ``. `(both platforms)`. `(revision 1|2|3)`, `Revision 1|2|3 adds|closes`.
Any body line with `VK_KHR_surface` that lacks "only when", "Linux", `SDL_Vulkan_GetInstanceExtensions`,
"no WSI" or "never". Script 6 keeps rev 3's banned list as a regression check.

## Appendix R (replace rev 3's)

A 3-column table with one row for each of R1 R2 R3 R9 R10 R11 R12 R13 R16 R17 R18 R21 B1 B2 B3 B4 B5 B6:

    | R11 | <one-line decision> | §4.6 |

The section is the one in the fixed-decisions table where one exists. The row is a summary only: a
decision that exists only in Appendix R counts as not done.

## Acceptance scripts (run all of them at the end; paste every output verbatim into the report)

These scripts were run against the rev-3 document, where they fail on every item above. They were
also run against a synthetic document built to this spec, where scripts 1–7 print PASS. So they
are passable as written. Set these once:

````bash
KD=~/Work/Kitty/Docs/Development; DOC=$KD/kitty_engine_design.md; SRC=$KD/kitty_engine_design_sources.md
VK=~/Work/projects/Vulkan4Aros; R=$(mktemp -d /tmp/kitty-rev4.XXXXXX); echo "scratch: $R"
````

**Script 0: baseline. Run it BEFORE editing, and keep `$R` for the session.**

````bash
git -C "$VK" status --short > "$R/vk_start.txt"
( cd "$KD" && find . -type f ! -name kitty_engine_design.md ! -name kitty_engine_design_sources.md -print0 \
  | sort -z | xargs -0 sha256sum ) > "$R/protected_start.txt"
wc -l "$DOC" "$SRC"
````

**Script 1: room grammar, example and negative examples (R2)**

````bash
python3 -I - "$DOC" <<'PY'
import re,sys,collections
t=open(sys.argv[1]).read(); F=[]
TPC=8; HOLD=25; MAXCLONES=16      # fixed by the prompt, never read from the document
EBNF='''
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
'''
norm=lambda s:[re.sub(r'\s+',' ',l).strip() for l in s.strip().splitlines() if l.strip()]
eb=re.findall(r'```ebnf\n(.*?)```',t,re.S)
if len(eb)!=1 or norm(eb[0])!=norm(EBNF): F.append("need exactly one ```ebnf block, equal to the prompt's grammar")
SYN={'SPAWN':r'SPAWN \d+,\d+','ENTITY':r'ENTITY \w+ nybble patrol \d+,\d+->\d+,\d+ speed=\d+ phase=\d+',
     'GATE':r'GATE \w+ (semaphore|mutex|requester) pos=\d+,\d+( n=\d+)?( retry| cancel-seals=\d+,\d+)*',
     'PANE':r'PANE \w+ pos=\d+,\d+ depth=(front|mid|back)','WIN':r'WIN exit pos=\d+,\d+'}
def validate(src):
    L=[(i+1,l.split(';')[0].rstrip()) for i,l in enumerate(src.splitlines())]; L=[x for x in L if x[1].strip()]
    kw=lambda j:L[j][1].split()[0] if j<len(L) else None
    ln=lambda j:L[j][0] if j<len(L) else (L[-1][0]+1 if L else 1)
    for j,s in enumerate(['ROOM','SEED','PAR','GRID']):
        if kw(j)!=s: return (ln(j),'order',f'expected {s}'),None
    h=L[0][1].split(); p=L[2][1].split()
    if not re.fullmatch(r'ROOM [A-Za-z]\w* \d+ \d+ ch\d',L[0][1]): return (L[0][0],'order','bad ROOM line'),None
    if not re.fullmatch(r'SEED 0x[0-9A-Fa-f]+',L[1][1]): return (L[1][0],'order','bad SEED line'),None
    if not re.fullmatch(r'PAR \d+ \d+ \d+',L[2][1]): return (L[2][0],'order','bad PAR line'),None
    w,hh=int(h[2]),int(h[3]); par=list(map(int,p[1:]))
    j=4; rows=[]
    while j<len(L) and kw(j)!='ENDGRID': rows.append(L[j]); j+=1
    if j>=len(L): return (ln(j),'order','expected ENDGRID'),None
    for i,(n,r) in enumerate(rows):
        if i==hh: return (n,'count',f'row count {len(rows)} != declared {hh}'),None
        b=sorted(set(r)-set('#.'))
        if b: return (n,'tile',f"illegal tile '{b[0]}' in row {i}"),None
        if len(r)!=w: return (n,'width',f'row width {len(r)} != declared {w}'),None
    if len(rows)<hh: return (L[j][0],'count',f'row count {len(rows)} != declared {hh}'),None
    tail=L[j+1:]; cell=lambda x,y:rows[y][1][x] if 0<=y<hh and 0<=x<w else None
    order=[l.split()[0] for _,l in tail]
    if not order or order[0]!='SPAWN': return (tail[0][0] if tail else ln(j+1),'order','expected SPAWN'),None
    if order.count('WIN')!=1 or order[-1]!='WIN' or order.count('SPAWN')!=1:
        return (tail[-1][0],'order','expected exactly one SPAWN first and one WIN last'),None
    co=set()
    for n,l in tail:
        k=l.split()[0]
        if k not in SYN or not re.fullmatch(SYN[k],l): return (n,'order',f'expected a valid object line'),None
        for x,y in re.findall(r'(\d+),(\d+)',l):
            x,y=int(x),int(y); co.add((x,y))
            if not(0<=x<w and 0<=y<hh): return (n,'bounds',f'position ({x},{y}) out of bounds ({w},{hh})'),None
            if cell(x,y)!='.': return (n,'floor',f'position ({x},{y}) not on floor'),None
        m=re.search(r'\bn=(\d+)',l)
        if k=='GATE' and ' semaphore ' in l and (not m or int(m.group(1))<1): return (n,'semaphore','semaphore n must be >= 1'),None
    sp=tuple(map(int,re.findall(r'\d+',tail[0][1]))); ex=tuple(map(int,re.findall(r'\d+',tail[-1][1])))
    return None,dict(w=w,h=hh,par=par,cell=cell,sp=sp,ex=ex,co=co)
g=re.findall(r'```kittyroom\n(.*?)```',t,re.S)
if len(g)!=1: F.append(f"need exactly one ```kittyroom block, found {len(g)}")
else:
    e,I=validate(g[0])
    if e: F.append(f"valid example rejected: line {e[0]}: {e[2]}")
    else:
        D={I['sp']:0}; q=collections.deque([I['sp']])
        while q:
            x,y=q.popleft()
            for n in ((x+1,y),(x-1,y),(x,y+1),(x,y-1)):
                if n not in D and I['cell'](*n)=='.': D[n]=D[(x,y)]+1; q.append(n)
        tk,vc,cl=I['par']
        if I['ex'] not in D: F.append("WIN unreachable from SPAWN")
        else:
            print(f"path {D[I['ex']]} cells x {TPC} = {D[I['ex']]*TPC} ticks min; PAR {tk} ticks, {vc} commits, {cl} clones")
            if tk<D[I['ex']]*TPC: F.append(f"PAR ticks {tk} < {D[I['ex']]} cells x {TPC}")
        if vc*HOLD>tk: F.append(f"PAR verb-commits {vc} x {HOLD}-tick hold > PAR ticks {tk}")
        if cl>MAXCLONES: F.append(f"PAR clones-max {cl} > {MAXCLONES}")
        hc=re.search(r'Hand-check:([^\n]*)',t)
        if not hc: F.append("no 'Hand-check:' line")
        else:
            got={(int(a),int(b)) for a,b in re.findall(r'\((\d+),(\d+)\)',hc.group(1))}
            if got!=I['co']: F.append(f"Hand-check coordinates {sorted(got)} != example {sorted(I['co'])}")
            for a,b in re.findall(r'(\d+) ticks = ([\d.]+) s',hc.group(1)):
                if abs(int(a)/50-float(b))>0.005: F.append(f"{a} ticks != {b} s at 50 Hz")
if not re.search(r'^TICKS_PER_CELL = 8\b',t,re.M): F.append("no line starting 'TICKS_PER_CELL = 8'")
if re.search(r'TICKS_PER_CELL\s*=\s*(?!8\b)\d',t): F.append("another TICKS_PER_CELL value appears")
KEY={'width':'row width','count':'row count','tile':'illegal tile','bounds':'out of bounds','floor':'not on floor','semaphore':'semaphore','order':'expected'}
bad=re.findall(r'```kittyroom-bad\n(.*?)```\s*\nExpected: `Software Failure\. room\.txt:(\d+): ([^`]+)`',t,re.S)
if len(bad)!=len(re.findall(r'```kittyroom-bad',t)): F.append("every ```kittyroom-bad block must be followed by an Expected: line")
seen=set()
for i,(src,n,msg) in enumerate(bad,1):
    e,_=validate(src)
    if not e: F.append(f"bad example {i} is accepted by the validator"); continue
    seen.add(e[1])
    if int(n)!=e[0] or KEY[e[1]] not in msg: F.append(f"bad example {i}: doc says line {n} '{msg}', validator says line {e[0]} '{e[2]}'")
need={'width','count','tile','bounds','semaphore'}
if need-seen: F.append(f"bad examples missing kinds: {sorted(need-seen)}")
print("ROOM PASS" if not F else "ROOM FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 2: §3 compiles; interfaces and module fields (B1, B2, R18)**

````bash
python3 -I - "$DOC" "$R" <<'PY'
import re,sys,subprocess
t=open(sys.argv[1]).read(); R=sys.argv[2]; F=[]
i3=t.index('\n## 3.'); i4=t.index('\n## 4.'); i5=t.index('\n## 5.')
s3,s4=t[i3:i4],t[i4:i5]
code='\n'.join(re.findall(r'```cpp\n(.*?)```',s3,re.S))
open(f"{R}/s3.h","w").write("#pragma once\n#include <cstdint>\n#include <cstddef>\n#include <vulkan/vulkan.h>\n"+code+"\n")
open(f"{R}/s3.cpp","w").write('#include "s3.h"\n')
r=subprocess.run(['g++','-std=c++20','-fsyntax-only','-Wall','-Werror','-Wredundant-decls',f'{R}/s3.cpp'],capture_output=True,text=True)
if r.returncode:
    e=[l for l in r.stderr.splitlines() if 'error' in l][:12]
    F.append(f"§3 cpp blocks do not compile (g++ -fsyntax-only -Wall -Werror -Wredundant-decls), first errors:\n    "+"\n    ".join(e))
c=re.sub(r'/\*.*?\*/','',re.sub(r'//[^\n]*','',code),flags=re.S)
fn=set(re.findall(r'\b([a-z_][a-z0-9_]*)\s*\(',c))
mods=re.split(r'^### (4\.\d+) ',s4,flags=re.M)
for k in range(1,len(mods),2):
    n,body=mods[k],mods[k+1]
    for f in ('R','I','D','T','E','A','Test'):
        cnt=len(re.findall(rf'^- \*\*{f}:\*\*',body,re.M))
        if cnt!=1: F.append(f"§{n}: {cnt} '- **{f}:**' bullets (need exactly 1)")
    if n=='4.13': continue
    il=re.search(r'^- \*\*I:\*\*([^\n]*)',body,re.M)
    if not il: continue
    names=[x for x in re.findall(r'`([^`]+)`',il.group(1)) if re.fullmatch(r'[a-z_][a-z0-9_]*',x)]
    if not names: F.append(f"§{n}: I: names no backticked function")
    miss=[x for x in names if x not in fn]
    if miss: F.append(f"§{n}: I: names not declared in §3: {', '.join(miss)}")
inl=re.findall(r'\b[a-z_]+\(\s*(?:const\s+)?[A-Z]\w*\s*[&*]',s4)
if inl: F.append(f"{len(inl)} signatures written in §4 (must live in §3): {inl[:5]}")
for a,b in (('RendCtx','RenderCtx'),('Alloc','Allocator'),('Plat','PlatAROS')):
    if re.search(rf'\b{a}\b',t) and re.search(rf'\b{b}\b',t): F.append(f"name drift: both {a} and {b}")
print(f"{len(fn)} functions declared in §3")
print("TYPES PASS" if not F else "TYPES FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 3: line budget and the 20k cap (R9)**

````bash
python3 -I - "$DOC" <<'PY'
import re,sys
t=open(sys.argv[1]).read(); F=[]
body=t[:t.index('## Appendix R')] if '## Appendix R' in t else t
H={m.group(1):float(m.group(2)) for m in re.finditer(r'^### 4\.\d+ `([^`]+)`[^\n]*?LB ([\d.]+)k',t,re.M)}
if '<!-- LB-TABLE -->' not in t: print("BUDGET FAIL: no <!-- LB-TABLE --> marker"); sys.exit(1)
i=t.index('<!-- LB-TABLE -->'); j=t.find('\n\n',i+20); seg=t[i:j if j>0 else len(t)]
T={m.group(1):float(m.group(2)) for m in re.finditer(r'^\| `([^`]+)` \| ([\d.]+) \|',seg,re.M)}
tm=re.search(r'\*\*Engine total\*\* \| \*\*([\d.]+)\*\*',seg)
if H!=T: F.append(f"headings vs table differ: {sorted(set(H.items())^set(T.items()))}")
eng=round(sum(v for k,v in T.items() if not k.startswith('tools')),2)
if not tm: F.append("no **Engine total** row")
elif abs(eng-float(tm.group(1)))>0.05: F.append(f"rows sum {eng} != stated total {tm.group(1)}")
if 'The 20k engine cap is binding.' not in body: F.append("missing literal 'The 20k engine cap is binding.'")
for s in re.split(r'(?<=[.;|])\s+',body):
    if re.search(r'\b20 ?k\b',s) and re.search(r'\b(target|tracked|soft|aspiration\w*|goal|enforced at|not enforced)\b',s,re.I):
        F.append(f"20k weakened: '{s.strip()[:110]}'")
q=[l for l in body.splitlines() if re.match(r'^\| Q\d+ \|',l) and re.search(r'\b20 ?k\b',l)]
allowed={round(eng,1),20.0}
if eng>20.0:
    if len(q)!=1: F.append(f"engine {eng}k > 20k: need exactly one §15 Q row mentioning 20k, found {len(q)}")
    else:
        cut=[m for m in re.findall(r'`([^`]+)`',q[0]) if m in T]
        cs=round(sum(T[m] for m in cut),1); rest=round(eng-cs,1); allowed|={cs,rest}
        if not cut: F.append("Q row names no backticked module to cut")
        elif not re.search(rf'→ ~?{rest}\s?k',q[0]): F.append(f"Q row must state '→ ~{rest} k' ({eng} − {cs} for {cut})")
        elif rest>20.0: F.append(f"proposed cut leaves {rest}k > 20k")
k4=t.index('\n## 4.'); k5=t.index('\n## 5.')
for blk in [t[k4:k5]]+q:
    for x in re.findall(r'~?(\d+(?:\.\d+)?)\s?k\b',blk):
        if round(float(x),1) not in allowed and not re.search(rf'LB {x}k',blk): F.append(f"stray budget figure {x} k (allowed {sorted(allowed)})")
print(f"engine total {eng}k"); print("BUDGET PASS" if not F else "BUDGET FAIL\n  "+"\n  ".join(sorted(set(F),key=F.index))); sys.exit(bool(F))
PY
````

**Script 4: World size and snapshot budget arithmetic (R16)**

````bash
python3 -I - "$DOC" "$R" <<'PY'
import re,sys,subprocess
t=open(sys.argv[1]).read(); R=sys.argv[2]; F=[]
H=[(m.start(),len(m.group(1)),m.group(2).rstrip('.')) for m in re.finditer(r'^(#{2,3}) (\S+)',t,re.M)]
def sec(n):
    for k,(p,l,num) in enumerate(H):
        if num==n: return t[p:next((q for q,l2,_ in H[k+1:] if l2<=l),len(t))]
    return ''
s=sec('5.5')
rows=[int(b) for b in re.findall(r'^\|[^\n]*\|\s*(\d+)\s*\|\s*$',s,re.M)]
tm=re.search(r'\*\*Total\*\*[^\n]*\*\*(\d+) B\*\*',s)
N=int(tm.group(1)) if tm else None
if not tm: F.append("§5.5: no '**Total** … **N B**' row (N without commas)")
elif sum(rows)!=N: F.append(f"§5.5 rows sum {sum(rows)} != total {N}")
open(f"{R}/sz.cpp","w").write('#include "s3.h"\n#include <cstdio>\nint main(){std::printf("%zu\\n",sizeof(ForkRec));}\n')
r=subprocess.run(['g++','-std=c++20',f'-I{R}',f'{R}/sz.cpp','-o',f'{R}/sz'],capture_output=True,text=True)
FR=int(subprocess.run([f'{R}/sz'],capture_output=True,text=True).stdout) if r.returncode==0 else None
if FR is None: F.append("cannot compute sizeof(ForkRec): §3 does not compile (see script 2)")
b=re.search(r'BUDGET keyframe=(\d+) B; ring=19\*(\d+)=(\d+) B; fork=(\d+) B; forks=16\*(\d+)=(\d+) B; total=(\d+)\+(\d+)=(\d+) B',sec('5.3'))
if not b: F.append("§5.3: no BUDGET line in the prompt's exact form")
elif N is not None:
    kf,rn,ring,fk,fn,forks,ta,tb,tot=map(int,b.groups())
    if kf!=N or rn!=N: F.append(f"BUDGET keyframe/ring use {kf}/{rn}, World is {N}")
    if ring!=19*rn: F.append(f"ring {ring} != 19*{rn}")
    if FR is not None and fk!=FR: F.append(f"fork={fk} B but sizeof(ForkRec)={FR}")
    if fn!=fk or forks!=16*fk: F.append(f"forks {forks} != 16*{fk}")
    if ta!=ring or tb!=forks or tot!=ta+tb: F.append(f"total {tot} != {ring}+{forks}")
    for n_ in ('4.4',):
        for v in (N,tot):
            if str(v) not in sec(n_): F.append(f"§{n_} does not state {v} B")
    print(f"World {N} B, sizeof(ForkRec) {FR} B, total {tot} B")
q07=''.join(l for l in t.splitlines() if l.startswith('| Q07 |'))
for name,blk in (('§4.4',sec('4.4')),('§5.3',sec('5.3')),('§5.5',s),('Q07',q07)):
    for x in re.findall(r'[\d.,]+\s*(?:KB|KiB|kB|MB)\b',blk): F.append(f"{name}: KB-style figure '{x}' (state bytes from the BUDGET line)")
print("WORLD PASS" if not F else "WORLD FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 5: fixed decisions, bans, SURFACE_LOST row, title, Appendix R**

````bash
python3 -I - "$DOC" "$SRC" <<'PY'
import re,sys
t=open(sys.argv[1]).read(); src=open(sys.argv[2]).read(); F=[]
body=t[:t.index('## Appendix R')] if '## Appendix R' in t else t
H=[(m.start(),len(m.group(1)),m.group(2).rstrip('.')) for m in re.finditer(r'^(#{2,3}) (\S+)',t,re.M)]
def sec(n):
    for k,(p,l,num) in enumerate(H):
        if num==n: return t[p:next((q for q,l2,_ in H[k+1:] if l2<=l),len(t))]
    return None
REQ=[  # (id, section, literal) — the literal must appear verbatim in that section
('R1','6.1','On AROS, `ppEnabledExtensionNames` contains no WSI extension; the loader adds `VK_KHR_surface` and `VK_KHR_xcb_surface` only when the ICD advertises them.'),
('R1','6.1','On Linux, the instance extension list is exactly what `SDL_Vulkan_GetInstanceExtensions` returns.'),
('R1','6.4','The NVK self-test instance is created with zero instance extensions.'),
('R3','4.5','Before `vkCreateInstance`, `platform/aros` calls `SetVar("LP_NUM_THREADS", "0", -1, GVF_LOCAL_ONLY)`.'),
('R3','4.5',"**UNVERIFIED:** whether lavapipe in the same process reads a process-local variable; the M0 probe logs the `GetVar` result and lavapipe's worker-thread count."),
('R9','4','The 20k engine cap is binding.'),
('R10','9','measured one depth-bias test with a pixel-count metric (`diff=153600` for both bias signs); it is not a bit-exactness proof.'),
('R10','2',"Golden ↔ host CI must be bit-exact; guest ↔ golden is a proxy check whose tolerance M0 sets, and the guest's LLVM version need not match the host's."),
('R11','4.6','Four chained 960-frame `CMD_WRITE` requests form an 80 ms queue.'),
('R11','4.6','The queue covers any gap between services shorter than 80 ms; a longer frame underruns, and the underrun is logged with its tick.'),
('R13','4.5','Known-bad (c): see H5.'),
('R17','4.12','Inferred from the handler sources: `Rename` onto an existing destination fails with `ERROR_OBJECT_EXISTS`, raised by the filesystem handler, not by `rom/dos/rename.c`; saves therefore use two slots instead of replace-by-rename.'),
('B3','5.1','Step 0 applies the `Event`s queued for this tick, sorted by (kind, a).'),
('B4','5.6','No draw occurs in 1.0 (§5.1).'),
('B5','9','Global `operator new`/`delete` are defined only as traps that call `guru_report` and exit; a link-time check fails the build if any engine object references them.'),
('B5','4.13','namespace-scope object with a non-trivial constructor'),
]
BAN=[  # (id, regex, scope) — scope 'b' = body (doc minus Appendix R), 's' = sources file
('R1',r'\(both platforms\)','b'),
('R3',r'not visible','bs'),('R3',r'S:User-Startup','bs'),('R3',r'launcher','bs'),('R3',r'global var','bs'),('R3',r'ENV: file','bs'),
('R10',r'\+ LLVM','bs'),('R10',r'LLVM version \+','bs'),('R10',r'same LLVM','bs'),('R10',r'held bit-exact','bs'),('R10',r'bit-exact (across|oracle)','bs'),
('R11',r'two (chained )?(requests|`?struct AHIRequest)','b'),('R11',r'two chained','b'),('R11',r'40 ms total','b'),('R11',r'< 80 ms queue','b'),('R11',r'double-buffer pair','b'),
('R12',r'same as device-lost','b'),
('R13',r'missing an explicit rasterization state','b'),
('R17',r'verified at M0','bs'),('R17',r'AmigaDOS returns','bs'),('R17',r'dos\.md`? §Rename','bs'),('R17',r'returns `?DOSFALSE`? with','bs'),
('B3',r'shell cd → room transition','b'),
('B4',r'Draws happen in','b'),
('B5',r'routed to the engine arena','b'),('B5',r'outside `platform/`','b'),
('B6',r'\(revision [123]\)','b'),('B6',r'Revision [123] (adds|closes)','b'),
]
for i,n,lit in REQ:
    s=sec(n)
    if s is None: F.append(f"{i}: section §{n} not found")
    elif lit not in s: F.append(f"{i}: §{n} lacks literal: {lit[:90]}")
for i,rx,sc in BAN:
    for name,txt in (('doc','b' in sc and body),('sources','s' in sc and src)):
        if txt:
            n=len(re.findall(rx,txt))
            if n: F.append(f"{i}: {n} x /{rx}/ in {name}")
for l in body.splitlines():
    if 'VK_KHR_surface' in l and not re.search(r'only when|Linux|SDL_Vulkan_GetInstanceExtensions|no WSI|never',l):
        F.append(f"R1: line requests VK_KHR_surface: {l.strip()[:100]}")
row=[l for l in sec('6.2').splitlines() if l.startswith('| `VK_ERROR_SURFACE_LOST_KHR`')] if sec('6.2') else []
if len(row)!=1: F.append("R12: need exactly one VK_ERROR_SURFACE_LOST_KHR row in §6.2")
elif not (re.search(r'recreate',row[0]) and 'loader' in row[0] and 'once' in row[0]) or re.search(r'fallback|lavapipe|device-lost',row[0],re.I):
    F.append(f"R12: SURFACE_LOST row must recreate through the loader once, with no fallback: {row[0][:100]}")
if t.splitlines()[0]!='# Kitty: Core Awakening — Engine Design Document (revision 4)': F.append("B6: line 1 is not the revision-4 title")
IDS=['R1','R2','R3','R9','R10','R11','R12','R13','R16','R17','R18','R21','B1','B2','B3','B4','B5','B6']
a=t[t.index('## Appendix R'):] if '## Appendix R' in t else ''
rows={m.group(1):m.group(2) for m in re.finditer(r'^\| (R\d+|B\d+) \| [^|\n]+ \| §([\d.]+) \|\s*$',a,re.M)}
miss=[i for i in IDS if i not in rows]
if miss: F.append(f"Appendix R missing rows: {miss}")
want={}
for i,n,_ in REQ: want.setdefault(i,n)
for i,n in rows.items():
    if i in want and n!=want[i] and not any(r[0]==i and r[1]==n for r in REQ): F.append(f"Appendix R {i}: §{n}, expected §{want[i]}")
print(f"{len(REQ)} literals, {len(BAN)} bans, {len(rows)} Appendix R rows")
print("DECISIONS PASS" if not F else "DECISIONS FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 6: rev-3 banned list (regression)**

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

**Script 7: sources — definitions, gaps, clusters, citation subjects, VK lines, Rename handlers (R17, R21)**

````bash
python3 -I - "$DOC" "$SRC" "$VK" <<'PY'
import re,sys,os,collections
doc=open(sys.argv[1]).read(); src=open(sys.argv[2]).read(); vk=os.path.expanduser(sys.argv[3]); F=[]
body=doc[:doc.index('## Appendix R')] if '## Appendix R' in doc else doc
ids=lambda s:[n for b in re.findall(r'\[([^\]]*S\d+[^\]]*)\]',s) for n in re.findall(r'S(\d+)',b)]
E={m.group(1):m.group(2) for m in re.finditer(r'^- \[S(\d+)\](.*)$',src,re.M)}
cited=set(ids(doc))
if cited-set(E): F.append(f"cited but undefined: {sorted(cited-set(E),key=int)}")
new=sorted(int(x) for x in E if int(x)>=80)
gaps=sorted(set(range(80,max(new)+1))-set(new)) if new else []
if gaps: F.append(f"numbering gaps: {gaps}")
unit=lambda b:[re.sub(r'\s+',' ',u).strip() for u in re.split(r'[,;]',b) if re.match(r'\s*S\d+',u)]
for k,l in enumerate(body.splitlines(),1):
    for cl in re.findall(r'(?:\[[^\]]*S\d+[^\]]*\][\s,;]*)+',l):
        d=[u for u,c in collections.Counter(x for b in re.findall(r'\[([^\]]*)\]',cl) for x in unit(b)).items() if c>1]
        if d: F.append(f"doc line {k}: '{d[0]}' twice in one citation cluster: {cl.strip()[:80]}")
TERMS={'SDL_Vulkan_GetInstanceExtensions':1,'ERROR_OBJECT_EXISTS':1,'GVF_LOCAL_ONLY':1,'diff=153600':1}
for k,l in enumerate(body.splitlines(),1):
    for term,req in TERMS.items():
        if term not in l: continue
        c=[n for n in ids(l) if n in E]
        if not c:
            if req: F.append(f"doc line {k}: '{term}' with no citation")
        elif not any(term in E[n] for n in c): F.append(f"doc line {k}: '{term}' cites S{', S'.join(c)}, none of which mention it")
VKC=lambda s:re.findall(r'VK/([\w./+-]+):(\d+)',s)
def line(p,l):
    f=os.path.join(vk,p)
    if not os.path.isfile(f): return None
    L=open(f,errors='replace').read().splitlines(); return L[int(l)-1] if 0<int(l)<=len(L) else None
for p,l in VKC(src):
    if line(p,l) is None: F.append(f"VK/{p}:{l} does not exist")
for n,e in E.items():
    if 'ERROR_OBJECT_EXISTS' in e:
        hits=[f"{p}:{l}" for p,l in VKC(e) if 'ERROR_OBJECT_EXISTS' in (line(p,l) or '') and 'rom/dos/' not in p]
        if len(hits)<2: F.append(f"S{n} mentions ERROR_OBJECT_EXISTS but cites {len(hits)} handler line(s) containing it (need >= 2, as VK/<path>:<line>)")
print("SOURCES PASS" if not F else "SOURCES FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 8: qmath harness (unchanged)**

````bash
T=$(mktemp -d)
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

**Loop rule:** if any script prints FAIL, fix the document and re-run **all** of scripts 1–8. Report
the final run only, with its full output. If a FAIL can't be fixed inside the authorization block,
report it as FAIL with the reason. Never weaken a script, and never word text just to slip past a
pattern. The scripts are the floor: a body sentence that contradicts a fixed decision fails review
even if every script passes.

## Final report (reply with this only, no narration)

1. Files changed, with line counts before → after (from scripts 0 and 9).
2. Skills loaded, and any that couldn't be loaded.
3. **The verbatim output of scripts 1–9 from the final run.**
4. Every place where you departed from this prompt, with quoted evidence. Write "none" only if
   every fixed sentence is verbatim and no structural rule was bent.
5. New or changed §15 owner questions (the 20k cap, if over).
6. UNVERIFIED items, each with its M0 probe.
7. For R17: the handler file, function name and line for each `ERROR_OBJECT_EXISTS` you cited.
8. Anything needed outside the authorization block.
