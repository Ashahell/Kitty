#pragma once
#include "platform.h"
#include "modules.h"

struct GuruCtx {
    Plat* p;
    u32 count;
    u32 head;
    struct Rec { char file[48]; u32 line; char msg[96]; } ring[8];
};

void guru_init(GuruCtx&, Plat*);
u32 guru_ring_count(const GuruCtx&);
u32 guru_ring_oldest(const GuruCtx&);
