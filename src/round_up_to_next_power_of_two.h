/**
 * @file round_up_to_next_power_of_two.h
 * @brief Round up to the next power of two.
 *
 * Sources:
 * - https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
 * - https://en.wikipedia.org/wiki/Find_first_set#Tool_and_library_support
 */

#pragma once

#include <assert.h> // assert
#include <stdint.h> // INTPTR_MAX, INT32_MAX, INT64_MAX
#include <stdlib.h> // abort
#include <stdlib.h> // size_t

/**
 * Round up to the next power of two.
 *
 * Assumes x is not 0.
 *
 * @param x The number at hand.
 * @return A power of two that is larger than or equal to the given number.
 */
static inline size_t round_up_to_next_power_of_two(size_t x) {
    assert(x != 0);

#if defined(UINT64_MAX) && UINTPTR_MAX == UINT32_MAX
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
    assert(("Not supported.", false));
    return 0;
#endif
}
