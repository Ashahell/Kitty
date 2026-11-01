#include <cstring>
#include "platform.h"
#include "modules.h"
#include "rt_fmt.h"
#include "audio_internal.h"

static u16 be16(const u8* p) { return (u16)((p[0] << 8) | p[1]); }

static u32 calc_step(u32 period, u32 rate) {
    if (!period || !rate) return 0;
    u64 a = (u64)3546895u << 16;
    return (u32)(a / ((u64)period * (u64)rate));
}

static void reject(Audio& a, const char* why) {
    a.mp.loaded = false;
    if (a.g) guru_report(*a.g, "src/audio/modplay.cpp", __LINE__, why);
}

void mod_reset(ModState& m) {
    bool mu[4];
    for (u32 i = 0; i < 4; i++) mu[i] = m.ch[i].muted;
    m.speed = 6;
    m.tempo = 125;
    m.acc = 0;
    m.order_i = 0;
    m.row = 0;
    m.tick_in_row = 0;
    m.tick_left = 0;
    m.tick = 0;
    m.pos_jump = false;
    m.row_break = false;
    m.jump_order = 0;
    m.break_row = 0;
    for (u32 i = 0; i < 4; i++) {
        m.ch[i].period = 0;
        m.ch[i].step = 0;
        m.ch[i].pos = 0;
        m.ch[i].volume = 0;
        m.ch[i].sample = 0;
        m.ch[i].active = false;
        m.ch[i].muted = mu[i];
    }
}

void mod_apply_effect(ModState& m, u8 ch, u8 eff, u8 param) {
    switch (eff) {
    case 0xF:
        if (param == 0) break;
        if (param < 0x20) m.speed = param;
        else m.tempo = param;
        break;
    case 0xC:
        m.ch[ch & 3u].volume = param > 64u ? 64u : param;
        break;
    case 0xB:
        m.pos_jump = true;
        m.jump_order = param;
        break;
    case 0xD: {
        u32 r = (u32)(param >> 4) * 10u + (u32)(param & 15u);
        if (r > 63u) r = 0;
        m.row_break = true;
        m.break_row = r;
        break;
    }
    default:
        break;
    }
}

u32 mod_tick_frames(ModState& m) {
    m.acc += 120000u;
    u32 f = m.acc / (m.tempo ? m.tempo : 125u);
    m.acc %= (m.tempo ? m.tempo : 125u);
    return f;
}

static void process_row(ModState& m) {
    const u8* base = m.pat + (u32)m.order[m.order_i] * 1024u + m.row * 16u;
    for (u32 ch = 0; ch < 4; ch++) {
        u8 b0 = base[ch * 4 + 0], b1 = base[ch * 4 + 1];
        u8 b2 = base[ch * 4 + 2], b3 = base[ch * 4 + 3];
        u32 period = ((u32)(b0 & 0x0Fu) << 8) | b1;
        u8 smp = (u8)((b0 & 0xF0u) | (b2 >> 4));
        u8 eff = (u8)(b2 & 0x0Fu);
        if (period) {
            m.ch[ch].period = period;
            m.ch[ch].step = calc_step(period, m.rate);
            m.ch[ch].pos = 0;
            m.ch[ch].active = m.ch[ch].step != 0;
        }
        if (smp > 0 && smp <= 31) {
            m.ch[ch].sample = (u8)(smp - 1u);
            if (m.smp[smp - 1].volume) m.ch[ch].volume = m.smp[smp - 1].volume;
        }
        mod_apply_effect(m, (u8)ch, eff, b3);
    }
}

static void advance_row(ModState& m) {
    if (m.pos_jump) {
        m.order_i = m.jump_order;
        m.row = 0;
        m.pos_jump = false;
        m.row_break = false;
    } else if (m.row_break) {
        m.order_i++;
        m.row = m.break_row;
        m.row_break = false;
    } else {
        m.row++;
        if (m.row >= 64u) { m.row = 0; m.order_i++; }
    }
    if (m.order_i >= m.song_len) {
        m.order_i = m.restart < m.song_len ? m.restart : 0u;
        m.row = 0;
    }
}

static i16 clip16(i32 v) {
    if (v > 32767) return 32767;
    if (v < -32768) return -32768;
    return (i16)v;
}

static void mix(ModState& m, i16* out, u32 n) {
    for (u32 f = 0; f < n; f++) {
        i32 l = 0, r = 0;
        for (u32 c = 0; c < 4; c++) {
            ModCh& ch = m.ch[c];
            if (!ch.active || ch.muted || !ch.step) continue;
            const ModSample& s = m.smp[ch.sample];
            u32 idx = ch.pos >> 16;
            if (s.loop && s.loop_len && idx >= s.loop_start + s.loop_len) {
                u32 edge = s.loop_start + s.loop_len;
                u32 over = idx - edge;
                ch.pos -= ((over / s.loop_len) + 1u) * (s.loop_len << 16);
                idx = ch.pos >> 16;
            }
            if (idx >= s.len) { ch.active = false; continue; }
            i32 v = ((i32)s.data[idx] * (i32)ch.volume) / 64;
            if (c == 0 || c == 3) l += v; else r += v;
            ch.pos += ch.step;
        }
        out[f * 2 + 0] = clip16(l);
        out[f * 2 + 1] = clip16(r);
    }
}

void mod_render(ModState& m, i16* out, u32 frames) {
    u32 done = 0;
    while (done < frames) {
        if (m.tick_left == 0) {
            if (m.tick_in_row == 0) process_row(m);
            m.tick_left = mod_tick_frames(m);
            if (!m.tick_left) m.tick_left = 1;
        }
        u32 n = frames - done;
        if (n > m.tick_left) n = m.tick_left;
        mix(m, out + done * 2, n);
        done += n;
        m.tick_left -= n;
        if (!m.tick_left) {
            m.tick++;
            m.tick_in_row++;
            if (m.tick_in_row >= m.speed) {
                m.tick_in_row = 0;
                advance_row(m);
            }
        }
    }
}

u32 mod_step_at(ModState& src, u32 tick) {
    ModState m = src;
    mod_reset(m);
    for (u32 i = 0; i <= tick; i++) {
        if (m.tick_in_row == 0) process_row(m);
        if (i == tick) return m.ch[0].step;
        m.tick_in_row++;
        if (m.tick_in_row >= m.speed) { m.tick_in_row = 0; advance_row(m); }
    }
    return m.ch[0].step;
}

void mod_advance(ModState& m, u32 ticks) {
    for (u32 i = 0; i < ticks; i++) {
        if (m.tick_in_row == 0) process_row(m);
        m.tick++;
        m.tick_in_row++;
        if (m.tick_in_row >= m.speed) { m.tick_in_row = 0; advance_row(m); }
    }
}

void mod_load(Audio& a, const u8* mod, size_t n) {
    ModState& m = a.mp;
    u32 rate = m.rate ? m.rate : a.rate;
    bool mu[4];
    for (u32 i = 0; i < 4; i++) mu[i] = m.ch[i].muted;
    memset(&m, 0, sizeof m);
    m.rate = rate ? rate : 48000u;
    for (u32 i = 0; i < 4; i++) m.ch[i].muted = mu[i];
    m.loaded = false;
    if (!mod || n < 1084u) { reject(a, "module shorter than header"); return; }
    u8 sl = mod[950];
    if (sl == 0) { reject(a, "song length 0"); return; }
    if (sl > 128) { reject(a, "song length above 128"); return; }
    if (memcmp(mod + 1080, "M.K.", 4) != 0) { reject(a, "not a 4-channel M.K. module"); return; }
    u32 maxp = 0;
    for (u32 i = 0; i < sl; i++) {
        m.order[i] = mod[952 + i];
        if (m.order[i] > maxp) maxp = m.order[i];
    }
    m.song_len = sl;
    m.restart = mod[951];
    if (m.restart >= m.song_len) m.restart = 0;
    u32 npat = maxp + 1u;
    u32 pat_bytes = npat * 1024u;
    if (1084u + pat_bytes > n) { reject(a, "pattern data shorter than the order table needs"); return; }
    u32 off = 1084u + pat_bytes;
    for (u32 i = 0; i < 31; i++) {
        const u8* s = mod + 20u + i * 30u;
        u32 words = be16(s + 22);
        u32 bytes = words * 2u;
        u32 lstart = be16(s + 26) * 2u;
        u32 llen = be16(s + 28) * 2u;
        if (off + bytes > n) { reject(a, "sample data past the end of the file"); return; }
        ModSample& d = m.smp[i];
        d.len = bytes;
        d.volume = s[25] > 64u ? 64u : s[25];
        d.loop_start = lstart;
        d.loop_len = llen;
        d.loop = (llen > 2u && lstart + llen <= bytes);
        d.data = 0;
        if (bytes) {
            d.data = (i16*)a.alloc((size_t)bytes * sizeof(i16));
            if (!d.data) { reject(a, "sample allocation failed"); return; }
            for (u32 j = 0; j < bytes; j++) d.data[j] = (i16)((i16)(i8)mod[off + j] << 8);
        }
        off += bytes;
    }
    m.pat = mod + 1084;
    m.npat = npat;
    m.tick = 0;
    m.effects_ignored = 0;
    for (u32 p = 0; p < npat; p++) {
        for (u32 r = 0; r < 64; r++) {
            for (u32 c = 0; c < 4; c++) {
                const u8* cell = m.pat + p * 1024u + r * 16u + c * 4u;
                u8 eff = (u8)(cell[2] & 0x0Fu);
                u8 par = cell[3];
                if (eff == 0) { if (par) m.effects_ignored++; }
                else if (eff != 0xF && eff != 0xC && eff != 0xB && eff != 0xD) m.effects_ignored++;
            }
        }
    }
    if (a.p) {
        char line[64];
        rt_fmt(line, sizeof line, "effects_ignored=%u", m.effects_ignored);
        pl_log(*a.p, line);
    }
    m.loaded = true;
    mod_reset(m);
}
