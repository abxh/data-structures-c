/**
 * @file is_pow2.h
 * @brief Check if a number is a power of two.
 */

#pragma once

#include <stdlib.h>

/**
 * @def IS_POW2(X)
 * @brief compile time is_pow2.
 */
#define IS_POW2(X) ((X) != 0 && ((X) & ((X) - 1)) == 0)

/**
 * Check if a number is a power of two.
 *
 * @param x The number at hand.
 * @return A boolean value indicating whether the number is a power of two.
 */
static inline size_t is_pow2(const size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

// vim: ft=c
