#include <cstddef>
#include "plat_svc.h"

void* operator new(size_t) { PL_TRAP("operator new called"); }
void* operator new[](size_t) { PL_TRAP("operator new[] called"); }
void operator delete(void*) noexcept { PL_TRAP("operator delete called"); }
void operator delete[](void*) noexcept { PL_TRAP("operator delete[] called"); }
void operator delete(void*, size_t) noexcept { PL_TRAP("operator delete sized called"); }
void operator delete[](void*, size_t) noexcept { PL_TRAP("operator delete[] sized called"); }
