#pragma once
#include "platform.h"
#include "modules.h"

struct GuruCtx;

using i8 = int8_t;
using i32 = int32_t;

struct ModSample {
    i16* data;
    u32 len;
    u32 loop_start;
    u32 loop_len;
    u32 volume;
    bool loop;
};

struct ModCh {
    u32 period;
    u32 step;
    u32 pos;
    u32 volume;
    u8 sample;
    bool active;
    bool muted;
};

struct ModState {
    const u8* pat;
    u32 npat;
    u32 rate;
    u8 order[128];
    u8 song_len;
    u8 restart;
    ModSample smp[31];
    u32 speed;
    u32 tempo;
    u32 acc;
    u32 order_i;
    u32 row;
    u32 tick_in_row;
    u32 tick_left;
    u32 tick;
    ModCh ch[4];
    bool pos_jump;
    bool row_break;
    u32 jump_order;
    u32 break_row;
    u32 effects_ignored;
    bool loaded;
};

struct Audio {
    u32 rate;
    u16 channels;
    Alloc alloc;
    Plat* p;
    GuruCtx* g;
    ModState mp;
    bool opened;
};

void mod_reset(ModState&);
void mod_apply_effect(ModState&, u8 ch, u8 eff, u8 param);
u32  mod_tick_frames(ModState&);
u32  mod_step_at(ModState&, u32 tick);
void mod_advance(ModState&, u32 ticks);
void mod_render(ModState&, i16* out, u32 frames);
void aud_tick(Audio&);
