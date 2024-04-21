/*
    wrapper for std functions to conform to allocator function types
*/

#pragma once

#include <stdlib.h> // aligned_alloc, realloc, free, size_t

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static inline void* std_allocate(void* allocator_struct_p, size_t alignment, size_t size) {
    return aligned_alloc(alignment, size);
}

static inline void* std_reallocate(void* allocator_struct_p, void* ptr, size_t alignment, size_t old_size, size_t new_size) {
    return realloc(ptr, new_size);
}

static inline void std_deallocate(void* allocator_struct_p, void* ptr) {
    return free(ptr);
}

#pragma GCC diagnostic pop
