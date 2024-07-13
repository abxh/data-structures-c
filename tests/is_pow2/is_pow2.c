#include "is_pow2.h"

#include <assert.h>

/*
    Test cases (N):
    - N := 0
    - N := 1
    - N := 2
    - N := 3
    - N := 4
    - N := 8
    - N := 42
    - N := 127
    - N := 128
    - N := 129
*/

int main(void) {
    {
        assert(!is_pow2(0));
        assert(is_pow2(1)); // 2**0
        assert(is_pow2(2));
        assert(!is_pow2(3));
        assert(is_pow2(4));
        assert(is_pow2(8));
        assert(!is_pow2(42));
        assert(!is_pow2(127));
        assert(is_pow2(128));
        assert(!is_pow2(129));
    }
}
