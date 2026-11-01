#include <cstdio>
#include "plat_priv.h"

int main() {
    int bad = 0;
    struct Case { const char* name; u32 due; u32 step; u32 drop; };
    const Case cases[3] = {
        { "on_time", 1, 1, 0 },
        { "late_3", 3, 3, 0 },
        { "late_10", 10, 4, 6 }
    };
    for (int i = 0; i < 3; i++) {
        PaceOut o = pace_step(cases[i].due);
        std::printf("PACE case=%s stepped=%u dropped=%u\n", cases[i].name, o.stepped, o.dropped);
        if (o.stepped != cases[i].step || o.dropped != cases[i].drop) bad = 1;
    }
    std::printf(bad ? "FAIL t-pacing\n" : "PASS t-pacing\n");
    return bad;
}
