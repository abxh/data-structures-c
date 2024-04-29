/**
 * @file arena.h
 * @brief arena allocator.
 *
 * Example of how `arena.h` header file is used in practice can be found in `hashtable/hashtable.c`.
 */

#pragma once

#include <stdlib.h> // size_t
#include <string.h> // strlen, memcpy

#include "allocator_ops.h" // allocator_ops

typedef struct arena_type {
    size_t buffer_length;
    unsigned char* buffer_ptr;

    size_t previous_offset;
    size_t current_offset;

    allocator_ops_type allocator_ops;
} arena_type;

/**
 * @def DEFAULT_ALIGNMENT
 * @brief Set to the shown default value. Can be defined before including the header. Is undefined once header is included.
 */
#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void*))
#endif

void arena_init(void* arena_ptr, void* backing_buffer, size_t backing_buffer_length);

void* arena_allocate_aligned(void* arena_ptr, size_t alignment, size_t size);

static inline void* arena_allocate(void* arena_ptr, size_t size) {
    return arena_allocate_aligned(arena_ptr, DEFAULT_ALIGNMENT, size);
}

static inline char* arena_clone_string(void* arena_ptr, const char* char_ptr) {
    size_t n = strlen(char_ptr) + 1;
    char* char_dup_ptr = (char*)arena_allocate_aligned(arena_ptr, 1, n);
    if (char_dup_ptr == NULL) {
        return NULL;
    }
    memcpy(char_dup_ptr, char_ptr, n);
    return char_dup_ptr;
}

void* arena_reallocate_aligned(void* arena_ptr, void* old_memory, size_t alignment, size_t old_size, size_t new_size);

static inline void* arena_reallocate(void* arena_ptr, void* old_memory, size_t old_size, size_t new_size) {
    return arena_reallocate_aligned(arena_ptr, old_memory, DEFAULT_ALIGNMENT, old_size, new_size);
}

void arena_deallocate(void* arena_ptr, void* ptr);

void arena_deallocate_all(void* arena_ptr);
