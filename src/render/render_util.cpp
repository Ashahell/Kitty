#include <cstring>
#include "render_util.h"

const char* rv_result_name(VkResult r) {
    switch (r) {
    case VK_SUCCESS: return "VK_SUCCESS";
    case VK_NOT_READY: return "VK_NOT_READY";
    case VK_TIMEOUT: return "VK_TIMEOUT";
    case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
    default: return "VK_ERROR_OTHER";
    }
}

void rv_chk(RendCtx& r, const char* call, VkResult res) {
    VkClass c = vk_classify(res);
    if (c == VKC_NORMAL) return;
    RV_LOGF("vkresult=%s call=%s class=%s", rv_result_name(res), call, vk_class_name(c));
    if (c == VKC_RETRY) { r.skip_frame = true; return; }
    if (c == VKC_RECREATE_SWAPCHAIN || c == VKC_RECREATE_SURFACE) { r.skip_frame = true; return; }
    rv_die("src/render/render_util.cpp", __LINE__, "fatal Vulkan error");
}

u32 rv_mem_type(RendCtx& r, u32 bits, VkMemoryPropertyFlags want) {
    for (u32 i = 0; i < r.memprops.memoryTypeCount; i++) {
        if ((bits & (1u << i)) && (r.memprops.memoryTypes[i].propertyFlags & want) == want) return i;
    }
    rv_die("src/render/render_util.cpp", __LINE__, "no matching memory type");
    return 0;
}

void rv_buffer(RendCtx& r, VkDeviceSize n, VkBufferUsageFlags use,
               VkBuffer* buf, VkDeviceMemory* mem, VkMemoryPropertyFlags props) {
    VkBufferCreateInfo bi;
    memset(&bi, 0, sizeof bi);
    bi.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bi.size = n;
    bi.usage = use;
    bi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    rv_chk(r, "vkCreateBuffer", r.api.CreateBuffer(r.dev, &bi, 0, buf));
    VkMemoryRequirements req;
    memset(&req, 0, sizeof req);
    r.api.GetBufferMemoryRequirements(r.dev, *buf, &req);
    VkMemoryAllocateInfo ai;
    memset(&ai, 0, sizeof ai);
    ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    ai.allocationSize = req.size;
    ai.memoryTypeIndex = rv_mem_type(r, req.memoryTypeBits, props);
    rv_chk(r, "vkAllocateMemory", r.api.AllocateMemory(r.dev, &ai, 0, mem));
    rv_chk(r, "vkBindBufferMemory", r.api.BindBufferMemory(r.dev, *buf, *mem, 0));
}

void rv_image(RendCtx& r, VkFormat fmt, u32 w, u32 h, VkImageUsageFlags use,
              VkImage* img, VkDeviceMemory* mem, VkMemoryPropertyFlags props) {
    VkImageCreateInfo ci;
    memset(&ci, 0, sizeof ci);
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ci.imageType = VK_IMAGE_TYPE_2D;
    ci.format = fmt;
    ci.extent.width = w;
    ci.extent.height = h;
    ci.extent.depth = 1;
    ci.mipLevels = 1;
    ci.arrayLayers = 1;
    ci.samples = VK_SAMPLE_COUNT_1_BIT;
    ci.tiling = VK_IMAGE_TILING_OPTIMAL;
    ci.usage = use;
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    rv_chk(r, "vkCreateImage", r.api.CreateImage(r.dev, &ci, 0, img));
    VkMemoryRequirements req;
    memset(&req, 0, sizeof req);
    r.api.GetImageMemoryRequirements(r.dev, *img, &req);
    VkMemoryAllocateInfo ai;
    memset(&ai, 0, sizeof ai);
    ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    ai.allocationSize = req.size;
    ai.memoryTypeIndex = rv_mem_type(r, req.memoryTypeBits, props);
    rv_chk(r, "vkAllocateMemory", r.api.AllocateMemory(r.dev, &ai, 0, mem));
    rv_chk(r, "vkBindImageMemory", r.api.BindImageMemory(r.dev, *img, *mem, 0));
}

void rv_destroy_buffer(RendCtx& r, VkBuffer& b, VkDeviceMemory& m) {
    if (b) { r.api.DestroyBuffer(r.dev, b, 0); b = 0; }
    if (m) { r.api.FreeMemory(r.dev, m, 0); m = 0; }
}

void rv_destroy_image(RendCtx& r, VkImage& i, VkDeviceMemory& m) {
    if (i) { r.api.DestroyImage(r.dev, i, 0); i = 0; }
    if (m) { r.api.FreeMemory(r.dev, m, 0); m = 0; }
}

void rv_barrier(RendCtx& r, VkCommandBuffer cb, VkImage img, VkImageLayout from, VkImageLayout to,
                VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage,
                VkAccessFlags src_acc, VkAccessFlags dst_acc) {
    VkImageMemoryBarrier b;
    memset(&b, 0, sizeof b);
    b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    b.srcAccessMask = src_acc;
    b.dstAccessMask = dst_acc;
    b.oldLayout = from;
    b.newLayout = to;
    b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    b.image = img;
    b.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    b.subresourceRange.baseMipLevel = 0;
    b.subresourceRange.levelCount = 1;
    b.subresourceRange.baseArrayLayer = 0;
    b.subresourceRange.layerCount = 1;
    r.api.CmdPipelineBarrier(cb, src_stage, dst_stage, 0, 0, 0, 0, 0, 1, &b);
}

void rv_one_shot(RendCtx& r, void (*fn)(RendCtx&, VkCommandBuffer, void*), void* user) {
    VkCommandBufferAllocateInfo ai;
    memset(&ai, 0, sizeof ai);
    ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    ai.commandPool = r.cpool;
    ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    ai.commandBufferCount = 1;
    VkCommandBuffer cb = 0;
    rv_chk(r, "vkAllocateCommandBuffers", r.api.AllocateCommandBuffers(r.dev, &ai, &cb));
    VkCommandBufferBeginInfo bi;
    memset(&bi, 0, sizeof bi);
    bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    rv_chk(r, "vkBeginCommandBuffer", r.api.BeginCommandBuffer(cb, &bi));
    fn(r, cb, user);
    rv_chk(r, "vkEndCommandBuffer", r.api.EndCommandBuffer(cb));
    VkFenceCreateInfo fi;
    memset(&fi, 0, sizeof fi);
    fi.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence = 0;
    rv_chk(r, "vkCreateFence", r.api.CreateFence(r.dev, &fi, 0, &fence));
    VkSubmitInfo si;
    memset(&si, 0, sizeof si);
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &cb;
    rv_chk(r, "vkQueueSubmit", r.api.QueueSubmit(r.queue, 1, &si, fence));
    rv_chk(r, "vkWaitForFences", r.api.WaitForFences(r.dev, 1, &fence, VK_TRUE, 1000000000ull));
    r.api.DestroyFence(r.dev, fence, 0);
    r.api.FreeCommandBuffers(r.dev, r.cpool, 1, &cb);
}

void rv_build_vp(float* m) {
    const float c = 0.8944272f;
    const float s = 0.4472136f;
    const float hx = 12.5f;
    const float hy = 10.0f;
    const float cx = 8.0f;
    const float cy = 5.0f;
    const float cz = 1.0f;
    const float a = c / hx;
    const float b = s / hy;
    const float tx = -c * (cx - cy) / hx;
    const float ty = -(s * (cx + cy) - cz) / hy;
    const float tz = 32.0f / 34.0f;
    const float d = 1.0f / 34.0f;
    for (u32 i = 0; i < 16; i++) m[i] = 0.0f;
    m[0] = a;  m[1] = b;   m[2] = -d;  m[3] = 0.0f;
    m[4] = -a; m[5] = b;   m[6] = -d;  m[7] = 0.0f;
    m[8] = 0.0f; m[9] = -1.0f / hy; m[10] = -2.0f * d; m[11] = 0.0f;
    m[12] = tx; m[13] = ty; m[14] = tz; m[15] = 1.0f;
}

void rv_palette(float pal[16][4]) {
    static const u8 src[16][4] = {
        {  36,  40,  56, 255 },
        {  88,  96, 120, 255 },
        { 148, 156, 172, 255 },
        {  68,  74,  92, 255 },
        { 232, 232, 224, 255 },
        { 208, 120, 136, 255 },
        { 104, 168, 128, 255 },
        { 236, 196,  96, 255 },
        {  56,  96, 148, 255 },
        { 172, 120,  72, 255 },
        { 120, 200, 208, 255 },
        { 196, 148, 200, 255 },
        {  24,  28,  40, 255 },
        { 252, 252, 252, 255 },
        { 144,  40,  56, 255 },
        {  40,  72,  48, 255 }
    };
    for (u32 i = 0; i < 16; i++) {
        for (u32 c = 0; c < 4; c++) pal[i][c] = (float)src[i][c] / 255.0f;
    }
}
