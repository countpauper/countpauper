#pragma once
#include <cstdlib>
#include <stdio.h>

#ifdef _POSIX
    #include <signal.h>
    #define debugbreak() raise(SIGTRAP)
#else
    #include <intrin.h>
    #define debugbreak() __debugbreak()
#endif

inline void _assert_handler(const char *expr, const char *file, int line)
{
    printf("Assertion failed: %s, file %s, line %d\n", expr, file, line);
    debugbreak();
    std::abort();
}

#undef assert
#define assert(expr) \
    if (!(expr)) { \
        _assert_handler(#expr, __FILE__, __LINE__); \
    }
