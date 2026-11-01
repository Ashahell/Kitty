#include <cstring>
#include "platform.h"
#include "modules.h"
#include "rt_fmt.h"
#include "guru_internal.h"

static void copyz(char* dst, size_t n, const char* src) {
    size_t i = 0;
    if (src) { for (; src[i] && i + 1 < n; i++) dst[i] = src[i]; }
    dst[i] = 0;
}

void guru_init(GuruCtx& g, Plat* p) {
    g.p = p;
    g.count = 0;
    g.head = 0;
    for (u32 i = 0; i < 8; i++) {
        g.ring[i].file[0] = 0;
        g.ring[i].line = 0;
        g.ring[i].msg[0] = 0;
    }
}

void guru_report(GuruCtx& g, const char* file, u32 line, const char* msg) {
    char buf[256];
    rt_fmt(buf, sizeof buf, "Software Failure. %s:%u: %s",
           file ? file : "?", line, msg ? msg : "?");
    if (g.p) pl_log(*g.p, buf);
    GuruCtx::Rec& r = g.ring[g.head];
    copyz(r.file, sizeof r.file, file ? file : "?");
    copyz(r.msg, sizeof r.msg, msg ? msg : "?");
    r.line = line;
    g.head = (g.head + 1u) % 8u;
    g.count++;
}

u32 guru_ring_count(const GuruCtx& g) { return g.count < 8u ? g.count : 8u; }
u32 guru_ring_oldest(const GuruCtx& g) { return g.count <= 8u ? 1u : g.count - 8u + 1u; }
