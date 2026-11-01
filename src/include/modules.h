#pragma once
#include <cstddef>
#include "platform.h"
#include "render_types.h"

struct Buf { u8* data; size_t n; size_t cap; };
struct Audio;
void aud_open(Audio&, u32 rate, u16 channels);
void aud_fill(Audio&, i16* buf, size_t frames);
void mod_load(Audio&, const u8* mod, size_t n);
void mod_mute(Audio&, u8 channel, bool muted);
void mod_seek(Audio&, u32 position);
struct GuruCtx;
void guru_report(GuruCtx&, const char* file, u32 line, const char* msg);
struct Bench;
void bench_lite(Bench&, RendCtx&, u32 frames);
