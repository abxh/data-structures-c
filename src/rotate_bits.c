#include <assert.h> // static_assert, assert
#include <limits.h> // CHAR_BIT
#include <stdlib.h> // size_t

static_assert(CHAR_BIT == 8, "a char is 8 bits");

// references:
// https://stackoverflow.com/questions/10134805/bitwise-rotate-left-function
// https://en.wikipedia.org/wiki/Circular_shift

size_t rotate_bits_left(size_t value, size_t count) {
    assert(count <= sizeof(value) * 8);
    if (count == 0 || count == sizeof(value) * 8) {
        return value;
    }
    return (value << count) | (value >> (sizeof(value) * 8 - count));
}

size_t rotate_bits_right(size_t value, size_t count) {
    assert(count <= sizeof(value) * 8);
    if (count == 0 || count == sizeof(value) * 8) {
        return value;
    }
    return (value << (sizeof(value) * 8 - count)) | value >> count;
}

size_t rotate_bits(size_t value, int shift) {
    if (shift >= 0) {
        shift = shift % sizeof(value);
        return rotate_bits_right(value, shift);
    } else {
        shift = (-shift) % sizeof(value);
        return rotate_bits_left(value, shift);
    }
}
