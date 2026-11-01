#pragma once
#include "render_types.h"
#include "vk_ctx.h"
#include "plat_svc.h"
#include "rt_fmt.h"

using i8 = int8_t;
using i32 = int32_t;
using i64 = int64_t;

struct RVert { float p[3]; float q[3]; u8 c[4]; float uv[2]; };
struct RDraw { u32 first; u32 count; u32 key; u8 pipe; u8 pad[3]; float w; float alpha; };
enum { PIPE_FLAT = 0, PIPE_NOCULL = 1, PIPE_TEX = 2, PIPE_DITHER = 3, PIPE_UI = 4 };

struct RScene {
    RVert* verts;
    u16* idx;
    u32 capv;
    u32 capi;
    u32 nv;
    u32 ni;
    RDraw draws[64];
    u32 ndraw;
    DrawList dl;
};

struct UboData {
    float vp[16];
    float pal[16][4];
    float params[4];
};

struct RendCtx {
    Alloc alloc;
    bool windowed;
    Plat* plat;
    VkApi api;
    VkInstance inst;
    VkDebugUtilsMessengerEXT dbg;
    bool dbg_on;
    u32 dbg_count;
    VkPhysicalDevice phys;
    VkDevice dev;
    VkQueue queue;
    u32 qfam;
    VkPhysicalDeviceProperties props;
    char driver_info[256];
    VkPhysicalDeviceMemoryProperties memprops;
    VkSurfaceKHR surf;

    VkSwapchainKHR sc;
    u32 sc_n;
    VkImage sc_img[8];
    VkFormat sc_fmt;
    VkExtent2D sc_ext;
    VkPresentModeKHR sc_mode;
    VkSemaphore render_sem[8];

    VkImage img;
    VkDeviceMemory img_mem;
    VkImageView img_view;
    VkImage depth;
    VkDeviceMemory depth_mem;
    VkImageView depth_view;
    VkRenderPass rp;
    VkFramebuffer fb;

    VkDescriptorSetLayout dsl;
    VkPipelineLayout playout;
    VkDescriptorPool dpool;
    VkDescriptorSet dset;
    VkSampler samp;
    VkImage tex;
    VkDeviceMemory tex_mem;
    VkImageView tex_view;

    VkShaderModule vs;
    VkShaderModule fs[4];
    VkPipeline pip[5];

    VkBuffer vbuf, ibuf, ubuf, rbuf, sbuf;
    VkDeviceMemory vmem, imem, umem, rmem, smem;
    RVert* vmap;
    u16* imap;
    void* umap;
    u8* rmap;
    u8* smap;
    u32 vcap, icap;

    VkCommandPool cpool;
    VkCommandBuffer cb;
    VkSemaphore sem_acquire;
    VkFence fence;
    bool submitted;

    const RScene* scene;
    u32 frame;
    u32 image_index;
    bool skip_frame;
    bool uploaded;
    VkPipeline cur_pipe;
    float vp[16];
    float pal[16][4];
    u8 pixels[320 * 256 * 4];
    float init_ms;
    float pipe_ms;
    bool plant_bad_copy;
    bool plant_bad_present;
    VkSemaphore bad_sem;
};

void r_set_scene(RendCtx&, const RScene*);
void r_set_spv_dir(const char*);
void r_shutdown(RendCtx&);
void r_recreate_swapchain(RendCtx&);
const char* r_device_name(RendCtx&);
const char* r_driver_info(RendCtx&);
u32 r_validation_count(RendCtx&);
const volatile u32* r_validation_counter(RendCtx&);
const u8* r_pixels(RendCtx&);
void r_set_plant_bad_copy(RendCtx&, bool);
void r_set_plant_bad_present(RendCtx&, bool);
void r_get_vp(RendCtx&, float out[16]);
void r_set_plat(Plat&);
Plat* r_plat(void);
void r_set_probe_candidate(const char*);
void r_set_probe_plant(bool);
const char* r_probe_candidate(void);
const char* r_probe_reason(void);
const u8* r_probe_cand_bytes(void);
[[noreturn]] void rv_die(const char* file, u32 line, const char* msg);

#define RV_LOGF(...) do { char rv_line_[1024]; rt_fmt(rv_line_, sizeof rv_line_, __VA_ARGS__); \
    if (r_plat()) pl_log(*r_plat(), rv_line_); } while (0)
