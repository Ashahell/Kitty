#include <cstring>
#include <cstdlib>
#include "platform.h"
#include "modules.h"
#include "wsi.h"
#include "plat_priv.h"
#include "rt_fmt.h"
#include "render_internal.h"
#include "guru_internal.h"
#include "audio_internal.h"
#include "bench_internal.h"
#include "m0_scene.h"

#define LOGF(p_, ...) do { char logf_line_[512]; rt_fmt(logf_line_, sizeof logf_line_, __VA_ARGS__); pl_log((p_), logf_line_); } while (0)

#if defined(__AROS__) && defined(KITTY_DEBUG)
/* D37: aros platform-side plant, defined in src/platform/aros/plat.cpp. */
extern void plat_aros_plant_loader_fail(void);
#endif

struct Opts {
    bool headless;
    bool windowed;
    bool bench;
    bool plant_copy;
    bool plant_present;
    int frames;
    int seconds;
    int overdraw;
    const char* dump;
    const char* out;
    const char* spv;
    const char* music;
    bool tier_probe;
    bool plant_probe;
    bool plant_no_demote;
    const char* probe_candidate;
    int present_dump;   // D30: AROS-only presented-pixel proof (-1 = off)
    bool force_fifo;    // D30: debug-only FIFO present-mode plant
    int lp_threads;     // D27: debug-only LP_NUM_THREADS override (-1 = engine default)
    bool plant_loader_fail;  // D37: debug-only AROS plant (D24 failure path)
};

static void usage(void) {
    plat_log_raw("usage: kitty (--headless | --windowed | --tier-probe) [--frames N | --seconds S] "
                  "[--dump i,j,..] [--out DIR] [--spv DIR] [--music FILE] [--bench] "
                  "[--overdraw 1|2|4] [--probe-candidate lavapipe] [--plant-bad-probe] [--plant-no-demote] "
                  "[--plant-loader-fail]");
}

static int parse_dump(const char* s, u32* out, u32 max) {
    u32 n = 0;
    const char* p = s;
    while (*p && n < max) {
        char* end = 0;
        unsigned long v = strtoul(p, &end, 10);
        if (end == p) return -1;
        out[n++] = (u32)v;
        p = end;
        if (*p == ',') p++;
        else if (*p) return -1;
    }
    return (int)n;
}

static bool listed(const u32* list, int n, u32 f) {
    for (int i = 0; i < n; i++) if (list[i] == f) return true;
    return false;
}

static void write_frame(Plat& p, const char* dir, u32 f, const u8* px) {
    char path[512];
    rt_fmt(path, sizeof path, "%s/frame_%03u.bgra", dir, f);
    if (!pl_write_file(p, path, px, 320u * 256u * 4u)) {
        char line_[1024];
        rt_fmt(line_, sizeof line_, "dump_failed path=%s", path);
        pl_log(p, line_);
    }
}

static void render_frame(RendCtx& r, RScene& s, u32 tick) {
    m0_scene_animate(&s, tick);
    r_begin(r, 0, tick);
    r_draw(r, s.dl, 1);
    UIRects ui;
    memset(&ui, 0, sizeof ui);
    ui.quads = 1;
    r_overlay(r, ui, 1);
    r_present(r);
}

[[gnu::noinline]] static int parse_args(int argc, char** argv, Opts& o) {
    memset(&o, 0, sizeof o);
    o.frames = -1;
    o.seconds = -1;
    o.overdraw = 1;
    o.out = ".";
    o.present_dump = -1;
    o.lp_threads = -1;

    for (int i = 1; i < argc; i++) {
        const char* a = argv[i];
        if (strcmp(a, "--headless") == 0) { o.headless = true; continue; }
        if (strcmp(a, "--windowed") == 0) { o.windowed = true; continue; }
        if (strcmp(a, "--bench") == 0) { o.bench = true; continue; }
        if (strcmp(a, "--plant-bad-copy") == 0) { o.plant_copy = true; continue; }
        if (strcmp(a, "--plant-bad-present") == 0) { o.plant_present = true; continue; }
        if (strcmp(a, "--tier-probe") == 0) { o.tier_probe = true; continue; }
        if (strcmp(a, "--plant-bad-probe") == 0) { o.plant_probe = true; continue; }
        if (strcmp(a, "--plant-no-demote") == 0) { o.plant_no_demote = true; continue; }
        if (strcmp(a, "--force-fifo") == 0) { o.force_fifo = true; continue; }
        if (strcmp(a, "--plant-loader-fail") == 0) { o.plant_loader_fail = true; continue; }
        if (strcmp(a, "--probe-candidate") == 0) {
            if (i + 1 >= argc) { usage(); return 2; }
            o.probe_candidate = argv[++i];
            continue;
        }
        if (strcmp(a, "--present-dump") == 0 || strcmp(a, "--lp-threads") == 0) {
            if (i + 1 >= argc) { usage(); return 2; }
            const char* v = argv[++i];
            if (strcmp(a, "--present-dump") == 0) o.present_dump = atoi(v);
            else o.lp_threads = atoi(v);
            continue;
        }
        if (strcmp(a, "--frames") == 0 || strcmp(a, "--seconds") == 0 ||
            strcmp(a, "--overdraw") == 0 || strcmp(a, "--dump") == 0 ||
            strcmp(a, "--out") == 0 || strcmp(a, "--spv") == 0 || strcmp(a, "--music") == 0) {
            if (i + 1 >= argc) { usage(); return 2; }
            const char* v = argv[++i];
            if (strcmp(a, "--frames") == 0) o.frames = atoi(v);
            else if (strcmp(a, "--seconds") == 0) o.seconds = atoi(v);
            else if (strcmp(a, "--overdraw") == 0) o.overdraw = atoi(v);
            else if (strcmp(a, "--dump") == 0) o.dump = v;
            else if (strcmp(a, "--out") == 0) o.out = v;
            else if (strcmp(a, "--spv") == 0) o.spv = v;
            else o.music = v;
            continue;
        }
        usage();
        return 2;
    }

    if (o.tier_probe && (o.headless || o.windowed)) { usage(); return 2; }
    if (o.tier_probe) {
        if (o.frames >= 0 || o.seconds >= 0 || o.bench || o.dump) { usage(); return 2; }
    } else {
        if ((!o.headless && !o.windowed) || (o.headless && o.windowed)) { usage(); return 2; }
    }
    if (o.frames < 0 && o.seconds < 0) o.seconds = 1;
    if (o.overdraw != 1 && o.overdraw != 2 && o.overdraw != 4) o.overdraw = 1;

    /* D27/D30: --lp-threads and --force-fifo are debug-only plants;
     * --present-dump and --lp-threads exist only on AROS (Linux exits 2).
     * D37: --plant-loader-fail is an AROS-debug plant only. */
#if !defined(__AROS__)
    if (o.present_dump >= 0) { plat_log_raw("--present-dump is AROS-only"); return 2; }
    if (o.lp_threads >= 0) { plat_log_raw("--lp-threads is AROS-only"); return 2; }
    if (o.plant_loader_fail) { plat_log_raw("--plant-loader-fail is AROS-only"); return 2; }
#endif
#ifndef KITTY_DEBUG
    if (o.present_dump >= 0 || o.force_fifo || o.lp_threads >= 0) {
        plat_log_raw("present/lp options are debug-only");
        return 2;
    }
    if (o.plant_loader_fail) {
        plat_log_raw("--plant-loader-fail is debug-only");
        return 2;
    }
#endif
#ifdef KITTY_DEBUG
    if (o.lp_threads >= 0 && o.lp_threads != 0 && o.lp_threads != 1 && o.lp_threads != 4) {
        usage();
        return 2;
    }
#endif

#ifndef KITTY_DEBUG
    if (o.plant_copy || o.plant_present) {
        plat_log_raw("plant options are debug-only");
        return 2;
    }
    if (o.tier_probe || o.plant_probe || o.probe_candidate || o.plant_no_demote) {
        plat_log_raw("tier-probe options are debug-only");
        return 2;
    }
#endif
    return 0;
}

struct RunArgs {
    Plat* plat;
    RendCtx* r;
    RScene* scene;
    Audio* au;
    const u32* dump_list;
    int dump_n;
    const char* out;
    bool plant_present;
    int present_dump;
};

[[gnu::noinline]] static void run_frames(RunArgs& a, u32 n, u64& ticks, u64& frames) {
    for (u32 i = 0; i < n; i++) {
        render_frame(*a.r, *a.scene, i);
        frames++;
        if (listed(a.dump_list, a.dump_n, i)) write_frame(*a.plat, a.out, i, r_pixels(*a.r));
        if (a.present_dump >= 0 && (u32)a.present_dump == i) pl_present_dump(*a.plat, i, a.out);
    }
    ticks = n;
}

[[gnu::noinline]] static void run_seconds(RunArgs& a, u64 target, u64& ticks, u64& dropped, u64& frames) {
    if (a.plant_present) plat_exit_watchdog_ms(*a.plat, 2000, r_validation_counter(*a.r));
    while (ticks + dropped < target && !a.plat->quit) {
        u64 due = 0;
        pl_step(*a.plat, due);
        pl_poll_events(*a.plat, 0);
        if (!due) { plat_sleep_us(500); continue; }
        PaceOut po = pace_step((u32)due);
        if (po.dropped) {
            dropped += po.dropped;
            LOGF(*a.plat, "catchup_drop ticks=%u", po.dropped);
        }
        for (u32 k = 0; k < po.stepped; k++) {
            u32 tick = (u32)ticks;
            ticks++;
            render_frame(*a.r, *a.scene, tick);
            frames++;
            if (listed(a.dump_list, a.dump_n, tick)) write_frame(*a.plat, a.out, tick, r_pixels(*a.r));
            if ((ticks % 3000ull) == 0ull) plat_soak_sample(*a.plat);
            if (a.au->opened) aud_tick(*a.au);
            if (a.plant_present) break;
        }
        if (a.plant_present) break;
    }
    if (a.plant_present) plat_exit_watchdog_cancel();
}

[[gnu::noinline]] static int run_probe(Plat& plat, const Opts& o) {
    if (o.probe_candidate) r_set_probe_candidate(o.probe_candidate);
    if (o.plant_probe) r_set_probe_plant(true);
    if (o.plant_no_demote) r_set_probe_no_demote(true);
    GoldenImg ref = {};
    MatchResult res;
    res.ok = false;
    res.mismatched_px = 0;
    r_tier_probe(plat_alloc_fn(plat), &ref, &res);
    u32 coverage = 0;
    for (u32 i = 0; i < 64u * 64u; i++) if (ref.rgba[i * 4u + 3u]) coverage++;
    const char* cname = r_probe_candidate();
    if (!cname) cname = "none";
    char log_line[512];
    if (res.ok) {
        rt_fmt(log_line, sizeof log_line,
               "probe draws=5 pipelines=5 candidate=%s coverage_px=%u mismatched_px=%u ok=1",
               cname, coverage, res.mismatched_px);
    } else {
        const char* why = r_probe_reason();
        if (!why) why = "unknown";
        rt_fmt(log_line, sizeof log_line,
               "probe draws=5 pipelines=5 candidate=%s coverage_px=%u mismatched_px=%u ok=0 reason=\"%s\"",
               cname, coverage, res.mismatched_px, why);
    }
    pl_log(plat, log_line);
    char path[512];
    rt_fmt(path, sizeof path, "%s/probe_ref.bgra", o.out);
    if (!pl_write_file(plat, path, ref.rgba, 64u * 64u * 4u)) {
        char bad[512];
        rt_fmt(bad, sizeof bad, "dump_failed path=%s", path);
        pl_log(plat, bad);
    }
    const u8* cand = r_probe_cand_bytes();
    if (cand) {
        rt_fmt(path, sizeof path, "%s/probe_cand.bgra", o.out);
        if (!pl_write_file(plat, path, cand, 64u * 64u * 4u)) {
            char bad[512];
            rt_fmt(bad, sizeof bad, "dump_failed path=%s", path);
            pl_log(plat, bad);
        }
    }
    pl_shutdown(plat, 0);
    return 0;
}

[[gnu::noinline]] static int run_engine(Plat& plat, const Opts& o) {
    TierPick pick = {};
    RendCtx* r = 0;
    r_set_force_fifo(o.force_fifo);   // read by r_init_wsi, before the first swapchain
    if (o.headless) r = r_init(plat_alloc_fn(plat), TIER_LAVAPIPE, &pick);
    else r = r_init_wsi(plat_alloc_fn(plat), TIER_LAVAPIPE, &pick, plat);
    if (!r) return 4;
    plat_lp_worker_tasks(plat);   // D27: after vkCreateDevice, which r_init* just did

    if (o.plant_copy) r_set_plant_bad_copy(*r, true);
    if (o.plant_present) r_set_plant_bad_present(*r, true);

    RScene scene;
    m0_scene_build(plat_alloc_fn(plat), &scene);
    m0_scene_animate(&scene, 0);
    r_set_scene(*r, &scene);
    LOGF(plat, "room_tris=%u kitty_tris=%u kitties=6", m0_room_tris(), m0_kitty_tris());

    GuruCtx guru;
    guru_init(guru, &plat);
    Audio au = {};
    au.p = &plat;
    au.g = &guru;
    au.rate = 48000;
    au.channels = 2;
    au.mp.rate = 48000;
    au.alloc = plat_alloc_fn(plat);

    if (o.music) {
        u8* md = 0;
        size_t mn = 0;
        if (!plat_read_path(o.music, &md, &mn, plat_alloc_fn(plat))) {
            guru_report(guru, o.music, 1, "music file not readable");
            LOGF(plat, "music=off");
        } else {
            mod_load(au, md, mn);
            if (au.mp.loaded) {
                aud_open(au, 48000, 2);
                if (au.opened) LOGF(plat, "music=on");
                else { LOGF(plat, "audio_open=failed"); LOGF(plat, "music=off"); }
            } else {
                LOGF(plat, "music=off");
            }
        }
    }

    if (o.bench) {
        Bench b;
        memset(&b, 0, sizeof b);
        b.overdraw = (u32)o.overdraw;
        b.p = &plat;
        bench_lite(b, *r, (u32)(o.frames > 0 ? o.frames : 300));
        LOGF(plat, "validation_messages=%u", r_validation_count(*r));
        r_shutdown(*r);
        pl_shutdown(plat, 0);
    }

    u32 dump_list[32];
    int dump_n = 0;
    if (o.dump) {
        int got = parse_dump(o.dump, dump_list, 32);
        if (got < 0) { usage(); return 2; }
        dump_n = got;
    }

    u64 ticks = 0;
    u64 dropped = 0;
    u64 frames = 0;

    RunArgs ra;
    ra.plat = &plat;
    ra.r = r;
    ra.scene = &scene;
    ra.au = &au;
    ra.dump_list = dump_list;
    ra.dump_n = dump_n;
    ra.out = o.out;
    ra.plant_present = o.plant_present;
    ra.present_dump = o.present_dump;

    if (o.frames >= 0) {
        run_frames(ra, (u32)o.frames, ticks, frames);
    } else {
        u64 target = (u64)(o.seconds > 0 ? o.seconds : 1) * 50ull;
        run_seconds(ra, target, ticks, dropped, frames);
    }

    LOGF(plat, "ticks=%llu catchup_dropped=%llu frames=%llu",
              (unsigned long long)ticks, (unsigned long long)dropped,
              (unsigned long long)frames);
    plat_exit_stats(plat);
    if (o.music) {
        LOGF(plat, "audio_device=%s audio_frames_queued=%llu",
                  plat_audio_driver(plat), (unsigned long long)plat_audio_total_frames(plat));
    }
    u32 vc = r_validation_count(*r);
    LOGF(plat, "validation_messages=%u", vc);

    int code = 0;
#ifdef KITTY_DEBUG
    if (vc) code = 3;
#endif
    r_shutdown(*r);   // AROS frees nothing at exit: the window and the device go now
    pl_shutdown(plat, code);
    return code;
}

int main(int argc, char** argv) {
    Opts o;
    int arc = parse_args(argc, argv, o);
    if (arc) return arc;

    Plat plat;
    PlatArgs pa;
    memset(&pa, 0, sizeof pa);
    pa.lp_threads = o.lp_threads;
    pa.lp_src = "flag";
#if defined(__AROS__) && defined(KITTY_DEBUG)
    /* D37: the plant is a platform-side switch; only the AROS debug build
     * can reach this (parse_args returns 2 everywhere else). */
    if (o.plant_loader_fail) plat_aros_plant_loader_fail();
#endif
    pl_init(plat, pa);
    r_set_plat(plat);
    if (o.spv) r_set_spv_dir(o.spv);

    if (o.tier_probe) return run_probe(plat, o);
    return run_engine(plat, o);
}
