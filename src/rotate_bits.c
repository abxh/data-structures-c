// sources:
// https://stackoverflow.com/questions/10134805/bitwise-rotate-left-function
// https://en.wikipedia.org/wiki/Circular_shift

#include <assert.h> // static_assert, assert
#include <limits.h> // CHAR_BIT
#include <stdint.h> // uint8_t, uint16_t, uint32_t, uint64_t

static_assert(CHAR_BIT == 8, "a char is 8 bits");

#define rotl(value, count)                              \
    if ((count) == 0 || (count) == sizeof(value) * 8) { \
        return (value);                                 \
    }                                                   \
    return ((value) << (count)) | ((value) >> (sizeof(value) * 8 - (count)));

#define rotr(value, count)                              \
    if ((count) == 0 || (count) == sizeof(value) * 8) { \
        return (value);                                 \
    }                                                   \
    return ((value) << (sizeof(value) * 8 - (count))) | (value) >> (count);

uint8_t rotate_bits_8(uint8_t value, int shift) {
    if (shift >= 0) {
        shift = shift % sizeof(value);
        rotr(value, shift);
    } else {
        shift = (-shift) % sizeof(value);
        rotl(value, shift);
    }
}

uint16_t rotate_bits_16(uint16_t value, int shift) {
    if (shift >= 0) {
        shift = shift % sizeof(value);
        rotr(value, shift);
    } else {
        shift = (-shift) % sizeof(value);
        rotl(value, shift);
    }
}

uint32_t rotate_bits_32(uint32_t value, int shift) {
    if (shift >= 0) {
        shift = shift % sizeof(value);
        rotr(value, shift);
    } else {
        shift = (-shift) % sizeof(value);
        rotl(value, shift);
    }
}

uint64_t rotate_bits_64(uint64_t value, int shift) {
    if (shift >= 0) {
        shift = shift % sizeof(value);
        rotr(value, shift);
    } else {
        shift = (-shift) % sizeof(value);
        rotl(value, shift);
    }
}
