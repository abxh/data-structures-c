#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

#include "is_pow2.h"

bool is_pow2(size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}
