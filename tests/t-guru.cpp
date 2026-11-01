#include <cstdio>
#include <cstring>
#include "platform.h"
#include "plat_priv.h"
#include "modules.h"
#include "guru_internal.h"

int main() {
    Plat p;
    PlatArgs pa;
    std::memset(&pa, 0, sizeof pa);
    pl_init(p, pa);
    GuruCtx g;
    guru_init(g, &p);
    guru_report(g, "room.txt", 12, "bad tile");
    for (int i = 0; i < 9; i++) guru_report(g, "room.txt", 12, "bad tile");
    u32 count = guru_ring_count(g);
    u32 oldest = guru_ring_oldest(g);
    std::printf("GURU ring_count=%u ring_oldest=%u\n", count, oldest);
    int bad = !(count == 8 && oldest == 3);
    std::printf(bad ? "FAIL t-guru\n" : "PASS t-guru\n");
    return bad;
}
