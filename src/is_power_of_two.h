/**
 * @file is_power_of_two.h
 * @brief Check if a number is a power of two.
 */

#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

/**
 * Check if a number is a power of two.
 *
 * @param x The number at hand.
 * @return A boolean value indicating whether the number is a power of two.
 */
static inline bool is_power_of_two(size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}
