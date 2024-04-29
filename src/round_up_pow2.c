/*
 * Sources:
 *   @li Fallback: https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
 *   @li Intrinsics: https://en.wikipedia.org/wiki/Find_first_set#Tool_and_library_support
 *   @li 64/32 bit detection: https://stackoverflow.com/a/61017823
*/

#include <assert.h> // assert, uint32_t

#if defined(_MSC_VER) && (_MSC_VER < 1600)
static_assert("Not Supported" && 0);
#endif 

#include <stdint.h> // INTPTR_MAX, INT32_MAX, INT64_MAX

#include "round_up_pow2.h"

size_t round_up_pow2(size_t x) {
    assert(x != 0);

#if defined(UINT32_MAX) && UINTPTR_MAX == UINT32_MAX
    assert(x <= UINT32_MAX / 4);
#endif
#if defined(UINT64_MAX) && UINTPTR_MAX == UINT64_MAX
    assert(x <= UINT64_MAX / 4);
#endif

#ifdef __GNUC__
#if __GNUC__ > 3 || (__GNUC__ == 3 && (__GNUC_MINOR__ > 4 || __GNUC_MINOR__ == 4)) // Test for GCC >= 3.4.0
#if UINTPTR_MAX == UINT32_MAX
    return x == 1 ? 1 : 1 << (32 - __builtin_clz(x - 1));
#elif UINTPTR_MAX == UINT64_MAX
    return x == 1 ? 1 : 1 << (64 - __builtin_clzl(x - 1));
#endif
#endif
#endif

#if defined(UINT32_MAX) && (UINTPTR_MAX == UINT32_MAX || UINTPTR_MAX == UINT64_MAX)
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
    assert("Not supported." && false);
    return 0;
#endif
}
