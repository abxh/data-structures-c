/**
 * @file fnvhash.h
 * @brief FNV-1a hashing function. 
 * @note FNV-1a is **not** a cryptographic hashing function.
 */

#include <stdint.h> // uint8_t, uint32_t
#include <stdlib.h> // size_t

/**
 * Get the FNV-1a 32-bit hash of a string of bytes.
 *
 * @param bytes Pointer to the string of bytes.
 * @param length Number of bytes.
 * @return A 32-bit hash of the bytes.
 */
uint32_t fnvhash_32(const uint8_t* bytes, size_t length);

/**
 * Get the FNV-1a 64-bit hash of a string of bytes.
 *
 * @param bytes Pointer to the string of bytes.
 * @param length Number of bytes.
 * @return A 64-bit hash of the bytes.
 */
uint64_t fnvhash_64(const uint8_t* bytes, size_t length);
