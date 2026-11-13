#include <cstdio>
#include <cstring>
#include <string>
#include "vk_ctx.h"

int main() {
    struct Case { VkResult r; const char* name; const char* cls; };
    const Case cases[8] = {
        { VK_SUCCESS, "VK_SUCCESS", "normal" },
        { VK_SUBOPTIMAL_KHR, "VK_SUBOPTIMAL_KHR", "normal" },
        { VK_ERROR_OUT_OF_DATE_KHR, "VK_ERROR_OUT_OF_DATE_KHR", "recreate-swapchain" },
        { VK_TIMEOUT, "VK_TIMEOUT", "retry" },
        { VK_NOT_READY, "VK_NOT_READY", "retry" },
        { VK_ERROR_DEVICE_LOST, "VK_ERROR_DEVICE_LOST", "fatal-fallback" },
        { VK_ERROR_SURFACE_LOST_KHR, "VK_ERROR_SURFACE_LOST_KHR", "recreate-surface" },
        { VK_ERROR_OUT_OF_HOST_MEMORY, "VK_ERROR_OUT_OF_HOST_MEMORY", "fatal-exit" }
    };
    int bad = 0;
    for (int i = 0; i < 8; i++) {
        const char* got = vk_class_name(vk_classify(cases[i].r));
        std::printf("VKR %s %s\n", cases[i].name, got);
        if (std::string(got) != cases[i].cls) bad = 1;
    }
    VkApi api;
    std::memset(&api, 1, sizeof api);
    if (vk_api_missing_device(api, true)) bad = 1;
    api.CreateSwapchainKHR = 0;
    if (vk_api_missing_device(api, false)) bad = 1;
    const char* m = vk_api_missing_device(api, true);
    if (!m || std::string(m) != "vkCreateSwapchainKHR") bad = 1;
    api.QueueSubmit = 0;
    m = vk_api_missing_device(api, false);
    if (!m || std::string(m) != "vkQueueSubmit") bad = 1;
    std::printf(bad ? "FAIL t-vkresult\n" : "PASS t-vkresult\n");
    return bad;
}
