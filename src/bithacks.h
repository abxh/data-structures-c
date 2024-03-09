/*
    C bit-hacks.

    Source:
    https://graphics.stanford.edu/~seander/bithacks.html
*/

#pragma once

#include <assert.h> // assert
#include <stdint.h> // uint32_t, uint64_t

static inline uint64_t rounduppow2(uint32_t v) {
    assert(v != 0);
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}
