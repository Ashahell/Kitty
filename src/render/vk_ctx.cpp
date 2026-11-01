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

void vk_api_load_instance(VkApi& a, VkInstance inst) {
    PFN_vkGetInstanceProcAddr gp = vkGetInstanceProcAddr;
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
