#pragma once
#include "render_types.h"

struct TierDev { u32 vendor; u32 driver; const char* name; };
struct TierQuery {
    const TierDev* devs;
    u32 n;
    const char* over;
    bool selftest_ok;
    bool have_selftest;
};

TierPick tier_choose(const TierQuery&, Tier* out);
const char* tier_pick_name(const TierPick&, Tier);
