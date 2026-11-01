#pragma once
#include <cstddef>
#include "platform.h"
#include "render_types.h"

using i8 = int8_t;
using i32 = int32_t;
using i64 = int64_t;

struct Plat {
    Alloc alloc;
    u64 t0_us;
    u64 next_us;
    u64 ticks;
    u64 dropped;
    int lp_threads;
    const char* lp_src;
    bool windowed;
    void* win;
    u32 win_w;
    u32 win_h;
    u32 adev;
    bool audio;
    u32 arate;
    u64 queued_frames;
    bool quit;
};

Alloc plat_alloc_fn(Plat&);
u64 plat_micros(Plat&);
u32 plat_lp_threads(Plat&);
void plat_audio_close(Plat&);
const char* plat_audio_driver(Plat&);
u64 plat_audio_total_frames(Plat&);
void plat_sleep_us(u32 us);
void plat_exit_watchdog_ms(Plat&, u32 ms, const volatile u32* validation_count);
void plat_exit_watchdog_cancel(void);
void plat_log_raw(const char* line);
void plat_die(const char* file, u32 line, const char* msg);
bool plat_read_path(const char* path, u8** data, size_t* n, Alloc a);
void plat_free(void* p);

struct PaceOut { u32 stepped; u32 dropped; };
inline PaceOut pace_step(u32 due) {
    PaceOut o;
    o.stepped = due > 4u ? 4u : due;
    o.dropped = due - o.stepped;
    return o;
}

