/**
 * @file std_allocator.h
 * @brief wrapper of std functions for conforming to interface of allocator operations.
 * @note NULL can be passed as context_ptr, which is not used by the std functions.
 */

#pragma once

#include "allocator_ops.h" // allocator_ops_type
#include <stdlib.h>        // size_t, malloc, realloc, free, size_t

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static inline void* std_allocate(void* context_ptr, size_t alignment, size_t size) {
    return malloc(size);
}
static inline void* std_reallocate(void* context_ptr, void* ptr, size_t alignment, size_t old_size, size_t new_size) {
    return realloc(ptr, new_size);
}
static inline void std_deallocate(void* context_ptr, void* ptr) {
    return free(ptr);
}

#pragma GCC diagnostic pop

static const allocator_ops_type std_allocator_ops =
    (allocator_ops_type){.allocate_f_p = std_allocate, .reallocate_f_p = std_reallocate, .deallocate_f_p = std_deallocate};
