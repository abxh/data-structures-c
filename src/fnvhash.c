/**
 * Source:
 * https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
 */

#include <stdint.h> // uint8_t, uint32_t
#include <stdlib.h> // size_t, NULL

#include "fnvhash.h"

#define FNV_PRIME_32 (0x01000193)
#define FNV_OFFSET_32 (0x811c9dc5)

uint32_t fnvhash_32(const uint8_t* char_p, size_t length) {
    uint32_t hash = FNV_OFFSET_32;
    for (size_t i = 0; i < length; i++) {
        hash ^= *(char_p++);
        hash *= FNV_PRIME_32;
    }
    return hash;
}

#define FNV_PRIME_64 (0x00000100000001B3LLU)
#define FNV_OFFSET_64 (0xcbf29ce484222325LLU)

uint64_t fnvhash_64(const uint8_t* char_p, size_t length) {
    uint64_t hash = FNV_OFFSET_64;
    for (size_t i = 0; i < length; i++) {
        hash ^= *(char_p++);
        hash *= FNV_PRIME_64;
    }
    return hash;
}
