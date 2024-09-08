/*  align.h
 *
 *  Copyright (C) 2023 abxh
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  See the file LICENSE included with this distribution for more
 *  information. */

/**
 * @file align.h
 * @brief Align memory
 *
 * This is a direct translation of the std::align implementation in libstdc++.
 *
 * See:
 * @li https://en.cppreference.com/w/cpp/memory/align
 */

#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "is_pow2.h"

/**
 * @brief Align pointer to the next alignment boundary.
 *
 * @param[in] alignment	        The desired alignment
 * @param[in] size              The size of the storage to be aligned
 * @param[in, out] ptr_ptr      Pointer to contiguous storage (a buffer) of at
 *                              least space bytes. Given it's pointer.
 * @param[in, out] space_ptr	The size of the buffer in which to operate.
 *                              Given it's pointer.
 *
 * @return The adjusted value of `*ptr_ptr`.
 *      @retval NULL            If the space provided is too small.
 */
static inline void *align(const size_t alignment, const size_t size, void **ptr_ptr, size_t *space_ptr)
{
    assert(ptr_ptr);
    assert(space_ptr);
    assert(is_pow2(alignment));

    if (*space_ptr < size) {
        return NULL;
    }

    /* explanation:
            -alignment == ~(alignment - 1U)

            example:
            for alignment = 8,

            aligned:
            ((intptr = 0) - 1 + 8)  & ~0b111 = 0b00000 = 0
            ((intptr = 1) - 1 + 8)  & ~0b111 = 0b01000 = 8
            ((intptr = 2) - 1 + 8)  & ~0b111 = 0b01000 = 8
            ...
            ((intptr = 8) - 1 + 8)  & ~0b111 = 0b01000 = 8
            ((intptr = 9) - 1 + 8)  & ~0b111 = 0b10000 = 16
            ...
            ((intptr = 17) - 1 + 8) & ~0b111 = 0b11000 = 24
    */

    const uintptr_t intptr = (uintptr_t)*ptr_ptr;
    const uintptr_t aligned = (intptr - 1u + alignment) & -alignment;
    const uintptr_t padding = aligned - intptr;

    if (padding > (*space_ptr - size)) {
        return NULL;
    }
    else {
        *space_ptr -= padding;
        return (*ptr_ptr = (void *)aligned);
    }
}

/**
 * @brief Calculate the alignment padding required to align a pointer.
 *
 * @param[in] alignment	        The alignment
 * @param[in] ptr               Pointer to a buffer as `uintptr_t`.
 *
 * @return The padding to be added to `ptr` to align `ptr`.
 */
static inline uintptr_t calc_alignment_padding(const size_t alignment, const uintptr_t ptr)
{
    assert(is_pow2(alignment));

    const uintptr_t aligned = (ptr - 1u + alignment) & -alignment;
    const uintptr_t padding = aligned - ptr;

    return padding;
}

/**
 * @def CALC_ALIGNMENT_PADDING(alignment, ptr)
 * @brief compile time `calc_alignment_padding`.
 */
#define CALC_ALIGNMENT_PADDING(alignment, ptr) ((((ptr) - 1u + (alignment)) & -(alignment)) - (ptr))
