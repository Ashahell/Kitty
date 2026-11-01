#pragma once
#include <cstdarg>
#include "platform.h"

u32 rt_fmt(char* out, u32 n, const char* fmt, ...);
u32 rt_vfmt(char* out, u32 n, const char* fmt, va_list ap);
