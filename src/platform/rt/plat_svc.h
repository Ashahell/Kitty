#pragma once
#include "platform.h"

u64  pl_now_us(Plat&);
bool pl_audio_open(Plat&, u32 rate, u16 channels);
u32  pl_audio_queued_frames(Plat&);
void pl_audio_queue(Plat&, const i16* frames, u32 n);
void pl_cpu_model(Plat&, char* out, size_t n);
[[noreturn]] void pl_trap(const char* msg);
