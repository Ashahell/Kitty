#include <cstring>
#include <cstdlib>
#include "platform.h"
#include "modules.h"
#include "plat_svc.h"
#include "render_internal.h"
#include "bench_internal.h"

static u32 env_u32(Plat* p, const char* k, u32 dflt) {
    if (!p) return dflt;
    char b[32];
    pl_env_get(*p, k, b, sizeof b);
    if (!b[0]) return dflt;
    int v = atoi(b);
    return v < 0 ? 0u : (u32)v;
}

void bench_lite(Bench& b, RendCtx& r, u32 frames) {
    if (frames == 0) frames = 1;
    double* t = (double*)r.alloc(sizeof(double) * (size_t)frames);
    if (!t) return;
    UIRects ui;
    memset(&ui, 0, sizeof ui);
    ui.quads = 1;
    u32 od = b.overdraw ? b.overdraw : 1u;

    for (u32 i = 0; i < frames; i++) {
        u64 a = pl_now_us(*b.p);
        r_begin(r, 0, i);
        if (r.scene) {
            DrawList dl = r.scene->dl;
            for (u32 k = 0; k < od; k++) r_draw(r, dl, 1);
        }
        r_overlay(r, ui, 1);
        r_present(r);
        u64 z = pl_now_us(*b.p);
        t[i] = (double)(z - a) / 1000.0;
    }

    for (u32 i = 1; i < frames; i++) {
        double v = t[i];
        u32 j = i;
        while (j > 0 && t[j - 1] > v) {
            t[j] = t[j - 1];
            j--;
        }
        t[j] = v;
    }
    double mn = t[0];
    double md = t[frames / 2];
    u32 p95i = (frames * 95u) / 100u;
    if (p95i >= frames) p95i = frames - 1;
    double p95 = t[p95i];
    double mx = t[frames - 1];

    char cpu[128];
    pl_cpu_model(*b.p, cpu, sizeof cpu);
    char line[768];
    rt_fmt(line, sizeof line, "BENCH lp_threads=%u overdraw=%u frames=%u init_ms=%.3f pipeline_ms=%.3f "
          "ms_min=%.3f ms_med=%.3f ms_p95=%.3f ms_max=%.3f cpu=\"%s\" virt=host driver=\"%s\"",
          env_u32(b.p, "LP_NUM_THREADS", 0), od, frames, (double)r.init_ms,
          (double)r.pipe_ms, mn, md, p95, mx, cpu, r_driver_info(r));
    pl_log(*b.p, line);
}
