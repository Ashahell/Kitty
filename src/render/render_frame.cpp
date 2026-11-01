#include <cstring>
#include "render_internal.h"
#include "render_util.h"

static void update_ubo(RendCtx& r) {
    if (!r.umap) return;
    UboData* u = (UboData*)r.umap;
    for (u32 i = 0; i < 16; i++) u->vp[i] = r.vp[i];
    for (u32 i = 0; i < 16; i++) {
        for (u32 c = 0; c < 4; c++) u->pal[i][c] = r.pal[i][c];
    }
    u->params[0] = 0.0f;
    u->params[1] = 0.0f;
    u->params[2] = 0.0f;
    u->params[3] = 0.0f;
}

static void identity(float* m) {
    for (u32 i = 0; i < 16; i++) m[i] = 0.0f;
    m[0] = 1.0f;
    m[5] = 1.0f;
    m[10] = 1.0f;
    m[15] = 1.0f;
}

struct PushData { float model[16]; float tint[4]; float params[4]; };

static void draw_one(RendCtx& r, const RDraw& d) {
    u8 pipe = (u8)(d.pipe & 7u);
    VkPipeline p = r.pip[pipe];
    if (!p) return;
    if (p != r.cur_pipe) {
        r.api.CmdBindPipeline(r.cb, VK_PIPELINE_BIND_POINT_GRAPHICS, p);
        r.cur_pipe = p;
    }
    PushData pc;
    memset(&pc, 0, sizeof pc);
    identity(pc.model);
    pc.tint[0] = 1.0f;
    pc.tint[1] = 1.0f;
    pc.tint[2] = 1.0f;
    pc.tint[3] = d.alpha > 0.0f ? d.alpha : 1.0f;
    pc.params[0] = d.w;
    pc.params[1] = 0.0f;
    pc.params[2] = pipe == PIPE_UI ? 1.0f : 0.0f;
    pc.params[3] = 0.0f;
    r.api.CmdPushConstants(r.cb, r.playout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0, (u32)sizeof pc, &pc);
    r.api.CmdDrawIndexed(r.cb, d.count, 1, d.first, 0, 0);
}

void r_begin(RendCtx& r, const RenderState* st, u32 frame_index) {
    (void)st;
    r.frame = frame_index;
    r.skip_frame = false;
    r.uploaded = false;
    r.cur_pipe = 0;
    update_ubo(r);

    if (r.windowed) {
        u32 idx = 0;
        VkResult res = r.api.AcquireNextImageKHR(r.dev, r.sc, 100000000ull, r.sem_acquire, 0, &idx);
        VkClass c = vk_classify(res);
        if (c == VKC_RECREATE_SWAPCHAIN || c == VKC_RECREATE_SURFACE) {
            r_recreate_swapchain(r);
            r.skip_frame = true;
            return;
        }
        if (c == VKC_RETRY) { r.skip_frame = true; return; }
        if (c != VKC_NORMAL) { rv_chk(r, "vkAcquireNextImageKHR", res); r.skip_frame = true; return; }
        if (idx >= r.sc_n) { r.skip_frame = true; return; }
        r.image_index = idx;
    }

    rv_chk(r, "vkResetCommandBuffer", r.api.ResetCommandBuffer(r.cb, 0));
    VkCommandBufferBeginInfo bi;
    memset(&bi, 0, sizeof bi);
    bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    rv_chk(r, "vkBeginCommandBuffer", r.api.BeginCommandBuffer(r.cb, &bi));

    VkClearValue clear[2];
    memset(clear, 0, sizeof clear);
    clear[0].color.float32[0] = 0.0f;
    clear[0].color.float32[1] = 0.0f;
    clear[0].color.float32[2] = 0.0f;
    clear[0].color.float32[3] = 1.0f;
    clear[1].depthStencil.depth = 1.0f;
    clear[1].depthStencil.stencil = 0;
    VkRenderPassBeginInfo rb;
    memset(&rb, 0, sizeof rb);
    rb.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rb.renderPass = r.rp;
    rb.framebuffer = r.fb;
    rb.renderArea.extent.width = 320;
    rb.renderArea.extent.height = 256;
    rb.clearValueCount = 2;
    rb.pClearValues = clear;
    r.api.CmdBeginRenderPass(r.cb, &rb, VK_SUBPASS_CONTENTS_INLINE);

    r.api.CmdBindDescriptorSets(r.cb, VK_PIPELINE_BIND_POINT_GRAPHICS, r.playout, 0, 1, &r.dset, 0, 0);
    VkDeviceSize off = 0;
    r.api.CmdBindVertexBuffers(r.cb, 0, 1, &r.vbuf, &off);
    r.api.CmdBindIndexBuffer(r.cb, r.ibuf, 0, VK_INDEX_TYPE_UINT16);
}

void r_draw(RendCtx& r, const DrawList& dl, size_t n) {
    (void)n;
    if (r.skip_frame || !r.scene) return;
    const RScene* s = r.scene;
    if (!r.uploaded) {
        if (s->nv > r.vcap || s->ni > r.icap) {
            rv_die("src/render/render_frame.cpp", __LINE__, "scene exceeds the vertex or index buffer");
        }
        if (s->nv) memcpy(r.vmap, s->verts, (size_t)s->nv * 36u);
        if (s->ni) memcpy(r.imap, s->idx, (size_t)s->ni * 2u);
        r.uploaded = true;
    }
    (void)dl;
    for (u32 i = 0; i < s->ndraw; i++) {
        if ((s->draws[i].pipe & 7u) == PIPE_UI) continue;
        draw_one(r, s->draws[i]);
    }
}

void r_overlay(RendCtx& r, const UIRects& u, size_t n) {
    (void)u;
    (void)n;
    if (r.skip_frame || !r.scene) return;
    const RScene* s = r.scene;
    for (u32 i = 0; i < s->ndraw; i++) {
        if ((s->draws[i].pipe & 7u) != PIPE_UI) continue;
        draw_one(r, s->draws[i]);
    }
}

static void blit_to_swapchain(RendCtx& r) {
    VkImageSubresourceRange range;
    memset(&range, 0, sizeof range);
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.levelCount = 1;
    range.layerCount = 1;
    VkImage sc = r.sc_img[r.image_index];
    rv_barrier(r, r.cb, sc, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
               VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
               0, VK_ACCESS_TRANSFER_WRITE_BIT);
    VkClearColorValue black;
    memset(&black, 0, sizeof black);
    black.float32[3] = 1.0f;
    r.api.CmdClearColorImage(r.cb, sc, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &black, 1, &range);
    u32 w = r.sc_ext.width, h = r.sc_ext.height;
    u32 sx = w / 320u, sy = h / 256u;
    u32 s = sx < sy ? sx : sy;
    if (s < 1u) s = 1u;
    i32 dw = (i32)(320u * s), dh = (i32)(256u * s);
    i32 ox = ((i32)w - dw) / 2, oy = ((i32)h - dh) / 2;
    if (ox < 0) ox = 0;
    if (oy < 0) oy = 0;
    VkImageBlit blit;
    memset(&blit, 0, sizeof blit);
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.layerCount = 1;
    blit.srcOffsets[1].x = 320;
    blit.srcOffsets[1].y = 256;
    blit.srcOffsets[1].z = 1;
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.layerCount = 1;
    blit.dstOffsets[0].x = ox;
    blit.dstOffsets[0].y = oy;
    blit.dstOffsets[1].x = ox + dw;
    blit.dstOffsets[1].y = oy + dh;
    blit.dstOffsets[1].z = 1;
    r.api.CmdBlitImage(r.cb, r.img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       sc, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_NEAREST);
    rv_barrier(r, r.cb, sc, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
               VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
               VK_ACCESS_TRANSFER_WRITE_BIT, 0);
}

VkResult r_present(RendCtx& r) {
    if (r.skip_frame) { r.skip_frame = false; return VK_SUCCESS; }

    r.api.CmdEndRenderPass(r.cb);
    VkBufferImageCopy region;
    memset(&region, 0, sizeof region);
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent.width = 320;
    region.imageExtent.height = 256;
    region.imageExtent.depth = 1;
    r.api.CmdCopyImageToBuffer(r.cb, r.img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, r.rbuf, 1, &region);

    if (r.plant_bad_copy) {
        VkImageCopy bad;
        memset(&bad, 0, sizeof bad);
        bad.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bad.srcSubresource.layerCount = 1;
        bad.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bad.dstSubresource.layerCount = 1;
        bad.extent.width = 8;
        bad.extent.height = 8;
        bad.extent.depth = 1;
        r.api.CmdCopyImage(r.cb, r.img, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                           r.tex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bad);
    }

    if (r.windowed) blit_to_swapchain(r);

    rv_chk(r, "vkEndCommandBuffer", r.api.EndCommandBuffer(r.cb));

    VkResult out = VK_SUCCESS;
    rv_chk(r, "vkWaitForFences", r.api.WaitForFences(r.dev, 1, &r.fence, VK_TRUE, 1000000000ull));
    rv_chk(r, "vkResetFences", r.api.ResetFences(r.dev, 1, &r.fence));

    VkSubmitInfo si;
    memset(&si, 0, sizeof si);
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &r.cb;
    VkSemaphore wait_sem = 0;
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSemaphore sig_sem = 0;
    if (r.windowed) {
        wait_sem = r.sem_acquire;
        sig_sem = r.render_sem[r.image_index];
        si.waitSemaphoreCount = 1;
        si.pWaitSemaphores = &wait_sem;
        si.pWaitDstStageMask = &wait_stage;
        si.signalSemaphoreCount = 1;
        si.pSignalSemaphores = &sig_sem;
    }
    rv_chk(r, "vkQueueSubmit", r.api.QueueSubmit(r.queue, 1, &si, r.fence));
    rv_chk(r, "vkWaitForFences", r.api.WaitForFences(r.dev, 1, &r.fence, VK_TRUE, 1000000000ull));

    if (r.rmap) memcpy(r.pixels, r.rmap, 320u * 256u * 4u);

    if (r.windowed) {
        if (r.plant_bad_present && !r.bad_sem) {
            VkSemaphoreCreateInfo smi;
            memset(&smi, 0, sizeof smi);
            smi.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            rv_chk(r, "vkCreateSemaphore", r.api.CreateSemaphore(r.dev, &smi, 0, &r.bad_sem));
        }
        VkPresentInfoKHR pi;
        memset(&pi, 0, sizeof pi);
        pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        VkSemaphore wait = r.plant_bad_present ? r.bad_sem : r.render_sem[r.image_index];
        pi.waitSemaphoreCount = 1;
        pi.pWaitSemaphores = &wait;
        pi.swapchainCount = 1;
        pi.pSwapchains = &r.sc;
        pi.pImageIndices = &r.image_index;
        out = r.api.QueuePresentKHR(r.queue, &pi);
        VkClass c = vk_classify(out);
        if (c == VKC_RECREATE_SWAPCHAIN || c == VKC_RECREATE_SURFACE) {
            r_recreate_swapchain(r);
            return VK_SUCCESS;
        }
        if (c != VKC_NORMAL) rv_chk(r, "vkQueuePresentKHR", out);
    }
    return out;
}
