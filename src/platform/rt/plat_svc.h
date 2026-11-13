#pragma once
#define VK_NO_PROTOTYPES 1
#include <vulkan/vulkan.h>
#include "platform.h"

u64 pl_now_us(Plat&);
bool pl_audio_open(Plat&, u32 rate, u16 channels);
u32 pl_audio_queued_frames(Plat&);
void pl_audio_queue(Plat&, const i16* frames, u32 n);
void pl_cpu_model(Plat&, char* out, size_t n);
[[noreturn]] void pl_trap(const char* file, u32 line, const char* msg);
PFN_vkGetInstanceProcAddr pl_vk_gipa(Plat&);
void pl_wsi_destroy_surface(Plat&, VkInstance, VkSurfaceKHR);

#define PL_TRAP(msg) pl_trap(__FILE__, __LINE__, (msg))
