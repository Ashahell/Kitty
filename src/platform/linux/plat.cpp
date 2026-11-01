#define VK_NO_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "platform.h"
#include "modules.h"
#include "wsi.h"
#include "plat_priv.h"
#include "plat_svc.h"
#include "rt_fmt.h"

static u64 mono_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (u64)ts.tv_sec * 1000000ull + (u64)(ts.tv_nsec / 1000);
}

Alloc plat_alloc_fn(Plat& p) { return p.alloc; }
u64 plat_micros(Plat& p) { (void)p; return mono_us(); }
u32 plat_lp_threads(Plat& p) { return (u32)p.lp_threads; }

void pl_init(Plat& p, const PlatArgs& args) {
    (void)args;
    p.alloc = 0;
    p.alloc = [](size_t n) -> void* { return malloc(n ? n : 1); };
    p.t0_us = mono_us();
    p.next_us = p.t0_us;
    p.ticks = 0;
    p.dropped = 0;
    p.windowed = false;
    p.win = 0;
    p.win_w = 320;
    p.win_h = 256;
    p.adev = 0;
    p.audio = false;
    p.arate = 48000;
    p.queued_frames = 0;
    p.quit = false;
    const char* v = getenv("LP_NUM_THREADS");
    if (!v || !v[0]) {
        setenv("LP_NUM_THREADS", "0", 1);
        p.lp_threads = 0;
        p.lp_src = "engine-default";
    } else {
        p.lp_threads = atoi(v);
        p.lp_src = "env";
    }
    char line[128];
    rt_fmt(line, sizeof line, "lp_threads=%d lp_threads_source=%s", p.lp_threads, p.lp_src);
    pl_log(p, line);
}

void pl_step(Plat& p, u64& due_us) {
    u64 now = mono_us();
    if (now < p.next_us) { due_us = 0; return; }
    u64 n = (now - p.next_us) / 20000ull + 1ull;
    p.next_us += n * 20000ull;
    due_us = n;
}

void pl_log(Plat& p, const char* line) {
    (void)p;
    fputs(line, stderr);
    fputs("\n", stderr);
}

void plat_log_raw(const char* line) {
    fputs(line, stderr);
    fputs("\n", stderr);
}

void plat_free(void* p) { free(p); }

// Exit watchdog for the validation-only planted-presentation path: lavapipe's
// software WSI blocks forever on an unsignalled semaphore, so a helper thread
// ends the process with the required code instead of letting a test hang. On
// expiry it reports the debug messenger's validation count and exits 3 when it
// is non-zero, 4 otherwise.
static volatile sig_atomic_t g_wd_stop = 0;
static u32 g_wd_ms = 0;
static const volatile u32* g_wd_validation = 0;
static volatile sig_atomic_t g_wd_started = 0;

static void* watchdog_main(void* arg) {
    (void)arg;
    u32 left = g_wd_ms;
    while (left > 0) {
        if (g_wd_stop) return 0;
        u32 chunk = left > 10u ? 10u : left;
        plat_sleep_us(chunk * 1000u);
        left -= chunk;
    }
    if (!g_wd_stop) {
        u32 vcnt = g_wd_validation ? (u32)*g_wd_validation : 0u;
        char line[256];
        rt_fmt(line, sizeof line, "validation_messages=%u", vcnt);
        plat_log_raw(line);
        rt_fmt(line, sizeof line, "Software Failure. %s:%u: present did not return within %u ms",
               __FILE__, (u32)__LINE__, g_wd_ms);
        plat_log_raw(line);
        _exit(vcnt > 0 ? 3 : 4);
    }
    return 0;
}

void plat_exit_watchdog_ms(Plat& p, u32 ms, const volatile u32* validation_count) {
    (void)p;
    g_wd_ms = ms;
    g_wd_validation = validation_count;
    if (g_wd_started) return;
    g_wd_started = 1;
    pthread_t t;
    if (pthread_create(&t, 0, watchdog_main, 0) == 0) pthread_detach(t);
}

void plat_exit_watchdog_cancel(void) {
    g_wd_stop = 1;
}

void plat_sleep_us(u32 us) {
    struct timespec ts;
    ts.tv_sec = (time_t)(us / 1000000u);
    ts.tv_nsec = (long)(us % 1000000u) * 1000L;
    nanosleep(&ts, 0);
}

u64 pl_now_us(Plat& p) { (void)p; return mono_us(); }

void pl_cpu_model(Plat& p, char* out, size_t n) {
    (void)p;
    if (!n) return;
    out[0] = 0;
    FILE* f = fopen("/proc/cpuinfo", "r");
    if (!f) return;
    char line[256];
    while (fgets(line, sizeof line, f)) {
        if (strncmp(line, "model name", 10) == 0) {
            char* c = strchr(line, ':');
            if (c) {
                c++;
                while (*c == ' ' || *c == '\t') c++;
                size_t k = strlen(c);
                while (k && (c[k-1] == '\n' || c[k-1] == '\r')) c[--k] = 0;
                size_t i = 0;
                for (; c[i] && i + 1 < n; i++) out[i] = c[i];
                out[i] = 0;
                break;
            }
        }
    }
    fclose(f);
}

u32 pl_poll_events(Plat& p, EventSink* sink) {
    u32 n = 0;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        u8 kind = 0, a = 0;
        u16 b = 0;
        if (e.type == SDL_QUIT) { p.quit = true; kind = 1; }
        else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            kind = 1; a = (u8)e.key.keysym.scancode; b = (u16)(e.type == SDL_KEYDOWN);
        } else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
            kind = 2; a = (u8)e.button.button; b = (u16)(e.type == SDL_MOUSEBUTTONDOWN);
        }
        if (kind && sink) {
            u32 next = (sink->head + 1u) % 64u;
            if (next != sink->tail) {
                sink->ev[sink->head].kind = kind;
                sink->ev[sink->head].a = a;
                sink->ev[sink->head].b = b;
                sink->ev[sink->head].tick = 0;
                sink->head = next;
                n++;
            }
        }
    }
    return n;
}

bool pl_read_file(Plat& p, const char* path, Buf* out) {
    if (!path || !out) return false;
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return false; }
    rewind(f);
    u8* d = (u8*)p.alloc((size_t)sz ? (size_t)sz : 1u);
    if (!d) { fclose(f); return false; }
    size_t got = fread(d, 1, (size_t)sz, f);
    fclose(f);
    out->data = d;
    out->n = got;
    out->cap = got;
    return got == (size_t)sz;
}

bool plat_read_path(const char* path, u8** data, size_t* n, Alloc a) {
    if (!path || !a) return false;
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return false; }
    rewind(f);
    u8* d = (u8*)a((size_t)sz ? (size_t)sz : 1u);
    if (!d) { fclose(f); return false; }
    size_t got = fread(d, 1, (size_t)sz, f);
    fclose(f);
    if (got != (size_t)sz) { plat_free(d); return false; }
    *data = d;
    *n = got;
    return true;
}

bool pl_write_file(Plat& p, const char* path, const void* data, size_t n) {
    (void)p;
    if (!path) return false;
    FILE* f = fopen(path, "wb");
    if (!f) return false;
    size_t w = fwrite(data, 1, n, f);
    fclose(f);
    return w == n;
}

void pl_env_get(Plat& p, const char* key, char* v, size_t n) {
    (void)p;
    if (!key || !n) return;
    char k[128];
    size_t i = 0;
    for (; key[i] && i + 1 < sizeof k; i++) {
        char c = key[i];
        if (c == '/') c = '_';
        if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
        k[i] = c;
    }
    k[i] = 0;
    const char* e = getenv(k);
    if (!e) { v[0] = 0; return; }
    size_t j = 0;
    for (; e[j] && j + 1 < n; j++) v[j] = e[j];
    v[j] = 0;
}

u32 pl_wsi_instance_extensions(Plat& p, const char** names, u32 max) {
    if (!p.win) {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) return 0;
        SDL_Window* w = SDL_CreateWindow("Kitty: Core Awakening",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 768,
            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
        if (!w) return 0;
        p.win = w;
        p.windowed = true;
    }
    unsigned cnt = 0;
    const char* tmp[16];
    unsigned got = 16;
    if (!SDL_Vulkan_GetInstanceExtensions((SDL_Window*)p.win, &got, tmp)) return 0;
    if (names && max) {
        u32 n = 0;
        for (unsigned i = 0; i < got && n < max; i++) names[n++] = tmp[i];
        return n;
    }
    cnt = got;
    return cnt;
}

bool pl_wsi_create_surface(Plat& p, VkInstance inst, VkSurfaceKHR* out) {
    if (!p.win || !out) return false;
    return SDL_Vulkan_CreateSurface((SDL_Window*)p.win, inst, out) == SDL_TRUE;
}

void pl_wsi_extent(Plat& p, u32* w, u32* h) {
    int dw = 320, dh = 256;
    if (p.win) SDL_Vulkan_GetDrawableSize((SDL_Window*)p.win, &dw, &dh);
    if (dw < 1) dw = 320;
    if (dh < 1) dh = 256;
    p.win_w = (u32)dw;
    p.win_h = (u32)dh;
    if (w) *w = p.win_w;
    if (h) *h = p.win_h;
}

bool pl_audio_open(Plat& p, u32 rate, u16 channels) {
    if (p.audio) return true;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) return false;
    SDL_AudioSpec want;
    SDL_AudioSpec have;
    SDL_memset(&want, 0, sizeof want);
    want.freq = (int)rate;
    want.format = AUDIO_S16SYS;
    want.channels = (Uint8)channels;
    want.samples = 960;
    want.callback = 0;
    SDL_AudioDeviceID id = SDL_OpenAudioDevice(0, 0, &want, &have, 0);
    if (!id) return false;
    SDL_PauseAudioDevice(id, 0);
    p.adev = (u32)id;
    p.audio = true;
    p.arate = rate;
    return true;
}

void plat_audio_close(Plat& p) {
    if (!p.audio) return;
    SDL_CloseAudioDevice((SDL_AudioDeviceID)p.adev);
    p.audio = false;
    p.adev = 0;
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

const char* plat_audio_driver(Plat& p) {
    (void)p;
    const char* d = SDL_GetCurrentAudioDriver();
    return d ? d : "none";
}

u32 pl_audio_queued_frames(Plat& p) {
    if (!p.audio) return 0;
    return (u32)(SDL_GetQueuedAudioSize((SDL_AudioDeviceID)p.adev) / 4u);
}

void pl_audio_queue(Plat& p, const i16* frames, u32 n) {
    if (!p.audio) return;
    SDL_QueueAudio((SDL_AudioDeviceID)p.adev, frames, n * 4u);
    p.queued_frames += n;
}

u64 plat_audio_total_frames(Plat& p) { return p.queued_frames; }

void pl_trap(const char* msg) {
    char buf[512];
    rt_fmt(buf, sizeof buf, "Software Failure. src/platform/rt/traps.cpp:1: %s", msg);
    fputs(buf, stderr);
    fputs("\n", stderr);
    _exit(4);
}

void pl_shutdown(Plat& p, int code) {
    plat_audio_close(p);
    if (p.win) { SDL_DestroyWindow((SDL_Window*)p.win); p.win = 0; }
    SDL_Quit();
    exit(code);
}
