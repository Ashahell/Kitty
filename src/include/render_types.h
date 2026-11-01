#pragma once
#include <cstddef>
#define VK_NO_PROTOTYPES 1
#include <vulkan/vulkan.h>
#include "platform.h"

struct RendCtx;
struct RenderState { u32 tick; };
struct DrawList { u32 items; };
struct UIRects { u32 quads; };
struct TierPick { bool nvk; const char* reason; };
enum Tier { TIER_LAVAPIPE, TIER_NVK };
struct GoldenImg { u8 rgba[64*64*4]; };
struct MatchResult { bool ok; u32 mismatched_px; };
RendCtx* r_init(Alloc, Tier, TierPick*);
void r_begin(RendCtx&, const RenderState*, u32 frame_index);
void r_draw(RendCtx&, const DrawList&, size_t n);
void r_overlay(RendCtx&, const UIRects&, size_t n);
VkResult r_present(RendCtx&);
void r_tier_probe(Alloc, GoldenImg*, MatchResult*);
