/**
 * @file round_up_pow2.h
 * @brief Round up to the next power of two.
 *
 * Sources used:
 *   @li Fallback: https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
 *   @li Intrinsics: https://en.wikipedia.org/wiki/Find_first_set#Tool_and_library_support
 *   @li 64/32 bit detection: https://stackoverflow.com/a/61017823
 */

#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Round up to the next power of two.
 *
 * Assumes:
 * @li x is strictly larger than 0.
 * @li x is smaller than than the defined UINTPTR_MAX / 4.
 *
 * @param x The number at hand.
 * @return A power of two that is larger than or equal to the given number.
 */
#if defined(UINT64_MAX) && UINTPTR_MAX == UINT64_MAX
uint64_t round_up_pow2(uint64_t x) {
    assert(0 < x && x <= UINTPTR_MAX / 4);

#if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && (__GNUC_MINOR__ > 4 || __GNUC_MINOR__ == 4))) // Test for GCC >= 3.4.0
    return x == 1 ? 1 : 1 << (64 - __builtin_clzl(x - 1));
#else
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    x++;
    return x;
#endif
}
#endif

#if defined(UINT32_MAX) && UINTPTR_MAX == UINT32_MAX
uint32_t round_up_pow2(uint32_t x) {
    assert(0 < x && x <= UINTPTR_MAX / 4);

#if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && (__GNUC_MINOR__ > 4 || __GNUC_MINOR__ == 4))) // Test for GCC >= 3.4.0
    return x == 1 ? 1 : 1 << (32 - __builtin_clz(x - 1));
#else
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
#endif
}
#endif

// vim: ft=c
