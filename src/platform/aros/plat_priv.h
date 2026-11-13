#pragma once
#include <cstddef>
#include "platform.h"
#include "render_types.h"
#include "pace.h"   // D23: shared pacing step (both platforms + t-pacing)

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
    void* surf;          // the platform surface handle (D25), opaque here
    u64 events_drained;  // pl_poll_events calls (guest exit stats)
    void* vk_lib;        // the loader library handle (D24; opened once, never closed)
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
void plat_exit_stats(Plat&);      // AROS: elapsed_us= / events_drained= keys
void plat_soak_sample(Plat&);     // AROS: mem_free_bytes= key
u32 plat_lp_worker_tasks(Plat&);  // AROS: lp_worker_tasks= key after vkCreateDevice
bool pl_present_dump(Plat&, u32 frame, const char* out_dir);
