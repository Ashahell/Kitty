#include <cstring>
#include "platform.h"
#include "modules.h"
#include "plat_svc.h"
#include "audio_internal.h"

void aud_open(Audio& a, u32 rate, u16 channels) {
    a.rate = rate ? rate : 48000u;
    a.channels = channels ? channels : 2u;
    a.mp.rate = a.rate;
    a.opened = a.p ? pl_audio_open(*a.p, a.rate, a.channels) : false;
}

void aud_fill(Audio& a, i16* buf, size_t frames) {
    if (a.mp.loaded) {
        mod_render(a.mp, buf, (u32)frames);
    } else {
        memset(buf, 0, frames * 2u * sizeof(i16));
    }
}

void mod_mute(Audio& a, u8 channel, bool muted) {
    if (channel < 4u) a.mp.ch[channel].muted = muted;
}

void mod_seek(Audio& a, u32 position) {
    mod_reset(a.mp);
    mod_advance(a.mp, position);
}

void aud_tick(Audio& a) {
    if (!a.opened || !a.mp.loaded || !a.p) return;
    if (pl_audio_queued_frames(*a.p) >= 3840u) return;
    i16 buf[960 * 2];
    mod_render(a.mp, buf, 960);
    pl_audio_queue(*a.p, buf, 960u);
}
