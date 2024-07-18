/*  is_pow2.h
 *
 *  Copyright (C) 2023 abfish
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

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
