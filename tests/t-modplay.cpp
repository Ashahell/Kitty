// Host test for the MOD replayer: effects, tick length, determinism hash, rejects.
// Uses the engine's own modplay code (no re-implementation).
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "platform.h"
#include "modules.h"
#include "plat_priv.h"
#include "guru_internal.h"
#include "audio_internal.h"

struct Mod {
    Plat p;
    GuruCtx g;
    Audio au;
    u8* data;
    size_t n;
};

static int g_fail;

static void check(const char* name, bool ok) {
    std::printf("CASE %s %s\n", name, ok ? "PASS" : "FAIL");
    if (!ok) g_fail = 1;
}

static void setup(Mod& m) {
    PlatArgs pa;
    std::memset(&pa, 0, sizeof pa);
    pl_init(m.p, pa);
    guru_init(m.g, &m.p);
    std::memset(&m.au, 0, sizeof m.au);
    m.au.p = &m.p;
    m.au.g = &m.g;
    m.au.rate = 48000;
    m.au.channels = 2;
    m.au.mp.rate = 48000;
    m.au.alloc = plat_alloc_fn(m.p);
    m.data = 0;
    m.n = 0;
}

static bool load(Mod& m, const char* path) {
    setup(m);
    if (!plat_read_path(path, &m.data, &m.n, plat_alloc_fn(m.p))) return false;
    mod_load(m.au, m.data, m.n);
    return m.au.mp.loaded;
}

static u64 hash_render(const ModState& src, u32 frames, bool* silent) {
    ModState m = src;
    mod_reset(m);
    i16 buf[960 * 2];
    u64 h = 14695981039346656037ull;
    u32 left = frames;
    bool zero = true;
    while (left) {
        u32 n = left > 960u ? 960u : left;
        mod_render(m, buf, n);
        for (u32 i = 0; i < n * 2u; i++) {
            u16 v = (u16)buf[i];
            u8 lo = (u8)(v & 0xFFu);
            u8 hi = (u8)(v >> 8);
            h ^= (u64)lo;
            h *= 1099511628211ull;
            h ^= (u64)hi;
            h *= 1099511628211ull;
            if (buf[i] != 0) zero = false;
        }
        left -= n;
    }
    if (silent) *silent = zero;
    return h;
}

static void fixture_path(const char* argv1, const char* name, char* out, size_t cap) {
    const char* slash = std::strrchr(argv1, '/');
    size_t dirlen = slash ? (size_t)(slash - argv1) : 0;
    if (dirlen) std::snprintf(out, cap, "%.*s/%s", (int)dirlen, argv1, name);
    else std::snprintf(out, cap, "%s", name);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: t-modplay FILE.mod\n");
        return 2;
    }
    Mod md;
    if (!load(md, argv[1])) {
        std::fprintf(stderr, "t-modplay: cannot load %s\n", argv[1]);
        return 1;
    }
    const ModState base = md.au.mp;

    {
        ModState m = base;
        mod_reset(m);
        u32 speed0 = m.speed, tempo0 = m.tempo;
        mod_apply_effect(m, 0, 0xF, 0x00);
        check("F00_ignored", m.speed == speed0 && m.tempo == tempo0);
    }
    {
        ModState m = base;
        mod_reset(m);
        mod_apply_effect(m, 0, 0xF, 0x01);
        check("F01_speed1", m.speed == 1u && m.tempo == 125u);
    }
    {
        ModState m = base;
        mod_reset(m);
        mod_apply_effect(m, 0, 0xF, 0x1F);
        check("F1F_speed31", m.speed == 31u && m.tempo == 125u);
    }
    {
        ModState m = base;
        mod_reset(m);
        mod_apply_effect(m, 0, 0xF, 0x20);
        check("F20_tempo32", m.tempo == 32u && m.speed == 6u);
    }
    {
        ModState m = base;
        mod_reset(m);
        mod_apply_effect(m, 2, 0xF, 0x7D);
        u32 first = mod_tick_frames(m);
        check("F7D_tempo125_960", m.tempo == 125u && first == 960u);
    }
    {
        ModState m = base;
        mod_reset(m);
        mod_apply_effect(m, 3, 0xF, 0xFF);
        check("FFF_tempo255", m.tempo == 255u);
    }
    {
        ModState m = base;
        mod_reset(m);
        mod_apply_effect(m, 0, 0xF, 0x7D);
        u32 total = 0;
        for (u32 i = 0; i < 125u; i++) total += mod_tick_frames(m);
        check("tick_total_125", total == 120000u);
    }
    {
        ModState m = base;
        mod_reset(m);
        mod_apply_effect(m, 0, 0xF, 0xFF);
        u32 total = 0;
        for (u32 i = 0; i < 255u; i++) total += mod_tick_frames(m);
        check("tick_total_255", total == 120000u);
    }
    {
        ModState m = base;
        mod_reset(m);
        mod_apply_effect(m, 3, 0xC, 0x40);
        check("C40_vol64", m.ch[3].volume == 64u);
    }
    {
        ModState m = base;
        mod_reset(m);
        mod_apply_effect(m, 1, 0xC, 0x41);
        check("C41_clamp64", m.ch[1].volume == 64u);
    }
    {
        ModState m = base;
        mod_reset(m);
        m.order_i = 1;
        m.row = 1;
        m.speed = 1;
        m.tick_in_row = 0;
        mod_apply_effect(m, 1, 0xD, 0x15);
        mod_advance(m, 1);
        check("D15_row15", m.row == 15u && m.order_i == 2u);
    }
    {
        ModState m = base;
        mod_reset(m);
        m.order_i = 1;
        m.row = 1;
        m.speed = 1;
        m.tick_in_row = 0;
        mod_apply_effect(m, 0, 0xD, 0x64);
        mod_advance(m, 1);
        check("D64_row0", m.row == 0u && m.order_i == 2u);
    }
    {
        ModState m = base;
        mod_reset(m);
        m.order_i = 1;
        m.row = 1;
        m.speed = 1;
        m.tick_in_row = 0;
        mod_apply_effect(m, 2, 0xB, 0x02);
        mod_advance(m, 1);
        check("B_jump", m.order_i == 2u && m.row == 0u && !m.pos_jump);
    }

    ModState step_src = base;
    u32 step428 = mod_step_at(step_src, 0);
    std::printf("step_428=%u\n", step428);
    check("step_428", step428 == 11314u);

    bool silent_a = false, silent_b = false;
    u64 h_all = hash_render(base, 480000u, &silent_a);
    {
        Audio muted = md.au;
        mod_mute(muted, 0, true);
        u64 h_mute = hash_render(muted.mp, 480000u, 0);
        check("mute_changes_hash", h_mute != h_all);
    }
    {
        Audio muted = md.au;
        for (u8 c = 0; c < 4; c++) mod_mute(muted, c, true);
        u64 h_mute = hash_render(muted.mp, 480000u, &silent_b);
        (void)h_mute;
        check("mute_all_silent", silent_b);
    }
    if (silent_a) {
        std::printf("t-modplay: base render is silent\n");
        g_fail = 1;
    }

    const char* bad[4] = { "bad_len0.mod", "bad_len129.mod",
                           "bad_trunc_pattern.mod", "bad_trunc_sample.mod" };
    const char* names[4] = { "reject_len0", "reject_len129",
                             "reject_trunc_pattern", "reject_trunc_sample" };
    for (u32 i = 0; i < 4; i++) {
        char path[512];
        fixture_path(argv[1], bad[i], path, sizeof path);
        u8* data = 0;
        size_t n = 0;
        bool read_ok = plat_read_path(path, &data, &n, plat_alloc_fn(md.p));
        u32 before = guru_ring_count(md.g);
        Audio probe;
        std::memset(&probe, 0, sizeof probe);
        probe.p = &md.p;
        probe.g = &md.g;
        probe.rate = 48000;
        probe.channels = 2;
        probe.mp.rate = 48000;
        probe.alloc = plat_alloc_fn(md.p);
        if (read_ok) mod_load(probe, data, n);
        u32 after = guru_ring_count(md.g);
        bool ok = read_ok && !probe.mp.loaded && after == before + 1u;
        check(names[i], ok);
        if (data) plat_free(data);
    }

    std::printf("hash10s=%016llx\n", (unsigned long long)h_all);
    std::printf(g_fail ? "FAIL t-modplay\n" : "PASS t-modplay\n");
    return g_fail;
}
