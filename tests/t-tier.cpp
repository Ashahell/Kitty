#include <cstdio>
#include <cstring>
#include <string>
#include "tier.h"

int main() {
    TierDev nvidia = { 0x10DEu, VK_DRIVER_ID_NVIDIA_PROPRIETARY, "nvidia" };
    TierDev radv = { 0x1002u, VK_DRIVER_ID_MESA_RADV, "radv" };
    TierDev llvmpipe = { 0x10005u, VK_DRIVER_ID_MESA_LLVMPIPE, "llvmpipe" };
    TierDev nvk = { 0x10DEu, VK_DRIVER_ID_MESA_NVK, "nvk" };

    struct Case {
        const char* name;
        TierDev devs[3];
        u32 n;
        const char* over;
        bool have_selftest;
        bool selftest_ok;
        const char* want;
    };
    Case cases[6];
    std::memset(cases, 0, sizeof cases);

    cases[0].name = "host_mix";
    cases[0].devs[0] = nvidia; cases[0].devs[1] = radv; cases[0].devs[2] = llvmpipe;
    cases[0].n = 3; cases[0].want = "lavapipe";

    cases[1].name = "nvk_selftest_pass";
    cases[1].devs[0] = nvk; cases[1].devs[1] = llvmpipe;
    cases[1].n = 2; cases[1].have_selftest = true; cases[1].selftest_ok = true;
    cases[1].want = "nvk";

    cases[2].name = "nvk_selftest_fail";
    cases[2].devs[0] = nvk; cases[2].devs[1] = llvmpipe;
    cases[2].n = 2; cases[2].have_selftest = true; cases[2].selftest_ok = false;
    cases[2].want = "lavapipe";

    cases[3].name = "override_lavapipe_with_nvk";
    cases[3].devs[0] = nvk; cases[3].devs[1] = llvmpipe;
    cases[3].n = 2; cases[3].over = "lavapipe";
    cases[3].have_selftest = true; cases[3].selftest_ok = true;
    cases[3].want = "lavapipe";

    cases[4].name = "override_nvk_absent";
    cases[4].devs[0] = llvmpipe; cases[4].n = 1; cases[4].over = "nvk";
    cases[4].want = "lavapipe";

    cases[5].name = "no_llvmpipe";
    cases[5].devs[0] = nvidia; cases[5].devs[1] = radv; cases[5].n = 2;
    cases[5].want = "none";

    int bad = 0;
    for (int i = 0; i < 6; i++) {
        TierQuery q;
        std::memset(&q, 0, sizeof q);
        TierDev list[3];
        std::memset(list, 0, sizeof list);
        for (u32 k = 0; k < cases[i].n; k++) list[k] = cases[i].devs[k];
        q.devs = list;
        q.n = cases[i].n;
        q.over = cases[i].over;
        q.have_selftest = cases[i].have_selftest;
        q.selftest_ok = cases[i].selftest_ok;
        Tier t = (Tier)-1;
        TierPick p = tier_choose(q, &t);
        const char* got = tier_pick_name(p, t);
        std::printf("TIER case=%s pick=%s\n", cases[i].name, got);
        if (std::string(got) != cases[i].want) bad = 1;
    }
    std::printf(bad ? "FAIL t-tier\n" : "PASS t-tier\n");
    return bad;
}
