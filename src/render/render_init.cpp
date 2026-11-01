#include <cstring>
#include "render_internal.h"
#include "render_util.h"
#include "tier.h"
#include "selftest.h"
#include "wsi.h"
#include "modules.h"

static char g_spv[512] = "build/spv";
static Plat* g_plat = 0;

Plat* r_plat(void) { return g_plat; }
void r_set_plat(Plat& p) { g_plat = &p; }

[[noreturn]] void rv_die(const char* file, u32 line, const char* msg) {
    if (!g_plat) pl_trap(msg);
    char buf[512];
    rt_fmt(buf, sizeof buf, "Software Failure. %s:%u: %s", file, line, msg);
    pl_log(*g_plat, buf);
    pl_shutdown(*g_plat, 4);
    pl_trap("rv_die returned");
}

static u64 now_us(void) {
    if (!g_plat) rv_die("src/render/render_init.cpp", __LINE__, "render plat not bound");
    return pl_now_us(*g_plat);
}

void r_set_spv_dir(const char* d) {
    if (!d || !d[0]) return;
    rt_fmt(g_spv, sizeof g_spv, "%s", d);
}

static VkShaderModule load_shader(RendCtx& r, const char* name) {
    char path[600];
    rt_fmt(path, sizeof path, "%s/%s.spv", g_spv, name);
    if (!g_plat) rv_die("src/render/render_init.cpp", __LINE__, "render plat not bound");
    Buf buf;
    if (!pl_read_file(*g_plat, path, &buf) || buf.n < 4) {
        rv_die("src/render/render_init.cpp", __LINE__, "shader module missing or truncated");
    }
    if ((buf.n & 3u) != 0) rv_die("src/render/render_init.cpp", __LINE__, "shader module size not a multiple of 4");
    VkShaderModuleCreateInfo ci;
    memset(&ci, 0, sizeof ci);
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = buf.n;
    ci.pCode = (const u32*)buf.data;
    VkShaderModule m = 0;
    rv_chk(r, "vkCreateShaderModule", r.api.CreateShaderModule(r.dev, &ci, 0, &m));
    return m;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL dbg_cb(VkDebugUtilsMessageSeverityFlagBitsEXT sev,
                                             VkDebugUtilsMessageTypeFlagsEXT type,
                                             const VkDebugUtilsMessengerCallbackDataEXT* data,
                                             void* user) {
    (void)sev;
    (void)type;
    RendCtx* r = (RendCtx*)user;
    if (r) __atomic_fetch_add(&r->dbg_count, 1u, __ATOMIC_RELAXED);
    if (data && data->pMessage && g_plat) pl_log(*g_plat, data->pMessage);
    return VK_FALSE;
}

#ifdef KITTY_DEBUG
static bool ext_available(VkApi& api, const char* name) {
    u32 n = 0;
    if (!api.EnumerateInstanceExtensionProperties) return false;
    if (api.EnumerateInstanceExtensionProperties(0, &n, 0) != VK_SUCCESS) return false;
    VkExtensionProperties list[64];
    if (n > 64) n = 64;
    if (api.EnumerateInstanceExtensionProperties(0, &n, list) != VK_SUCCESS) return false;
    for (u32 i = 0; i < n; i++) {
        if (strcmp(list[i].extensionName, name) == 0) return true;
    }
    return false;
}
#endif

static void dev_props(VkApi& api, VkPhysicalDevice dev, VkPhysicalDeviceProperties* out, u32* driver) {
    VkPhysicalDeviceDriverProperties dp;
    memset(&dp, 0, sizeof dp);
    dp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
    VkPhysicalDeviceProperties2 p2;
    memset(&p2, 0, sizeof p2);
    p2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    p2.pNext = &dp;
    api.GetPhysicalDeviceProperties2(dev, &p2);
    *out = p2.properties;
    if (driver) *driver = dp.driverID;
}

static RendCtx* ctx_alloc(Alloc a) {
    void* mem = a(sizeof(RendCtx));
    if (!mem) return 0;
    RendCtx* r = (RendCtx*)mem;
    memset(r, 0, sizeof(RendCtx));
    r->alloc = a;
    rv_build_vp(r->vp);
    rv_palette(r->pal);
    r->img_view = 0;
    return r;
}

static void find_queue(RendCtx& r, bool present) {
    u32 n = 0;
    r.api.GetPhysicalDeviceQueueFamilyProperties(r.phys, &n, 0);
    if (n > 32) n = 32;
    VkQueueFamilyProperties fam[32];
    r.api.GetPhysicalDeviceQueueFamilyProperties(r.phys, &n, fam);
    for (u32 i = 0; i < n; i++) {
        if (!(fam[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) continue;
        if (present) {
            VkBool32 sup = VK_FALSE;
            rv_chk(r, "vkGetPhysicalDeviceSurfaceSupportKHR",
                   r.api.GetPhysicalDeviceSurfaceSupportKHR(r.phys, i, r.surf, &sup));
            if (!sup) continue;
        }
        r.qfam = i;
        return;
    }
    rv_die("src/render/render_init.cpp", __LINE__, "no usable queue family");
}

static void create_swapchain(RendCtx& r) {
    if (!r.windowed || !r.surf) return;
    rv_chk(r, "vkDeviceWaitIdle", r.api.DeviceWaitIdle(r.dev));
    for (u32 i = 0; i < r.sc_n; i++) {
        if (r.render_sem[i]) r.api.DestroySemaphore(r.dev, r.render_sem[i], 0);
        r.render_sem[i] = 0;
        r.sc_img[i] = 0;
    }
    if (r.sc) { r.api.DestroySwapchainKHR(r.dev, r.sc, 0); r.sc = 0; }
    r.sc_n = 0;

    VkSurfaceCapabilitiesKHR caps;
    memset(&caps, 0, sizeof caps);
    rv_chk(r, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR",
           r.api.GetPhysicalDeviceSurfaceCapabilitiesKHR(r.phys, r.surf, &caps));
    u32 nf = 0;
    rv_chk(r, "vkGetPhysicalDeviceSurfaceFormatsKHR",
           r.api.GetPhysicalDeviceSurfaceFormatsKHR(r.phys, r.surf, &nf, 0));
    if (nf == 0) rv_die("src/render/render_init.cpp", __LINE__, "surface offers no formats");
    if (nf > 256) nf = 256;
    VkSurfaceFormatKHR fmt[256];
    {
        VkResult fr = r.api.GetPhysicalDeviceSurfaceFormatsKHR(r.phys, r.surf, &nf, fmt);
        if (fr != VK_SUCCESS && fr != VK_INCOMPLETE) rv_chk(r, "vkGetPhysicalDeviceSurfaceFormatsKHR", fr);
    }
    VkSurfaceFormatKHR pick = fmt[0];
    for (u32 i = 0; i < nf; i++) {
        if (fmt[i].format == VK_FORMAT_B8G8R8A8_UNORM) { pick = fmt[i]; break; }
    }
    u32 nm = 0;
    rv_chk(r, "vkGetPhysicalDeviceSurfacePresentModesKHR",
           r.api.GetPhysicalDeviceSurfacePresentModesKHR(r.phys, r.surf, &nm, 0));
    if (nm > 32) nm = 32;
    VkPresentModeKHR modes[32];
    VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;
    if (nm) {
        VkResult mr = r.api.GetPhysicalDeviceSurfacePresentModesKHR(r.phys, r.surf, &nm, modes);
        if (mr != VK_SUCCESS && mr != VK_INCOMPLETE) rv_chk(r, "vkGetPhysicalDeviceSurfacePresentModesKHR", mr);
        for (u32 i = 0; i < nm; i++) if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) mode = modes[i];
    }
    VkCompositeAlphaFlagBitsKHR alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if (!(caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)) {
        if (caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
            alpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        } else if (caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
            alpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
        } else {
            alpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
        }
    }
    VkExtent2D ext = caps.currentExtent;
    if (ext.width == 0xFFFFFFFFu || ext.height == 0xFFFFFFFFu) {
        u32 w = 320, h = 256;
        if (r.plat) pl_wsi_extent(*r.plat, &w, &h);
        ext.width = w;
        ext.height = h;
    }
    if (ext.width == 0) ext.width = 320;
    if (ext.height == 0) ext.height = 256;
    u32 count = caps.minImageCount > 2u ? caps.minImageCount : 2u;
    if (caps.maxImageCount > 0 && count > caps.maxImageCount) count = caps.maxImageCount;
    VkSwapchainCreateInfoKHR ci;
    memset(&ci, 0, sizeof ci);
    ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    ci.surface = r.surf;
    ci.minImageCount = count;
    ci.imageFormat = pick.format;
    ci.imageColorSpace = pick.colorSpace;
    ci.imageExtent = ext;
    ci.imageArrayLayers = 1;
    ci.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ci.preTransform = caps.currentTransform;
    ci.compositeAlpha = alpha;
    ci.presentMode = mode;
    ci.clipped = VK_TRUE;
    ci.oldSwapchain = 0;
    rv_chk(r, "vkCreateSwapchainKHR", r.api.CreateSwapchainKHR(r.dev, &ci, 0, &r.sc));
    r.sc_fmt = pick.format;
    r.sc_ext = ext;
    r.sc_mode = mode;
    u32 got = 0;
    rv_chk(r, "vkGetSwapchainImagesKHR", r.api.GetSwapchainImagesKHR(r.dev, r.sc, &got, 0));
    if (got > 8) got = 8;
    {
        VkResult gr = r.api.GetSwapchainImagesKHR(r.dev, r.sc, &got, r.sc_img);
        if (gr != VK_SUCCESS && gr != VK_INCOMPLETE) rv_chk(r, "vkGetSwapchainImagesKHR", gr);
    }
    r.sc_n = got;
    RV_LOGF("swapchain_min=%u swapchain_max=%u swapchain_request=%u images=%u",
            caps.minImageCount, caps.maxImageCount, count, got);
    VkSemaphoreCreateInfo si;
    memset(&si, 0, sizeof si);
    si.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (u32 i = 0; i < got; i++) {
        rv_chk(r, "vkCreateSemaphore", r.api.CreateSemaphore(r.dev, &si, 0, &r.render_sem[i]));
    }
    RV_LOGF("present_mode=%s swapchain_format=%d swapchain=%ux%u images=%u",
            mode == VK_PRESENT_MODE_IMMEDIATE_KHR ? "IMMEDIATE" : "FIFO",
            (int)pick.format, ext.width, ext.height, got);
}

void r_recreate_swapchain(RendCtx& r) { create_swapchain(r); }

static void create_render_pass(RendCtx& r, u32 w, u32 h) {
    VkAttachmentDescription att[2];
    memset(att, 0, sizeof att);
    att[0].format = VK_FORMAT_B8G8R8A8_UNORM;
    att[0].samples = VK_SAMPLE_COUNT_1_BIT;
    att[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    att[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    att[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    att[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    att[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    att[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    att[1].format = VK_FORMAT_D32_SFLOAT;
    att[1].samples = VK_SAMPLE_COUNT_1_BIT;
    att[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    att[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    att[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    att[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    att[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    att[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference ref[2];
    memset(ref, 0, sizeof ref);
    ref[0].attachment = 0;
    ref[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    ref[1].attachment = 1;
    ref[1].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription sub;
    memset(&sub, 0, sizeof sub);
    sub.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    sub.colorAttachmentCount = 1;
    sub.pColorAttachments = &ref[0];
    sub.pDepthStencilAttachment = &ref[1];

    VkSubpassDependency dep[2];
    memset(dep, 0, sizeof dep);
    dep[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dep[0].dstSubpass = 0;
    dep[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                          VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dep[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                          VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dep[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                           VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dep[1].srcSubpass = 0;
    dep[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dep[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep[1].dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dep[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dep[1].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    VkRenderPassCreateInfo ci;
    memset(&ci, 0, sizeof ci);
    ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    ci.attachmentCount = 2;
    ci.pAttachments = att;
    ci.subpassCount = 1;
    ci.pSubpasses = &sub;
    ci.dependencyCount = 2;
    ci.pDependencies = dep;
    rv_chk(r, "vkCreateRenderPass", r.api.CreateRenderPass(r.dev, &ci, 0, &r.rp));

    VkImageView views[2];
    views[0] = r.img_view;
    views[1] = r.depth_view;
    VkFramebufferCreateInfo fi;
    memset(&fi, 0, sizeof fi);
    fi.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fi.renderPass = r.rp;
    fi.attachmentCount = 2;
    fi.pAttachments = views;
    fi.width = w;
    fi.height = h;
    fi.layers = 1;
    rv_chk(r, "vkCreateFramebuffer", r.api.CreateFramebuffer(r.dev, &fi, 0, &r.fb));
}

struct PipeParts {
    VkPipelineShaderStageCreateInfo st[2];
    VkVertexInputBindingDescription bind;
    VkVertexInputAttributeDescription attr[4];
    VkPipelineVertexInputStateCreateInfo vin;
    VkPipelineInputAssemblyStateCreateInfo ia;
    VkViewport vp;
    VkRect2D sc;
    VkPipelineViewportStateCreateInfo vps;
    VkPipelineRasterizationStateCreateInfo rs;
    VkPipelineMultisampleStateCreateInfo ms;
    VkPipelineDepthStencilStateCreateInfo ds;
    VkPipelineColorBlendAttachmentState ba;
    VkPipelineColorBlendStateCreateInfo cb;
};

static void fill_pipe_ci(RendCtx& r, VkGraphicsPipelineCreateInfo* ci, VkShaderModule fs,
                         VkPrimitiveTopology topo, VkCullModeFlags cull, bool depth,
                         u32 w, u32 h, PipeParts* pp) {
    VkPipelineShaderStageCreateInfo* st = &pp->st[0];
    memset(st, 0, sizeof pp->st);
    st[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    st[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    st[0].module = r.vs;
    st[0].pName = "main";
    st[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    st[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    st[1].module = fs;
    st[1].pName = "main";

    VkVertexInputBindingDescription& bind = pp->bind;
    memset(&bind, 0, sizeof bind);
    bind.binding = 0;
    bind.stride = 36;
    bind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    VkVertexInputAttributeDescription& a0 = pp->attr[0];
    VkVertexInputAttributeDescription& a1 = pp->attr[1];
    VkVertexInputAttributeDescription& a2 = pp->attr[2];
    VkVertexInputAttributeDescription& a3 = pp->attr[3];
    memset(pp->attr, 0, sizeof pp->attr);
    a0.location = 0; a0.binding = 0; a0.format = VK_FORMAT_R32G32B32_SFLOAT; a0.offset = 0;
    a1.location = 1; a1.binding = 0; a1.format = VK_FORMAT_R32G32B32_SFLOAT; a1.offset = 12;
    a2.location = 2; a2.binding = 0; a2.format = VK_FORMAT_R8G8B8A8_UINT; a2.offset = 24;
    a3.location = 3; a3.binding = 0; a3.format = VK_FORMAT_R32G32_SFLOAT; a3.offset = 28;

    VkPipelineVertexInputStateCreateInfo& vin = pp->vin;
    memset(&vin, 0, sizeof vin);
    vin.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vin.vertexBindingDescriptionCount = 1;
    vin.pVertexBindingDescriptions = &bind;
    vin.vertexAttributeDescriptionCount = 4;
    vin.pVertexAttributeDescriptions = pp->attr;

    VkPipelineInputAssemblyStateCreateInfo& ia = pp->ia;
    memset(&ia, 0, sizeof ia);
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology = topo;
    ia.primitiveRestartEnable = VK_FALSE;

    VkViewport& vp = pp->vp;
    memset(&vp, 0, sizeof vp);
    vp.width = (float)w;
    vp.height = (float)h;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
    VkRect2D& sc = pp->sc;
    memset(&sc, 0, sizeof sc);
    sc.extent.width = w;
    sc.extent.height = h;
    VkPipelineViewportStateCreateInfo& vps = pp->vps;
    memset(&vps, 0, sizeof vps);
    vps.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vps.viewportCount = 1;
    vps.pViewports = &vp;
    vps.scissorCount = 1;
    vps.pScissors = &sc;

    VkPipelineRasterizationStateCreateInfo& rs = pp->rs;
    memset(&rs, 0, sizeof rs);
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.depthClampEnable = VK_FALSE;
    rs.rasterizerDiscardEnable = VK_FALSE;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = cull;
    rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.depthBiasEnable = VK_FALSE;
    rs.depthBiasConstantFactor = 0.0f;
    rs.depthBiasClamp = 0.0f;
    rs.depthBiasSlopeFactor = 0.0f;
    rs.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo& ms = pp->ms;
    memset(&ms, 0, sizeof ms);
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    ms.sampleShadingEnable = VK_FALSE;
    ms.minSampleShading = 1.0f;

    VkPipelineDepthStencilStateCreateInfo& ds = pp->ds;
    memset(&ds, 0, sizeof ds);
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable = depth ? VK_TRUE : VK_FALSE;
    ds.depthWriteEnable = depth ? VK_TRUE : VK_FALSE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable = VK_FALSE;
    ds.stencilTestEnable = VK_FALSE;
    ds.minDepthBounds = 0.0f;
    ds.maxDepthBounds = 1.0f;

    VkPipelineColorBlendAttachmentState& ba = pp->ba;
    memset(&ba, 0, sizeof ba);
    ba.blendEnable = VK_FALSE;
    ba.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    ba.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    ba.colorBlendOp = VK_BLEND_OP_ADD;
    ba.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    ba.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ba.alphaBlendOp = VK_BLEND_OP_ADD;
    ba.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    VkPipelineColorBlendStateCreateInfo& cb = pp->cb;
    memset(&cb, 0, sizeof cb);
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.logicOpEnable = VK_FALSE;
    cb.logicOp = VK_LOGIC_OP_COPY;
    cb.attachmentCount = 1;
    cb.pAttachments = &ba;

    ci->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    ci->stageCount = 2;
    ci->pStages = st;
    ci->pVertexInputState = &vin;
    ci->pInputAssemblyState = &ia;
    ci->pViewportState = &vps;
    ci->pRasterizationState = &rs;
    ci->pMultisampleState = &ms;
    ci->pDepthStencilState = &ds;
    ci->pColorBlendState = &cb;
    ci->layout = r.playout;
    ci->renderPass = r.rp;
    ci->subpass = 0;
}

void rv_create_pipelines(RendCtx& r, u32 w, u32 h) {
    VkGraphicsPipelineCreateInfo ci[5];
    memset(ci, 0, sizeof ci);
    PipeParts parts[5];
    u32 fs_idx[5] = {0, 0, 1, 2, 3};
    VkCullModeFlags cull[5] = {VK_CULL_MODE_BACK_BIT, VK_CULL_MODE_NONE,
                               VK_CULL_MODE_BACK_BIT, VK_CULL_MODE_BACK_BIT, VK_CULL_MODE_NONE};
    bool depth[5] = {true, true, true, true, false};
    for (u32 i = 0; i < 5; i++)
        fill_pipe_ci(r, &ci[i], r.fs[fs_idx[i]], VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, cull[i], depth[i], w, h, &parts[i]);
    VkPipeline created[5];
    memset(created, 0, sizeof created);
    rv_chk(r, "vkCreateGraphicsPipelines", r.api.CreateGraphicsPipelines(r.dev, 0, 5, ci, 0, created));
    for (u32 i = 0; i < 5; i++) r.pip[i] = created[i];
}

typedef struct { VkImage img; VkBuffer buf; } ProbeRec;

static u32 count_true10(const VkPhysicalDeviceFeatures* f) {
    const u32* w = (const u32*)f;
    u32 c = 0;
    for (u32 i = 0; i < (u32)(sizeof(VkPhysicalDeviceFeatures) / 4u); i++)
        if (w[i] == VK_TRUE) c++;
    return c;
}

static u32 count_true13(const VkPhysicalDeviceVulkan13Features* f) {
    const u32* p = (const u32*)&f->robustImageAccess;
    const u32* e = (const u32*)((u8*)f + sizeof(VkPhysicalDeviceVulkan13Features));
    u32 c = 0;
    while (p < e) if (*p++ == VK_TRUE) c++;
    return c;
}

static RendCtx* common_init(Alloc a, Tier want, TierPick* pick, Plat* plat) {
    RendCtx* r = ctx_alloc(a);
    if (!r) return 0;
    r->windowed = plat != 0;
    r->plat = plat;

    VkApi api;
    memset(&api, 0, sizeof api);
    vk_api_load_instance(api, 0);
    if (!api.CreateInstance) rv_die("src/render/render_init.cpp", __LINE__, "Vulkan loader has no vkCreateInstance");

    const char* ext[8];
    u32 next = 0;
    if (plat) {
        u32 got = pl_wsi_instance_extensions(*plat, ext, 8);
        if (got == 0 || got > 8) rv_die("src/render/render_init.cpp", __LINE__, "no window surface extensions");
        next = got;
    }
    bool dbg = false;
#ifdef KITTY_DEBUG
    if (ext_available(api, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        bool have = false;
        for (u32 i = 0; i < next; i++) if (strcmp(ext[i], VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) have = true;
        if (!have) ext[next++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        dbg = true;
    }
#endif

    VkApplicationInfo app;
    memset(&app, 0, sizeof app);
    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "Kitty: Core Awakening";
    app.applicationVersion = 1;
    app.pEngineName = "kitty-engine";
    app.engineVersion = 1;
    app.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo ici;
    memset(&ici, 0, sizeof ici);
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ici.pApplicationInfo = &app;
    ici.enabledExtensionCount = next;
    ici.ppEnabledExtensionNames = next ? ext : 0;
    rv_chk(*r, "vkCreateInstance", api.CreateInstance(&ici, 0, &r->inst));
    vk_api_load_instance(r->api, r->inst);

    RV_LOGF("api=1.3 instance_extensions=%u", next);
    for (u32 i = 0; i < next; i++) RV_LOGF("instance_extension=%s", ext[i]);
    r->dbg_on = dbg;
    RV_LOGF("debug_utils=%u", dbg ? 1u : 0u);
    if (dbg) {
        VkDebugUtilsMessengerCreateInfoEXT dci;
        memset(&dci, 0, sizeof dci);
        dci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        dci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        dci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                          VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        dci.pfnUserCallback = dbg_cb;
        dci.pUserData = r;
        rv_chk(*r, "vkCreateDebugUtilsMessengerEXT",
               r->api.CreateDebugUtilsMessengerEXT(r->inst, &dci, 0, &r->dbg));
    }

    u32 np = 0;
    rv_chk(*r, "vkEnumeratePhysicalDevices", r->api.EnumeratePhysicalDevices(r->inst, &np, 0));
    if (np == 0) rv_die("src/render/render_init.cpp", __LINE__, "no Vulkan physical devices");
    if (np > 16) np = 16;
    VkPhysicalDevice phys[16];
    rv_chk(*r, "vkEnumeratePhysicalDevices", r->api.EnumeratePhysicalDevices(r->inst, &np, phys));
    TierDev devs[16];
    VkPhysicalDeviceProperties props[16];
    u32 drivers[16];
    memset(devs, 0, sizeof devs);
    memset(props, 0, sizeof props);
    memset(drivers, 0, sizeof drivers);
    u32 nvk_i = 0xFFFFFFFFu;
    for (u32 i = 0; i < np; i++) {
        dev_props(r->api, phys[i], &props[i], &drivers[i]);
        devs[i].vendor = props[i].vendorID;
        devs[i].driver = drivers[i];
        devs[i].name = props[i].deviceName;
        if (props[i].vendorID == 0x10DEu && drivers[i] == VK_DRIVER_ID_MESA_NVK) nvk_i = i;
    }
    TierQuery q;
    memset(&q, 0, sizeof q);
    q.devs = devs;
    q.n = np;
    char over[32];
    over[0] = 0;
    const char* ov = 0;
    if (g_plat) {
        pl_env_get(*g_plat, "Kitty/Renderer", over, sizeof over);
        ov = over[0] ? over : 0;
        if (ov && strcmp(ov, "lavapipe") != 0 && strcmp(ov, "nvk") != 0) {
            char ign[96];
            rt_fmt(ign, sizeof ign, "tier_override_ignored=\"%s\"", ov);
            pl_log(*g_plat, ign);
            ov = 0;
        }
    }
    q.over = ov;
    if (want == TIER_NVK && !q.over) q.over = "nvk";
    if (nvk_i != 0xFFFFFFFFu && (!q.over || strcmp(q.over, "lavapipe") != 0)) {
        GoldenImg img;
        MatchResult mr;
        memset(&img, 0, sizeof img);
        r_tier_probe(a, &img, &mr);
        q.have_selftest = true;
        q.selftest_ok = mr.ok;
    }
    Tier t = (Tier)-1;
    TierPick tp = tier_choose(q, &t);
    RV_LOGF("tier=%s", tier_pick_name(tp, t));
    if (tp.reason) RV_LOGF("tier_fallback=\"%s\"", tp.reason);
    if ((int)t < 0) rv_die("src/render/render_init.cpp", __LINE__, "no lavapipe device");
    u32 chosen = 0xFFFFFFFFu;
    for (u32 i = 0; i < np; i++) {
        if (t == TIER_NVK) {
            if (props[i].vendorID == 0x10DEu && drivers[i] == VK_DRIVER_ID_MESA_NVK) { chosen = i; break; }
        } else if (drivers[i] == VK_DRIVER_ID_MESA_LLVMPIPE) { chosen = i; break; }
    }
    if (chosen == 0xFFFFFFFFu) rv_die("src/render/render_init.cpp", __LINE__, "selected device is not present");
    r->phys = phys[chosen];
    r->props = props[chosen];
    r->api.GetPhysicalDeviceMemoryProperties(r->phys, &r->memprops);
    {
        VkPhysicalDeviceDriverProperties dp;
        memset(&dp, 0, sizeof dp);
        dp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
        VkPhysicalDeviceProperties2 p2;
        memset(&p2, 0, sizeof p2);
        p2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        p2.pNext = &dp;
        r->api.GetPhysicalDeviceProperties2(r->phys, &p2);
        rt_fmt(r->driver_info, sizeof r->driver_info, "%s", dp.driverInfo);
    }
    RV_LOGF("device=\"%s\"", r->props.deviceName);

    if (plat) {
        if (!pl_wsi_create_surface(*plat, r->inst, &r->surf)) {
            rv_die("src/render/render_init.cpp", __LINE__, "window surface creation failed");
        }
    }
    find_queue(*r, plat != 0);

    float prio = 1.0f;
    VkDeviceQueueCreateInfo qci;
    memset(&qci, 0, sizeof qci);
    qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qci.queueFamilyIndex = r->qfam;
    qci.queueCount = 1;
    qci.pQueuePriorities = &prio;
    VkPhysicalDeviceFeatures feats;
    memset(&feats, 0, sizeof feats);
    VkPhysicalDeviceVulkan13Features f13;
    memset(&f13, 0, sizeof f13);
    f13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    VkPhysicalDeviceFeatures2 f2;
    memset(&f2, 0, sizeof f2);
    f2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    f2.pNext = &f13;
    r->api.GetPhysicalDeviceFeatures2(r->phys, &f2);
    if (!f13.shaderDemoteToHelperInvocation)
        rv_die("src/render/render_init.cpp", __LINE__, "shaderDemoteToHelperInvocation unsupported");
    memset(&f13, 0, sizeof f13);
    f13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    f13.shaderDemoteToHelperInvocation = VK_TRUE;
    RV_LOGF("features10_true=%u features13_true=%u", count_true10(&feats), count_true13(&f13));
    VkDeviceCreateInfo dci;
    memset(&dci, 0, sizeof dci);
    dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dci.pNext = &f13;
    dci.queueCreateInfoCount = 1;
    dci.pQueueCreateInfos = &qci;
    dci.pEnabledFeatures = &feats;
    const char* dext[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    if (plat) {
        dci.enabledExtensionCount = 1;
        dci.ppEnabledExtensionNames = dext;
    }
    rv_chk(*r, "vkCreateDevice", r->api.CreateDevice(r->phys, &dci, 0, &r->dev));
    r->api.GetDeviceQueue(r->dev, r->qfam, 0, &r->queue);
    vk_api_load_device(r->api, r->dev);

    if (pick) *pick = tp;
    return r;
}

static void tex_upload_fn(RendCtx& r, VkCommandBuffer cb, void* user) {
    VkBuffer st = (VkBuffer)(size_t)user;
    VkImageSubresourceRange range;
    memset(&range, 0, sizeof range);
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.levelCount = 1;
    range.layerCount = 1;
    rv_barrier(r, cb, r.tex, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
               VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT);
    VkBufferImageCopy region;
    memset(&region, 0, sizeof region);
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent.width = 8;
    region.imageExtent.height = 8;
    region.imageExtent.depth = 1;
    r.api.CmdCopyBufferToImage(cb, st, r.tex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    rv_barrier(r, cb, r.tex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
               VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
               VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
}

static void upload_texture(RendCtx& r) {
    u8 px[8 * 8 * 4];
    for (u32 y = 0; y < 8; y++) {
        for (u32 x = 0; x < 8; x++) {
            u32 i = (y * 8 + x) * 4;
            bool on = ((x ^ y) & 1u) != 0;
            bool frame = (x == 0 || y == 0 || x == 7 || y == 7);
            px[i + 0] = frame ? 24 : (on ? 236 : 104);
            px[i + 1] = frame ? 28 : (on ? 196 : 168);
            px[i + 2] = frame ? 40 : (on ? 96 : 128);
            px[i + 3] = 255;
        }
    }
    VkBuffer st = 0;
    VkDeviceMemory stm = 0;
    rv_buffer(r, 256, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &st, &stm,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    void* map = 0;
    rv_chk(r, "vkMapMemory", r.api.MapMemory(r.dev, stm, 0, VK_WHOLE_SIZE, 0, &map));
    if (map) memcpy(map, px, 256);
    r.api.UnmapMemory(r.dev, stm);
    rv_one_shot(r, tex_upload_fn, (void*)(size_t)st);
    rv_destroy_buffer(r, st, stm);
}

static void write_descriptors(RendCtx& r) {
    VkDescriptorBufferInfo bi;
    memset(&bi, 0, sizeof bi);
    bi.buffer = r.ubuf;
    bi.offset = 0;
    bi.range = 336;
    VkDescriptorImageInfo ii;
    memset(&ii, 0, sizeof ii);
    ii.sampler = r.samp;
    ii.imageView = r.tex_view;
    ii.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    VkWriteDescriptorSet w[2];
    memset(w, 0, sizeof w);
    w[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    w[0].dstSet = r.dset;
    w[0].dstBinding = 0;
    w[0].descriptorCount = 1;
    w[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    w[0].pBufferInfo = &bi;
    w[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    w[1].dstSet = r.dset;
    w[1].dstBinding = 1;
    w[1].descriptorCount = 1;
    w[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    w[1].pImageInfo = &ii;
    r.api.UpdateDescriptorSets(r.dev, 2, w, 0, 0);
}

// ---- D18: the 64x64 tier probe: one scratch scene with the five pipeline
// entry points, drawn once on the reference (lavapipe) device and once on
// the candidate device, compared with image_compare. ----

struct ProbeSceneData {
    RVert verts[18];
    u16 idx[24];
    RDraw draws[5];
    u32 nv;
    u32 ni;
};

static ProbeSceneData g_ps = {};

struct ProbeState {
    GoldenImg cand;
    bool has_cand;
    bool plant_shift;
    const char* name;
    const char* reason;
    const char* req;
};

static ProbeState g_probe = {};

// The probe viewport is identity: probe scene positions are direct clip-space
// coordinates (pixel positions on the 64x64 target), not iso-world coords —
// the main camera's iso-mapping clips z below 0 and drops scene draws.
static void probe_world(float* out, float px, float py) {
    out[0] = 2.0f * px / 64.0f - 1.0f;
    out[1] = 1.0f - 2.0f * py / 64.0f;
    out[2] = 0.5f;
}

static void probe_add_vtx(ProbeSceneData& s, u32 idx, float x, float y, float z,
                          u8 pal, u8 shade, float u, float v) {
    RVert& w = s.verts[idx];
    w.p[0] = x; w.p[1] = y; w.p[2] = z;
    w.q[0] = x; w.q[1] = y; w.q[2] = z;
    w.c[0] = pal; w.c[1] = shade; w.c[2] = 0; w.c[3] = 0;
    w.uv[0] = u; w.uv[1] = v;
}

static void probe_add_draw(ProbeSceneData& s, u32 i, u32 first, u32 count, u32 pipe,
                           float w, float alpha) {
    RDraw& d = s.draws[i];
    d.first = first;
    d.count = count;
    d.key = 0;
    d.pipe = (u8)pipe;
    d.w = w;
    d.alpha = alpha;
}

// Fixed positions for the five probe draws, in one function. The pixel
// placements below are the contract: P_flat and P_flat_nocull triangles,
// P_tex and P_dither quads, one P_ui HUD quad (direct clip-space entry).
static void probe_fill_scene(u32 plant_px) {
    ProbeSceneData& s = g_ps;
    const float sh = (float)plant_px * 0.03125f;
    float w0[3], w1[3], w2[3], w3[3];
    s.nv = 18;
    s.ni = 24;
    // P_flat front-facing triangle, pixels (8,8)-(28,8)-(18,26)
    probe_world(w0, 8.0f, 8.0f);
    probe_world(w1, 28.0f, 8.0f);
    probe_world(w2, 18.0f, 26.0f);
    probe_add_vtx(s, 0, w0[0] + sh, w0[1], w0[2], 5, 165, 0.0f, 0.0f);
    probe_add_vtx(s, 1, w1[0] + sh, w1[1], w1[2], 5, 165, 0.0f, 0.0f);
    probe_add_vtx(s, 2, w2[0] + sh, w2[1], w2[2], 5, 165, 0.0f, 0.0f);
    // P_flat_nocull back-facing triangle, pixels (36,6)-(46,24)-(56,6)
    probe_world(w0, 36.0f, 6.0f);
    probe_world(w1, 46.0f, 24.0f);
    probe_world(w2, 56.0f, 6.0f);
    probe_add_vtx(s, 3, w0[0], w0[1], w0[2], 6, 200, 0.0f, 0.0f);
    probe_add_vtx(s, 4, w1[0], w1[1], w1[2], 6, 200, 0.0f, 0.0f);
    probe_add_vtx(s, 5, w2[0], w2[1], w2[2], 6, 200, 0.0f, 0.0f);
    // P_tex quad, pixels (8,36)-(28,56)
    probe_world(w0, 8.0f, 36.0f);
    probe_world(w1, 28.0f, 36.0f);
    probe_world(w2, 8.0f, 56.0f);
    probe_world(w3, 28.0f, 56.0f);
    probe_add_vtx(s, 6, w0[0], w0[1], w0[2], 4, 255, 0.1f, 0.1f);
    probe_add_vtx(s, 7, w1[0], w1[1], w1[2], 4, 255, 0.9f, 0.1f);
    probe_add_vtx(s, 8, w2[0], w2[1], w2[2], 4, 255, 0.1f, 0.9f);
    probe_add_vtx(s, 9, w3[0], w3[1], w3[2], 4, 255, 0.9f, 0.9f);
    // P_dither quad, pixels (36,36)-(56,56), alpha 0.5
    probe_world(w0, 36.0f, 36.0f);
    probe_world(w1, 56.0f, 36.0f);
    probe_world(w2, 36.0f, 56.0f);
    probe_world(w3, 56.0f, 56.0f);
    probe_add_vtx(s, 10, w0[0], w0[1], w0[2], 13, 255, 0.1f, 0.1f);
    probe_add_vtx(s, 11, w1[0], w1[1], w1[2], 13, 255, 0.9f, 0.1f);
    probe_add_vtx(s, 12, w2[0], w2[1], w2[2], 13, 255, 0.1f, 0.9f);
    probe_add_vtx(s, 13, w3[0], w3[1], w3[2], 13, 255, 0.9f, 0.9f);
    // P_ui HUD quad, direct clip-space coords: pixels (24,24)-(40,40)
    static const float ui[8] = {-0.25f, 0.25f, 0.25f, 0.25f,
                                -0.25f, -0.25f, 0.25f, -0.25f};
    for (u32 i = 0; i < 4; i++)
        probe_add_vtx(s, 14 + i, ui[i * 2 + 0], ui[i * 2 + 1], 0.0f, 7, 255, 0.0f, 0.0f);
    static const u16 qtex[6] = {6, 7, 8, 8, 7, 9};
    static const u16 qdth[6] = {10, 11, 12, 12, 11, 13};
    static const u16 qui[6] = {14, 15, 16, 16, 15, 17};
    s.idx[0] = 0; s.idx[1] = 1; s.idx[2] = 2;
    s.idx[3] = 3; s.idx[4] = 4; s.idx[5] = 5;
    for (u32 i = 0; i < 6; i++) s.idx[6 + i] = qtex[i];
    for (u32 i = 0; i < 6; i++) s.idx[12 + i] = qdth[i];
    for (u32 i = 0; i < 6; i++) s.idx[18 + i] = qui[i];
    probe_add_draw(s, 0, 0, 3, PIPE_FLAT, 0.0f, 1.0f);
    probe_add_draw(s, 1, 3, 3, PIPE_NOCULL, 0.0f, 1.0f);
    probe_add_draw(s, 2, 6, 6, PIPE_TEX, 0.0f, 1.0f);
    probe_add_draw(s, 3, 12, 6, PIPE_DITHER, 0.0f, 0.5f);
    probe_add_draw(s, 4, 18, 6, PIPE_UI, 0.0f, 1.0f);
}

struct ProbePush { float model[16]; float tint[4]; float params[4]; };

static void probe_record_run(RendCtx& r, VkCommandBuffer cb, void* user) {
    ProbeRec* rec = (ProbeRec*)user;
    (void)rec;
    VkClearValue clear[2];
    memset(clear, 0, sizeof clear);
    clear[1].depthStencil.depth = 1.0f;
    VkRenderPassBeginInfo rb;
    memset(&rb, 0, sizeof rb);
    rb.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rb.renderPass = r.rp;
    rb.framebuffer = r.fb;
    rb.renderArea.extent.width = 64;
    rb.renderArea.extent.height = 64;
    rb.clearValueCount = 2;
    rb.pClearValues = clear;
    r.api.CmdBeginRenderPass(cb, &rb, VK_SUBPASS_CONTENTS_INLINE);
    r.api.CmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, r.playout, 0, 1, &r.dset, 0, 0);
    VkDeviceSize off = 0;
    r.api.CmdBindVertexBuffers(cb, 0, 1, &r.vbuf, &off);
    r.api.CmdBindIndexBuffer(cb, r.ibuf, 0, VK_INDEX_TYPE_UINT16);
    for (u32 i = 0; i < 5; i++) {
        const RDraw& d = g_ps.draws[i];
        r.api.CmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, r.pip[d.pipe]);
        ProbePush pc;
        memset(&pc, 0, sizeof pc);
        pc.model[0] = 1.0f;
        pc.model[5] = 1.0f;
        pc.model[10] = 1.0f;
        pc.model[15] = 1.0f;
        pc.tint[0] = 1.0f;
        pc.tint[1] = 1.0f;
        pc.tint[2] = 1.0f;
        pc.tint[3] = d.alpha;
        pc.params[0] = d.w;
        pc.params[2] = d.pipe == PIPE_UI ? 1.0f : 0.0f;
        r.api.CmdPushConstants(cb, r.playout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                               0, (u32)sizeof pc, &pc);
        r.api.CmdDrawIndexed(cb, d.count, 1, d.first, 0, 0);
    }
    r.api.CmdEndRenderPass(cb);
    VkBufferImageCopy region;
    memset(&region, 0, sizeof region);
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent.width = 64;
    region.imageExtent.height = 64;
    region.imageExtent.depth = 1;
    r.api.CmdCopyImageToBuffer(cb, r.img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, r.rbuf, 1, &region);
}

static void probe_cleanup(RendCtx& tmp) {
    for (u32 i = 0; i < 5; i++) if (tmp.pip[i]) { tmp.api.DestroyPipeline(tmp.dev, tmp.pip[i], 0); tmp.pip[i] = 0; }
    if (tmp.playout) { tmp.api.DestroyPipelineLayout(tmp.dev, tmp.playout, 0); tmp.playout = 0; }
    if (tmp.dpool) { tmp.api.DestroyDescriptorPool(tmp.dev, tmp.dpool, 0); tmp.dpool = 0; }
    if (tmp.dsl) { tmp.api.DestroyDescriptorSetLayout(tmp.dev, tmp.dsl, 0); tmp.dsl = 0; }
    if (tmp.samp) { tmp.api.DestroySampler(tmp.dev, tmp.samp, 0); tmp.samp = 0; }
    if (tmp.tex_view) { tmp.api.DestroyImageView(tmp.dev, tmp.tex_view, 0); tmp.tex_view = 0; }
    rv_destroy_image(tmp, tmp.tex, tmp.tex_mem);
    rv_destroy_buffer(tmp, tmp.vbuf, tmp.vmem);
    rv_destroy_buffer(tmp, tmp.ibuf, tmp.imem);
    rv_destroy_buffer(tmp, tmp.ubuf, tmp.umem);
    rv_destroy_buffer(tmp, tmp.rbuf, tmp.rmem);
    if (tmp.fb) { tmp.api.DestroyFramebuffer(tmp.dev, tmp.fb, 0); tmp.fb = 0; }
    if (tmp.rp) { tmp.api.DestroyRenderPass(tmp.dev, tmp.rp, 0); tmp.rp = 0; }
    if (tmp.img_view) { tmp.api.DestroyImageView(tmp.dev, tmp.img_view, 0); tmp.img_view = 0; }
    if (tmp.depth_view) { tmp.api.DestroyImageView(tmp.dev, tmp.depth_view, 0); tmp.depth_view = 0; }
    rv_destroy_image(tmp, tmp.img, tmp.img_mem);
    rv_destroy_image(tmp, tmp.depth, tmp.depth_mem);
    if (tmp.cpool) { tmp.api.DestroyCommandPool(tmp.dev, tmp.cpool, 0); tmp.cpool = 0; }
    for (u32 i = 0; i < 4; i++) if (tmp.fs[i]) { tmp.api.DestroyShaderModule(tmp.dev, tmp.fs[i], 0); tmp.fs[i] = 0; }
    if (tmp.vs) { tmp.api.DestroyShaderModule(tmp.dev, tmp.vs, 0); tmp.vs = 0; }
    if (tmp.dev) { tmp.api.DestroyDevice(tmp.dev, 0); tmp.dev = 0; }
}

static bool probe_render_device(VkApi& api, VkInstance inst, VkPhysicalDevice phys,
                                Alloc a, u32 plant_px, GoldenImg* out) {
    RendCtx tmp;
    memset(&tmp, 0, sizeof tmp);
    tmp.alloc = a;
    tmp.api = api;
    tmp.phys = phys;
    api.GetPhysicalDeviceMemoryProperties(phys, &tmp.memprops);
    u32 nq = 0;
    api.GetPhysicalDeviceQueueFamilyProperties(phys, &nq, 0);
    if (nq == 0 || nq > 32) return false;
    VkQueueFamilyProperties fam[32];
    api.GetPhysicalDeviceQueueFamilyProperties(phys, &nq, fam);
    u32 qf = 0;
    bool found = false;
    for (u32 i = 0; i < nq; i++) if (fam[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) { qf = i; found = true; break; }
    if (!found) return false;
    float prio = 1.0f;
    VkDeviceQueueCreateInfo qci;
    memset(&qci, 0, sizeof qci);
    qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qci.queueFamilyIndex = qf;
    qci.queueCount = 1;
    qci.pQueuePriorities = &prio;
    VkPhysicalDeviceFeatures feats;
    memset(&feats, 0, sizeof feats);
    VkPhysicalDeviceVulkan13Features f13sup;
    memset(&f13sup, 0, sizeof f13sup);
    f13sup.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    VkPhysicalDeviceFeatures2 f2;
    memset(&f2, 0, sizeof f2);
    f2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    f2.pNext = &f13sup;
    api.GetPhysicalDeviceFeatures2(phys, &f2);
    VkPhysicalDeviceVulkan13Features f13;
    memset(&f13, 0, sizeof f13);
    f13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    if (f13sup.shaderDemoteToHelperInvocation)
        f13.shaderDemoteToHelperInvocation = VK_TRUE;
    VkDeviceCreateInfo dci;
    memset(&dci, 0, sizeof dci);
    dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dci.pNext = &f13;
    dci.queueCreateInfoCount = 1;
    dci.pQueueCreateInfos = &qci;
    dci.pEnabledFeatures = &feats;
    RV_LOGF("features10_true=%u features13_true=%u", count_true10(&feats), count_true13(&f13));
    if (api.CreateDevice(phys, &dci, 0, &tmp.dev) != VK_SUCCESS) return false;
    api.GetDeviceQueue(tmp.dev, qf, 0, &tmp.queue);
    vk_api_load_device(tmp.api, tmp.dev);
    VkCommandPoolCreateInfo pci;
    memset(&pci, 0, sizeof pci);
    pci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pci.queueFamilyIndex = qf;
    if (tmp.api.CreateCommandPool(tmp.dev, &pci, 0, &tmp.cpool) != VK_SUCCESS) {
        tmp.api.DestroyDevice(tmp.dev, 0);
        tmp.dev = 0;
        return false;
    }
    (void)inst;
    rv_image(tmp, VK_FORMAT_B8G8R8A8_UNORM, 64, 64,
             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
             &tmp.img, &tmp.img_mem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    rv_image(tmp, VK_FORMAT_D32_SFLOAT, 64, 64, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
             &tmp.depth, &tmp.depth_mem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkImageViewCreateInfo vi;
    memset(&vi, 0, sizeof vi);
    vi.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    vi.viewType = VK_IMAGE_VIEW_TYPE_2D;
    vi.format = VK_FORMAT_B8G8R8A8_UNORM;
    vi.image = tmp.img;
    vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vi.subresourceRange.levelCount = 1;
    vi.subresourceRange.layerCount = 1;
    rv_chk(tmp, "vkCreateImageView", tmp.api.CreateImageView(tmp.dev, &vi, 0, &tmp.img_view));
    vi.image = tmp.depth;
    vi.format = VK_FORMAT_D32_SFLOAT;
    vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    rv_chk(tmp, "vkCreateImageView", tmp.api.CreateImageView(tmp.dev, &vi, 0, &tmp.depth_view));
    create_render_pass(tmp, 64, 64);

    VkDescriptorSetLayoutBinding b[2];
    memset(b, 0, sizeof b);
    b[0].binding = 0;
    b[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    b[0].descriptorCount = 1;
    b[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    b[1].binding = 1;
    b[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    b[1].descriptorCount = 1;
    b[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo dl;
    memset(&dl, 0, sizeof dl);
    dl.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dl.bindingCount = 2;
    dl.pBindings = b;
    rv_chk(tmp, "vkCreateDescriptorSetLayout", tmp.api.CreateDescriptorSetLayout(tmp.dev, &dl, 0, &tmp.dsl));

    VkPushConstantRange pr;
    memset(&pr, 0, sizeof pr);
    pr.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pr.offset = 0;
    pr.size = 96;
    VkPipelineLayoutCreateInfo pl;
    memset(&pl, 0, sizeof pl);
    pl.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pl.setLayoutCount = 1;
    pl.pSetLayouts = &tmp.dsl;
    pl.pushConstantRangeCount = 1;
    pl.pPushConstantRanges = &pr;
    rv_chk(tmp, "vkCreatePipelineLayout", tmp.api.CreatePipelineLayout(tmp.dev, &pl, 0, &tmp.playout));

    rv_buffer(tmp, 336, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &tmp.ubuf, &tmp.umem,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    rv_chk(tmp, "vkMapMemory", tmp.api.MapMemory(tmp.dev, tmp.umem, 0, VK_WHOLE_SIZE, 0, &tmp.umap));
    rv_buffer(tmp, 18 * 36, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &tmp.vbuf, &tmp.vmem,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    rv_buffer(tmp, 24 * 2, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &tmp.ibuf, &tmp.imem,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    rv_buffer(tmp, 64 * 64 * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT, &tmp.rbuf, &tmp.rmem,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    rv_chk(tmp, "vkMapMemory", tmp.api.MapMemory(tmp.dev, tmp.vmem, 0, VK_WHOLE_SIZE, 0, (void**)&tmp.vmap));
    rv_chk(tmp, "vkMapMemory", tmp.api.MapMemory(tmp.dev, tmp.imem, 0, VK_WHOLE_SIZE, 0, (void**)&tmp.imap));
    rv_chk(tmp, "vkMapMemory", tmp.api.MapMemory(tmp.dev, tmp.rmem, 0, VK_WHOLE_SIZE, 0, (void**)&tmp.rmap));

    rv_image(tmp, VK_FORMAT_B8G8R8A8_UNORM, 8, 8,
             VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
             &tmp.tex, &tmp.tex_mem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vi.image = tmp.tex;
    vi.format = VK_FORMAT_B8G8R8A8_UNORM;
    vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    rv_chk(tmp, "vkCreateImageView", tmp.api.CreateImageView(tmp.dev, &vi, 0, &tmp.tex_view));
    upload_texture(tmp);

    VkSamplerCreateInfo si;
    memset(&si, 0, sizeof si);
    si.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    si.magFilter = VK_FILTER_NEAREST;
    si.minFilter = VK_FILTER_NEAREST;
    si.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    si.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    si.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    si.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    si.anisotropyEnable = VK_FALSE;
    si.maxAnisotropy = 1.0f;
    si.compareEnable = VK_FALSE;
    si.compareOp = VK_COMPARE_OP_ALWAYS;
    si.minLod = 0.0f;
    si.maxLod = 0.0f;
    si.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    si.unnormalizedCoordinates = VK_FALSE;
    rv_chk(tmp, "vkCreateSampler", tmp.api.CreateSampler(tmp.dev, &si, 0, &tmp.samp));

    VkDescriptorPoolSize ps[2];
    memset(ps, 0, sizeof ps);
    ps[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ps[0].descriptorCount = 1;
    ps[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    ps[1].descriptorCount = 1;
    VkDescriptorPoolCreateInfo pci2;
    memset(&pci2, 0, sizeof pci2);
    pci2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pci2.maxSets = 1;
    pci2.poolSizeCount = 2;
    pci2.pPoolSizes = ps;
    rv_chk(tmp, "vkCreateDescriptorPool", tmp.api.CreateDescriptorPool(tmp.dev, &pci2, 0, &tmp.dpool));
    VkDescriptorSetAllocateInfo dsa;
    memset(&dsa, 0, sizeof dsa);
    dsa.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    dsa.descriptorPool = tmp.dpool;
    dsa.descriptorSetCount = 1;
    dsa.pSetLayouts = &tmp.dsl;
    rv_chk(tmp, "vkAllocateDescriptorSets", tmp.api.AllocateDescriptorSets(tmp.dev, &dsa, &tmp.dset));

    for (u32 i = 0; i < 16; i++) tmp.vp[i] = 0.0f;
    tmp.vp[0] = tmp.vp[5] = tmp.vp[10] = tmp.vp[15] = 1.0f;
    probe_fill_scene(plant_px);
    memcpy(tmp.vmap, g_ps.verts, sizeof g_ps.verts);
    memcpy(tmp.imap, g_ps.idx, sizeof g_ps.idx);
    if (tmp.umap) {
        UboData* u = (UboData*)tmp.umap;
        for (u32 i = 0; i < 16; i++) u->vp[i] = tmp.vp[i];
        rv_palette(u->pal);
        for (u32 c = 0; c < 4; c++) u->params[c] = 0.0f;
    }
    write_descriptors(tmp);

    tmp.vs = load_shader(tmp, "VS_iso");
    tmp.fs[0] = load_shader(tmp, "FS_flat");
    tmp.fs[1] = load_shader(tmp, "FS_tex");
    tmp.fs[2] = load_shader(tmp, "FS_dither");
    tmp.fs[3] = load_shader(tmp, "FS_ui");
    rv_create_pipelines(tmp, 64, 64);

    ProbeRec rec;
    rec.img = 0;
    rec.buf = tmp.rbuf;
    rv_one_shot(tmp, probe_record_run, &rec);
    bool ok = true;
    if (out && tmp.rmap) memcpy(out->rgba, tmp.rmap, 64 * 64 * 4);
    else ok = false;
    probe_cleanup(tmp);
    return ok;
}

void r_tier_probe(Alloc a, GoldenImg* ref, MatchResult* res) {
    g_probe.name = 0;
    g_probe.reason = 0;
    g_probe.has_cand = false;
    if (!res) return;
    res->ok = false;
    res->mismatched_px = 0;
    if (!ref) return;
    bool want_lvp = g_probe.req && strcmp(g_probe.req, "lavapipe") == 0;

    VkApi api;
    memset(&api, 0, sizeof api);
    vk_api_load_instance(api, 0);
    if (!api.CreateInstance) { g_probe.reason = "no vkCreateInstance"; return; }
    VkApplicationInfo ai;
    memset(&ai, 0, sizeof ai);
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pApplicationName = "kitty-tier-probe";
    ai.apiVersion = VK_API_VERSION_1_3;
    VkInstanceCreateInfo ici;
    memset(&ici, 0, sizeof ici);
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ici.pApplicationInfo = &ai;
    VkInstance inst = 0;
    if (api.CreateInstance(&ici, 0, &inst) != VK_SUCCESS) { g_probe.reason = "no Vulkan instance"; return; }
    vk_api_load_instance(api, inst);
    u32 n = 0;
    if (api.EnumeratePhysicalDevices(inst, &n, 0) != VK_SUCCESS || n == 0 || n > 16) {
        g_probe.reason = "no physical devices";
        api.DestroyInstance(inst, 0);
        return;
    }
    VkPhysicalDevice phys[16];
    if (api.EnumeratePhysicalDevices(inst, &n, phys) != VK_SUCCESS) {
        g_probe.reason = "no physical devices";
        api.DestroyInstance(inst, 0);
        return;
    }
    VkPhysicalDevice ref_phys = 0;
    VkPhysicalDevice nvk_phys = 0;
    for (u32 i = 0; i < n; i++) {
        VkPhysicalDeviceProperties pr;
        u32 drv = 0;
        dev_props(api, phys[i], &pr, &drv);
        if (drv == VK_DRIVER_ID_MESA_LLVMPIPE && !ref_phys) ref_phys = phys[i];
        if (pr.vendorID == 0x10DEu && drv == VK_DRIVER_ID_MESA_NVK && !nvk_phys) nvk_phys = phys[i];
    }
    VkPhysicalDevice cand_phys = 0;
    const char* name = "none";
    if (want_lvp) {
        if (ref_phys) { cand_phys = ref_phys; name = "lavapipe"; }
    } else if (nvk_phys) {
        cand_phys = nvk_phys;
        name = "nvk";
    } else {
        g_probe.reason = "no NVK device";
    }
    g_probe.name = name;
    if (!ref_phys) {
        if (!g_probe.reason) g_probe.reason = "no lavapipe device";
        api.DestroyInstance(inst, 0);
        return;
    }
    if (!probe_render_device(api, inst, ref_phys, a, 0, ref)) {
        g_probe.reason = "reference render failed";
        api.DestroyInstance(inst, 0);
        return;
    }
    if (cand_phys) {
        u32 plant = g_probe.plant_shift ? 4u : 0u;
        if (probe_render_device(api, inst, cand_phys, a, plant, &g_probe.cand)) {
            g_probe.has_cand = true;
            *res = image_compare(g_probe.cand.rgba, ref->rgba, 4096, 4);
            if (!res->ok) g_probe.reason = "candidate image mismatch";
        } else {
            g_probe.reason = "candidate render failed";
        }
    }
    api.DestroyInstance(inst, 0);
}

const char* r_probe_candidate(void) { return g_probe.name; }
const char* r_probe_reason(void) { return g_probe.reason; }
const u8* r_probe_cand_bytes(void) { return g_probe.has_cand ? g_probe.cand.rgba : 0; }
void r_set_probe_candidate(const char* name) { g_probe.req = name; }
void r_set_probe_plant(bool on) { g_probe.plant_shift = on; }

static void setup_resources(RendCtx* r) {
    VkCommandPoolCreateInfo pci;
    memset(&pci, 0, sizeof pci);
    pci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pci.queueFamilyIndex = r->qfam;
    rv_chk(*r, "vkCreateCommandPool", r->api.CreateCommandPool(r->dev, &pci, 0, &r->cpool));

    rv_image(*r, VK_FORMAT_B8G8R8A8_UNORM, 320, 256,
             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
             &r->img, &r->img_mem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    rv_image(*r, VK_FORMAT_D32_SFLOAT, 320, 256, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
             &r->depth, &r->depth_mem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkImageViewCreateInfo vi;
    memset(&vi, 0, sizeof vi);
    vi.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    vi.viewType = VK_IMAGE_VIEW_TYPE_2D;
    vi.format = VK_FORMAT_B8G8R8A8_UNORM;
    vi.image = r->img;
    vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vi.subresourceRange.levelCount = 1;
    vi.subresourceRange.layerCount = 1;
    rv_chk(*r, "vkCreateImageView", r->api.CreateImageView(r->dev, &vi, 0, &r->img_view));
    vi.image = r->depth;
    vi.format = VK_FORMAT_D32_SFLOAT;
    vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    rv_chk(*r, "vkCreateImageView", r->api.CreateImageView(r->dev, &vi, 0, &r->depth_view));

    create_render_pass(*r, 320, 256);

    VkDescriptorSetLayoutBinding b[2];
    memset(b, 0, sizeof b);
    b[0].binding = 0;
    b[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    b[0].descriptorCount = 1;
    b[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    b[1].binding = 1;
    b[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    b[1].descriptorCount = 1;
    b[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo dl;
    memset(&dl, 0, sizeof dl);
    dl.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dl.bindingCount = 2;
    dl.pBindings = b;
    rv_chk(*r, "vkCreateDescriptorSetLayout", r->api.CreateDescriptorSetLayout(r->dev, &dl, 0, &r->dsl));

    VkPushConstantRange pr;
    memset(&pr, 0, sizeof pr);
    pr.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pr.offset = 0;
    pr.size = 96;
    VkPipelineLayoutCreateInfo pl;
    memset(&pl, 0, sizeof pl);
    pl.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pl.setLayoutCount = 1;
    pl.pSetLayouts = &r->dsl;
    pl.pushConstantRangeCount = 1;
    pl.pPushConstantRanges = &pr;
    rv_chk(*r, "vkCreatePipelineLayout", r->api.CreatePipelineLayout(r->dev, &pl, 0, &r->playout));

    rv_buffer(*r, 336, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &r->ubuf, &r->umem,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    rv_chk(*r, "vkMapMemory", r->api.MapMemory(r->dev, r->umem, 0, VK_WHOLE_SIZE, 0, &r->umap));
    r->vcap = 20000;
    r->icap = 65536;
    rv_buffer(*r, (VkDeviceSize)r->vcap * 36, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &r->vbuf, &r->vmem,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    rv_buffer(*r, (VkDeviceSize)r->icap * 2, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &r->ibuf, &r->imem,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    rv_buffer(*r, 320 * 256 * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT, &r->rbuf, &r->rmem,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    rv_chk(*r, "vkMapMemory", r->api.MapMemory(r->dev, r->vmem, 0, VK_WHOLE_SIZE, 0, (void**)&r->vmap));
    rv_chk(*r, "vkMapMemory", r->api.MapMemory(r->dev, r->imem, 0, VK_WHOLE_SIZE, 0, (void**)&r->imap));
    rv_chk(*r, "vkMapMemory", r->api.MapMemory(r->dev, r->rmem, 0, VK_WHOLE_SIZE, 0, (void**)&r->rmap));

    rv_image(*r, VK_FORMAT_B8G8R8A8_UNORM, 8, 8,
             VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
             &r->tex, &r->tex_mem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vi.image = r->tex;
    vi.format = VK_FORMAT_B8G8R8A8_UNORM;
    vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    rv_chk(*r, "vkCreateImageView", r->api.CreateImageView(r->dev, &vi, 0, &r->tex_view));
    upload_texture(*r);

    VkSamplerCreateInfo si;
    memset(&si, 0, sizeof si);
    si.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    si.magFilter = VK_FILTER_NEAREST;
    si.minFilter = VK_FILTER_NEAREST;
    si.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    si.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    si.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    si.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    si.anisotropyEnable = VK_FALSE;
    si.maxAnisotropy = 1.0f;
    si.compareEnable = VK_FALSE;
    si.compareOp = VK_COMPARE_OP_ALWAYS;
    si.minLod = 0.0f;
    si.maxLod = 0.0f;
    si.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    si.unnormalizedCoordinates = VK_FALSE;
    rv_chk(*r, "vkCreateSampler", r->api.CreateSampler(r->dev, &si, 0, &r->samp));

    VkDescriptorPoolSize ps[2];
    memset(ps, 0, sizeof ps);
    ps[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ps[0].descriptorCount = 1;
    ps[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    ps[1].descriptorCount = 1;
    VkDescriptorPoolCreateInfo pci2;
    memset(&pci2, 0, sizeof pci2);
    pci2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pci2.maxSets = 1;
    pci2.poolSizeCount = 2;
    pci2.pPoolSizes = ps;
    rv_chk(*r, "vkCreateDescriptorPool", r->api.CreateDescriptorPool(r->dev, &pci2, 0, &r->dpool));
    VkDescriptorSetAllocateInfo dsa;
    memset(&dsa, 0, sizeof dsa);
    dsa.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    dsa.descriptorPool = r->dpool;
    dsa.descriptorSetCount = 1;
    dsa.pSetLayouts = &r->dsl;
    rv_chk(*r, "vkAllocateDescriptorSets", r->api.AllocateDescriptorSets(r->dev, &dsa, &r->dset));
    write_descriptors(*r);

    r->vs = load_shader(*r, "VS_iso");
    r->fs[0] = load_shader(*r, "FS_flat");
    r->fs[1] = load_shader(*r, "FS_tex");
    r->fs[2] = load_shader(*r, "FS_dither");
    r->fs[3] = load_shader(*r, "FS_ui");

    u64 p0 = now_us();
    rv_create_pipelines(*r, 320, 256);
    u64 p1 = now_us();
    r->pipe_ms = (float)(p1 - p0) / 1000.0f;
    RV_LOGF("shaders=5 pipelines=5 pipeline_ms=%.3f", (double)r->pipe_ms);

    VkCommandBufferAllocateInfo cba;
    memset(&cba, 0, sizeof cba);
    cba.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cba.commandPool = r->cpool;
    cba.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cba.commandBufferCount = 1;
    rv_chk(*r, "vkAllocateCommandBuffers", r->api.AllocateCommandBuffers(r->dev, &cba, &r->cb));

    VkSemaphoreCreateInfo smi;
    memset(&smi, 0, sizeof smi);
    smi.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    rv_chk(*r, "vkCreateSemaphore", r->api.CreateSemaphore(r->dev, &smi, 0, &r->sem_acquire));
    VkFenceCreateInfo fci;
    memset(&fci, 0, sizeof fci);
    fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    rv_chk(*r, "vkCreateFence", r->api.CreateFence(r->dev, &fci, 0, &r->fence));
}

RendCtx* r_init(Alloc a, Tier want, TierPick* pick) {
    u64 t0 = now_us();
    RendCtx* r = common_init(a, want, pick, 0);
    if (!r) return 0;
    setup_resources(r);
    r->init_ms = (float)(now_us() - t0) / 1000.0f;
    return r;
}

RendCtx* r_init_wsi(Alloc a, Tier want, TierPick* pick, Plat& plat) {
    u64 t0 = now_us();
    RendCtx* r = common_init(a, want, pick, &plat);
    if (!r) return 0;
    setup_resources(r);
    create_swapchain(*r);
    r->init_ms = (float)(now_us() - t0) / 1000.0f;
    return r;
}

void r_shutdown(RendCtx& r) {
    if (r.dev) rv_chk(r, "vkDeviceWaitIdle", r.api.DeviceWaitIdle(r.dev));
    for (u32 i = 0; i < r.sc_n; i++) {
        if (r.render_sem[i]) r.api.DestroySemaphore(r.dev, r.render_sem[i], 0);
    }
    r.sc_n = 0;
    if (r.sc) { r.api.DestroySwapchainKHR(r.dev, r.sc, 0); r.sc = 0; }
    if (r.sem_acquire) { r.api.DestroySemaphore(r.dev, r.sem_acquire, 0); r.sem_acquire = 0; }
    if (r.fence) { r.api.DestroyFence(r.dev, r.fence, 0); r.fence = 0; }
    if (r.bad_sem) { r.api.DestroySemaphore(r.dev, r.bad_sem, 0); r.bad_sem = 0; }
    if (r.samp) r.api.DestroySampler(r.dev, r.samp, 0);
    if (r.tex_view) r.api.DestroyImageView(r.dev, r.tex_view, 0);
    rv_destroy_image(r, r.tex, r.tex_mem);
    if (r.dpool) r.api.DestroyDescriptorPool(r.dev, r.dpool, 0);
    if (r.dsl) r.api.DestroyDescriptorSetLayout(r.dev, r.dsl, 0);
    for (u32 i = 0; i < 5; i++) if (r.pip[i]) r.api.DestroyPipeline(r.dev, r.pip[i], 0);
    if (r.playout) r.api.DestroyPipelineLayout(r.dev, r.playout, 0);
    for (u32 i = 0; i < 4; i++) if (r.fs[i]) r.api.DestroyShaderModule(r.dev, r.fs[i], 0);
    if (r.vs) r.api.DestroyShaderModule(r.dev, r.vs, 0);
    rv_destroy_buffer(r, r.vbuf, r.vmem);
    rv_destroy_buffer(r, r.ibuf, r.imem);
    rv_destroy_buffer(r, r.ubuf, r.umem);
    rv_destroy_buffer(r, r.rbuf, r.rmem);
    if (r.fb) r.api.DestroyFramebuffer(r.dev, r.fb, 0);
    if (r.rp) r.api.DestroyRenderPass(r.dev, r.rp, 0);
    if (r.img_view) r.api.DestroyImageView(r.dev, r.img_view, 0);
    if (r.depth_view) r.api.DestroyImageView(r.dev, r.depth_view, 0);
    rv_destroy_image(r, r.img, r.img_mem);
    rv_destroy_image(r, r.depth, r.depth_mem);
    if (r.cpool) r.api.DestroyCommandPool(r.dev, r.cpool, 0);
    if (r.dbg) r.api.DestroyDebugUtilsMessengerEXT(r.inst, r.dbg, 0);
    if (r.dev) r.api.DestroyDevice(r.dev, 0);
    if (r.surf) r.api.DestroySurfaceKHR(r.inst, r.surf, 0);
    if (r.inst) r.api.DestroyInstance(r.inst, 0);
    memset(&r, 0, sizeof r);
}

const char* r_device_name(RendCtx& r) { return r.props.deviceName; }
const char* r_driver_info(RendCtx& r) { return r.driver_info; }
u32 r_validation_count(RendCtx& r) { return r.dbg_count; }
const volatile u32* r_validation_counter(RendCtx& r) { return &r.dbg_count; }
const u8* r_pixels(RendCtx& r) { return r.pixels; }

void r_set_scene(RendCtx& r, const RScene* s) { r.scene = s; }
void r_set_plant_bad_copy(RendCtx& r, bool on) { r.plant_bad_copy = on; }
void r_set_plant_bad_present(RendCtx& r, bool on) { r.plant_bad_present = on; }
void r_get_vp(RendCtx& r, float out[16]) { for (u32 i = 0; i < 16; i++) out[i] = r.vp[i]; }
