/*  align_forward.h
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
 * @file align_forward.h
 * @brief align pointer to the next alignment boundary
 *
 * Using source:
 * https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */

#pragma once

#include "is_pow2.h" // is_pow2

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Align pointer to the next alignment boundary.
 *
 * @param ptr The pointer as number of bytes.
 * @param align The alignment
 *
 * @return A new pointer as number of bytes, which is aligned to `align` and is larger than or equal to `ptr`.
 */
static inline uintptr_t align_forward(const uintptr_t ptr, const size_t align)
{
    assert(is_pow2(align));

    const uintptr_t p = ptr;
    const uintptr_t a = (uintptr_t)align;
    const uintptr_t r = p & (a - 1); // Same as (p % a) but faster as 'a' is a power of two

    // % is defined so for r = (p % a), q: some integer
    //     p = q * a + r
    // <=> r = p - q * a

    // assuming r != 0:
    //
    //           (q+1)*a
    //         ----------->
    //           r    a-r
    //         ----> ----->
    //       q*a   p     p+a-r
    // <------|----|------|------>

    if (r != 0) {
        // If 'p' address is not aligned, push the address to the
        // next value which is aligned
        return p + a - r;
    }
    else {
        return p;
    }
}
