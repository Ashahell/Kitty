/* aros_wsi.c — the one C11 translation unit that reaches the loader's window
 * helpers (D25).  aros_window_surface.h is deliberately included here and
 * nowhere else: it defines static inline LVO wrappers that read the library's
 * jump table, and the engine's C++ translation units must not each grow their
 * own copy.  The functions below are the whole ABI plat.cpp sees.
 *
 * Nothing private is registered with Vulkan here: the helper's create path
 * ends in vkCreateXcbSurfaceKHR inside vulkan_loader.library. */
#include "aros_window_surface.h"
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <intuition/intuition.h>
#include <string.h>

VkResult cf_aros_create_surface(void *base, VkInstance inst, unsigned w, unsigned h,
                                const char *title, const VkAllocationCallbacks *alloc,
                                VkSurfaceKHR *out)
{
    return arosCreateWindowSurfaceVia(base, inst, w, h, title,
                                      (ArosAllocCB)alloc, out);
}

int cf_aros_poll(void *base, VkSurfaceKHR surf)
{
    return arosPollWindowEventsVia(base, surf);
}

VkResult cf_aros_destroy_surface(void *base, VkInstance inst, VkSurfaceKHR surf)
{
    return arosDestroyWindowSurfaceVia(base, inst, surf, (ArosAllocCB)0);
}

/* Helper: search one screen's window list for a matching title.
 * Kept separate to limit live register count in find_titled_window. */
static struct Window* find_on_screen(struct Screen* scr, const char* title)
{
    for (struct Window* w = scr->FirstWindow; w; w = w->NextWindow) {
        const char* t = w->Title ? (const char*)w->Title : "";
        if (strcmp(t, title) == 0) return w;
    }
    return 0;
}

/* The AROS x86-64 ABI reserves %r12; this function must not keep a
 * value in a callee-saved GPR across the LockIBase/UnlockIBase calls.
 * Use volatile for IntuitionBase to force stack spill and avoid %r12. */
struct Window* find_titled_window(const char* title)
{
    if (!IntuitionBase) {
        IntuitionBase = (struct IntuitionBase*)OpenLibrary((CONST_STRPTR)"intuition.library", 0);
    }
    struct IntuitionBase* volatile ib = IntuitionBase;
    if (!ib || !title) return 0;
    struct Window* found = 0;
    const ULONG lock = LockIBase(0);
    for (struct Screen* scr = ib->FirstScreen; scr && !found; scr = scr->NextScreen) {
        found = find_on_screen(scr, title);
    }
    UnlockIBase(lock);
    return found;
}

/* Open a window flagged as the helper's (vkpal_surface_create_sized) long enough
 * to read the borders Intuition gives it on this screen. */
int cf_aros_window_borders(const char* title, unsigned* bl, unsigned* bt, unsigned* br, unsigned* bb)
{
    if (!IntuitionBase) {
        IntuitionBase = (struct IntuitionBase*)OpenLibrary((CONST_STRPTR)"intuition.library", 0);
    }
    if (!IntuitionBase) return 0;
    struct NewWindow nw;
    memset(&nw, 0, sizeof nw);
    nw.LeftEdge = 10;
    nw.TopEdge = 10;
    nw.Width = 320;
    nw.Height = 256;
    nw.BlockPen = 1;
    nw.Flags = WFLG_CLOSEGADGET | WFLG_DEPTHGADGET | WFLG_DRAGBAR | WFLG_SMART_REFRESH | WFLG_GIMMEZEROZERO;
    nw.Title = (STRPTR)title;
    nw.MinWidth = 64;
    nw.MinHeight = 64;
    nw.MaxWidth = 4096;
    nw.MaxHeight = 4096;
    nw.Type = WBENCHSCREEN;
    struct Window* volatile w = OpenWindowTags(&nw, TAG_DONE);
    if (!w) return 0;
    *bl = w->BorderLeft;
    *bt = w->BorderTop;
    *br = w->BorderRight;
    *bb = w->BorderBottom;
    CloseWindow(w);
    return 1;
}

void close_titled_windows(const char* title)
{
    for (;;) {
        struct Window* volatile w = find_titled_window(title);
        if (!w) break;
        CloseWindow(w);
    }
}

