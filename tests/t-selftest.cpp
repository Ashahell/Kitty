#include <cstdio>
#include <cstring>
#include "selftest.h"

static void make(u8* px, u8 r, u8 g, u8 b, u8 a) {
    for (u32 i = 0; i < 4; i++) {
        px[i * 4 + 0] = r;
        px[i * 4 + 1] = g;
        px[i * 4 + 2] = b;
        px[i * 4 + 3] = a;
    }
}

int main() {
    int bad = 0;
    u8 want[16], got[16];
    MatchResult m;

    make(want, 10, 20, 30, 255);
    make(got, 10, 20, 30, 255);
    m = image_compare(got, want, 4, 4);
    std::printf("SELFTEST case=exact ok=%d\n", m.ok ? 1 : 0);
    if (!m.ok) bad = 1;

    make(got, 14, 16, 34, 255);
    m = image_compare(got, want, 4, 4);
    std::printf("SELFTEST case=delta4 ok=%d\n", m.ok ? 1 : 0);
    if (!m.ok) bad = 1;

    make(got, 15, 16, 34, 255);
    m = image_compare(got, want, 4, 4);
    std::printf("SELFTEST case=delta5 ok=%d\n", m.ok ? 1 : 0);
    if (m.ok) bad = 1;

    make(got, 10, 20, 30, 0);
    m = image_compare(got, want, 4, 4);
    std::printf("SELFTEST case=coverage_mismatch ok=%d\n", m.ok ? 1 : 0);
    if (m.ok) bad = 1;

    std::printf(bad ? "FAIL t-selftest\n" : "PASS t-selftest\n");
    return bad;
}
