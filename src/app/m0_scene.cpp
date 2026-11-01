#include <cstring>
#include "platform.h"
#include "render_internal.h"
#include "m0_scene.h"

static const u8 kFace[6][4] = {
    { 0, 1, 3, 2 },
    { 2, 3, 1, 0 },
    { 0, 4, 6, 2 },
    { 1, 3, 7, 5 },
    { 0, 1, 5, 4 },
    { 3, 2, 6, 7 }
};
static const u8 kShade[6] = { 235, 110, 165, 195, 215, 150 };

static void vtx(RScene& s, float px, float py, float pz, float qx, float qy, float qz,
                u8 pal, u8 shade, float u, float v) {
    if (s.nv >= s.capv) return;
    RVert& w = s.verts[s.nv++];
    w.p[0] = px; w.p[1] = py; w.p[2] = pz;
    w.q[0] = qx; w.q[1] = qy; w.q[2] = qz;
    w.c[0] = pal; w.c[1] = shade; w.c[2] = 0; w.c[3] = 0;
    w.uv[0] = u; w.uv[1] = v;
}

static void idx3(RScene& s, u16 a, u16 b, u16 c) {
    if (s.ni + 3 > s.capi) return;
    s.idx[s.ni++] = a;
    s.idx[s.ni++] = b;
    s.idx[s.ni++] = c;
}

static void corner(const float* a, u32 i, float* out) {
    out[0] = (i & 1u) ? a[3] : a[0];
    out[1] = (i & 2u) ? a[4] : a[1];
    out[2] = (i & 4u) ? a[5] : a[2];
}

static void box(RScene& s, const float* a, const float* b, u8 pal) {
    for (u32 f = 0; f < 6; f++) {
        u16 base = (u16)s.nv;
        for (u32 k = 0; k < 4; k++) {
            float pa[3], pb[3];
            corner(a, kFace[f][k], pa);
            corner(b, kFace[f][k], pb);
            vtx(s, pa[0], pa[1], pa[2], pb[0], pb[1], pb[2], pal, kShade[f], 0.0f, 0.0f);
        }
        idx3(s, base, (u16)(base + 1), (u16)(base + 2));
        idx3(s, base, (u16)(base + 2), (u16)(base + 3));
    }
}

static void quad_tex(RScene& s, float x0, float x1, float y, float z0, float z1, u8 pal) {
    u16 base = (u16)s.nv;
    vtx(s, x0, y, z0, x0, y, z0, pal, 255, 0.0f, 0.0f);
    vtx(s, x1, y, z0, x1, y, z0, pal, 255, 1.0f, 0.0f);
    vtx(s, x1, y, z1, x1, y, z1, pal, 255, 1.0f, 1.0f);
    vtx(s, x0, y, z1, x0, y, z1, pal, 255, 0.0f, 1.0f);
    idx3(s, base, (u16)(base + 1), (u16)(base + 2));
    idx3(s, base, (u16)(base + 2), (u16)(base + 3));
}

static void quad_ui(RScene& s, float x0, float x1, float y0, float y1, u8 pal) {
    u16 base = (u16)s.nv;
    vtx(s, x0, y0, 0.0f, x0, y0, 0.0f, pal, 255, 0.0f, 0.0f);
    vtx(s, x1, y0, 0.0f, x1, y0, 0.0f, pal, 255, 0.0f, 0.0f);
    vtx(s, x1, y1, 0.0f, x1, y1, 0.0f, pal, 255, 0.0f, 0.0f);
    vtx(s, x0, y1, 0.0f, x0, y1, 0.0f, pal, 255, 0.0f, 0.0f);
    idx3(s, base, (u16)(base + 1), (u16)(base + 2));
    idx3(s, base, (u16)(base + 2), (u16)(base + 3));
}

static void cat(RScene& s, float cx, float cy, u8 pal, int variant) {
    float body_a[6] = { cx - 0.35f, cy - 0.25f, 0.30f, cx + 0.35f, cy + 0.25f, 0.90f };
    float head_a[6] = { cx - 0.25f, cy - 0.20f, 0.90f, cx + 0.25f, cy + 0.20f, 1.30f };
    float ear_a[6] = { cx - 0.20f, cy - 0.12f, 1.30f, cx - 0.06f, cy + 0.02f, 1.52f };
    float ear_b[6] = { cx + 0.06f, cy - 0.12f, 1.30f, cx + 0.20f, cy + 0.02f, 1.52f };
    float tail_a[6] = { cx + 0.30f, cy + 0.10f, 0.55f, cx + 0.62f, cy + 0.30f, 0.80f };
    float leg0[6] = { cx - 0.30f, cy - 0.20f, 0.00f, cx - 0.10f, cy - 0.02f, 0.32f };
    float leg1[6] = { cx + 0.10f, cy - 0.20f, 0.00f, cx + 0.30f, cy - 0.02f, 0.32f };
    float leg2[6] = { cx - 0.30f, cy + 0.02f, 0.00f, cx - 0.10f, cy + 0.20f, 0.32f };
    float leg3[6] = { cx + 0.10f, cy + 0.02f, 0.00f, cx + 0.30f, cy + 0.20f, 0.32f };

    float body_b[6], head_b[6], ear_ab[6], ear_bb[6], tail_b[6];
    memcpy(body_b, body_a, sizeof body_b);
    memcpy(head_b, head_a, sizeof head_b);
    memcpy(ear_ab, ear_a, sizeof ear_ab);
    memcpy(ear_bb, ear_b, sizeof ear_bb);
    memcpy(tail_b, tail_a, sizeof tail_b);
    body_b[5] += 0.10f;
    head_b[2] += 0.10f;
    head_b[5] += 0.14f;
    head_b[0] -= 0.05f;
    head_b[3] -= 0.05f;
    ear_ab[2] += 0.24f;
    ear_ab[5] += 0.22f;
    ear_bb[2] += 0.24f;
    ear_bb[5] += 0.22f;
    tail_b[2] += 0.26f;
    tail_b[4] += 0.24f;
    tail_b[5] += 0.30f;

    if (variant) {
        leg0[4] += 0.10f;
        leg1[4] -= 0.10f;
        leg2[4] += 0.08f;
        leg3[4] -= 0.08f;
    }

    box(s, body_a, body_b, pal);
    box(s, head_a, head_b, pal);
    box(s, ear_a, ear_ab, (u8)(pal + 1));
    box(s, ear_b, ear_bb, (u8)(pal + 1));
    box(s, tail_a, tail_b, (u8)(pal + 1));
    box(s, leg0, leg0, pal);
    box(s, leg1, leg1, pal);
    box(s, leg2, leg2, pal);
    box(s, leg3, leg3, pal);
}

static void draw_rec(RScene& s, u8 pipe, u32 key, u32 first, u32 count, float w, float alpha) {
    if (s.ndraw >= 64) return;
    RDraw& d = s.draws[s.ndraw++];
    d.first = first;
    d.count = count;
    d.key = key;
    d.pipe = pipe;
    d.pad[0] = d.pad[1] = d.pad[2] = 0;
    d.w = w;
    d.alpha = alpha;
}

void m0_scene_build(Alloc a, RScene* s) {
    memset(s, 0, sizeof *s);
    s->capv = 20000;
    s->capi = 65536;
    s->verts = (RVert*)a((size_t)s->capv * sizeof(RVert));
    s->idx = (u16*)a((size_t)s->capi * sizeof(u16));
    if (!s->verts || !s->idx) {
        s->capv = 0;
        s->capi = 0;
        return;
    }

    u32 first_i = 0;
    for (u32 iy = 0; iy < 10; iy++) {
        for (u32 ix = 0; ix < 16; ix++) {
            float b[6] = { ix + 0.02f, iy + 0.02f, 0.00f, ix + 0.98f, iy + 0.98f, 0.25f };
            box(*s, b, b, 1);
        }
    }
    for (u32 ix = 0; ix < 16; ix++) {
        float lo[6] = { (float)ix, 9.55f, 0.25f, (float)ix + 1.0f, 9.95f, 1.50f };
        float hi[6] = { (float)ix, 9.55f, 1.50f, (float)ix + 1.0f, 9.95f, 2.75f };
        box(*s, lo, lo, 2);
        box(*s, hi, hi, 3);
    }
    for (u32 iy = 0; iy < 10; iy++) {
        float l[6] = { 0.05f, (float)iy, 0.25f, 0.45f, (float)iy + 1.0f, 2.75f };
        box(*s, l, l, 2);
    }
    for (u32 iy = 0; iy < 10; iy++) {
        float rr[6] = { 15.55f, (float)iy, 0.25f, 15.95f, (float)iy + 1.0f, 2.75f };
        box(*s, rr, rr, 3);
    }
    for (u32 i = 0; i < 4; i++) {
        float fw[6] = { (float)(i * 4), 0.05f, 0.25f, (float)(i * 4 + 4), 0.45f, 2.75f };
        box(*s, fw, fw, 2);
    }
    u32 lcg = 20261030u;
    for (u32 i = 0; i < 48; i++) {
        lcg = lcg * 1664525u + 1013904223u;
        float bx = 1.4f + (float)((lcg >> 8) % 1100u) / 100.0f;
        lcg = lcg * 1664525u + 1013904223u;
        float by = 1.4f + (float)((lcg >> 8) % 700u) / 100.0f;
        float sz = 0.35f + (float)((lcg >> 20) % 40u) / 100.0f;
        float blk[6] = { bx, by, 0.25f, bx + sz, by + sz, 0.25f + sz };
        u8 pal = (u8)(3 + (i % 3) * 3);
        box(*s, blk, blk, pal);
    }
    draw_rec(*s, PIPE_FLAT, 0u, first_i, s->ni - first_i, 0.0f, 1.0f);

    static const float catPos[6][2] = {
        { 3.0f, 3.0f }, { 6.0f, 5.0f }, { 9.0f, 4.0f },
        { 12.0f, 6.0f }, { 5.0f, 7.0f }, { 10.0f, 2.5f }
    };
    static const u8 catPal[6] = { 4, 4, 4, 4, 4, 5 };
    for (u32 i = 0; i < 6; i++) {
        u32 first = s->ni;
        cat(*s, catPos[i][0], catPos[i][1], catPal[i], (int)(i & 1u));
        u32 key = (u32)((catPos[i][0] + catPos[i][1]) * 100.0f);
        draw_rec(*s, i == 5 ? PIPE_DITHER : PIPE_FLAT, key, first, s->ni - first, 0.0f, i == 5 ? 0.5f : 1.0f);
        s->draws[s->ndraw - 1].pad[0] = (u8)(i + 1u);
    }

    u32 ftx = s->ni;
    quad_tex(*s, 6.0f, 9.0f, 9.45f, 1.0f, 2.2f, 6);
    draw_rec(*s, PIPE_TEX, 1450u, ftx, s->ni - ftx, 0.0f, 1.0f);

    u32 fpr = s->ni;
    float prop[6] = { 13.0f, 2.0f, 1.0f, 14.0f, 3.0f, 2.0f };
    box(*s, prop, prop, 9);
    draw_rec(*s, PIPE_NOCULL, 400u, fpr, s->ni - fpr, 0.0f, 1.0f);

    u32 fui = s->ni;
    quad_ui(*s, -0.98f, -0.30f, -0.98f, -0.86f, 7);
    draw_rec(*s, PIPE_UI, 0xFFFFFFFFu, fui, s->ni - fui, 0.0f, 1.0f);

    s->dl.items = s->ndraw;
}

void m0_scene_animate(RScene* s, u32 tick) {
    for (u32 i = 0; i < s->ndraw; i++) {
        u8 c = s->draws[i].pad[0];
        if (c >= 1u && c <= 6u) {
            u32 w = (tick + (u32)(c - 1u) * 13u) % 48u;
            s->draws[i].w = (float)w / 48.0f;
        }
    }
    u32 lo = 1;
    u32 hi = s->ndraw > 2 ? s->ndraw - 2 : 1;
    for (u32 i = lo + 1; i <= hi && i < s->ndraw; i++) {
        RDraw tmp = s->draws[i];
        u32 j = i;
        while (j > lo && s->draws[j - 1].key > tmp.key) {
            s->draws[j] = s->draws[j - 1];
            j--;
        }
        s->draws[j] = tmp;
    }
}

u32 m0_room_tris(void) { return 3168u; }
u32 m0_kitty_tris(void) { return 648u; }
