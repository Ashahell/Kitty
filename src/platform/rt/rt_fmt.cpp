#include "rt_fmt.h"

static const u64 kpow10[7] = { 1ull, 10ull, 100ull, 1000ull, 10000ull, 100000ull, 1000000ull };
static const char khexd[] = "0123456789abcdef";

u32 rt_vfmt(char* out, u32 n, const char* fmt, va_list ap) {
    u32 len = 0u;
    auto put = [&](char c) { if (n != 0u && len + 1u < n) out[len] = c; ++len; };
    auto pad = [&](u32 cnt, char fill) { for (u32 i = 0u; i < cnt; ++i) put(fill); };

    for (const char* p = fmt; *p != '\0'; ++p) {
        if (*p != '%') { put(*p); continue; }

        const char* q = p + 1;
        bool zero = false;
        if (*q == '0') { zero = true; ++q; }
        u32 width = 0u;
        if (*q >= '1' && *q <= '9') {
            while (*q >= '0' && *q <= '9') {
                width = width * 10u + static_cast<u32>(*q - '0');
                if (width > 32u) width = 32u;
                ++q;
            }
        }
        u32 prec = 0u;
        bool dot = false;
        if (*q == '.') {
            ++q;
            if (*q >= '0' && *q <= '9') {
                dot = true;
                while (*q >= '0' && *q <= '9') {
                    prec = prec * 10u + static_cast<u32>(*q - '0');
                    if (prec > 6u) prec = 99u;
                    ++q;
                }
            }
        }
        bool llflag = false;
        if (!dot && q[0] == 'l' && q[1] == 'l') { llflag = true; q += 2; }
        const char conv = *q;

        bool ok;
        if (dot) ok = (prec <= 6u && conv == 'f');
        else if (llflag) ok = (conv == 'd' || conv == 'u' || conv == 'x');
        else ok = (conv == 'd' || conv == 'u' || conv == 'x' || conv == 's' || conv == 'c' || conv == '%');
        if (!ok) { put('%'); continue; }

        p = q;

        if (conv == '%') { put('%'); continue; }

        if (conv == 'c') {
            const int ch = va_arg(ap, int);
            pad(width > 1u ? width - 1u : 0u, ' ');
            put(static_cast<char>(ch));
            continue;
        }

        if (conv == 's') {
            const char* s = va_arg(ap, const char*);
            if (s == nullptr) s = "(null)";
            u32 d = 0u;
            const char volatile* v = s;
            while (*v != '\0') { ++v; ++d; }
            pad(width > d ? width - d : 0u, ' ');
            for (const char* t = s; *t != '\0'; ++t) put(*t);
            continue;
        }

        if (conv == 'f') {
            const double v = va_arg(ap, double);
            const bool negf = v < 0.0;
            const double av = negf ? -v : v;
            const u64 pw = kpow10[prec];
            const u64 qq = static_cast<u64>(av * static_cast<double>(pw) + 0.5);
            char seq[72];
            u32 L = 0u;
            if (negf) seq[L++] = '-';
            u64 ip = qq / pw;
            if (ip == 0u) seq[L++] = '0';
            else { char rev[24]; u32 k = 0u; while (ip != 0u) { rev[k++] = static_cast<char>('0' + static_cast<int>(ip % 10u)); ip /= 10u; } while (k != 0u) seq[L++] = rev[--k]; }
            if (prec > 0u) {
                seq[L++] = '.';
                u64 fr = qq % pw;
                char rev[8];
                for (u32 i = 0u; i < prec; ++i) { rev[i] = static_cast<char>('0' + static_cast<int>(fr % 10u)); fr /= 10u; }
                for (u32 i = prec; i > 0u; --i) seq[L++] = rev[i - 1u];
            }
            pad(width > L ? width - L : 0u, ' ');
            for (u32 i = 0u; i < L; ++i) put(seq[i]);
            continue;
        }

        bool negv = false;
        u64 mag = 0u;
        if (conv == 'd') {
            if (llflag) { const long long x = va_arg(ap, long long); negv = x < 0; mag = 0ull - static_cast<u64>(x); if (!negv) mag = static_cast<u64>(x); }
            else { const int x = va_arg(ap, int); negv = x < 0; mag = 0ull - static_cast<u64>(x); if (!negv) mag = static_cast<u64>(x); }
        } else if (conv == 'u') {
            mag = llflag ? va_arg(ap, unsigned long long) : va_arg(ap, unsigned int);
        } else {
            mag = llflag ? va_arg(ap, unsigned long long) : va_arg(ap, unsigned int);
        }
        const u32 base = conv == 'x' ? 16u : 10u;
        char buf[40];
        u32 d = 0u;
        do { buf[d++] = khexd[mag % base]; mag /= base; } while (mag != 0u);
        const u32 total = d + (negv ? 1u : 0u);
        const u32 padn = width > total ? width - total : 0u;
        if (!zero) pad(padn, ' ');
        if (negv) put('-');
        if (zero) pad(padn, '0');
        while (d != 0u) put(buf[--d]);
    }

    if (n != 0u) out[len < n ? len : n - 1u] = '\0';
    return len;
}

u32 rt_fmt(char* out, u32 n, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const u32 r = rt_vfmt(out, n, fmt, ap);
    va_end(ap);
    return r;
}
