/*  fnvhash.h
 *
 *  Copyright (C) 2023 abxh
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
 * @file fnvhash.h
 * @brief FNV-1a hashing function
 * @note FNV-1a is **not** a cryptographic hashing function.
 *
 * Source:
 * https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
 */

#pragma once

#include <stdint.h> // uint8_t, uint32_t
#include <stdlib.h> // size_t, NULL

/**
 * Get the FNV-1a 32-bit hash of a char array (ending with a `\0`).
 *
 * @param char_p Pointer to the string of bytes.
 * @return A 32-bit hash of the bytes.
 */
static inline uint32_t fnvhash_32_str(const char* char_p)
{
    uint32_t hash = 0x811c9dc5;
    while (*char_p != '\0') {
        hash ^= *(char_p++);
        hash *= 0x01000193;
    }
    return hash;
}

/**
 * Get the FNV-1a 32-bit hash of a string of bytes.
 *
 * @param char_p Pointer to the string of bytes.
 * @param length Number of bytes.
 * @return A 32-bit hash of the bytes.
 */
static inline uint32_t fnvhash_32(const uint8_t* char_p, const size_t length)
{
    uint32_t hash = 0x811c9dc5;
    for (size_t i = 0; i < length; i++) {
        hash ^= *(char_p++);
        hash *= 0x01000193;
    }
    return hash;
}

// vim: ft=c
