#pragma once
#include "render_internal.h"

void rv_chk(RendCtx&, const char* call, VkResult);
u32 rv_mem_type(RendCtx&, u32 bits, VkMemoryPropertyFlags);
void rv_buffer(RendCtx&, VkDeviceSize, VkBufferUsageFlags, VkBuffer*, VkDeviceMemory*, VkMemoryPropertyFlags);
void rv_image(RendCtx&, VkFormat, u32 w, u32 h, VkImageUsageFlags, VkImage*, VkDeviceMemory*, VkMemoryPropertyFlags);
void rv_destroy_buffer(RendCtx&, VkBuffer&, VkDeviceMemory&);
void rv_destroy_image(RendCtx&, VkImage&, VkDeviceMemory&);
void rv_build_vp(float*);
void rv_barrier(RendCtx&, VkCommandBuffer, VkImage, VkImageLayout, VkImageLayout,
                VkPipelineStageFlags, VkPipelineStageFlags, VkAccessFlags, VkAccessFlags);
void rv_one_shot(RendCtx&, void (*)(RendCtx&, VkCommandBuffer, void*), void*);
const char* rv_result_name(VkResult);
void rv_palette(float pal[16][4]);
