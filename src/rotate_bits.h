/**
 * @file rotate_bits.h
 * @brief rotate bits of 8-, 16-, 32-, and 64-bit numbers.
 */

#include <stdint.h> // uint8_t, uint16_t, uint32_t, uint64_t

uint8_t rotate_bits_8(uint8_t value, int shift);

uint16_t rotate_bits_16(uint16_t value, int shift);

uint32_t rotate_bits_32(uint32_t value, int shift);

uint64_t rotate_bits_64(uint64_t value, int shift);
