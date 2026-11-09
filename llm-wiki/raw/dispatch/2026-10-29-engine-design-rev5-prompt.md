# opencode prompt: engine design rev 5

> Source: Work/Kitty/Docs/Development/dispatch/engine_design_rev5_opencode_prompt.md
> Collected: 2026-10-30
> Published: 2026-10-29

# opencode prompt — Kitty: Core Awakening, engine design **revision 5** (2026-10-29)

## Owner authorization (read first — opencode never edits this block)

    WRITE: Docs/Development/kitty_engine_design.md (edit in place)
           Docs/Development/kitty_engine_design_sources.md (edit in place)
    DO NOT EDIT: every other file under Docs/Development/ (critiques, v2, review, dispatch/)
    CODE: NONE in any repository — no source files, build scripts, prototypes or test programs saved anywhere
    VERIFY: you may run the acceptance scripts below exactly as given, plus read-only shell commands.
           Scratch output goes only in a `mktemp -d /tmp/kitty-rev5.XXXXXX` directory, deleted at the end.
           The scripts compile the doc's §3 code blocks (g++ -fsyntax-only, plus one sizeof probe) and
           the `qmath` block with host g++ inside that directory
    BUILDS / BOOTS / DEPLOYS: NONE (no cross-compile, no QEMU, no lane)
    VULKAN4AROS REPO: READ-ONLY (cat/grep/ls/sed -n/git show/git log/git status only)
    GIT: no commit, no push, in any repository
    WEB: read-only research allowed; post nothing, sign up for nothing, download no binaries

Don't ask for, infer, widen or grant any authorization yourself. If the work seems to need
something outside this block, stop and list it in the final report.

## Why there's a revision 5

Revision 4 passed every script and fixed every rev-3 item. Review still found five blocking defects
that the scripts couldn't see:

- §4.6 and §7 open `ahi.device` on `AHI_NO_UNIT` and then send `CMD_WRITE`. That dereferences a NULL `io_Unit`.
- The `InputLine` layout is given three different ways.
- The default fix for the 20k cap cut three v2 1.0 features.
- The example room is an empty box that needs no verb.
- The FORK start snapshot has nowhere to live.

The MOD facts in §7 also contradict their own source. This revision fixes those. The scripts now
check meaning where they can: whether the example room needs a verb, AHI unit semantics against the
AHI source, and number ranges against the cited source text. They also check that no source entry
is lost.

**Owner decision, 2026-10-29:** the engine cap is raised to **23k** so that v2 1.0 scope stays whole
(`shell/`, `replay/` and `benchmark/` remain in 1.0). Don't propose cutting v2 scope anywhere.

## Inputs (read fully before editing)

- `~/Work/Kitty/Docs/Development/kitty_engine_design_rev4_critique.md` — the work list (§A 1–5, §B, §C).
  The ids below are A1–A5 (blocking), M1–M5 (major) and N1–N5 (minor), in the critique's order.
- `~/Work/Kitty/Docs/Development/kitty_engine_design.md` and `kitty_engine_design_sources.md` — the files you edit.
- `~/Work/Kitty/Docs/Development/kitty_core_awakening_design_v2.md` — authoritative. **If a fix would
  contradict v2, don't decide it. Add it to §15 as an owner question and leave v2's rule in force.**
  The 23k cap is the one owner-made exception, and it is recorded in §1.
- `dispatch/engine_design_rev4_opencode_prompt.md` and the earlier prompts — still in force wherever
  this prompt doesn't override them. Every rev-4 fixed sentence stays verbatim, except R9's
  "The 20k engine cap is binding.", which this prompt replaces.
- Vulkan4Aros (READ-ONLY) for every platform fact. Never run a write or destructive git command in `src/abi/v1/AROS*`.

## Skills (load each, and report which ones you loaded)

| Skill | Use it for |
|---|---|
| `aros-development` | **Mandatory, first.** Every AROS claim must agree with it. Where the skill and the wiki disagree, the newer dated wiki record wins: cite it and flag the conflict |
| `karpathy-guidelines` | Simplest decision that meets v2. Delete rather than specify something shaky |
| `karpathy-llm-wiki` | **Query mode only** over the Vulkan4Aros wiki |
| Web research | Primary sources only (ProTracker/MOD format docs, AHI developer docs, the Khronos spec, Mesa docs, AROS/AmigaOS autodocs) |

## Evidence rules

- **A citation must say only what its source says.** Before writing or keeping any `[S#]`, re-read the
  cited lines. A sources entry describes what the source states, not your conclusion from it.
- Code-derived facts are written "inferred from `VK/<path>:<line>`". Runtime behaviour is **UNVERIFIED:**
  with the M0 probe that settles it. The string "verified at M0" stays banned.
- **No source entry may be deleted.** You may correct an entry only to make it say what its source
  states. Script 7 lists every changed and added entry, and your report must justify each change.
- New sources continue from the highest number (S91 onward), with no gaps.
- **Number ranges in §4.6 and §7** (`a–b` or `a..b`, decimal or `0x` hex) must be stated by a source
  cited on the same line. Script 7 checks that both endpoints appear in that source entry.
- **MOD facts:** fetch a primary ProTracker effect/format reference and quote it in a new entry. If
  the source contradicts a fixed MOD sentence below, **don't paste that sentence and don't edit the
  source entry to match it**. Report the item as FAIL, quote the source, and add a §15 question.

## Fixed decisions — paste each sentence verbatim into its section

Each sentence must appear **exactly** (same backticks, punctuation, Unicode dashes and arrows) inside
the named section (`§4` means anywhere under `## 4.`; `§3` means under `## 3.` but outside code
blocks). A citation may follow the sentence's full stop on the same line. Then remove or rewrite
every other sentence in the document, Appendix R included, that contradicts it. Contradictions the
scripts can't see will be found in review and will fail the item.

| Item | § | Sentence |
|---|---|---|
| A1 | 4.6 | `ahi.device` is opened on `AHI_DEFAULT_UNIT` (0) with a full `struct AHIRequest`. |
| A1 | 4.6 | `AHI_NO_UNIT` is never used: it leaves `io_Unit` NULL, and `CMD_WRITE` dereferences `io_Unit`. *(cite a source entry that quotes `VK/…/AHI/Device/device.c` for the unit values and `VK/…/AHI/Device/devcommands.c` where `CMD_WRITE` does `PutMsg(&ioreq->ahir_Std.io_Unit->unit_MsgPort, …)`, both as `VK/<path>:<line>`)* |
| A1 | 7 | `ahi.device` is opened on `AHI_DEFAULT_UNIT` (0) with a full `struct AHIRequest`. |
| A2 | 5.1 | `InputLine` is two bytes: byte 0 bits 0–3 are the direction mask and bits 4–7 are reserved; byte 1 bit 0 is fire and bits 1–7 are reserved. *(§3.1's declaration is `struct InputLine { u8 dir; u8 fire; };`, and its comment may only point at §5.1. §4.7 and Appendix R14 must not restate a different layout)* |
| A3 | 1 | v2 §8.1 targets at most 20k lines; on 2026-10-29 the owner raised the engine cap to 23k so that `shell/`, `replay/` and `benchmark/` stay in 1.0. |
| A3 | 4 | The 23k engine cap is binding. |
| A5 | 5.3 | The fork-moment `World` is kept in one `fork_base` keyframe outside the ring; only one FORK recording is open at a time, so one slot is enough. |
| A5 | 5.3 | `ForkRec` holds 500 ticks because v2 caps a FORK recording at 10 s [S1 §5.1]. |
| M1 | 7 | `Fxx` with xx = 00–1F sets the speed (ticks per row), and xx = 00 is ignored; xx = 20–FF sets the tempo (32–255 BPM). |
| M1 | 7 | A note's playback rate is the Paula clock divided by its period (PAL: 3546895 Hz); 50 Hz is the tick rate, not the pitch clock. |
| M1 | 7 | Set volume is effect `Cxx`, with volume 0–64. |
| M1 | 7 | MOD samples are signed 8-bit, so they have no byte order; `modplay` widens them to `i16` at load. |
| M2 | 4.6 | - \*\*T:\*\* Single-threaded; audio requests are serviced from the tick loop, audio never feeds `sim/`, and it is never hashed. *(this replaces the whole T bullet; the `**` markers are part of it)* |
| M3 | 6.1 | `VK_EXT_debug_utils` is enabled only in debug builds and only when `vkEnumerateInstanceExtensionProperties` lists it. *(every sentence in the body that mentions `VK_EXT_debug_utils` must be this one)* |
| M5 | 3 | This list is the frame loop; the per-tick order is §5.1's, starting with Step 0. *(directly after the §3 tick/frame list)* |
| N1 | 4.12 | A `current/` that fails to load is discarded with a log line; progress loads from the newest valid slot. |
| N2 | 5.1 | `qdiv` saturates and records `SIM_OVF` when the quotient leaves the `int32_t` range (e.g. `INT32_MIN / -1.0`). |
| N3 | 5.6 | The `SEED` hex value is zero-extended to 64 bits and used as the splitmix64 state. |
| N5 | 9 | No source id appears twice in one citation cluster. *(in the "Source numbering" paragraph that Appendix R21 points at)* |

## Structural requirements (no sentence to paste — the scripts check the structure)

- **A3 line budget (script 3).** The §4 headings and the `LB-TABLE` must carry exactly these LBs
  (in k): `platform/aros` 1.5, `platform/linux` 1.2, `sim/` 3.5, `snapshot/` 1.2, `render/` 2.5,
  **`audio/` 2.0**, `input/` 1.2, `assets/` 1.0, `level/` 1.2, `ui/` 1.0, `locale/` 0.8, `save/` 1.0,
  `tools/` 2.0, `game/` 1.0, `settings/` 0.6, `shell/` 0.8, `guru/` 0.6, `replay/` 0.6,
  `benchmark/` 0.8.
  - The total row is exactly `| **Engine total (excl. `tools/`)** | **22.5** |`.
  - §4.6 states: `` `audio/` LB 2.0k = `modplay` 1.5k + AHI/SDL stream and mixer 0.5k (C16). ``
  - The only body sentence that may mention 20k is the A3 §1 sentence. That includes §14's
    "~20k-line" and §1's old "at most 20k".
  - No sentence pairs 23k with target, tracked, soft, goal or provisional.
  - Exactly one §15 Q row covers the engine cap: it says `Decided 2026-10-29` and 23k.
  - No cut proposal remains anywhere: not "modules to cut", and not `cut `shell/``.
  - No other budget figure appears in §4 or in that Q row.
- **A4 example room (script 1).** Rev 4's grammar, the `TICKS_PER_CELL = 8` rule, the PAR rules and
  the negative examples all stay. In addition:
  - **With every `GATE` cell treated as closed, WIN must be unreachable from SPAWN.** Interior walls
    must force the path through a gate.
  - PAR verb-commits ≥ 1.
  - The `Hand-check:` line contains `0-verb path: none`, still lists exactly the example's
    coordinates, and states the path length with gates open.
  - Every position must be on floor, patrol endpoints included.
  - Keep the room believable as a Ch1 room. If its gate types don't fit Ch1, say so in the report.
- **A5 snapshot budget (script 4).**
  - §5.3 carries this line, with the numbers filled in:
    `BUDGET keyframe=N B; ring=19*N=X B; fork_base=N B; fork=F B; forks=16*F=Y B; total=X+N+Y=Z B`.
  - §4.4 states N and Z.
  - §5.5 has exactly one `| clones` row, naming `length` and `spawn position`.
  - The clone recording cursor (`input cursor` or `recording cursor`) appears in exactly one §5.5 row.
  - Update §5.5's derived-budget sentence and every other place where the old total appeared.
- **A2 (script 2).** §3.1 declares `InputLine` exactly once as `struct InputLine { u8 dir; u8 fire; };`.
  Its comment must not say "bit 4" or "5–7".
- **M4 (script 5).** The §4.5 `E:` bullet points at the §6.2 VkResult table.
- **Title (script 5).** Line 1 is exactly `# Kitty: Core Awakening — Engine Design Document (revision 5)`.
  The appendix heading is exactly `## Appendix R — Revision 5 changelog`. No "(revision 1–4)" text
  remains in the body, nor "Revision 4 —" in the closing line.
- **Sources (script 7).**
  - A body line containing `AHI_DEFAULT_UNIT`, `3546895` or `Cxx` must cite an entry that contains
    the same string.
  - An entry that mentions `AHI_DEFAULT_UNIT` must cite a VK line containing `AHI_DEFAULT_UNIT` and a
    VK line containing `io_Unit->unit_MsgPort`.
  - Every rev-4 citation rule still holds.

## Banned (script 5 checks the body, i.e. the doc minus Appendix R; "+A" also checks Appendix R, "+S" the sources file)

- **AHI:** any sentence that mentions `AHI_NO_UNIT` and isn't the A1 sentence;
  `OpenDevice("ahi.device", AHI_NO_UNIT`.
- **InputLine:** `fire bit 4` / `fire 4`, and `bits 5–7` (+A).
- **MOD:** `17..64`, `50 Hz clock`, `` `$`/`/` `` (+S), `byte-swizzl`, `big-endian signed 8-bit`,
  `tempo 28`.
- **Audio service:** `tick-independent`, `own AHI timing`.
- **Cross-reference and minors:** `§6.5 table`, `falls back to the other slot`,
  `qdiv cannot overflow`, `first 8 bytes of the seed`.
- **Revision labels:** `(revision 1–4)` and `Revision 1–4 adds|closes|—`.
- **Rev 4's bans** stay. Script 6 keeps rev 3's banned list.

## Appendix R

- Keep rev 4's rows and update any row whose decision changed: R9 → the 23k cap, R14 → "layout in §5.1".
- Add one row each for A1 A2 A3 A4 A5 M1 M2 M3 M4 M5 N1 N2 N3 N4 N5, in the form
  `| A1 | <one-line decision> | §4.6 |`.
- The section is the fixed-decision section where one exists; otherwise A4 → §8.1, M4 → §4.5, N4 → §4.
- A decision that exists only in Appendix R counts as not done.

## Acceptance scripts (run all of them at the end; paste every output verbatim into the report)

These scripts were run against the rev-4 document, where scripts 1, 2, 3, 4, 5 and 7 fail on the
items above. They were also run against a copy of the rev-4 document with exactly these fixes
applied, where all of them print PASS. They catch 11 of 11 single deliberate errors (a lost source
entry, an open gap in the example's wall, the old fire bit, audio at 1.2k, a BUDGET without
`fork_base`, a stray `AHI_NO_UNIT`, an unsourced range, a second `VK_EXT_debug_utils` sentence, a cut
proposal, a "target" next to 23k, an AHI entry citing the wrong line). So they are passable as written.
Set these once:

````bash
KD=~/Work/Kitty/Docs/Development; DOC=$KD/kitty_engine_design.md; SRC=$KD/kitty_engine_design_sources.md
VK=~/Work/projects/Vulkan4Aros; R=$(mktemp -d /tmp/kitty-rev5.XXXXXX); echo "scratch: $R"
````

**Script 0: baseline. Run it BEFORE editing, and keep `$R` for the session (script 7 reads its sources snapshot).**

````bash
git -C "$VK" status --short > "$R/vk_start.txt"
( cd "$KD" && find . -type f ! -name kitty_engine_design.md ! -name kitty_engine_design_sources.md -print0 \
  | sort -z | xargs -0 sha256sum ) > "$R/protected_start.txt"
cp "$DOC" "$R/doc_start.md"; cp "$SRC" "$R/src_start.md"
wc -l "$DOC" "$SRC"
````

**Script 1: room grammar, example, the room must need a verb, negative examples (R2, A4)**

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
    co=set(); gates=[]
    for n,l in tail:
        k=l.split()[0]
        if k not in SYN or not re.fullmatch(SYN[k],l): return (n,'order',f'expected a valid object line'),None
        for x,y in re.findall(r'(\d+),(\d+)',l):
            x,y=int(x),int(y); co.add((x,y))
            if not(0<=x<w and 0<=y<hh): return (n,'bounds',f'position ({x},{y}) out of bounds ({w},{hh})'),None
            if cell(x,y)!='.': return (n,'floor',f'position ({x},{y}) not on floor'),None
        if k=='GATE': gates.append(tuple(map(int,re.search(r'pos=(\d+),(\d+)',l).groups())))
        m=re.search(r'\bn=(\d+)',l)
        if k=='GATE' and ' semaphore ' in l and (not m or int(m.group(1))<1): return (n,'semaphore','semaphore n must be >= 1'),None
    sp=tuple(map(int,re.findall(r'\d+',tail[0][1]))); ex=tuple(map(int,re.findall(r'\d+',tail[-1][1])))
    return None,dict(w=w,h=hh,par=par,cell=cell,sp=sp,ex=ex,co=co,gates=gates)
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
        B={I['sp']}; q=collections.deque([I['sp']])      # 0-verb walk: every GATE cell is closed
        while q:
            x,y=q.popleft()
            for n in ((x+1,y),(x-1,y),(x,y+1),(x,y-1)):
                if n not in B and n not in I['gates'] and I['cell'](*n)=='.': B.add(n); q.append(n)
        if I['ex'] in B: F.append("WIN is reachable with every GATE closed: the example needs no verb")
        if vc<1: F.append("PAR verb-commits must be >= 1 in the example")
        if I['ex'] not in D: F.append("WIN unreachable from SPAWN")
        else:
            print(f"path {D[I['ex']]} cells x {TPC} = {D[I['ex']]*TPC} ticks min; PAR {tk} ticks, {vc} commits, {cl} clones")
            if tk<D[I['ex']]*TPC: F.append(f"PAR ticks {tk} < {D[I['ex']]} cells x {TPC}")
        if vc*HOLD>tk: F.append(f"PAR verb-commits {vc} x {HOLD}-tick hold > PAR ticks {tk}")
        if cl>MAXCLONES: F.append(f"PAR clones-max {cl} > {MAXCLONES}")
        hc=re.search(r'Hand-check:([^\n]*)',t)
        if not hc: F.append("no 'Hand-check:' line")
        else:
            if '0-verb path: none' not in hc.group(1): F.append("Hand-check line lacks '0-verb path: none'")
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

**Script 2: §3 compiles; interfaces, module fields, InputLine declaration (B1, B2, R18, A2)**

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
il=re.findall(r'^\s*struct InputLine\b[^\n]*',s3,re.M)
if len(il)!=1 or not il[0].strip().startswith('struct InputLine { u8 dir; u8 fire; };'): F.append(f"§3: InputLine must be declared once as 'struct InputLine {{ u8 dir; u8 fire; }};' (A2): {il}")
elif re.search(r'bit ?4|5-7|5–7',il[0]): F.append(f"§3: InputLine comment contradicts the §5.1 layout: {il[0].strip()}")
print(f"{len(fn)} functions declared in §3")
print("TYPES PASS" if not F else "TYPES FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 3: line budget and the 23k cap (A3, N4)**

````bash
python3 -I - "$DOC" <<'PY'
import re,sys
t=open(sys.argv[1]).read(); F=[]
body=t[:t.index('## Appendix R')] if '## Appendix R' in t else t
CAP=23.0      # fixed by the owner 2026-10-29, never read from the document
EXP={'platform/aros':1.5,'platform/linux':1.2,'sim/':3.5,'snapshot/':1.2,'render/':2.5,'audio/':2.0,
     'input/':1.2,'assets/':1.0,'level/':1.2,'ui/':1.0,'locale/':0.8,'save/':1.0,'tools/':2.0,'game/':1.0,
     'settings/':0.6,'shell/':0.8,'guru/':0.6,'replay/':0.6,'benchmark/':0.8}
H={m.group(1):float(m.group(2)) for m in re.finditer(r'^### 4\.\d+ `([^`]+)`[^\n]*?LB ([\d.]+)k',t,re.M)}
if '<!-- LB-TABLE -->' not in t: print("BUDGET FAIL: no <!-- LB-TABLE --> marker"); sys.exit(1)
i=t.index('<!-- LB-TABLE -->'); j=t.find('\n\n',i+20); seg=t[i:j if j>0 else len(t)]
T={m.group(1):float(m.group(2)) for m in re.finditer(r'^\| `([^`]+)` \| ([\d.]+) \|',seg,re.M)}
if H!=EXP: F.append(f"§4 headings differ from the fixed LB set: {sorted(set(H.items())^set(EXP.items()))}")
if T!=EXP: F.append(f"LB table differs from the fixed LB set: {sorted(set(T.items())^set(EXP.items()))}")
eng=round(sum(v for k,v in EXP.items() if k!='tools/'),1)
tm=re.search(r'^\| \*\*Engine total \(excl\. `tools/`\)\*\* \| \*\*([\d.]+)\*\* \|',seg,re.M)
if not tm: F.append("no '| **Engine total (excl. `tools/`)** | **N** |' row")
elif float(tm.group(1))!=eng: F.append(f"stated total {tm.group(1)} != {eng}")
RAISE="v2 §8.1 targets at most 20k lines; on 2026-10-29 the owner raised the engine cap to 23k so that `shell/`, `replay/` and `benchmark/` stay in 1.0."
s1=body[body.index('\n## 1.'):body.index('\n## 2.')]
if RAISE not in s1: F.append("§1 lacks the cap-raise sentence (A3)")
if 'The 23k engine cap is binding.' not in body[body.index('\n## 4.'):body.index('\n## 5.')]: F.append("§4 lacks 'The 23k engine cap is binding.'")
rest=body.replace(RAISE,'')
for s in re.split(r'(?<=[.;|])\s+',rest):
    if re.search(r'\b20 ?k\b',s): F.append(f"20k mentioned outside the §1 raise sentence: '{s.strip()[:110]}'")
    if re.search(r'\b23 ?k\b',s) and re.search(r'\b(target|tracked|soft|aspiration\w*|goal|enforced at|not enforced|provisional)\b',s,re.I):
        F.append(f"23k cap weakened: '{s.strip()[:110]}'")
q=[l for l in body.splitlines() if re.match(r'^\| Q\d+ \|',l) and re.search(r'\b2[03] ?k\b|engine cap',l)]
if len(q)!=1 or 'Decided 2026-10-29' not in q[0] or not re.search(r'\b23 ?k\b',q[0]): F.append(f"need exactly one §15 Q row on the engine cap, marked 'Decided 2026-10-29' with 23k: {[x[:80] for x in q]}")
for l in body.splitlines()+t[t.index('## Appendix R'):].splitlines() if '## Appendix R' in t else body.splitlines():
    if re.search(r'modules to cut|\bcut `(shell|replay|benchmark)/`|`shell/`\s*\+\s*`benchmark/`',l): F.append(f"cut proposal remains: {l.strip()[:100]}")
allowed={eng,CAP,1.5,0.5}
k4=t.index('\n## 4.'); k5=t.index('\n## 5.')
for blk in [t[k4:k5]]+q:
    for x in re.findall(r'~?(\d+(?:\.\d+)?)\s?k\b',blk):
        if round(float(x),1) not in allowed and not re.search(rf'LB {x}k',blk): F.append(f"stray budget figure {x} k (allowed {sorted(allowed)})")
A='`audio/` LB 2.0k = `modplay` 1.5k + AHI/SDL stream and mixer 0.5k (C16).'
if A not in t[t.index('\n### 4.6'):t.index('\n### 4.7')]: F.append("§4.6 lacks the audio LB split sentence")
print(f"engine total {eng}k, cap {CAP}k"); print("BUDGET PASS" if not F else "BUDGET FAIL\n  "+"\n  ".join(sorted(set(F),key=F.index))); sys.exit(bool(F))
PY
````

**Script 4: World size, fork_base and snapshot budget arithmetic (R16, A5)**

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
b=re.search(r'BUDGET keyframe=(\d+) B; ring=19\*(\d+)=(\d+) B; fork_base=(\d+) B; fork=(\d+) B; forks=16\*(\d+)=(\d+) B; total=(\d+)\+(\d+)\+(\d+)=(\d+) B',sec('5.3'))
if not b: F.append("§5.3: no BUDGET line in the prompt's exact form")
elif N is not None:
    kf,rn,ring,fb,fk,fn,forks,ta,tb,tc,tot=map(int,b.groups())
    if kf!=N or rn!=N or fb!=N: F.append(f"BUDGET keyframe/ring/fork_base use {kf}/{rn}/{fb}, World is {N}")
    if ring!=19*rn: F.append(f"ring {ring} != 19*{rn}")
    if FR is not None and fk!=FR: F.append(f"fork={fk} B but sizeof(ForkRec)={FR}")
    if fn!=fk or forks!=16*fk: F.append(f"forks {forks} != 16*{fk}")
    if (ta,tb,tc)!=(ring,fb,forks) or tot!=ta+tb+tc: F.append(f"total must be {ring}+{fb}+{forks}={ring+fb+forks}, got {ta}+{tb}+{tc}={tot}")
    for v in (N,tot):
        if str(v) not in sec('4.4'): F.append(f"§4.4 does not state {v} B")
    print(f"World {N} B, sizeof(ForkRec) {FR} B, total {tot} B")
cl=[l for l in s.splitlines() if l.startswith('| clones')]
if len(cl)!=1 or not ('length' in cl[0] and 'spawn position' in cl[0]): F.append(f"§5.5 needs one '| clones' row naming 'length' and 'spawn position' (A5): {cl}")
cur=[l for l in s.splitlines() if l.startswith('|') and re.search(r'(input|recording) cursor',l)]
if len(cur)!=1: F.append(f"§5.5: the clone recording cursor must appear in exactly one row, found {len(cur)}")
q07=''.join(l for l in t.splitlines() if l.startswith('| Q07 |'))
for name,blk in (('§4.4',sec('4.4')),('§5.3',sec('5.3')),('§5.5',s),('Q07',q07)):
    for x in re.findall(r'[\d.,]+\s*(?:KB|KiB|kB|MB)\b',blk): F.append(f"{name}: KB-style figure '{x}' (state bytes from the BUDGET line)")
print("WORLD PASS" if not F else "WORLD FAIL\n  "+"\n  ".join(F)); sys.exit(bool(F))
PY
````

**Script 5: fixed decisions, bans, AHI/debug_utils sentences, SURFACE_LOST row, title, Appendix R**

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
('A1','4.6','`ahi.device` is opened on `AHI_DEFAULT_UNIT` (0) with a full `struct AHIRequest`.'),
('A1','4.6','`AHI_NO_UNIT` is never used: it leaves `io_Unit` NULL, and `CMD_WRITE` dereferences `io_Unit`.'),
('A1','7','`ahi.device` is opened on `AHI_DEFAULT_UNIT` (0) with a full `struct AHIRequest`.'),
('A2','5.1','`InputLine` is two bytes: byte 0 bits 0–3 are the direction mask and bits 4–7 are reserved; byte 1 bit 0 is fire and bits 1–7 are reserved.'),
('A3','4','The 23k engine cap is binding.'),
('A3','1','v2 §8.1 targets at most 20k lines; on 2026-10-29 the owner raised the engine cap to 23k so that `shell/`, `replay/` and `benchmark/` stay in 1.0.'),
('A5','5.3','The fork-moment `World` is kept in one `fork_base` keyframe outside the ring; only one FORK recording is open at a time, so one slot is enough.'),
('A5','5.3','`ForkRec` holds 500 ticks because v2 caps a FORK recording at 10 s [S1 §5.1].'),
('M1','7','`Fxx` with xx = 00–1F sets the speed (ticks per row), and xx = 00 is ignored; xx = 20–FF sets the tempo (32–255 BPM).'),
('M1','7',"A note's playback rate is the Paula clock divided by its period (PAL: 3546895 Hz); 50 Hz is the tick rate, not the pitch clock."),
('M1','7','Set volume is effect `Cxx`, with volume 0–64.'),
('M1','7','MOD samples are signed 8-bit, so they have no byte order; `modplay` widens them to `i16` at load.'),
('M2','4.6','- **T:** Single-threaded; audio requests are serviced from the tick loop, audio never feeds `sim/`, and it is never hashed.'),
('M3','6.1','`VK_EXT_debug_utils` is enabled only in debug builds and only when `vkEnumerateInstanceExtensionProperties` lists it.'),
('M5','3',"This list is the frame loop; the per-tick order is §5.1's, starting with Step 0."),
('N1','4.12','A `current/` that fails to load is discarded with a log line; progress loads from the newest valid slot.'),
('N2','5.1','`qdiv` saturates and records `SIM_OVF` when the quotient leaves the `int32_t` range (e.g. `INT32_MIN / -1.0`).'),
('N3','5.6','The `SEED` hex value is zero-extended to 64 bits and used as the splitmix64 state.'),
('N5','9','No source id appears twice in one citation cluster.'),
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
('B6',r'\(revision [1-4]\)','b'),('B6',r'Revision [1-4] (adds|closes|—)','b'),
('A1',r'OpenDevice\("ahi\.device", AHI_NO_UNIT','b'),
('A2',r'fire (bit )?4\b','ba'),('A2',r'bits 5[–-]7','ba'),
('M1',r'17\.\.64','b'),('M1',r'50 Hz clock','b'),('M1',r'`\$`/`/`','bs'),('M1',r'byte-swizzl','b'),('M1',r'big-endian signed 8-bit','b'),('M1',r'tempo 28','b'),
('M2',r'tick-independent','b'),('M2',r'own AHI timing','b'),
('M4',r'§6\.5 table','b'),
('N1',r'falls back to the other slot','b'),
('N2',r'qdiv cannot overflow','b'),
('N3',r'first 8 bytes of the seed','b'),
]
for i,n,lit in REQ:
    s=sec(n)
    if s is None: F.append(f"{i}: section §{n} not found")
    elif lit not in s: F.append(f"{i}: §{n} lacks literal: {lit[:90]}")
for i,rx,sc in BAN:
    for name,txt in (('doc','b' in sc and body),('sources','s' in sc and src),('Appendix R','a' in sc and t[len(body):])):
        if txt:
            n=len(re.findall(rx,txt))
            if n: F.append(f"{i}: {n} x /{rx}/ in {name}")
for l in body.splitlines():
    if 'VK_KHR_surface' in l and not re.search(r'only when|Linux|SDL_Vulkan_GetInstanceExtensions|no WSI|never',l):
        F.append(f"R1: line requests VK_KHR_surface: {l.strip()[:100]}")
for l in re.split(r'(?<=[.;])\s+|\n',body):   # sentence level; a leading citation belongs to the previous sentence
    l=re.sub(r'^[\s-]*(\[[^\]]*\]\s*)*','',l)
    if 'AHI_NO_UNIT' in l and not l.startswith('`AHI_NO_UNIT` is never used'): F.append(f"A1: sentence uses AHI_NO_UNIT: {l.strip()[:100]}")
    if 'VK_EXT_debug_utils' in l and not l.startswith('`VK_EXT_debug_utils` is enabled only in debug builds and only when'): F.append(f"M3: VK_EXT_debug_utils sentence is not the fixed one: {l.strip()[:100]}")
e45=re.search(r'^- \*\*E:\*\*[^\n]*',sec('4.5') or '',re.M)
if not e45 or '§6.2' not in e45.group(0): F.append("M4: §4.5 E: must point at the §6.2 VkResult table")
row=[l for l in sec('6.2').splitlines() if l.startswith('| `VK_ERROR_SURFACE_LOST_KHR`')] if sec('6.2') else []
if len(row)!=1: F.append("R12: need exactly one VK_ERROR_SURFACE_LOST_KHR row in §6.2")
elif not (re.search(r'recreate',row[0]) and 'loader' in row[0] and 'once' in row[0]) or re.search(r'fallback|lavapipe|device-lost',row[0],re.I):
    F.append(f"R12: SURFACE_LOST row must recreate through the loader once, with no fallback: {row[0][:100]}")
if t.splitlines()[0]!='# Kitty: Core Awakening — Engine Design Document (revision 5)': F.append("B6: line 1 is not the revision-5 title")
if '## Appendix R — Revision 5 changelog' not in t: F.append("B6: Appendix R heading is not '## Appendix R — Revision 5 changelog'")
IDS=['R1','R2','R3','R9','R10','R11','R12','R13','R16','R17','R18','R21','B1','B2','B3','B4','B5','B6',
     'A1','A2','A3','A4','A5','M1','M2','M3','M4','M5','N1','N2','N3','N4','N5']
a=t[t.index('## Appendix R'):] if '## Appendix R' in t else ''
rows={m.group(1):m.group(2) for m in re.finditer(r'^\| ([RBAMN]\d+) \| [^|\n]+ \| §([\d.]+) \|\s*$',a,re.M)}
miss=[i for i in IDS if i not in rows]
if miss: F.append(f"Appendix R missing rows: {miss}")
want={'A4':'8.1','M4':'4.5','N4':'4','R9':'4'}
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

**Script 7: sources — definitions, gaps, clusters, subjects, VK lines, Rename handlers, AHI lines, ranges, no lost entries**

````bash
python3 -I - "$DOC" "$SRC" "$VK" "$R" <<'PY'
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
TERMS={'SDL_Vulkan_GetInstanceExtensions':1,'ERROR_OBJECT_EXISTS':1,'GVF_LOCAL_ONLY':1,'diff=153600':1,
       'AHI_DEFAULT_UNIT':1,'3546895':1,'Cxx':1}
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
for n,e in E.items():
    if 'AHI_DEFAULT_UNIT' in e:
        L=[line(p,l) or '' for p,l in VKC(e)]
        if not any('AHI_DEFAULT_UNIT' in x for x in L): F.append(f"S{n} mentions AHI_DEFAULT_UNIT but cites no VK line containing it")
        if not any('io_Unit->unit_MsgPort' in x for x in L): F.append(f"S{n} mentions AHI_DEFAULT_UNIT but cites no VK line where CMD_WRITE uses io_Unit->unit_MsgPort")
H=[(m.start(),len(m.group(1)),m.group(2).rstrip('.')) for m in re.finditer(r'^(#{2,3}) (\S+)',doc,re.M)]
def sec(x):
    for k,(p,l,num) in enumerate(H):
        if num==x: return doc[p:next((q for q,l2,_ in H[k+1:] if l2<=l),len(doc))]
    return ''
RG=r'\b(0x[0-9A-Fa-f]+|\d+)\s*(?:–|\.\.)\s*(0x[0-9A-Fa-f]+|\d+)\b'
for name in ('4.6','7'):
    for l in sec(name).splitlines():
        for a,b in re.findall(RG,l):
            c=[E[x] for x in ids(l) if x in E]
            if not any(re.search(rf'(?<![0-9A-Za-z]){re.escape(a)}(?![0-9A-Za-z])',e,re.I) and re.search(rf'(?<![0-9A-Za-z]){re.escape(b)}(?![0-9A-Za-z])',e,re.I) for e in c):
                F.append(f"§{name}: range {a}–{b} is not stated by any source cited on its line: {l.strip()[:90]}")
st=os.path.join(sys.argv[4],'src_start.md')
if os.path.isfile(st):
    E0={m.group(1):m.group(2) for m in re.finditer(r'^- \[S(\d+)\](.*)$',open(st).read(),re.M)}
    gone=sorted(set(E0)-set(E),key=int)
    if gone: F.append(f"source entries removed: {gone}")
    ch=sorted((x for x in E0 if x in E and E0[x]!=E[x]),key=int)
    print(f"source entries changed since script 0: {ch or 'none'}; added: {sorted(set(E)-set(E0),key=int) or 'none'}")
else: F.append("no src_start.md: script 0 was not run in this $R")
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
the final run only, with its full output. If a FAIL can't be fixed inside the authorization block
(or because a source contradicts a fixed sentence), report it as FAIL with the reason. Never weaken
a script, and never word text just to slip past a pattern. The scripts are the floor: a body
sentence that contradicts a fixed decision fails review even if every script passes.

## Final report (reply with this only, no narration)

1. Files changed, with line counts before → after (from scripts 0 and 9).
2. Skills loaded, and any that couldn't be loaded.
3. **The verbatim output of scripts 1–9 from the final run.**
4. Every place where you departed from this prompt, with quoted evidence. Write "none" only if
   every fixed sentence is verbatim and no structural rule was bent.
5. Every source entry script 7 lists as changed or added: the old text, the new text, and the
   source line or quote that justifies it.
6. The example room: its grid, which gate the 0-verb walk is blocked by, and whether its gate types
   fit Ch1 per v2.
7. New or changed §15 owner questions.
8. UNVERIFIED items, each with its M0 probe.
9. Anything needed outside the authorization block.
