/* plat.cpp — AROS platform layer (CopperForge engine).
 * AROS-native only: exec.library / dos.library / timer.device / ahi.device
 * through the SDK proto inlines; no libc stdio, no printf family, no malloc.
 * D23 clock + pacing, D24 loader + GIPA, D25 WSI, D26 files/env/memory/log,
 * D27 LP_NUM_THREADS, D28 AHI audio, D29 joystick probe, D30 presented-pixel
 * readback, D19 exit watchdog. */
#include <exec/execbase.h>
#include <exec/semaphores.h>
#include <exec/tasks.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <devices/timer.h>
#include <dos/dos.h>
#include <proto/intuition.h>
#include <intuition/intuition.h>
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#include <graphics/rastport.h>
#include <devices/ahi.h>
#include <defines/ahi.h>
#include <proto/lowlevel.h>
#include <string.h>
#include "platform.h"
#include "modules.h"
#include "wsi.h"
#include "plat_priv.h"
#include "plat_svc.h"
#include "rt_fmt.h"

/* D23: timer.device once — a full struct timerequest from CreateIORequest on
 * a CreateMsgPort port; TimerBase set from io_Device. */
/* Window title (D25 uses this; kept here so the source gate can check it). */
[[maybe_unused]] constexpr const char aros_window_title[] = "CopperForge";

static struct MsgPort* g_timer_port = 0;
static struct timerequest* g_timreq = 0;
struct Device* TimerBase __attribute__((weak)) = 0;

/* Basis for the SDK proto inlines (D25/D29/D30). IntuitionBase is assigned the
 * GLOBAL, never a local: a local leaves the global NULL and faults in LockIBase. */
struct IntuitionBase* IntuitionBase = 0;
struct Library* CyberGfxBase = 0;
struct Library* LowLevelBase = 0;

/* The three D25 wrappers live in aros_wsi.c (the one C11 TU that includes the
 * loader header); C++ sees them through this declaration. */
extern "C" VkResult cf_aros_create_surface(void* base, VkInstance inst, unsigned w, unsigned h,
                                           const char* title, const VkAllocationCallbacks* alloc,
                                           VkSurfaceKHR* out);
extern "C" int cf_aros_poll(void* base, VkSurfaceKHR surf);
extern "C" VkResult cf_aros_destroy_surface(void* base, VkInstance inst, VkSurfaceKHR surf);
extern "C" int cf_aros_window_borders(const char* title, u32* bl, u32* bt, u32* br, u32* bb);

Alloc plat_alloc_fn(Plat& p) { return p.alloc; }
void plat_free(void* p) { FreeVec(p); }
u64 plat_micros(Plat& p) { return pl_now_us(p); }
u32 plat_lp_threads(Plat& p) { return (u32)p.lp_threads; }

/* ---- log (D26): Output() via Write, one SignalSemaphore, RawPutChar mirror
 * in the debug build ---- */
static struct SignalSemaphore g_log_sem;
static bool g_log_sem_ready = false;

static void alog_line(const char* line) {
    if (!line) return;
    if (!g_log_sem_ready) { InitSemaphore(&g_log_sem); g_log_sem_ready = true; }
    ObtainSemaphore(&g_log_sem);
    BPTR fh = Output();
    if (fh) {
        u32 n = 0u;
        while (line[n]) n++;
        Write(fh, line, (LONG)n);
        Write(fh, "\n", 1);
    }
#ifdef KITTY_DEBUG
    for (const char* c = line; *c; ++c) RawPutChar((UBYTE)*c);
    RawPutChar((UBYTE)'\n');
#endif
    ReleaseSemaphore(&g_log_sem);
}

/* D23 helper: TR_ADDREQUEST sleep on the timer.device request. Sleeping is DoIO
 * on that request — there is no wait-on-timeout, no call-delay, no spin-loop. */
static void timer_sleep_us(u32 us) {
    if (us == 0u || !g_timreq) return;
    g_timreq->tr_node.io_Command = TR_ADDREQUEST;
    g_timreq->tr_time.tv_secs = (u32)(us / 1000000u);
    g_timreq->tr_time.tv_micro = (u32)(us % 1000000u);
    DoIO((struct IORequest*)g_timreq);
}

static u64 timer_now_us(void) {
    struct timeval tv;
    tv.tv_secs = 0u;
    tv.tv_micro = 0u;
    GetUpTime(&tv);
    return (u64)tv.tv_secs * 1000000ull + (u64)tv.tv_micro;
}

/* D37: release everything acquired so far, in reverse order of acquisition,
 * before an exit-4 path leaves the process. Declared here, defined after the
 * subsystems it releases. */
static void plat_release_all(void);

/* guru line + exit 4 (D20 rule; the AROS half exits via dos Exit). D37: the
 * teardown runs first, so the Shell never reports an unfreed signal and no
 * child task outlives the segments its code lives in. */
[[noreturn]] static void alog_trap(const char* file, u32 line, const char* msg) {
    char buf[512];
    rt_fmt(buf, sizeof buf, "Software Failure. %s:%u: %s", file, line, msg);
    alog_line(buf);
    plat_release_all();
    Exit(4);
    __builtin_unreachable();
}

[[noreturn]] void pl_trap(const char* file, u32 line, const char* msg) { alog_trap(file, line, msg); }
void plat_die(const char* file, u32 line, const char* msg) { alog_trap(file, line, msg); }

void pl_log(Plat& p, const char* line) { (void)p; alog_line(line); }
void plat_log_raw(const char* line) { alog_line(line); }

/* ---- files (D26): DOS Open/Read/Seek/Close, MODE_OLDFILE / MODE_NEWFILE ---- */
static bool dos_read(const char* path, u8** data, size_t* n_out, Alloc a) {
    if (!path || !a || !data || !n_out) return false;
    BPTR fh = Open((CONST_STRPTR)path, (LONG)MODE_OLDFILE);
    if (!fh) return false;
    /* Seek returns the position BEFORE the seek, -1 on error: the size is what
     * the seek back to the start returns. */
    if (Seek(fh, 0L, OFFSET_END) < 0) { Close(fh); return false; }
    LONG sz = Seek(fh, 0L, OFFSET_BEGINNING);
    if (sz < 0) { Close(fh); return false; }
    u8* d = (u8*)a((size_t)sz ? (size_t)sz : 1u);
    if (!d) { Close(fh); return false; }
    LONG got = 0;
    if (sz > 0) got = Read(fh, d, (LONG)sz);
    Close(fh);
    if (sz > 0 && got != sz) { FreeVec(d); return false; }
    *data = d;
    *n_out = (size_t)sz;
    return true;
}

bool plat_read_path(const char* path, u8** data, size_t* n, Alloc a) {
    return dos_read(path, data, n, a);
}

bool pl_read_file(Plat& p, const char* path, Buf* out) {
    if (!out) return false;
    u8* d = 0;
    size_t n = 0;
    if (!dos_read(path, &d, &n, p.alloc)) return false;
    out->data = d;
    out->n = n;
    out->cap = n;
    return true;
}

bool pl_write_file(Plat& p, const char* path, const void* data, size_t n) {
    (void)p;
    if (!path) return false;
    BPTR fh = Open((CONST_STRPTR)path, (LONG)MODE_NEWFILE);
    if (!fh) return false;
    bool ok = true;
    if (n > 0) ok = (Write(fh, (CONST_APTR)data, (LONG)n) == (LONG)n);   /* byte count, -1 on error */
    Close(fh);
    return ok;
}

/* ---- env (D26): GetVar(key, v, n, 0), empty string on failure ---- */
void pl_env_get(Plat& p, const char* key, char* v, size_t n) {
    (void)p;
    if (!n) return;
    v[0] = 0;
    if (!key) return;
    GetVar((CONST_STRPTR)key, (STRPTR)v, (i32)n, 0);
}

u64 pl_now_us(Plat& p) { (void)p; return timer_now_us(); }

/* ---- D24 GIPA: the loader handle minus 40 is the loader's proc-address
 * entry (dlfcn.h / vulkan_loader.library) ---- */
PFN_vkGetInstanceProcAddr pl_vk_gipa(Plat& p) {
    if (!p.vk_lib) return 0;
    return (PFN_vkGetInstanceProcAddr)*(void**)((u8*)p.vk_lib - 40);
}

/* ---- D23: pl_step on the shared pace (main.cpp uses pace_step(due)) ---- */
void pl_step(Plat& p, u64& due_us) {
    const u64 now = pl_now_us(p);
    if (now < p.next_us) { due_us = 0; return; }
    const u64 n = (now - p.next_us) / 20000ull + 1ull;
    p.next_us += n * 20000ull;
    due_us = n;
}

void plat_sleep_us(u32 us) { timer_sleep_us(us); }

/* ---- D19 AROS half: a child process from CreateNewProcTags wakes on its own
 * TR_ADDREQUEST and, if the validation counter did not move, sets the shared
 * flag and Signals the main task, which then exits 4 (3 if the counter moved). */
struct WdCtx {
    struct Task* main_task;
    const volatile u32* counter;
    u32 seen;
    u32 ms;
    volatile u32 fired;
};

static struct Process* g_wd_proc = 0;
static struct WdCtx g_wd;
static bool g_wd_stop = false;

static void wd_entry(void) {
    struct MsgPort* port = CreateMsgPort();
    struct timerequest* req = 0;
    if (port) req = (struct timerequest*)CreateIORequest(port, sizeof(struct timerequest));
    if (req && OpenDevice((CONST_STRPTR)"timer.device", UNIT_MICROHZ, (struct IORequest*)req, 0) == 0) {
        while (!g_wd_stop) {
            req->tr_node.io_Command = TR_ADDREQUEST;
            req->tr_time.tv_secs = g_wd.ms / 1000u;
            req->tr_time.tv_micro = (u32)((g_wd.ms % 1000u) * 1000u);
            DoIO((struct IORequest*)req);
            if (g_wd_stop) break;
            const u32 v = g_wd.counter
                              ? (u32)__atomic_load_n(g_wd.counter, __ATOMIC_RELAXED)
                              : 0u;
            if (v == g_wd.seen) {
                g_wd.fired = 1u;
                Signal(g_wd.main_task, SIGBREAKF_CTRL_C);
                break;
            }
            g_wd.seen = v;
        }
        CloseDevice((struct IORequest*)req);
    }
    if (req) DeleteIORequest(req);
    if (port) DeleteMsgPort(port);
}

void plat_exit_watchdog_ms(Plat& p, u32 ms, const volatile u32* validation_count) {
    (void)p;
    if (g_wd_proc) return;
    g_wd.main_task = FindTask(0);
    g_wd.counter = validation_count;
    g_wd.seen = validation_count ? (u32)__atomic_load_n(validation_count, __ATOMIC_RELAXED) : 0u;
    g_wd.ms = ms;
    g_wd.fired = 0u;
    g_wd_stop = false;
    g_wd_proc = CreateNewProcTags(
        (ULONG)NP_Entry, (IPTR)wd_entry,
        (ULONG)NP_Name, (IPTR)"cf-watchdog",
        (ULONG)NP_StackSize, 32768,
        TAG_DONE);
    if (!g_wd_proc) plat_log_raw("watchdog=no-process");
}

void plat_exit_watchdog_cancel(void) {
    g_wd_stop = true;   /* it sleeps in DoIO and may already be gone: never Signal it */
}

/* ---- D25 WSI ---- */
u32 pl_wsi_instance_extensions(Plat& p, const char** names, u32 max) {
    (void)p;
    if (!names || max == 0u) return 0u;
    names[0] = "VK_KHR_surface";
    return 1u;
}

/* The helper's window is GIMMEZEROZERO, its size is the outer size, and Mesa's
 * wsi_aros_blit writes at (BorderLeft, BorderTop) of the inner RastPort. So 320x256
 * of picture needs an inner size of 320+BorderLeft x 256+BorderTop. */
bool pl_wsi_create_surface(Plat& p, VkInstance inst, VkSurfaceKHR* out) {
    if (!out) return false;
    u32 bl = 0u, bt = 0u, br = 0u, bb = 0u;
    if (!cf_aros_window_borders(aros_window_title, &bl, &bt, &br, &bb))
        alog_trap(__FILE__, __LINE__, "window border probe failed");
    const VkResult r = cf_aros_create_surface(p.vk_lib, inst, 320u + bl + bl + br, 256u + bt + bt + bb,
                                              aros_window_title, 0, out);
    if (r != VK_SUCCESS) {
        char line[128];
        rt_fmt(line, sizeof line, "surface_create failed result=%d", (int)r);
        pl_log(p, line);
        alog_trap(__FILE__, __LINE__, "vulkan surface create failed");
    }
    p.surf = (void*)(size_t)(*out);
    p.win_w = 320u;
    p.win_h = 256u;
    pl_log(p, "surface=ok window_w=320 window_h=256");
    return true;
}

void pl_wsi_extent(Plat& p, u32* w, u32* h) {
    p.win_w = 320u;
    p.win_h = 256u;
    if (w) *w = p.win_w;
    if (h) *h = p.win_h;
}

/* Window walk (the surface_probe pattern): the GLOBAL IntuitionBase must be
 * assigned, then LockIBase, then FirstScreen/FirstWindow matched on Title.
 * Implemented in aros_wsi.c (C) to avoid C++ register allocation using %r12. */
extern "C" struct Window* find_titled_window(const char* title);
extern "C" void close_titled_windows(const char* title);

void pl_wsi_destroy_surface(Plat& p, VkInstance inst, VkSurfaceKHR s) {
    if (!s) return;
    const VkResult r = cf_aros_destroy_surface(p.vk_lib, inst, s);
    const char* how = "lvo80";
    if (r != VK_SUCCESS) {
        how = "fallback";
        PFN_vkGetInstanceProcAddr g = pl_vk_gipa(p);
        if (g) {
            PFN_vkDestroySurfaceKHR d = (PFN_vkDestroySurfaceKHR)g(inst, "vkDestroySurfaceKHR");
            if (d) d(inst, s, 0);
        }
        close_titled_windows(aros_window_title);
    }
    char line[64];
    rt_fmt(line, sizeof line, "surface_destroy=%s", how);
    pl_log(p, line);
    p.surf = 0;
}

u32 pl_poll_events(Plat& p, EventSink* sink) {
    p.events_drained = p.events_drained + 1ull;
    if (!p.surf) return 0u;   /* headless: no window, and the helper answers 1 for no surface */
    const int keep = cf_aros_poll(p.vk_lib, (VkSurfaceKHR)(size_t)p.surf);   /* non-zero: close */
    u32 n = 0u;
    if (keep != 0 && !p.quit) {
        p.quit = true;
        if (sink) {
            const u32 next = (sink->head + 1u) % 64u;
            if (next != sink->tail) {
                sink->ev[sink->head].kind = 1;
                sink->ev[sink->head].a = 0;
                sink->ev[sink->head].b = 0;
                sink->ev[sink->head].tick = 0;
                sink->head = next;
                n = 1u;
            }
        }
    }
    return n;
}

/* ---- D30 presented-pixel proof ---- */
/* Small noinline helpers keep each value's live range short: the AROS target
 * reserves %r12, so a long-lived call result must not be parked in a
 * callee-saved GPR across a call. */
[[gnu::noinline]] static u32 cyber_depth_of(struct RastPort* rp) {
    if (!rp || !rp->BitMap) return 0u;
    return (u32)GetCyberMapAttr(rp->BitMap, CYBRMATTR_DEPTH);
}

[[gnu::noinline]] static void present_blocked(Plat& p, u32 cw, u32 ch, u32 depth) {
    char line[128];
    rt_fmt(line, sizeof line, "present_dump blocked reason=\"client %ux%u depth %u\"",
           cw, ch, depth);
    pl_log(p, line);
}

[[gnu::noinline]] static void present_ok(Plat& p, u32 frame, u32 cw, u32 ch, u32 depth,
                                         u32 rpa) {
    char line[160];
    rt_fmt(line, sizeof line, "present_dump frame=%u client=%ux%u depth=%u rpa=%u",
           frame, cw, ch, depth, rpa);
    pl_log(p, line);
}

/* The AROS x86-64 ABI reserves %r12; this function must not keep a
 * long-lived Window* across calls. Split into noinline helpers so the
 * compiler doesn't park the Window* in %r12 across calls. */
[[gnu::noinline]] static struct Window* present_find_window(const char* title) {
    return find_titled_window(title);
}

/* The whole inner area, so one boot shows where the picture landed. */
[[gnu::noinline]] static void present_dump_inner(Plat& p, struct Window* w, u32 frame,
                                                 const char* out_dir, u32 iw, u32 ih) {
    u8* buf = (u8*)p.alloc(iw * ih * 4u);
    if (!buf) return;
    ReadPixelArray((APTR)buf, 0, 0, (UWORD)(iw * 4u), w->RPort, 0, 0, (UWORD)iw, (UWORD)ih, RECTFMT_BGRA32);
    char path[512];
    rt_fmt(path, sizeof path, "%s/present_%u.inner.bgra", out_dir ? out_dir : ".", frame);
    pl_write_file(p, path, buf, iw * ih * 4u);
    FreeVec(buf);
    char line[96];
    rt_fmt(line, sizeof line, "present_inner=%ux%u border=%u,%u", iw, ih, (u32)w->BorderLeft, (u32)w->BorderTop);
    pl_log(p, line);
}

[[gnu::noinline]] static bool present_dump_impl(Plat& p, struct Window* w,
                                                 u32 frame, const char* out_dir) {
    /* The picture starts at (BorderLeft, BorderTop) of the inner RastPort. */
    const u32 iw = (u32)(w->Width - w->BorderLeft - w->BorderRight);
    const u32 ih = (u32)(w->Height - w->BorderTop - w->BorderBottom);
    const u32 cw = iw - w->BorderLeft;
    const u32 ch = ih - w->BorderTop;
    const u32 depth = cyber_depth_of(w->RPort);
    if (cw != 320u || ch != 256u || depth < 24u) {
        present_blocked(p, cw, ch, depth);
        return false;
    }
    u8* buf = (u8*)p.alloc(320u * 256u * 4u);
    if (!buf) {
        pl_log(p, "present_dump blocked reason=\"alloc\"");
        return false;
    }
    const ULONG rpa = ReadPixelArray((APTR)buf, 0, 0, 1280, w->RPort,
                                     w->BorderLeft, w->BorderTop, 320, 256, RECTFMT_BGRA32);
    present_ok(p, frame, cw, ch, depth, (u32)rpa);
    char path[512];
    rt_fmt(path, sizeof path, "%s/present_%u.bgra", out_dir ? out_dir : ".", frame);
    const bool ok = pl_write_file(p, path, buf, 320u * 256u * 4u);
    FreeVec(buf);
    present_dump_inner(p, w, frame, out_dir, iw, ih);
    return ok;
}

bool pl_present_dump(Plat& p, u32 frame, const char* out_dir) {
    if (!CyberGfxBase) {
        pl_log(p, "present_dump blocked reason=\"no cybergraphics\"");
        return false;
    }
    struct Window* w = present_find_window(aros_window_title);
    if (!w) {
        pl_log(p, "present_dump blocked reason=\"no window\"");
        return false;
    }
    return present_dump_impl(p, w, frame, out_dir);
}

/* ---- D28 AHI: four AHIRequests, ring by ahir_Link; D91: 1920 stereo S16 frames each ---- */
static const u32 kAhiFrames = 1920u;
static const u32 kAhiBytes = 1920u * 2u * 2u;   /* 7680 */

static struct MsgPort* g_ahi_port = 0;
static struct AHIRequest* g_ahi_req[4] = { 0, 0, 0, 0 };
static u8* g_ahi_buf[4] = { 0, 0, 0, 0 };
static bool g_ahi_open = false;
static struct Library* g_ahi_hidd = 0;
static u64 g_ahi_submitted = 0;
static u64 g_ahi_dropped = 0;
static int g_ahi_last = -1;   /* the request submitted last */
/* D74: what AHI handed back, so a stalled queue says why */
static bool g_ahi_sent[4] = { false, false, false, false };
static u64 g_ahi_returned = 0;
static u64 g_ahi_errors = 0;
static int g_ahi_first_error = 0;
static u64 g_ahi_t0 = 0;      /* D90: pl_now_us at open; xrun and error lines give us= from it */
static int g_ahi_stage = -1;  /* D91: the request being filled, not yet sent */
static u32 g_ahi_staged = 0;  /* D91: frames already in it */
static u64 g_ahi_first_us = 0; /* D93: first request sent, us since start (elapsed_us base) */

static u32 ahi_inflight(void) {
    u32 n = 0u;
    for (int i = 0; i < 4; i++) {
        if (g_ahi_req[i] && CheckIO((struct IORequest*)g_ahi_req[i]) == 0) n++;
    }
    return n;
}

/* D77: AHI fills a mode string; keep it printable and quote-free for the log */
static void ahi_clean(char* s) {
    for (; *s; s++) if (*s < ' ' || *s == '"') *s = '?';
}

/* D77: the modes AHI's database holds, and the one AHI picks for a unit with no prefs */
[[gnu::noinline]] static void ahi_log_modes(Plat& p) {
    struct Library* AHIBase = (struct Library*)g_ahi_req[0]->ahir_Std.io_Device;
    char line[192];
    char name[64];
    char drv[64];
    u32 n = 0;
    for (ULONG id = AHI_NextAudioID(AHI_INVALID_ID); id != AHI_INVALID_ID && n < 32u;
         id = AHI_NextAudioID(id)) {
        name[0] = 0;
        drv[0] = 0;
        AHI_GetAudioAttrs(id, (struct AHIAudioCtrl*)0, AHIDB_BufferLen, (IPTR)sizeof name,
                          AHIDB_Name, (IPTR)name, AHIDB_Driver, (IPTR)drv, TAG_DONE);
        ahi_clean(name);
        ahi_clean(drv);
        rt_fmt(line, sizeof line, "ahi_mode id=0x%x name=\"%s\" driver=\"%s\"",
               (unsigned)id, name, drv);
        pl_log(p, line);
        n++;
    }
    rt_fmt(line, sizeof line, "ahi_modes=%u", n);
    pl_log(p, line);
    ULONG best = AHI_BestAudioID(AHIDB_Realtime, TRUE, TAG_DONE);
    name[0] = 0;
    if (best != AHI_INVALID_ID)
        AHI_GetAudioAttrs(best, (struct AHIAudioCtrl*)0, AHIDB_BufferLen, (IPTR)sizeof name,
                          AHIDB_Name, (IPTR)name, TAG_DONE);
    ahi_clean(name);
    rt_fmt(line, sizeof line, "ahi_best id=0x%x name=\"%s\"", (unsigned)best, name);
    pl_log(p, line);
}

/* D80: before AHI opens: is its mode database built, does the HDA driver see its
   config, and does the driver open (AHI drops a mode whose driver does not) */
[[gnu::noinline]] static int ahi_probe_db(void) {
    Forbid();
    int db = FindSemaphore((STRPTR)"Audio Mode Database") != 0;
    Permit();
    return db;
}

[[gnu::noinline]] static int ahi_probe_cfg(void) {
    BPTR l = Lock((CONST_STRPTR)"ENVARC:hdaudio.config", ACCESS_READ);
    if (!l) return -(int)IoErr();
    UnLock(l);
    return 1;
}

[[gnu::noinline]] static void ahi_probe_close(struct Library* d) {
    CloseLibrary(d);
}

[[gnu::noinline]] static int ahi_probe_drv(void) {
    struct Library* d = OpenLibrary((CONST_STRPTR)"DEVS:AHI/hdaudio.audio", 2);
    if (!d) return 0;
    ahi_probe_close(d);
    return 1;
}

/* D83: the HDA driver opens DRIVERS:hdaudio.hidd; where DRIVERS: points, whether
   the HIDD is reachable through it, and whether it is resident before and after */
[[gnu::noinline]] static int ahi_probe_hidd(void) {
    BPTR l = Lock((CONST_STRPTR)"DRIVERS:hdaudio.hidd", ACCESS_READ);
    if (!l) return -(int)IoErr();
    UnLock(l);
    return 1;
}

[[gnu::noinline]] static int ahi_probe_dir(char* buf, int len) {
    buf[0] = 0;
    BPTR l = Lock((CONST_STRPTR)"DRIVERS:", ACCESS_READ);
    if (!l) return -(int)IoErr();
    int r = NameFromLock(l, (STRPTR)buf, len) ? 1 : -(int)IoErr();
    UnLock(l);
    if (r < 0) buf[0] = 0;
    return r;
}

[[gnu::noinline]] static int ahi_probe_res(void) {
    Forbid();
    int r = FindName(&((struct ExecBase*)SysBase)->LibList, (CONST_STRPTR)"hdaudio.hidd") != 0;
    Permit();
    return r;
}

[[gnu::noinline]] static void ahi_probe(Plat& p) {
    char line[200];
    char dir[96];
    int db = ahi_probe_db();
    int cfg = ahi_probe_cfg();
    int hl = ahi_probe_hidd();
    int dl = ahi_probe_dir(dir, (int)sizeof dir);
    int rb = ahi_probe_res();
    int drv = ahi_probe_drv();
    int ra = ahi_probe_res();
    rt_fmt(line, sizeof line, "ahi_probe db_before=%d envarc_config=%d envarc_err=%d hdaudio_open=%d",
           db, cfg > 0 ? 1 : 0, cfg > 0 ? 0 : -cfg, drv);
    pl_log(p, line);
    rt_fmt(line, sizeof line, "ahi_drivers hidd_lock=%d hidd_err=%d dir_err=%d dir=\"%s\" hidd_before=%d hidd_after=%d",
           hl > 0 ? 1 : 0, hl > 0 ? 0 : -hl, dl > 0 ? 0 : -dl, dir, rb, ra);
    pl_log(p, line);
}

/* D86: AROS boot (rom/dos/cliinit.c) hands one DEVS:Drivers lock to both DRIVERS:
   and LIBS:, so replacing LIBS: leaves DRIVERS: on a freed lock, and hdaudio.audio's
   OpenLibrary("DRIVERS:hdaudio.hidd") fails. LDDemon finds a resident library by its
   bare name before it loads anything, so open the HIDD by a SYS: path first and hold
   it while audio is open. Failure is ignored. Remove once cliinit.c is fixed. */
[[gnu::noinline]] static int ahi_hidd_preload(void) {
    if (!g_ahi_hidd) g_ahi_hidd = OpenLibrary((CONST_STRPTR)"SYS:Devs/Drivers/hdaudio.hidd", 0);
    return g_ahi_hidd != 0;
}

[[gnu::noinline]] static void ahi_hidd_release(void) {
    if (g_ahi_hidd) { CloseLibrary(g_ahi_hidd); g_ahi_hidd = 0; }
}

bool pl_audio_open(Plat& p, u32 rate, u16 channels) {
    (void)rate; (void)channels;
    if (g_ahi_open) return true;
    char line[160];
    int pre = ahi_hidd_preload();
    rt_fmt(line, sizeof line, "ahi_hidd_preload=%d", pre);
    pl_log(p, line);
    ahi_probe(p);
    g_ahi_port = CreateMsgPort();
    if (!g_ahi_port) {
        pl_log(p, "audio_device=none reason=\"CreateMsgPort\"");
        return false;
    }
    for (int i = 0; i < 4; i++) {
        struct AHIRequest* rq =
            (struct AHIRequest*)CreateIORequest(g_ahi_port, sizeof(struct AHIRequest));
        if (!rq) { pl_log(p, "audio_device=none reason=\"CreateIORequest\""); return false; }
        g_ahi_req[i] = rq;
        u8* b = (u8*)p.alloc(kAhiBytes);
        if (!b) { pl_log(p, "audio_device=none reason=\"AllocVec\""); return false; }
        g_ahi_buf[i] = b;
        rq->ahir_Version = 4;
        rq->ahir_Type = AHIST_S16S;
        rq->ahir_Frequency = 48000;
        rq->ahir_Volume = 0x10000;
        rq->ahir_Position = 0x8000;
        rq->ahir_Std.io_Command = CMD_WRITE;
        rq->ahir_Std.io_Data = b;
        rq->ahir_Std.io_Length = (LONG)kAhiBytes;
    }
    /* D71: an AHI set beside the program wins; lddemon may not honour a DEVS: shadow */
    const char* ahi_src = "PROGDIR:Devs/ahi.device";
    BYTE ahi_err = OpenDevice((CONST_STRPTR)ahi_src, AHI_DEFAULT_UNIT,
                              (struct IORequest*)g_ahi_req[0], 0);
    if (ahi_err != 0) {
        /* the fallback overwrites io_Error, so say why PROGDIR failed first */
        rt_fmt(line, sizeof line, "audio_progdir_error=%d",
               (int)g_ahi_req[0]->ahir_Std.io_Error);
        pl_log(p, line);
        ahi_src = "ahi.device";
        ahi_err = OpenDevice((CONST_STRPTR)ahi_src, AHI_DEFAULT_UNIT,
                             (struct IORequest*)g_ahi_req[0], 0);
    }
    if (ahi_err != 0) {
        rt_fmt(line, sizeof line, "audio_device=none reason=\"%d\"",
               (int)g_ahi_req[0]->ahir_Std.io_Error);
        pl_log(p, line);
        return false;
    }
    for (int i = 1; i < 4; i++) {
        g_ahi_req[i]->ahir_Std.io_Device = g_ahi_req[0]->ahir_Std.io_Device;
        g_ahi_req[i]->ahir_Std.io_Unit = g_ahi_req[0]->ahir_Std.io_Unit;
    }
    g_ahi_open = true;
    g_ahi_t0 = pl_now_us(p);
    rt_fmt(line, sizeof line, "audio_source=%s", ahi_src);
    pl_log(p, line);
    /* audio_source is the name asked for; the resident device's id says which bytes answered */
    const struct Library* ahi_lib = &g_ahi_req[0]->ahir_Std.io_Device->dd_Library;
    char ahi_id[80] = "none";
    if (ahi_lib->lib_IdString) {
        u32 n = 0;
        for (const char* s = (const char*)ahi_lib->lib_IdString;
             *s >= ' ' && *s != '"' && n + 1 < sizeof ahi_id; s++) ahi_id[n++] = *s;
        ahi_id[n] = 0;
    }
    rt_fmt(line, sizeof line, "audio_device_id=\"%s\" version=%u.%u", ahi_id,
           (unsigned)ahi_lib->lib_Version, (unsigned)ahi_lib->lib_Revision);
    pl_log(p, line);
    pl_log(p, "audio_device=ahi.device unit=0");
    ahi_log_modes(p);
    return true;
}

/* D89: ahir_Link double-buffers only: AHI attaches a request to its link only
   while that link is playing, so a third in flight is never started and stalls
   every later one. Keep two in flight; at two, report the mixer's full mark
   (audio.cpp renders only below 3840 frames) so it waits instead of dropping.
   D91: each request holds 1920 frames (40 ms), longer than one tick's play,
   so a refill at every tick keeps the queue ahead of the device. */
static const u32 kAhiDepth = 2u;

u32 pl_audio_queued_frames(Plat& p) {
    (void)p;
    u32 n = ahi_inflight();
    return n >= kAhiDepth ? kAhiFrames * kAhiDepth : kAhiFrames * n + g_ahi_staged;
}

[[gnu::noinline]] static void ahi_log_xrun(Plat& p, u64 submitted) {
    char line[96];
    rt_fmt(line, sizeof line, "ahi_xrun pass=%llu us=%llu",
           (unsigned long long)submitted, (unsigned long long)(pl_now_us(p) - g_ahi_t0));
    pl_log(p, line);
}

[[gnu::noinline]] static void ahi_log_error(Plat& p, int err) {
    char line[96];
    rt_fmt(line, sizeof line, "ahi_io_error=%d pass=%llu us=%llu", err,
           (unsigned long long)g_ahi_submitted, (unsigned long long)(pl_now_us(p) - g_ahi_t0));
    pl_log(p, line);
}

/* D74: once, at shutdown, while the requests still exist */
[[gnu::noinline]] static void ahi_log_summary(Plat& p) {
    if (!g_ahi_open) return;
    char line[192];
    rt_fmt(line, sizeof line,
           "ahi_summary submitted=%llu returned=%llu errors=%llu first_error=%d dropped=%llu inflight=%u",
           (unsigned long long)g_ahi_submitted, (unsigned long long)g_ahi_returned,
           (unsigned long long)g_ahi_errors, g_ahi_first_error,
           (unsigned long long)g_ahi_dropped, ahi_inflight());
    pl_log(p, line);
    rt_fmt(line, sizeof line, "ahi_first_submit_us=%llu", (unsigned long long)g_ahi_first_us);
    pl_log(p, line);
}

[[gnu::noinline]] static void ahi_copy_frames(u8* dst, const u8* src, u32 bytes) {
    for (u32 i = 0; i < bytes; i++) dst[i] = src[i];
}

void pl_audio_queue(Plat& p, const i16* frames, u32 n) {
    if (!g_ahi_open || !frames || n == 0u) return;
    if (g_ahi_stage < 0) {
        if (ahi_inflight() >= kAhiDepth) { g_ahi_dropped += n; return; }
        int idx = -1;
        for (int i = 0; i < 4; i++) {
            if (g_ahi_req[i] && CheckIO((struct IORequest*)g_ahi_req[i]) != 0) { idx = i; break; }
        }
        if (idx < 0) { g_ahi_dropped += n; return; }
        (void)WaitIO((struct IORequest*)g_ahi_req[idx]);
        if (g_ahi_sent[idx]) {
            g_ahi_returned = g_ahi_returned + 1ull;
            int err = (int)g_ahi_req[idx]->ahir_Std.io_Error;
            if (err != 0) {
                if (g_ahi_errors == 0ull) { g_ahi_first_error = err; ahi_log_error(p, err); }
                g_ahi_errors = g_ahi_errors + 1ull;
            }
            g_ahi_sent[idx] = false;
        }
        g_ahi_stage = idx;
        g_ahi_staged = 0u;
    }
    /* D91: gather the mixer's chunks; send only a full 1920-frame request */
    const int idx = g_ahi_stage;
    u32 room = kAhiFrames - g_ahi_staged;
    if (n > room) { g_ahi_dropped += n - room; n = room; }
    ahi_copy_frames(g_ahi_buf[idx] + g_ahi_staged * 4u, (const u8*)frames, n * 4u);
    g_ahi_staged = g_ahi_staged + n;
    p.queued_frames = p.queued_frames + n;
    if (g_ahi_staged < kAhiFrames) return;
    if (g_ahi_submitted > 0ull && ahi_inflight() == 0u) ahi_log_xrun(p, g_ahi_submitted);
    struct AHIRequest* rq = g_ahi_req[idx];
    rq->ahir_Std.io_Length = (LONG)kAhiBytes;
    rq->ahir_Std.io_Command = CMD_WRITE;
    /* Each request names the one it follows, or NULL: AHI queues it behind that
     * one if it is still sounding, and clears a link to anything else. */
    struct AHIRequest* prev = g_ahi_last >= 0 ? g_ahi_req[g_ahi_last] : 0;
    rq->ahir_Link = (prev && CheckIO((struct IORequest*)prev) == 0) ? prev : 0;
    if (g_ahi_submitted == 0ull) g_ahi_first_us = pl_now_us(p) - p.t0_us;
    SendIO((struct IORequest*)rq);
    g_ahi_sent[idx] = true;
    g_ahi_last = idx;
    g_ahi_submitted = g_ahi_submitted + 1ull;
    g_ahi_stage = -1;
    g_ahi_staged = 0u;
}

static void ahi_release(void) {
    if (!g_ahi_open && !g_ahi_port) { ahi_hidd_release(); return; }
    for (int i = 0; i < 4; i++) {
        struct AHIRequest* rq = g_ahi_req[i];
        if (!rq) continue;
        if (CheckIO((struct IORequest*)rq) == 0) {
            AbortIO((struct IORequest*)rq);
            (void)WaitIO((struct IORequest*)rq);
        }
    }
    if (g_ahi_open) CloseDevice((struct IORequest*)g_ahi_req[0]);
    for (int i = 0; i < 4; i++) {
        if (g_ahi_req[i]) { DeleteIORequest((struct IORequest*)g_ahi_req[i]); g_ahi_req[i] = 0; }
        if (g_ahi_buf[i]) { FreeVec(g_ahi_buf[i]); g_ahi_buf[i] = 0; }
    }
    if (g_ahi_port) { DeleteMsgPort(g_ahi_port); g_ahi_port = 0; }
    g_ahi_open = false;
    g_ahi_last = -1;
    g_ahi_stage = -1;
    g_ahi_staged = 0u;
    ahi_hidd_release();
}

void plat_audio_close(Plat& p) {
    (void)p;
    ahi_release();
}

/* D37: the watchdog is a child process whose code and data live in THIS
 * process's segments, so it must be gone before Exit() or it would run on
 * unloaded memory. It tests g_wd_stop after its own timer reply, so the wait
 * is bounded by one watchdog interval (2 s today). */
static void wd_release(void) {
    if (!g_wd_proc) return;
    g_wd_stop = true;
    for (int i = 0; i < 500; i++) {
        if (!FindTask((CONST_STRPTR)"cf-watchdog")) break;
        timer_sleep_us(5000u);
    }
    g_wd_proc = 0;
}

/* D37: every exit-4 path releases what pl_init (and the steps after it)
 * acquired, in reverse order. The loader is deliberately NOT released: D24
 * says it is never closed. */
static void plat_release_all(void) {
    wd_release();              /* child process (D19) */
    ahi_release();             /* requests, device, buffers, port (D28) */
    if (IntuitionBase) {       /* opened lazily by the window walk (D30) */
        close_titled_windows(aros_window_title);
        CloseLibrary((struct Library*)IntuitionBase);
        IntuitionBase = 0;
    }
    if (CyberGfxBase) { CloseLibrary(CyberGfxBase); CyberGfxBase = 0; }
    if (LowLevelBase) { CloseLibrary(LowLevelBase); LowLevelBase = 0; }
    if (g_timreq) {
        CloseDevice((struct IORequest*)g_timreq);
        DeleteIORequest(g_timreq);
        g_timreq = 0;
    }
    if (g_timer_port) {        /* frees the signal bit CreateMsgPort took */
        DeleteMsgPort(g_timer_port);
        g_timer_port = 0;
    }
    TimerBase = 0;
}

const char* plat_audio_driver(Plat& p) {
    (void)p;
    return g_ahi_open ? "ahi.device unit=0" : "none";
}

u64 plat_audio_total_frames(Plat& p) { return p.queued_frames; }

/* ---- D27 worker-task probe: llvmpipe-named tasks while the device exists ---- */
u32 plat_lp_worker_tasks(Plat& p) {
    u32 n = 0u;
    Forbid();
    struct ExecBase* eb = (struct ExecBase*)SysBase;
    struct List* lists[2] = { &eb->TaskReady, &eb->TaskWait };
    for (int li = 0; li < 2; li++) {
        struct Node* node = lists[li]->lh_Head;
        while (node && node->ln_Succ) {
            const char* nm = node->ln_Name;
            if (nm) {
                bool ok = true;
                const char* pre = "llvmpipe";
                for (int k = 0; pre[k]; k++) {
                    if (nm[k] != pre[k]) { ok = false; break; }
                }
                if (ok) n++;
            }
            node = node->ln_Succ;
        }
    }
    Permit();
    char line[64];
    rt_fmt(line, sizeof line, "lp_worker_tasks=%u", n);
    pl_log(p, line);
    return n;
}

/* ---- guest-only exit keys (W4): elapsed_us= and events_drained= ---- */
void plat_exit_stats(Plat& p) {
    char line[128];
    const u64 now = pl_now_us(p);
    const u64 el = now > p.t0_us ? now - p.t0_us : 0ull;
    rt_fmt(line, sizeof line, "elapsed_us=%llu", (unsigned long long)el);
    pl_log(p, line);
    rt_fmt(line, sizeof line, "events_drained=%llu", (unsigned long long)p.events_drained);
    pl_log(p, line);
}

void plat_soak_sample(Plat& p) {
    char line[96];
    rt_fmt(line, sizeof line, "mem_free_bytes=%llu", (unsigned long long)AvailMem(MEMF_ANY));
    pl_log(p, line);
}

/* ---- D26 CPU model: the AROS guest is QEMU; name it honestly ---- */
void pl_cpu_model(Plat& p, char* out, size_t n) {
    (void)p;
    if (!n || !out) return;
    const char* v = "emulated";
    size_t i = 0;
    for (; v[i] && i + 1u < n; i++) out[i] = v[i];
    out[i] = 0;
}

/* D37: debug-only --plant-loader-fail (AROS only). main.cpp calls this before
 * pl_init; the release build and both Linux builds reject the flag with exit 2.
 * It skips all three OpenLibrary calls, so D24's real failure path (trap text
 * and exit 4) runs verbatim. */
static bool g_plant_loader_fail = false;

void plat_aros_plant_loader_fail(void) { g_plant_loader_fail = true; }

void pl_init(Plat& p, const PlatArgs& args) {
    p.alloc = 0;
    p.alloc = [](size_t n) -> void* { return AllocVec((IPTR)n, MEMF_ANY | MEMF_CLEAR); };
    p.t0_us = 0;
    p.next_us = 0;
    p.ticks = 0;
    p.dropped = 0;
    p.lp_threads = 0;
    p.lp_src = "engine-default";
    p.windowed = false;
    p.win = 0;
    p.win_w = 320;
    p.win_h = 256;
    p.adev = 0;
    p.audio = false;
    p.arate = 48000;
    p.queued_frames = 0;
    p.quit = false;
    p.surf = 0;
    p.events_drained = 0;
    p.vk_lib = 0;

    char line[256];

    /* D27: LP_NUM_THREADS first, before any Vulkan call. */
    char lpv[8];
    if (args.lp_threads < 0) {
        lpv[0] = '0';
        lpv[1] = 0;
        p.lp_threads = 0;
        p.lp_src = "engine-default";
    } else {
        rt_fmt(lpv, sizeof lpv, "%d", args.lp_threads);
        p.lp_threads = args.lp_threads;
        p.lp_src = (args.lp_src && args.lp_src[0]) ? args.lp_src : "flag";
    }
    SetVar((CONST_STRPTR)"LP_NUM_THREADS", (CONST_STRPTR)lpv, -1, (ULONG)GVF_LOCAL_ONLY);
    char gv[16];
    gv[0] = 0;
    const LONG grc = GetVar((CONST_STRPTR)"LP_NUM_THREADS", (STRPTR)gv, (LONG)sizeof gv, 0);
    rt_fmt(line, sizeof line, "lp_num_threads set=%s getvar=\"%s\" getvar_rc=%d",
           lpv, gv, (int)grc);
    pl_log(p, line);

    rt_fmt(line, sizeof line, "lp_threads=%d lp_threads_source=%s", p.lp_threads, p.lp_src);
    pl_log(p, line);

    /* D26: startup stack — FindTask(0)'s tc_SPUpper − tc_SPLower. */
    struct Task* t = FindTask(0);
    u64 stack_bytes = 0;
    if (t) stack_bytes = (u64)(IPTR)t->tc_SPUpper - (u64)(IPTR)t->tc_SPLower;
    rt_fmt(line, sizeof line, "stack_bytes=%llu", stack_bytes);
    pl_log(p, line);
    if (stack_bytes < 262144ull)
        alog_trap(__FILE__, __LINE__, "startup stack smaller than 256 KiB");

    /* D23: open timer.device UNIT_MICROHZ once. */
    g_timer_port = CreateMsgPort();
    if (!g_timer_port)
        alog_trap(__FILE__, __LINE__, "timer.device: CreateMsgPort failed");
    g_timreq = (struct timerequest*)CreateIORequest(g_timer_port, sizeof(struct timerequest));
    if (!g_timreq) {
        DeleteMsgPort(g_timer_port);
        g_timer_port = 0;
        alog_trap(__FILE__, __LINE__, "timer.device: CreateIORequest failed");
    }
    if (OpenDevice((CONST_STRPTR)"timer.device", UNIT_MICROHZ, (struct IORequest*)g_timreq, 0) != 0) {
        DeleteIORequest(g_timreq);
        g_timreq = 0;
        DeleteMsgPort(g_timer_port);
        g_timer_port = 0;
        alog_trap(__FILE__, __LINE__, "timer.device open failed");
    }
    TimerBase = (struct Device*)g_timreq->tr_node.io_Device;

    /* D23: uptime probe — GetUpTime, 100,000 µs TR_ADDREQUEST, GetUpTime. */
    const u64 u0 = timer_now_us();
    timer_sleep_us(100000u);
    const u64 u1 = timer_now_us();
    const u64 delta_us = u1 >= u0 ? u1 - u0 : 0ull;
    rt_fmt(line, sizeof line, "uptime_probe delta_us=%llu", delta_us);
    pl_log(p, line);
    if (delta_us < 80000ull || delta_us > 250000ull)
        alog_trap(__FILE__, __LINE__, "timer.device uptime not advancing");

    p.t0_us = u1;
    p.next_us = u1;

    /* D29: joystick probe, once, no other use in M0. */
    LowLevelBase = OpenLibrary((CONST_STRPTR)"lowlevel.library", 0);
    if (!LowLevelBase) {
        pl_log(p, "joy lowlevel=none");
    } else {
        for (u32 i = 0u; i < 4u; i++) {
            const ULONG st = ReadJoyPort(i);
            rt_fmt(line, sizeof line, "joy port=%u state=0x%08x", i, (u32)st);
            pl_log(p, line);
        }
    }

    /* D30 readback needs cybergraphics for GetCyberMapAttr/ReadPixelArray. */
    CyberGfxBase = OpenLibrary((CONST_STRPTR)"cybergraphics.library", 0);

    /* D24: open the loader once, in the dlfcn.h order; never closed.
     * D37: --plant-loader-fail skips all three OpenLibrary calls and falls
     * through to the real failure path below. */
    struct Library* lib = 0;
    const char* loader_name = "RAM:vulkan_loader.library";
    if (!g_plant_loader_fail) {
        lib = OpenLibrary((CONST_STRPTR)"RAM:vulkan_loader.library", 0);
        if (!lib) {
            lib = OpenLibrary((CONST_STRPTR)"vulkan_loader.library", 0);
            loader_name = "vulkan_loader.library";
            if (!lib) {
                lib = OpenLibrary((CONST_STRPTR)"RAM:vulkan.library", 0);
                loader_name = "RAM:vulkan.library";
            }
        }
    }
    if (!lib) {
        alog_trap(__FILE__, __LINE__,
                  "vulkan loader open failed: RAM:vulkan_loader.library, "
                  "vulkan_loader.library, RAM:vulkan.library");
    }
    p.vk_lib = (void*)lib;
    rt_fmt(line, sizeof line, "vk_loader=\"%s\"", loader_name);
    pl_log(p, line);
}

void pl_shutdown(Plat& p, int code) {
    if (g_wd.fired) code = 4;   /* D19: the watchdog fired and the counter did not move */
    /* the loader stays resident (C12: never closed, never flushed) */
    if (p.vk_lib) {
        struct Library* lib = (struct Library*)p.vk_lib;
        char line[128];
        rt_fmt(line, sizeof line, "vk_loader_open_count=%u", (u32)lib->lib_OpenCnt);
        pl_log(p, line);
    }
    ahi_log_summary(p);
    plat_release_all();
    Exit((LONG)code);
    __builtin_unreachable();
}
