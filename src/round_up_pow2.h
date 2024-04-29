/**
 * @file round_up_pow2.h
 * @brief Round up to the next power of two.
 */

#pragma once

#include <stdlib.h> // size_t

/**
 * Round up to the next power of two.
 *
 * Asserts x is not 0.
 *
 * @param x The number at hand.
 * @return A power of two that is larger than or equal to the given number.
 */
size_t round_up_pow2(size_t x);
