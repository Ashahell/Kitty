#include <cstdio>
#include <cstring>
#include "../src/platform/rt/rt_fmt.h"

static int bad = 0;

static void zero(char* b, int n) { for (int i = 0; i < n; ++i) b[i] = '\0'; }

static void report(const char* name, const char* got, u32 gotret, const char* want, u32 wantret) {
    std::printf("FMT case=%s out=\"%s\" ret=%u\n", name, got, gotret);
    if (std::strcmp(got, want) != 0 || gotret != wantret) bad = 1;
}

int main() {
    char b[64];
    u32 r;

    zero(b, 64); r = rt_fmt(b, 64, "%u", 0u);
    report("u0", b, r, "0", 1u);

    zero(b, 64); r = rt_fmt(b, 64, "%03u", 7u);
    report("u_pad", b, r, "007", 3u);

    zero(b, 64); r = rt_fmt(b, 64, "%5u", 42u);
    report("u_wide", b, r, "   42", 5u);

    zero(b, 64); r = rt_fmt(b, 64, "%d", -42);
    report("d_neg", b, r, "-42", 3u);

    zero(b, 64); r = rt_fmt(b, 64, "%d", (int)0x80000000u);
    report("d_min", b, r, "-2147483648", 11u);

    zero(b, 64); r = rt_fmt(b, 64, "%lld", -9000000000LL);
    report("lld_neg", b, r, "-9000000000", 11u);

    zero(b, 64); r = rt_fmt(b, 64, "%llu", 18446744073709551615ULL);
    report("llu_max", b, r, "18446744073709551615", 20u);

    zero(b, 64); r = rt_fmt(b, 64, "%x", 255u);
    report("x", b, r, "ff", 2u);

    zero(b, 64); r = rt_fmt(b, 64, "%016llx", 0xe663d2da535e69a5ULL);
    report("llx_hash", b, r, "e663d2da535e69a5", 16u);

    zero(b, 64); r = rt_fmt(b, 64, "%016llx", 255ULL);
    report("llx_pad", b, r, "00000000000000ff", 16u);

    zero(b, 64); r = rt_fmt(b, 64, "%s", "abc");
    report("s", b, r, "abc", 3u);

    zero(b, 64); r = rt_fmt(b, 64, "%s", (const char*)0);
    report("s_null", b, r, "(null)", 6u);

    zero(b, 64); r = rt_fmt(b, 64, "%c", 'K');
    report("c", b, r, "K", 1u);

    zero(b, 64); r = rt_fmt(b, 64, "%u%%", 100u);
    report("pct", b, r, "100%", 4u);

    zero(b, 64); r = rt_fmt(b, 64, "%.3f", 1.5);
    report("f3", b, r, "1.500", 5u);

    zero(b, 64); r = rt_fmt(b, 64, "%.3f", 2.0625);
    report("f3_round", b, r, "2.063", 5u);

    zero(b, 64); r = rt_fmt(b, 64, "%.3f", -0.25);
    report("f3_neg", b, r, "-0.250", 6u);

    zero(b, 64); r = rt_fmt(b, 64, "%.0f", 2.5);
    report("f0_half", b, r, "3", 1u);

    zero(b, 64); r = rt_fmt(b, 64, "tier=%s n=%u ms=%.3f", "lavapipe", 5u, 0.694);
    report("mixed", b, r, "tier=lavapipe n=5 ms=0.694", 26u);

    zero(b, 64); r = rt_fmt(b, 8, "%s", "abcdefghij");
    report("trunc", b, r, "abcdefg", 10u);

    { char z[4];
      zero(z, 4); r = rt_fmt(z, 0, "abc");
      report("n0", z, r, "", 3u); }

    zero(b, 64); r = rt_fmt(b, 64, "%q");
    report("unknown", b, r, "%q", 2u);

    std::printf(bad ? "FAIL t-rtfmt\n" : "PASS t-rtfmt\n");
    return bad;
}
