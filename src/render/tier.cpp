#include <cstring>
#include "tier.h"
#include "vk_ctx.h"

static bool has_driver(const TierQuery& q, u32 drv) {
    for (u32 i = 0; i < q.n; i++) if (q.devs[i].driver == drv) return true;
    return false;
}

static bool has_nvk(const TierQuery& q) {
    for (u32 i = 0; i < q.n; i++)
        if (q.devs[i].vendor == 0x10DEu && q.devs[i].driver == VK_DRIVER_ID_MESA_NVK) return true;
    return false;
}

const char* tier_pick_name(const TierPick& p, Tier t) {
    if ((int)t < 0) return "none";
    return p.nvk ? "nvk" : "lavapipe";
}

TierPick tier_choose(const TierQuery& q, Tier* out) {
    TierPick p;
    p.nvk = false;
    p.reason = 0;
    bool lvp = has_driver(q, VK_DRIVER_ID_MESA_LLVMPIPE);
    bool nvk = has_nvk(q);
    bool want_nvk = q.over && strcmp(q.over, "nvk") == 0;
    bool want_lvp = q.over && strcmp(q.over, "lavapipe") == 0;
    *out = (Tier)-1;
    if (want_lvp) {
        if (lvp) { *out = TIER_LAVAPIPE; return p; }
        p.reason = "lavapipe requested but no lavapipe device";
        return p;
    }
    if (want_nvk) {
        if (nvk && q.have_selftest && q.selftest_ok) { *out = TIER_NVK; p.nvk = true; return p; }
        if (lvp) {
            *out = TIER_LAVAPIPE;
            p.reason = nvk ? "NVK self-test failed" : "nvk requested but no NVK device";
        } else {
            p.reason = "no lavapipe device";
        }
        return p;
    }
    if (nvk && q.have_selftest && q.selftest_ok) { *out = TIER_NVK; p.nvk = true; return p; }
    if (lvp) { *out = TIER_LAVAPIPE; return p; }
    p.reason = "no lavapipe device";
    return p;
}
