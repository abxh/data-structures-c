// Sources:
// - https://stackoverflow.com/questions/10134805/bitwise-rotate-left-function
// - https://en.wikipedia.org/wiki/Circular_shift

#include <assert.h> // static_assert, assert
#include <limits.h> // CHAR_BIT
#include <stdlib.h> // size_t

#include "rotate_bits.h"

static_assert(CHAR_BIT == 8, "a char is 8 bits");

size_t rotl(size_t value, size_t count) {
    assert(count <= sizeof(value) * 8);
    if (count == 0 || count == sizeof(value) * 8) {
        return value;
    }
    return (value << count) | (value >> (sizeof(value) * 8 - count));
}

size_t rotr(size_t value, size_t count) {
    assert(count <= sizeof(value) * 8);
    if (count == 0 || count == sizeof(value) * 8) {
        return value;
    }
    return (value << (sizeof(value) * 8 - count)) | value >> count;
}

size_t rotate_bits(size_t value, int shift) {
    if (shift >= 0) {
        shift = shift % sizeof(value);
        return rotr(value, shift);
    } else {
        shift = (-shift) % sizeof(value);
        return rotl(value, shift);
    }
}
