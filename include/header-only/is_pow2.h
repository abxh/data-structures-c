#pragma once

#include <stdlib.h> // size_t

static inline size_t is_pow2(size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}
