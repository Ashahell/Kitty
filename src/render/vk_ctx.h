#pragma once
#include "render_types.h"

extern "C" VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance, const char*);

enum VkClass {
    VKC_NORMAL = 0,
    VKC_RECREATE_SWAPCHAIN,
    VKC_RETRY,
    VKC_FATAL_FALLBACK,
    VKC_RECREATE_SURFACE,
    VKC_FATAL_EXIT
};

VkClass vk_classify(VkResult);
const char* vk_class_name(VkClass);

#define KITTY_VK_INSTANCE \
    X(CreateInstance) X(DestroyInstance) \
    X(EnumerateInstanceExtensionProperties) \
    X(CreateDebugUtilsMessengerEXT) X(DestroyDebugUtilsMessengerEXT) \
    X(EnumeratePhysicalDevices) X(GetPhysicalDeviceProperties) X(GetPhysicalDeviceProperties2) X(GetPhysicalDeviceFeatures2) \
    X(GetPhysicalDeviceQueueFamilyProperties) X(GetPhysicalDeviceMemoryProperties) \
    X(GetPhysicalDeviceSurfaceSupportKHR) X(GetPhysicalDeviceSurfaceCapabilitiesKHR) \
    X(GetPhysicalDeviceSurfaceFormatsKHR) X(GetPhysicalDeviceSurfacePresentModesKHR) \
    X(CreateDevice) X(GetDeviceProcAddr) X(GetDeviceQueue) \
    X(DestroySurfaceKHR)

#define KITTY_VK_DEVICE \
    X(DestroyDevice) X(DeviceWaitIdle) \
    X(CreateSwapchainKHR) X(DestroySwapchainKHR) X(GetSwapchainImagesKHR) \
    X(AcquireNextImageKHR) X(QueuePresentKHR) \
    X(CreateImage) X(DestroyImage) X(GetImageMemoryRequirements) X(BindImageMemory) \
    X(CreateBuffer) X(DestroyBuffer) X(GetBufferMemoryRequirements) X(BindBufferMemory) \
    X(AllocateMemory) X(FreeMemory) X(MapMemory) X(UnmapMemory) \
    X(CreateImageView) X(DestroyImageView) \
    X(CreateRenderPass) X(DestroyRenderPass) \
    X(CreateFramebuffer) X(DestroyFramebuffer) \
    X(CreateShaderModule) X(DestroyShaderModule) \
    X(CreatePipelineLayout) X(DestroyPipelineLayout) \
    X(CreateGraphicsPipelines) X(DestroyPipeline) \
    X(CreateDescriptorSetLayout) X(DestroyDescriptorSetLayout) \
    X(CreateDescriptorPool) X(DestroyDescriptorPool) \
    X(AllocateDescriptorSets) X(UpdateDescriptorSets) \
    X(CreateSampler) X(DestroySampler) \
    X(CreateCommandPool) X(DestroyCommandPool) \
    X(AllocateCommandBuffers) X(ResetCommandBuffer) X(FreeCommandBuffers) \
    X(BeginCommandBuffer) X(EndCommandBuffer) \
    X(CmdBeginRenderPass) X(CmdEndRenderPass) \
    X(CmdBindPipeline) X(CmdBindVertexBuffers) X(CmdBindIndexBuffer) \
    X(CmdBindDescriptorSets) X(CmdPushConstants) X(CmdDrawIndexed) \
    X(CmdPipelineBarrier) X(CmdCopyImageToBuffer) X(CmdCopyBufferToImage) X(CmdCopyImage) \
    X(CmdBlitImage) X(CmdClearColorImage) \
    X(CreateSemaphore) X(DestroySemaphore) X(CreateFence) X(DestroyFence) \
    X(WaitForFences) X(ResetFences) X(QueueSubmit) X(QueueWaitIdle)

struct VkApi {
#define X(name) PFN_vk##name name;
    KITTY_VK_INSTANCE
    KITTY_VK_DEVICE
#undef X
};

void vk_api_load_instance(VkApi&, VkInstance);
void vk_api_load_device(VkApi&, VkDevice);
