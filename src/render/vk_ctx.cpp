#include "vk_ctx.h"

VkClass vk_classify(VkResult r) {
    switch (r) {
    case VK_SUCCESS: return VKC_NORMAL;
    case VK_SUBOPTIMAL_KHR: return VKC_NORMAL;
    case VK_ERROR_OUT_OF_DATE_KHR: return VKC_RECREATE_SWAPCHAIN;
    case VK_TIMEOUT: return VKC_RETRY;
    case VK_NOT_READY: return VKC_RETRY;
    case VK_ERROR_DEVICE_LOST: return VKC_FATAL_FALLBACK;
    case VK_ERROR_SURFACE_LOST_KHR: return VKC_RECREATE_SURFACE;
    default: return VKC_FATAL_EXIT;
    }
}

const char* vk_class_name(VkClass c) {
    switch (c) {
    case VKC_NORMAL: return "normal";
    case VKC_RECREATE_SWAPCHAIN: return "recreate-swapchain";
    case VKC_RETRY: return "retry";
    case VKC_FATAL_FALLBACK: return "fatal-fallback";
    case VKC_RECREATE_SURFACE: return "recreate-surface";
    default: return "fatal-exit";
    }
}

void vk_api_load_instance(VkApi& a, VkInstance inst, PFN_vkGetInstanceProcAddr gp) {
#define X(name) a.name = (PFN_vk##name)gp(inst, "vk" #name);
    KITTY_VK_INSTANCE
#undef X
}

void vk_api_load_device(VkApi& a, VkDevice dev) {
    PFN_vkGetDeviceProcAddr gd = a.GetDeviceProcAddr;
#define X(name) a.name = (PFN_vk##name)gd(dev, "vk" #name);
    KITTY_VK_DEVICE
#undef X
}

static bool is_khr(const char* name, unsigned long n) {
    return n > 3 && name[n - 3] == 'K' && name[n - 2] == 'H' && name[n - 1] == 'R';
}

/* The first device entry the driver did not give us, or 0.  The KHR entries
 * exist only on a device with VK_KHR_swapchain, so headless skips them. */
const char* vk_api_missing_device(const VkApi& a, bool wsi) {
#define X(name) if (!a.name && (wsi || !is_khr("vk" #name, sizeof("vk" #name) - 1))) return "vk" #name;
    KITTY_VK_DEVICE
#undef X
    return 0;
}
