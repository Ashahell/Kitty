#pragma once
#include "render_types.h"

u32 pl_wsi_instance_extensions(Plat&, const char** names, u32 max);
bool pl_wsi_create_surface(Plat&, VkInstance, VkSurfaceKHR* out);
void pl_wsi_extent(Plat&, u32* w, u32* h);
RendCtx* r_init_wsi(Alloc, Tier, TierPick*, Plat&);
