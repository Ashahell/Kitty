#include <cstddef>
#include "plat_svc.h"

void* operator new(size_t) { pl_trap("operator new called"); }
void* operator new[](size_t) { pl_trap("operator new[] called"); }
void operator delete(void*) noexcept { pl_trap("operator delete called"); }
void operator delete[](void*) noexcept { pl_trap("operator delete[] called"); }
void operator delete(void*, size_t) noexcept { pl_trap("operator delete sized called"); }
void operator delete[](void*, size_t) noexcept { pl_trap("operator delete[] sized called"); }
