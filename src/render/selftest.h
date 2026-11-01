#pragma once
#include "render_types.h"

MatchResult image_compare(const u8* got, const u8* want, u32 pixels, u32 tol);
