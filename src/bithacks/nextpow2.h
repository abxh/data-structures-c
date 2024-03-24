
#include <assert.h> // assert
#include <stdint.h> // uint32_t, uint64_t

// Reference:
// https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2

static inline uint32_t next_pow2_u32_portable(uint32_t x) {
    assert(x != 0);
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

#if __x86_64__ // Test for 64-bit support
static inline uint64_t next_pow2_u64_portable(uint64_t x) {
    assert(x != 0);
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    x++;
    return x;
}
#endif

// See the following for info on builtin instructions:
// https://en.wikipedia.org/wiki/Find_first_set#Tool_and_library_support

#if __GNUC__ > 3 || (__GNUC__ == 3 && (__GNUC_MINOR__ > 4 || __GNUC_MINOR__ == 4)) // Test for GCC >= 3.4.0
static inline uint32_t next_pow2_u32_gcc_builtin(uint32_t x) {
    assert(x > 1);
    return 1 << (32 - __builtin_clz(x - 1));
}
#endif

#if __x86_64__ // Test for 64-bit support
#if __GNUC__ > 3 || (__GNUC__ == 3 && (__GNUC_MINOR__ > 4 || __GNUC_MINOR__ == 4)) // Test for GCC >= 3.4.0
static inline uint64_t next_pow2_u64_gcc_builtin(uint64_t x) {
    assert(x > 1);
    return 1 << (64 - __builtin_clzl(x - 1));
}
#endif
#endif
