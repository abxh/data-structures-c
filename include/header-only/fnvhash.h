#pragma once

#include <assert.h> // assert
#include <stdint.h> // uint64_t
#include <stdlib.h> // NULL

// FNV-1a hash
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function

static inline uint64_t fnvhash(const unsigned char* char_p, size_t length) {
    assert(char_p != NULL);

    static const uint64_t FNV_OFFSET = 14695981039346656037UL;
    static const uint64_t FNV_PRIME = 1099511628211UL;

    uint64_t hash = FNV_OFFSET;
    for (size_t i = 0; i < length; i++) {
        hash ^= *(char_p++);
        hash *= FNV_PRIME;
    }

    return hash;
}
