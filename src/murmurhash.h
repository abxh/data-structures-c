/**
 * @file murmurhash.h
 * @brief Murmur3 hash hashing function. 
 * @note Murmur3 hash is **not** a cryptographic hashing function.
 *
 * MurmurHash3 was written by Austin Appleby, and is placed in the public domain. 
 */

#pragma once

#include <stdint.h> 
#include <stdlib.h>

/**
 * Get the Murmur3 (32-bit) hash of a string of bytes.
 *
 * @param key Pointer to the string of bytes.
 * @param len Number of bytes.
 * @param seed The seed - determining the hash.
 * @return A `size_t`-sized hash of the bytes.
 */
uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed);
