#pragma once

#ifdef __linux__
#define PLATFORM_IS_LINUX 1
#define EXPORT __attribute__((__visibility__("default")))
#elif defined(_WIN64)
#define PLATFORM_IS_WINDOWS 1
#else
#error "Only Windows or Linux supports Pain"
#endif

#define BIT(x) (1 << x)
