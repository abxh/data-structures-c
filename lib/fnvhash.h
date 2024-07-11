/**
 * @file fnvhash.h
 * @brief FNV-1a hashing function.
 * @note FNV-1a is **not** a cryptographic hashing function.
 *
 * Source:
 * https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
 */

#pragma once

#include <stdint.h> // uint8_t, uint32_t
#include <stdlib.h> // size_t, NULL

/**
 * Get the FNV-1a 32-bit hash of a string of bytes.
 *
 * @param char_p Pointer to the string of bytes.
 * @param length Number of bytes.
 * @return A 32-bit hash of the bytes.
 */
static inline uint32_t fnvhash_32(const uint8_t* char_p, const size_t length) {
    uint32_t hash = 0x811c9dc5;
    for (size_t i = 0; i < length; i++) {
        hash ^= *(char_p++);
        hash *= 0x01000193;
    }
    return hash;
}

/**
 * Get the FNV-1a 64-bit hash of a string of bytes.
 *
 * @param char_p Pointer to the string of bytes.
 * @param length Number of bytes.
 * @return A 64-bit hash of the bytes.
 */
static inline uint64_t fnvhash_64(const uint8_t* char_p, const size_t length) {
    uint64_t hash = 0xcbf29ce484222325LLU;
    for (size_t i = 0; i < length; i++) {
        hash ^= *(char_p++);
        hash *= 0x00000100000001B3LLU;
    }
    return hash;
}

// vim: ft=c
