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
    const char* probe_candidate;
};

static void usage(void) {
    plat_log_raw("usage: kitty (--headless | --windowed | --tier-probe) [--frames N | --seconds S] "
                  "[--dump i,j,..] [--out DIR] [--spv DIR] [--music FILE] [--bench] "
                  "[--overdraw 1|2|4] [--probe-candidate lavapipe] [--plant-bad-probe]");
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

int main(int argc, char** argv) {
    Opts o;
    memset(&o, 0, sizeof o);
    o.frames = -1;
    o.seconds = -1;
    o.overdraw = 1;
    o.out = ".";

    for (int i = 1; i < argc; i++) {
        const char* a = argv[i];
        if (strcmp(a, "--headless") == 0) { o.headless = true; continue; }
        if (strcmp(a, "--windowed") == 0) { o.windowed = true; continue; }
        if (strcmp(a, "--bench") == 0) { o.bench = true; continue; }
        if (strcmp(a, "--plant-bad-copy") == 0) { o.plant_copy = true; continue; }
        if (strcmp(a, "--plant-bad-present") == 0) { o.plant_present = true; continue; }
        if (strcmp(a, "--tier-probe") == 0) { o.tier_probe = true; continue; }
        if (strcmp(a, "--plant-bad-probe") == 0) { o.plant_probe = true; continue; }
        if (strcmp(a, "--probe-candidate") == 0) {
            if (i + 1 >= argc) { usage(); return 2; }
            o.probe_candidate = argv[++i];
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

#ifndef KITTY_DEBUG
    if (o.plant_copy || o.plant_present) {
        plat_log_raw("plant options are debug-only");
        return 2;
    }
    if (o.tier_probe || o.plant_probe || o.probe_candidate) {
        plat_log_raw("tier-probe options are debug-only");
        return 2;
    }
#endif

    Plat plat;
    PlatArgs pa;
    memset(&pa, 0, sizeof pa);
    pl_init(plat, pa);
    r_set_plat(plat);
    if (o.spv) r_set_spv_dir(o.spv);

    if (o.tier_probe) {
        if (o.probe_candidate) r_set_probe_candidate(o.probe_candidate);
        if (o.plant_probe) r_set_probe_plant(true);
        GoldenImg ref;
        memset(&ref, 0, sizeof ref);
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

    TierPick pick;
    memset(&pick, 0, sizeof pick);
    RendCtx* r = 0;
    if (o.headless) r = r_init(plat_alloc_fn(plat), TIER_LAVAPIPE, &pick);
    else r = r_init_wsi(plat_alloc_fn(plat), TIER_LAVAPIPE, &pick, plat);
    if (!r) return 4;

    if (o.plant_copy) r_set_plant_bad_copy(*r, true);
    if (o.plant_present) r_set_plant_bad_present(*r, true);

    RScene scene;
    m0_scene_build(plat_alloc_fn(plat), &scene);
    m0_scene_animate(&scene, 0);
    r_set_scene(*r, &scene);
    LOGF(plat, "room_tris=%u kitty_tris=%u kitties=6", m0_room_tris(), m0_kitty_tris());

    GuruCtx guru;
    guru_init(guru, &plat);
    Audio au;
    memset(&au, 0, sizeof au);
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

    if (o.frames >= 0) {
        u32 n = (u32)o.frames;
        for (u32 i = 0; i < n; i++) {
            render_frame(*r, scene, i);
            frames++;
            if (listed(dump_list, dump_n, i)) write_frame(plat, o.out, i, r_pixels(*r));
        }
        ticks = n;
    } else {
        u64 target = (u64)(o.seconds > 0 ? o.seconds : 1) * 50ull;
        if (o.plant_present) plat_exit_watchdog_ms(plat, 2000, r_validation_counter(*r));
        while (ticks + dropped < target && !plat.quit) {
            u64 due = 0;
            pl_step(plat, due);
            pl_poll_events(plat, 0);
            if (!due) { plat_sleep_us(500); continue; }
            PaceOut po = pace_step((u32)due);
            if (po.dropped) {
                dropped += po.dropped;
                LOGF(plat, "catchup_drop ticks=%u", po.dropped);
            }
            for (u32 k = 0; k < po.stepped; k++) {
                u32 tick = (u32)ticks;
                ticks++;
                render_frame(*r, scene, tick);
                frames++;
                if (listed(dump_list, dump_n, tick)) write_frame(plat, o.out, tick, r_pixels(*r));
                if (au.opened) aud_tick(au);
                if (o.plant_present) break;
            }
            if (o.plant_present) break;
        }
        if (o.plant_present) plat_exit_watchdog_cancel();
    }

    LOGF(plat, "ticks=%llu catchup_dropped=%llu frames=%llu",
              (unsigned long long)ticks, (unsigned long long)dropped,
              (unsigned long long)frames);
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
    pl_shutdown(plat, code);
    return code;
}
