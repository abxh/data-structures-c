/*
 * Source:
 * https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */

#include <assert.h> // assert
#include <stdint.h> // uintptr_t
#include <stdlib.h> // size_t

#include "is_pow2.h" // is_pow2

uintptr_t align_forward(uintptr_t ptr, size_t align) {
    assert(is_pow2(align));

    uintptr_t p = ptr;
    uintptr_t a = (uintptr_t)align;
    uintptr_t r = p & (a - 1); // Same as (p % a) but faster as 'a' is a power of two

    // % is defined so for r = (p % a), q: some integer
    //     p = q * a + r
    // <=> r = p - q * a

    //           r    a-r
    //         ----> ----->
    //       q*a   p     p+a-r
    // <------|----|------|------>

    if (r != 0) {
        // If 'p' address is not aligned, push the address to the
        // next value which is aligned
        p += a - r;
    }
    return p;
}
