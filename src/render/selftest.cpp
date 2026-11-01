#include "selftest.h"

MatchResult image_compare(const u8* got, const u8* want, u32 pixels, u32 tol) {
    MatchResult m;
    m.ok = true;
    m.mismatched_px = 0;
    for (u32 i = 0; i < pixels; i++) {
        bool bad = false;
        for (u32 c = 0; c < 4; c++) {
            u32 a = got[i * 4 + c];
            u32 b = want[i * 4 + c];
            u32 d = a > b ? a - b : b - a;
            if (d > tol) bad = true;
        }
        bool ca = got[i * 4 + 3] != 0;
        bool cb = want[i * 4 + 3] != 0;
        if (ca != cb) bad = true;
        if (bad) m.mismatched_px++;
    }
    m.ok = m.mismatched_px == 0;
    return m;
}
