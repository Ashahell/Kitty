#pragma once
#include <cstdint>
#include <cstddef>

// platform.h — one interface, two implementations (platform/aros, platform/linux)
using u8 = uint8_t; using u16 = uint16_t; using u32 = uint32_t; using u64 = uint64_t;
using i16 = int16_t;
using Alloc = void*(*)(size_t);     // AllocVec-backed on AROS, malloc-backed on Linux
struct Buf;                         // file read buffer; defined in §3.2
struct Plat;                        // opaque, initialized explicitly in main
struct PlatEvent { u8 kind; u8 a; u16 b; u32 tick; };
struct EventSink { PlatEvent ev[64]; u32 head, tail; };
struct PlatEvents { EventSink* sink; };
struct PlatArgs { const char* renderer_override; const char* save_root; };
enum ErrCode : u16 { OK=0, E_IO, E_VK, E_SCHEMA, E_SIM_OVF, E_SIM_DIV0,
                     E_SNAP_UNDERFLOW, E_AUDIO, E_TIER_FALLBACK };
void pl_init(Plat&, const PlatArgs&);
void pl_step(Plat&, u64& due_us);
u32  pl_poll_events(Plat&, EventSink*);
bool pl_read_file(Plat&, const char* path, Buf* out);
bool pl_write_file(Plat&, const char* path, const void* data, size_t n);
void pl_env_get(Plat&, const char* key, char* v, size_t n);
void pl_log(Plat&, const char* line);
void pl_shutdown(Plat&, int code);
