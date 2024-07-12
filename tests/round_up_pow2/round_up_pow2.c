#include "round_up_pow2.h"
#include "math.h"

/*
    Test cases (N):
    - N := 1
    - N := 2
    - N := 127
    - N := 128
    - N := 129
    - N := 768
    - N := 1e+6
    - N := 1e+9
    - N := UINT32_MAX / 4
*/

int main(void) {
    {
        assert(round_up_pow2(1) == 1);
        assert(round_up_pow2(2) == 2);
        assert(round_up_pow2(127) == 128);
        assert(round_up_pow2(128) == 128);
        assert(round_up_pow2(129) == 256);
        assert(round_up_pow2(768) == 1024);
        assert(round_up_pow2(1e+6) == (int)pow(2, round(log2(1e+6))));
        assert(round_up_pow2(1e+9) == (int)pow(2, round(log2(1e+9))));
        assert(round_up_pow2(UINT32_MAX / 4) == (int)pow(2, 30));
    }
}
