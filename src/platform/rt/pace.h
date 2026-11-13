#pragma once
#include "platform.h"

struct PaceOut { u32 stepped; u32 dropped; };
inline PaceOut pace_step(u32 due) {
    PaceOut o;
    o.stepped = due > 4u ? 4u : due;
    o.dropped = due - o.stepped;
    return o;
}
