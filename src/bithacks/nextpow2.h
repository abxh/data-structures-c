
#include <assert.h> // assert
#include <stdint.h> // INTPTR_MAX, INT32_MAX, INT64_MAX
#include <stdlib.h> // abort
#include <stdlib.h> // size_t

// Reference:
// https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
// https://en.wikipedia.org/wiki/Find_first_set#Tool_and_library_support
// https://stackoverflow.com/questions/5272825/detecting-64bit-compile-in-c

static inline size_t next_pow2(size_t x) {
    assert(x != 0);

#ifdef __GNUC__
#if __GNUC__ > 3 || (__GNUC__ == 3 && (__GNUC_MINOR__ > 4 || __GNUC_MINOR__ == 4)) // Test for GCC >= 3.4.0
#if UINTPTR_MAX == UINT32_MAX
    return x == 1 ? 1 : 1 << (32 - __builtin_clz(x - 1));
#elif UINTPTR_MAX == UINT64_MAX
    return x == 1 ? 1 : 1 << (64 - __builtin_clzl(x - 1));
#endif
#endif
#endif

#if UINTPTR_MAX == UINT32_MAX || UINTPTR_MAX == UINT64_MAX
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
#if defined(UINT64_MAX) && UINTPTR_MAX == UINT64_MAX
    x |= x >> 32;
#endif
    x++;
    return x;
#else
    abort();
#endif
}
